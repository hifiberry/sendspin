#pragma once
#include "song.h"
#include <string>
std::string make_state_changed(const std::string& state);
std::string make_song_changed(const Song& s);
std::string make_position_changed(double seconds);
std::string make_stream_info(const std::string& codec, unsigned sample_rate,
                             unsigned bits_per_sample, unsigned channels);
