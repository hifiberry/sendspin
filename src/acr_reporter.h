// src/acr_reporter.h
#pragma once
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>
#include <thread>

class AcrReporter {
 public:
    explicit AcrReporter(std::string url);
    ~AcrReporter();
    void start();
    void post(std::string json_body);

 private:
    void run();
    std::string url_;
    std::thread worker_;
    std::deque<std::string> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool stop_ = false;
};
