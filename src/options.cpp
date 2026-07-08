#include "options.h"
#include <stdexcept>
#include <string>

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
        else if (a == "--command-port") o.command_port = std::stoi(need(i));
        else if (a == "--acr-url") o.acr_url = need(i);
        else if (a == "--name") o.name = need(i);
        else if (a == "--sendspin-port") o.sendspin_port = static_cast<uint16_t>(std::stoi(need(i)));
        else throw std::runtime_error("unknown flag: " + a);
    }
    return o;
}
