#include "command_map.h"
#include <cassert>
using sendspin::SendspinControllerCommand;

int main() {
    assert(parse_command(R"({"command":"play"})").valid);
    assert(parse_command(R"({"command":"play"})").cmd == SendspinControllerCommand::PLAY);
    assert(parse_command(R"({"command":"pause"})").cmd == SendspinControllerCommand::PAUSE);
    assert(parse_command(R"({"command":"stop"})").cmd == SendspinControllerCommand::STOP);
    assert(parse_command(R"({"command":"next"})").cmd == SendspinControllerCommand::NEXT);
    assert(parse_command(R"({"command":"previous"})").cmd == SendspinControllerCommand::PREVIOUS);
    // seek not supported by the Sendspin controller protocol
    assert(!parse_command(R"({"command":"seek","position":5})").valid);
    // unknown + malformed
    assert(!parse_command(R"({"command":"frobnicate"})").valid);
    assert(!parse_command("not json").valid);
    assert(!parse_command(R"({"nope":1})").valid);
    return 0;
}
