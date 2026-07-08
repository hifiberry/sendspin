#include "metadata_map.h"

Song song_from_metadata(const sendspin::ServerMetadataStateObject& m) {
    Song s;
    if (m.title) s.title = *m.title;
    if (m.artist) s.artist = *m.artist;
    if (m.album) s.album = *m.album;
    if (m.artwork_url) s.artwork_url = *m.artwork_url;
    if (m.progress && m.progress->track_duration > 0) {
        s.duration_seconds = m.progress->track_duration / 1000.0;
    }
    return s;
}
