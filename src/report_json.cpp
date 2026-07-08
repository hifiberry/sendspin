#include "report_json.h"
#include <nlohmann/json.hpp>
using nlohmann::json;

std::string make_state_changed(const std::string& state) {
    return json({{"type", "state_changed"}, {"state", state}}).dump();
}

std::string make_song_changed(const Song& s) {
    json song = json::object();
    if (s.title) song["title"] = *s.title;
    if (s.artist) song["artist"] = *s.artist;
    if (s.album) song["album"] = *s.album;
    if (s.duration_seconds) song["duration"] = *s.duration_seconds;
    if (s.artwork_url) song["artwork_url"] = *s.artwork_url;
    return json({{"type", "song_changed"}, {"song", song}}).dump();
}

std::string make_position_changed(double seconds) {
    return json({{"type", "position_changed"}, {"position", seconds}}).dump();
}
