#pragma once
#include <string>
struct HttpRequest { std::string method, path, body; bool complete = false; };
HttpRequest parse_http_request(const std::string& raw);
