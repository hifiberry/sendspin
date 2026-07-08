// src/mdns.cpp
#include "mdns.h"
#include <arpa/inet.h>
#include <cstdio>

MdnsAdvertiser::~MdnsAdvertiser() { stop(); }

bool MdnsAdvertiser::start(const std::string& name, uint16_t port, const std::string& path) {
    TXTRecordRef txt;
    TXTRecordCreate(&txt, 0, nullptr);
    TXTRecordSetValue(&txt, "path", static_cast<uint8_t>(path.size()), path.c_str());
    TXTRecordSetValue(&txt, "name", static_cast<uint8_t>(name.size()), name.c_str());

    DNSServiceErrorType err = DNSServiceRegister(
        &service_ref_, 0, 0, name.c_str(), "_sendspin._tcp", nullptr, nullptr,
        htons(port), TXTRecordGetLength(&txt), TXTRecordGetBytesPtr(&txt), nullptr, nullptr);

    TXTRecordDeallocate(&txt);
    if (err != kDNSServiceErr_NoError) {
        std::fprintf(stderr, "mdns: register failed: %d\n", err);
        return false;
    }
    std::fprintf(stderr, "mdns: advertising _sendspin._tcp:%u (%s)\n", port, name.c_str());
    return true;
}

void MdnsAdvertiser::stop() {
    if (service_ref_) { DNSServiceRefDeallocate(service_ref_); service_ref_ = nullptr; }
}
