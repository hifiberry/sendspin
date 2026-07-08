#include "volume_scale.h"
#include <cassert>

int main() {
    // full-scale endpoints
    assert(sendspin_to_raw(0, 0, 100) == 0);
    assert(sendspin_to_raw(255, 0, 100) == 100);
    // midpoint ~50
    assert(sendspin_to_raw(128, 0, 100) == 50);
    // non-zero min
    assert(sendspin_to_raw(0, -10239, 400) == -10239);
    assert(sendspin_to_raw(255, -10239, 400) == 400);
    // inverse
    assert(raw_to_sendspin(0, 0, 100) == 0);
    assert(raw_to_sendspin(100, 0, 100) == 255);
    assert(raw_to_sendspin(50, 0, 100) == 128);
    // clamp out-of-range raw
    assert(raw_to_sendspin(150, 0, 100) == 255);
    assert(raw_to_sendspin(-5, 0, 100) == 0);
    return 0;
}
