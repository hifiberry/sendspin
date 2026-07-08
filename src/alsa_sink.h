// src/alsa_sink.h
#pragma once
#include <alsa/asoundlib.h>
#include <cstdint>
#include <mutex>
#include <string>

snd_pcm_format_t alsa_format_for_bits(uint8_t bit_depth);

class AlsaSink {
 public:
    explicit AlsaSink(std::string device);
    ~AlsaSink();
    bool configure(unsigned rate, unsigned channels, uint8_t bit_depth);
    size_t write(const uint8_t* data, size_t len, unsigned timeout_ms);
    void stop();

 private:
    std::string device_;
    snd_pcm_t* pcm_ = nullptr;
    unsigned channels_ = 2;
    unsigned frame_bytes_ = 4;  // channels * bytes_per_sample
    std::mutex mtx_;
};
