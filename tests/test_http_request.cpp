#include "http_request.h"
#include <cassert>

int main() {
    std::string raw =
        "POST /command HTTP/1.1\r\nHost: x\r\nContent-Length: 18\r\n\r\n"
        "{\"command\":\"play\"}";
    HttpRequest r = parse_http_request(raw);
    assert(r.complete);
    assert(r.method == "POST");
    assert(r.path == "/command");
    assert(r.body == "{\"command\":\"play\"}");

    // headers not yet terminated -> incomplete
    assert(!parse_http_request("POST /command HTTP/1.1\r\nContent-Length: 5\r\n").complete);
    // body short of Content-Length -> incomplete
    assert(!parse_http_request(
        "POST /command HTTP/1.1\r\nContent-Length: 10\r\n\r\nshort").complete);
    // no body, no content-length -> complete with empty body
    HttpRequest g = parse_http_request("GET /x HTTP/1.1\r\n\r\n");
    assert(g.complete && g.method == "GET" && g.path == "/x" && g.body.empty());

    // malformed Content-Length must NOT throw (would abort the daemon) -> incomplete
    assert(!parse_http_request(
        "POST /command HTTP/1.1\r\nContent-Length: notanumber\r\n\r\n{}").complete);
    // overflowing Content-Length must NOT throw -> incomplete
    assert(!parse_http_request(
        "POST /command HTTP/1.1\r\nContent-Length: 99999999999999999999\r\n\r\n{}").complete);
    return 0;
}
