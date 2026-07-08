#pragma once
#include <sendspin/controller_role.h>
#include <string>
struct ParsedCommand { bool valid = false; sendspin::SendspinControllerCommand cmd{}; };
ParsedCommand parse_command(const std::string& json_body);
