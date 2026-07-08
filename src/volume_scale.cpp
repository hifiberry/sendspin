#include "volume_scale.h"

// The Sendspin wire protocol carries volume as 0-100 (Music Assistant's
// aiosendspin server rejects a client/state whose volume is outside 0-100).
// Note: sendspin-cpp's PlayerRole API documents volume as 0-255, but the value
// is transmitted verbatim, so the daemon works in 0-100 end to end.
static constexpr long SENDSPIN_VOL_MAX = 100;

long sendspin_to_raw(uint8_t v, long min, long max) {
    if (max <= min) return min;
    if (v > SENDSPIN_VOL_MAX) v = static_cast<uint8_t>(SENDSPIN_VOL_MAX);
    // round to nearest
    return min + (static_cast<long>(v) * (max - min) + SENDSPIN_VOL_MAX / 2) / SENDSPIN_VOL_MAX;
}

uint8_t raw_to_sendspin(long raw, long min, long max) {
    if (max <= min) return 0;
    if (raw <= min) return 0;
    if (raw >= max) return static_cast<uint8_t>(SENDSPIN_VOL_MAX);
    long span = max - min;
    long v = ((raw - min) * SENDSPIN_VOL_MAX + span / 2) / span;
    if (v < 0) v = 0;
    if (v > SENDSPIN_VOL_MAX) v = SENDSPIN_VOL_MAX;
    return static_cast<uint8_t>(v);
}
