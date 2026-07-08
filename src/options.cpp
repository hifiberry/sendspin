#include "options.h"
#include <stdexcept>
#include <string>

static int parse_int_strict(const std::string& s, const std::string& flag) {
    size_t pos = 0;
    int v = 0;
    try { v = std::stoi(s, &pos); }
    catch (const std::logic_error&) {
        throw std::runtime_error("invalid value for " + flag + ": " + s);
    }
    if (pos != s.size())
        throw std::runtime_error("invalid value for " + flag + ": " + s);
    return v;
}

Options parse_args(int argc, char** argv) {
    Options o;
    auto need = [&](int& i) -> std::string {
        if (i + 1 >= argc) throw std::runtime_error(std::string("missing value for ") + argv[i]);
        return argv[++i];
    };
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--alsa-device") o.alsa_device = need(i);
        else if (a == "--mixer-control") o.mixer_control = need(i);
        else if (a == "--mixer-device") o.mixer_device = need(i);
        else if (a == "--command-port") o.command_port = parse_int_strict(need(i), "--command-port");
        else if (a == "--acr-url") o.acr_url = need(i);
        else if (a == "--name") o.name = need(i);
        else if (a == "--sendspin-port") o.sendspin_port = static_cast<uint16_t>(parse_int_strict(need(i), "--sendspin-port"));
        else throw std::runtime_error("unknown flag: " + a);
    }
    return o;
}
