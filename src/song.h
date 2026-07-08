#pragma once
#include <optional>
#include <string>
struct Song {
    std::optional<std::string> title, artist, album, artwork_url;
    std::optional<double> duration_seconds;
};
