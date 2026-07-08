#pragma once
#include <cstdint>
long sendspin_to_raw(uint8_t v, long min, long max);
uint8_t raw_to_sendspin(long raw, long min, long max);
