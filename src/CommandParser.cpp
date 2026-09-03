#include "CommandParser.h"

#include <sstream>

Command CommandParser::parse(const std::string& input) {
    Command command;

    std::stringstream ss(input);

    ss >> command.action;

    std::string argument;

    while (ss >> argument) {
        command.arguments.push_back(argument);
    }

    return command;
}