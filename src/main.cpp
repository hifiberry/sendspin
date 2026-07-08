// src/main.cpp
#include "acr_reporter.h"
#include "alsa_sink.h"
#include "command_server.h"
#include "listeners.h"
#include "mdns.h"
#include "options.h"
#include "report_json.h"
#include "volume_control.h"
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <sendspin/client.h>
#include <sendspin/controller_role.h>
#include <sendspin/metadata_role.h>
#include <sendspin/player_role.h>
#include <thread>

static std::atomic<bool> g_running{true};
static void on_sigterm(int) { g_running = false; }

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i)
        if (std::strcmp(argv[i], "--version") == 0) { std::printf("sendspind 0.1.0\n"); return 0; }

    Options opt;
    try { opt = parse_args(argc, argv); }
    catch (const std::exception& e) { std::fprintf(stderr, "args: %s\n", e.what()); return 2; }

    std::signal(SIGINT, on_sigterm);
    std::signal(SIGTERM, on_sigterm);

    AlsaSink sink(opt.alsa_device);
    VolumeControl volume(opt.mixer_device, opt.mixer_control);
    volume.open();  // volume disabled gracefully if control missing
    AcrReporter reporter(opt.acr_url);
    reporter.start();

    // sendspin client
    using namespace sendspin;

    PlayerRole* player_ptr = nullptr;
    std::atomic<int> last_applied{-1};

    // Declared before the client so they outlive it (sendspin-cpp: listener must outlive the client); prevents a shutdown-time use-after-free when ~SendspinClient joins the sync task.
    PlayerListener player_listener(sink, volume, reporter, &player_ptr, &last_applied);
    MetaListener meta_listener(reporter);
    NetProvider net;

    SendspinClientConfig cfg;
    cfg.client_id = opt.name;   // stable-ish id; MA also gets a MAC via auto-detect
    cfg.name = opt.name;
    cfg.product_name = "HiFiBerry";
    cfg.manufacturer = "HiFiBerry";
    cfg.software_version = "0.1.0";
    cfg.server_port = opt.sendspin_port;
    SendspinClient client(std::move(cfg));

    PlayerRoleConfig pcfg;
    pcfg.audio_formats = {
        {SendspinCodecFormat::FLAC, 2, 44100, 16},
        {SendspinCodecFormat::OPUS, 2, 48000, 16},
        {SendspinCodecFormat::PCM,  2, 44100, 16},
    };
    auto& player = client.add_player(std::move(pcfg));
    player_ptr = &player;
    auto& controller = client.add_controller();
    auto& metadata = client.add_metadata();

    player.set_listener(&player_listener);
    metadata.set_listener(&meta_listener);
    client.set_network_provider(&net);

    PendingCommands pending;
    CommandServer cmd_server(opt.command_port, [&pending](const ParsedCommand& pc) {
        pending.push(pc.cmd);   // dispatched on the main loop below
    });
    if (!cmd_server.start())
        std::fprintf(stderr, "warning: command server failed to bind :%d\n", opt.command_port);

    if (!client.start_server()) { std::fprintf(stderr, "sendspin: start_server failed\n"); return 1; }

    MdnsAdvertiser mdns;
    mdns.start(opt.name, opt.sendspin_port, "/sendspin");

    auto last_poll = std::chrono::steady_clock::now();
    while (g_running.load()) {
        client.loop();

        // dispatch queued transport commands on the main loop thread
        SendspinControllerCommand c;
        while (pending.pop(c)) controller.send_command(c);

        auto now = std::chrono::steady_clock::now();
        if (now - last_poll >= std::chrono::seconds(1)) {
            last_poll = now;
            // local volume change -> report to MA (set-if-changed)
            int cur = volume.current_sendspin_volume();
            if (cur >= 0 && cur != last_applied.load()) {
                player.update_volume(static_cast<uint8_t>(cur));
                last_applied.store(cur);
            }
            // position -> ACR
            if (client.is_connected()) {
                uint32_t dur = metadata.get_track_duration_ms();
                if (dur > 0)
                    reporter.post(make_position_changed(metadata.get_track_progress_ms() / 1000.0));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return 0;
}
