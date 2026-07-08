#include "volume_scale.h"

long sendspin_to_raw(uint8_t v, long min, long max) {
    if (max <= min) return min;
    // round to nearest
    return min + (static_cast<long>(v) * (max - min) + 127) / 255;
}

uint8_t raw_to_sendspin(long raw, long min, long max) {
    if (max <= min) return 0;
    if (raw <= min) return 0;
    if (raw >= max) return 255;
    long span = max - min;
    long v = ((raw - min) * 255 + span / 2) / span;
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    return static_cast<uint8_t>(v);
}
