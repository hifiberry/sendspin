#include "volume_scale.h"
#include <cassert>

int main() {
    // Sendspin wire volume is 0-100. full-scale endpoints
    assert(sendspin_to_raw(0, 0, 100) == 0);
    assert(sendspin_to_raw(100, 0, 100) == 100);
    // midpoint 50
    assert(sendspin_to_raw(50, 0, 100) == 50);
    // non-zero / negative min
    assert(sendspin_to_raw(0, -10239, 400) == -10239);
    assert(sendspin_to_raw(100, -10239, 400) == 400);
    // an out-of-range input (e.g. a stray 0-255 value) is clamped to 100
    assert(sendspin_to_raw(171, 0, 100) == 100);
    // inverse maps back into 0-100 and never exceeds 100
    assert(raw_to_sendspin(0, 0, 100) == 0);
    assert(raw_to_sendspin(100, 0, 100) == 100);
    assert(raw_to_sendspin(50, 0, 100) == 50);
    // clamp out-of-range raw
    assert(raw_to_sendspin(150, 0, 100) == 100);
    assert(raw_to_sendspin(-5, 0, 100) == 0);
    return 0;
}
