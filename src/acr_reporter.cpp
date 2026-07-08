// src/acr_reporter.cpp
#include "acr_reporter.h"
#include <curl/curl.h>
#include <cstdio>
#include <utility>

static size_t discard_body(char*, size_t size, size_t nmemb, void*) { return size * nmemb; }

AcrReporter::AcrReporter(std::string url) : url_(std::move(url)) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

AcrReporter::~AcrReporter() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stop_ = true;
    }
    cv_.notify_all();
    if (worker_.joinable()) worker_.join();
    curl_global_cleanup();
}

void AcrReporter::start() { worker_ = std::thread(&AcrReporter::run, this); }

void AcrReporter::post(std::string json_body) {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (stop_) return;
        queue_.push_back(std::move(json_body));
    }
    cv_.notify_one();
}

void AcrReporter::run() {
    CURL* curl = curl_easy_init();
    struct curl_slist* headers = curl_slist_append(nullptr, "Content-Type: application/json");
    while (true) {
        std::string body;
        {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [&] { return stop_ || !queue_.empty(); });
            if (stop_ && queue_.empty()) break;
            body = std::move(queue_.front());
            queue_.pop_front();
        }
        if (!curl) continue;
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.size());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 2L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, discard_body);
        CURLcode rc = curl_easy_perform(curl);
        if (rc != CURLE_OK)
            std::fprintf(stderr, "acr: POST failed: %s\n", curl_easy_strerror(rc));
    }
    curl_slist_free_all(headers);
    if (curl) curl_easy_cleanup(curl);
}
