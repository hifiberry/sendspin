// src/listeners.cpp
#include "listeners.h"
#include "metadata_map.h"
#include "report_json.h"
#include <curl/curl.h>
#include <utility>

const char* codec_name(sendspin::SendspinCodecFormat c) {
    switch (c) {
        case sendspin::SendspinCodecFormat::FLAC: return "FLAC";
        case sendspin::SendspinCodecFormat::OPUS: return "Opus";
        case sendspin::SendspinCodecFormat::PCM:  return "PCM";
        default: return "";
    }
}

void PlayerListener::on_stream_start() {
    if (*player_) {
        const auto& p = (*player_)->get_current_stream_params();
        unsigned rate = p.sample_rate.value_or(44100);
        unsigned ch = p.channels.value_or(2);
        uint8_t bits = p.bit_depth.value_or(16);
        sink_.configure(rate, ch, bits);
    }
    // The main loop posts stream_info once the codec header is decoded (the
    // codec is not yet available in the stream params at stream start).
    streaming_->store(true);
    reporter_.post(make_state_changed("playing"));
}

void PlayerListener::on_stream_end() {
    sink_.stop();
    streaming_->store(false);
    reporter_.post(make_state_changed("stopped"));
}

void PlayerListener::on_volume_changed(uint8_t v) {
    vol_.set_sendspin_volume(v);
    last_applied_->store(v);
}

void PlayerListener::on_mute_changed(bool muted) { vol_.set_muted(muted); }

void MetaListener::on_metadata(const sendspin::ServerMetadataStateObject& m) {
    std::fprintf(stderr, "DIAG on_metadata title=%s artist=%s album=%s art=%s\n",
                 m.title.value_or("-").c_str(), m.artist.value_or("-").c_str(),
                 m.album.value_or("-").c_str(), m.artwork_url.value_or("-").c_str());
    reporter_.post(make_song_changed(song_from_metadata(m)));
}

void MetaListener::on_metadata_clear() {
    std::fprintf(stderr, "DIAG on_metadata_clear\n");
    reporter_.post(make_state_changed("stopped"));
}

static size_t discard_persistence_body(char*, size_t size, size_t nmemb, void*) {
    return size * nmemb;
}

ConfigDbPersistenceProvider::ConfigDbPersistenceProvider(std::string config_url)
    : config_url_(std::move(config_url)) {}

bool ConfigDbPersistenceProvider::save_static_delay(uint16_t delay_ms) {
    CURL* curl = curl_easy_init();
    if (!curl) return false;

    std::string body = "{\"value\":\"" + std::to_string(delay_ms) + "\"}";
    struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, config_url_.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_persistence_body);

    CURLcode rc = curl_easy_perform(curl);
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (rc != CURLE_OK || http_code < 200 || http_code >= 300) {
        std::fprintf(stderr, "persistence: POST to ConfigDB failed (rc=%d http=%ld)\n", rc, http_code);
        return false;
    }
    return true;
}
