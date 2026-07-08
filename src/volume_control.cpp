// src/volume_control.cpp
#include "volume_control.h"
#include "volume_scale.h"
#include <cstdio>
#include <utility>

VolumeControl::VolumeControl(std::string device, std::string control)
    : device_(std::move(device)), control_(std::move(control)) {}

VolumeControl::~VolumeControl() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (handle_) snd_mixer_close(handle_);
}

bool VolumeControl::open() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (control_.empty()) {
        std::fprintf(stderr, "volume: no mixer control name; volume disabled\n");
        return false;
    }
    if (snd_mixer_open(&handle_, 0) < 0) return false;
    if (snd_mixer_attach(handle_, device_.c_str()) < 0) return false;
    if (snd_mixer_selem_register(handle_, nullptr, nullptr) < 0) return false;
    if (snd_mixer_load(handle_) < 0) return false;

    snd_mixer_selem_id_t* sid;
    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, control_.c_str());
    elem_ = snd_mixer_find_selem(handle_, sid);
    if (!elem_) {
        std::fprintf(stderr, "volume: control '%s' not found on %s\n",
                     control_.c_str(), device_.c_str());
        return false;
    }
    snd_mixer_selem_get_playback_volume_range(elem_, &min_, &max_);
    return true;
}

void VolumeControl::set_sendspin_volume(uint8_t v) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!elem_) return;
    long raw = sendspin_to_raw(v, min_, max_);
    snd_mixer_selem_set_playback_volume_all(elem_, raw);
}

void VolumeControl::set_muted(bool muted) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!elem_) return;
    if (snd_mixer_selem_has_playback_switch(elem_)) {
        snd_mixer_selem_set_playback_switch_all(elem_, muted ? 0 : 1);
    }
}

int VolumeControl::current_sendspin_volume() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!elem_) return -1;
    snd_mixer_handle_events(handle_);
    long raw = 0;
    if (snd_mixer_selem_get_playback_volume(elem_, SND_MIXER_SCHN_FRONT_LEFT, &raw) < 0) return -1;
    return raw_to_sendspin(raw, min_, max_);
}
