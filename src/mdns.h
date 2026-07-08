// src/mdns.h
#pragma once
#include <dns_sd.h>
#include <cstdint>
#include <string>

class MdnsAdvertiser {
 public:
    ~MdnsAdvertiser();
    bool start(const std::string& name, uint16_t port, const std::string& path);
    void stop();

 private:
    DNSServiceRef service_ref_ = nullptr;
};
