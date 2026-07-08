#pragma once
#include <cstdint>
#include <string>

struct Options {
    std::string alsa_device = "default";
    std::string mixer_control;
    std::string mixer_device = "hw:0";
    int command_port = 3547;
    std::string acr_url = "http://localhost:1080/api/player/sendspin/update";
    std::string name = "HiFiBerry";
    uint16_t sendspin_port = 8928;
};

Options parse_args(int argc, char** argv);
