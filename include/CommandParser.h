#pragma once

#include <string>
#include <vector>

struct Command {
    std::string action;
    std::vector<std::string> arguments;
};

class CommandParser {
public:
    static Command parse(
        const std::string& input
    );

private:
    static std::vector<std::string> tokenize(
        const std::string& input
    );
};