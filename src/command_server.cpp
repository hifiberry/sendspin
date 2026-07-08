// src/command_server.cpp
#include "command_server.h"
#include "http_request.h"
#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

CommandServer::CommandServer(int port, Handler handler)
    : port_(port), handler_(std::move(handler)) {}

CommandServer::~CommandServer() {
    stop_ = true;
    if (listen_fd_ >= 0) { ::shutdown(listen_fd_, SHUT_RDWR); ::close(listen_fd_); }
    if (thread_.joinable()) thread_.join();
}

bool CommandServer::start() {
    listen_fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ < 0) return false;
    int one = 1;
    ::setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port_));
    ::inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (::bind(listen_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::fprintf(stderr, "command: bind 127.0.0.1:%d failed\n", port_);
        ::close(listen_fd_); listen_fd_ = -1;
        return false;
    }
    if (::listen(listen_fd_, 4) < 0) { ::close(listen_fd_); listen_fd_ = -1; return false; }
    thread_ = std::thread(&CommandServer::run, this);
    return true;
}

void CommandServer::run() {
    while (!stop_) {
        int fd = ::accept(listen_fd_, nullptr, nullptr);
        if (fd < 0) { if (stop_) break; continue; }
        std::string raw;
        char buf[2048];
        HttpRequest req;
        for (int i = 0; i < 16; ++i) {  // bounded reads
            ssize_t n = ::recv(fd, buf, sizeof(buf), 0);
            if (n <= 0) break;
            raw.append(buf, static_cast<size_t>(n));
            req = parse_http_request(raw);
            if (req.complete) break;
        }
        const char* resp = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n";
        if (req.complete && req.path == "/command") {
            ParsedCommand pc = parse_command(req.body);
            if (pc.valid) { if (handler_) handler_(pc); }
            else resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
        } else {
            resp = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        }
        ::send(fd, resp, std::strlen(resp), 0);
        ::close(fd);
    }
}
