// tests/test_report_json.cpp
#include "report_json.h"
#include <nlohmann/json.hpp>
#include <cassert>
using nlohmann::json;

int main() {
    assert(json::parse(make_state_changed("playing")) ==
           json({{"type","state_changed"},{"state","playing"}}));

    Song s;
    s.title = "One"; s.artist = "Johnny Cash"; s.album = "American IV";
    s.artwork_url = "http://x/c.jpg"; s.duration_seconds = 214.0;
    json j = json::parse(make_song_changed(s));
    assert(j["type"] == "song_changed");
    assert(j["song"]["title"] == "One");
    assert(j["song"]["artist"] == "Johnny Cash");
    assert(j["song"]["album"] == "American IV");
    assert(j["song"]["artwork_url"] == "http://x/c.jpg");
    assert(j["song"]["duration"] == 214.0);

    // absent optionals are omitted
    Song empty; empty.title = "T";
    json je = json::parse(make_song_changed(empty));
    assert(je["song"].contains("title"));
    assert(!je["song"].contains("artist"));
    assert(!je["song"].contains("artwork_url"));

    json p = json::parse(make_position_changed(42.5));
    assert(p["type"] == "position_changed");
    assert(p["position"] == 42.5);

    json si = json::parse(make_stream_info("FLAC", 44100, 16, 2));
    assert(si["type"] == "stream_info");
    assert(si["stream"]["codec"] == "FLAC");
    assert(si["stream"]["sample_rate"] == 44100);
    assert(si["stream"]["bits_per_sample"] == 16);
    assert(si["stream"]["channels"] == 2);
    // empty/zero fields are omitted
    json si2 = json::parse(make_stream_info("", 0, 0, 0));
    assert(si2["stream"].empty());
    return 0;
}
