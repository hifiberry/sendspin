#include "http_request.h"
#include <algorithm>
#include <cctype>
#include <sstream>

HttpRequest parse_http_request(const std::string& raw) {
    HttpRequest r;
    auto header_end = raw.find("\r\n\r\n");
    if (header_end == std::string::npos) return r;  // headers incomplete
    std::string head = raw.substr(0, header_end);
    std::string body = raw.substr(header_end + 4);

    std::istringstream hs(head);
    std::string line;
    if (!std::getline(hs, line)) return r;
    if (!line.empty() && line.back() == '\r') line.pop_back();
    std::istringstream rl(line);
    rl >> r.method >> r.path;  // "POST /command HTTP/1.1"

    long content_length = -1;
    while (std::getline(hs, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string key = line.substr(0, colon);
        std::transform(key.begin(), key.end(), key.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (key == "content-length") {
            std::string val = line.substr(colon + 1);
            content_length = std::stol(val);
        }
    }

    if (content_length < 0) { r.body.clear(); r.complete = true; return r; }
    if (static_cast<long>(body.size()) < content_length) return r;  // body incomplete
    r.body = body.substr(0, static_cast<size_t>(content_length));
    r.complete = true;
    return r;
}
