// tests/test_metadata_map.cpp
#include "metadata_map.h"
#include <cassert>
using sendspin::ServerMetadataStateObject;
using sendspin::MetadataProgressObject;

int main() {
    ServerMetadataStateObject m;
    m.title = "One"; m.artist = "Johnny Cash"; m.album = "American IV";
    m.artwork_url = "http://x/c.jpg";
    m.progress = MetadataProgressObject{5000u, 214000u, 1000u};  // ms
    Song s = song_from_metadata(m);
    assert(s.title && *s.title == "One");
    assert(s.artist && *s.artist == "Johnny Cash");
    assert(s.album && *s.album == "American IV");
    assert(s.artwork_url && *s.artwork_url == "http://x/c.jpg");
    assert(s.duration_seconds && *s.duration_seconds == 214.0);

    // no progress -> no duration; missing fields stay unset
    ServerMetadataStateObject m2; m2.title = "T";
    Song s2 = song_from_metadata(m2);
    assert(s2.title && !s2.artist && !s2.duration_seconds);

    // zero duration (live stream) -> unset
    ServerMetadataStateObject m3;
    m3.progress = MetadataProgressObject{0u, 0u, 1000u};
    assert(!song_from_metadata(m3).duration_seconds);
    return 0;
}
