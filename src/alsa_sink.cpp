// src/alsa_sink.cpp
#include "alsa_sink.h"
#include <cstdio>
#include <utility>

snd_pcm_format_t alsa_format_for_bits(uint8_t bit_depth) {
    switch (bit_depth) {
        case 16: return SND_PCM_FORMAT_S16_LE;
        case 24: return SND_PCM_FORMAT_S24_LE;
        case 32: return SND_PCM_FORMAT_S32_LE;
        default: return SND_PCM_FORMAT_S16_LE;
    }
}

AlsaSink::AlsaSink(std::string device) : device_(std::move(device)) {}
AlsaSink::~AlsaSink() { stop(); }

bool AlsaSink::configure(unsigned rate, unsigned channels, uint8_t bit_depth) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (pcm_) { snd_pcm_drain(pcm_); snd_pcm_close(pcm_); pcm_ = nullptr; }
    snd_pcm_format_t fmt = alsa_format_for_bits(bit_depth);
    int err = snd_pcm_open(&pcm_, device_.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    if (err < 0) {
        std::fprintf(stderr, "alsa: open %s failed: %s\n", device_.c_str(), snd_strerror(err));
        pcm_ = nullptr;
        return false;
    }
    err = snd_pcm_set_params(pcm_, fmt, SND_PCM_ACCESS_RW_INTERLEAVED,
                             channels, rate, /*soft_resample=*/1, /*latency_us=*/200000);
    if (err < 0) {
        std::fprintf(stderr, "alsa: set_params failed: %s\n", snd_strerror(err));
        snd_pcm_close(pcm_); pcm_ = nullptr;
        return false;
    }
    channels_ = channels;
    frame_bytes_ = channels * (snd_pcm_format_physical_width(fmt) / 8);
    if (frame_bytes_ == 0) frame_bytes_ = 4;
    return true;
}

size_t AlsaSink::write(const uint8_t* data, size_t len, unsigned timeout_ms) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!pcm_ || frame_bytes_ == 0) return 0;
    snd_pcm_uframes_t frames = len / frame_bytes_;
    if (frames == 0) return 0;
    if (timeout_ms > 0) snd_pcm_wait(pcm_, static_cast<int>(timeout_ms));
    snd_pcm_sframes_t written = snd_pcm_writei(pcm_, data, frames);
    if (written == -EPIPE) {            // underrun
        snd_pcm_prepare(pcm_);
        written = snd_pcm_writei(pcm_, data, frames);
    }
    if (written < 0) {
        std::fprintf(stderr, "alsa: writei failed: %s\n", snd_strerror((int)written));
        return 0;
    }
    return static_cast<size_t>(written) * frame_bytes_;
}

void AlsaSink::stop() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (pcm_) { snd_pcm_drain(pcm_); snd_pcm_close(pcm_); pcm_ = nullptr; }
}
