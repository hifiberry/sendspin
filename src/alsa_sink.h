// src/alsa_sink.h
#pragma once
#include <alsa/asoundlib.h>
#include <cstdint>
#include <functional>
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

    // Fired after each successful write with the number of frames just queued
    // and the client-clock (steady_clock) microsecond timestamp at which they
    // will finish playing. Wire to PlayerRole::notify_audio_played() — the
    // player role stays in start-up priming silence until it receives this
    // consumed-frames feedback. Called on the audio (sync-task) thread.
    std::function<void(uint32_t frames, int64_t finish_us)> on_frames_played;

 private:
    std::string device_;
    snd_pcm_t* pcm_ = nullptr;
    unsigned channels_ = 2;
    unsigned rate_ = 44100;
    unsigned frame_bytes_ = 4;  // channels * bytes_per_sample
    std::mutex mtx_;
};
