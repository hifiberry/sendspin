// src/volume_control.h
#pragma once
#include <alsa/asoundlib.h>
#include <cstdint>
#include <mutex>
#include <string>

class VolumeControl {
 public:
    VolumeControl(std::string mixer_device, std::string control_name);
    ~VolumeControl();
    bool open();
    void set_sendspin_volume(uint8_t v);
    void set_muted(bool muted);
    int current_sendspin_volume();

 private:
    std::string device_, control_;
    snd_mixer_t* handle_ = nullptr;
    snd_mixer_elem_t* elem_ = nullptr;
    long min_ = 0, max_ = 0;
    std::mutex mtx_;
};
