// src/command_server.h
#pragma once
#include "command_map.h"
#include <atomic>
#include <functional>
#include <thread>

class CommandServer {
 public:
    using Handler = std::function<void(const ParsedCommand&)>;
    CommandServer(int port, Handler handler);
    ~CommandServer();
    bool start();

 private:
    void run();
    int port_;
    Handler handler_;
    int listen_fd_ = -1;
    std::thread thread_;
    std::atomic<bool> stop_{false};
};
