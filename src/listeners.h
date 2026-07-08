// src/listeners.h
#pragma once
#include "acr_reporter.h"
#include "alsa_sink.h"
#include "command_map.h"
#include "volume_control.h"
#include <atomic>
#include <deque>
#include <mutex>
#include <sendspin/client.h>
#include <sendspin/metadata_role.h>
#include <sendspin/player_role.h>

// Thread-safe holder for controller commands produced off the main loop.
class PendingCommands {
 public:
    void push(sendspin::SendspinControllerCommand c) {
        std::lock_guard<std::mutex> lock(mtx_); q_.push_back(c);
    }
    bool pop(sendspin::SendspinControllerCommand& out) {
        std::lock_guard<std::mutex> lock(mtx_);
        if (q_.empty()) return false;
        out = q_.front(); q_.pop_front(); return true;
    }
 private:
    std::deque<sendspin::SendspinControllerCommand> q_;
    std::mutex mtx_;
};

class PlayerListener : public sendspin::PlayerRoleListener {
 public:
    PlayerListener(AlsaSink& sink, VolumeControl& vol, AcrReporter& reporter,
                   sendspin::PlayerRole** player, std::atomic<int>* last_applied)
        : sink_(sink), vol_(vol), reporter_(reporter), player_(player),
          last_applied_(last_applied) {}
    size_t on_audio_write(uint8_t* data, size_t len, uint32_t timeout_ms) override {
        return sink_.write(data, len, timeout_ms);
    }
    void on_stream_start() override;
    void on_stream_end() override;
    void on_volume_changed(uint8_t v) override;
    void on_mute_changed(bool muted) override;

 private:
    AlsaSink& sink_;
    VolumeControl& vol_;
    AcrReporter& reporter_;
    sendspin::PlayerRole** player_;
    std::atomic<int>* last_applied_;
};

class MetaListener : public sendspin::MetadataRoleListener {
 public:
    explicit MetaListener(AcrReporter& reporter) : reporter_(reporter) {}
    void on_metadata(const sendspin::ServerMetadataStateObject& m) override;
    void on_metadata_clear() override;
 private:
    AcrReporter& reporter_;
};

class NetProvider : public sendspin::SendspinNetworkProvider {
 public:
    bool is_network_ready() override { return true; }
};
