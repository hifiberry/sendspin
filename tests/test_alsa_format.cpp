// tests/test_alsa_format.cpp
#include "alsa_sink.h"
#include <cassert>

int main() {
    assert(alsa_format_for_bits(16) == SND_PCM_FORMAT_S16_LE);
    assert(alsa_format_for_bits(24) == SND_PCM_FORMAT_S24_LE);
    assert(alsa_format_for_bits(32) == SND_PCM_FORMAT_S32_LE);
    assert(alsa_format_for_bits(99) == SND_PCM_FORMAT_S16_LE);  // fallback
    return 0;
}
