#include "options.h"
#include <cassert>
#include <cstring>
#include <string>

int main() {
    // Defaults when no args
    {
        char prog[] = "sendspind";
        char* argv[] = {prog, nullptr};
        Options o = parse_args(1, argv);
        assert(o.alsa_device == "default");
        assert(o.command_port == 3547);
        assert(o.sendspin_port == 8928);
    }
    // Parses provided values
    {
        char prog[] = "sendspind";
        char a1[] = "--mixer-control"; char v1[] = "Digital";
        char a2[] = "--mixer-device";  char v2[] = "hw:0";
        char a3[] = "--command-port";  char v3[] = "3547";
        char a4[] = "--name";          char v4[] = "Living Room";
        char* argv[] = {prog, a1, v1, a2, v2, a3, v3, a4, v4, nullptr};
        Options o = parse_args(9, argv);
        assert(o.mixer_control == "Digital");
        assert(o.mixer_device == "hw:0");
        assert(o.command_port == 3547);
        assert(o.name == "Living Room");
    }
    // Unknown flag throws
    {
        char prog[] = "sendspind"; char bad[] = "--nope";
        char* argv[] = {prog, bad, nullptr};
        bool threw = false;
        try { parse_args(2, argv); } catch (const std::exception&) { threw = true; }
        assert(threw);
    }
    return 0;
}
