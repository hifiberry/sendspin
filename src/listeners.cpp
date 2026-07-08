// src/listeners.cpp
#include "listeners.h"
#include "metadata_map.h"
#include "report_json.h"

void PlayerListener::on_stream_start() {
    if (*player_) {
        const auto& p = (*player_)->get_current_stream_params();
        unsigned rate = p.sample_rate.value_or(44100);
        unsigned ch = p.channels.value_or(2);
        uint8_t bits = p.bit_depth.value_or(16);
        sink_.configure(rate, ch, bits);
    }
    reporter_.post(make_state_changed("playing"));
}

void PlayerListener::on_stream_end() {
    sink_.stop();
    reporter_.post(make_state_changed("stopped"));
}

void PlayerListener::on_volume_changed(uint8_t v) {
    vol_.set_sendspin_volume(v);
    last_applied_->store(v);
}

void PlayerListener::on_mute_changed(bool muted) { vol_.set_muted(muted); }

void MetaListener::on_metadata(const sendspin::ServerMetadataStateObject& m) {
    reporter_.post(make_song_changed(song_from_metadata(m)));
}

void MetaListener::on_metadata_clear() { reporter_.post(make_state_changed("stopped")); }
