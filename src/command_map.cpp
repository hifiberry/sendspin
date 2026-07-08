#include "command_map.h"
#include <nlohmann/json.hpp>
using sendspin::SendspinControllerCommand;

ParsedCommand parse_command(const std::string& body) {
    ParsedCommand r;
    nlohmann::json j = nlohmann::json::parse(body, nullptr, /*allow_exceptions=*/false);
    if (!j.is_object() || !j.contains("command") || !j["command"].is_string()) return r;
    std::string c = j["command"].get<std::string>();
    if (c == "play") { r.cmd = SendspinControllerCommand::PLAY; r.valid = true; }
    else if (c == "pause") { r.cmd = SendspinControllerCommand::PAUSE; r.valid = true; }
    else if (c == "stop") { r.cmd = SendspinControllerCommand::STOP; r.valid = true; }
    else if (c == "next") { r.cmd = SendspinControllerCommand::NEXT; r.valid = true; }
    else if (c == "previous") { r.cmd = SendspinControllerCommand::PREVIOUS; r.valid = true; }
    return r;
}
