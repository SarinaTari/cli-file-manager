#include "CommandParser.h"

#include <cctype>
#include <stdexcept>

std::vector<std::string> CommandParser::tokenize(
    const std::string& input
) {
    std::vector<std::string> tokens;

    std::string current;
    char quote = '\0';

    for (char character : input) {
        if (character == '"' || character == '\'') {
            if (quote == '\0') {
                quote = character;
            }
            else if (quote == character) {
                quote = '\0';
            }
            else {
                current += character;
            }

            continue;
        }

        if (
            std::isspace(static_cast<unsigned char>(character))
            && quote == '\0'
        ) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        }
        else {
            current += character;
        }
    }

    if (quote != '\0') {
        throw std::invalid_argument(
            "Unmatched quote in command."
        );
    }

    if (!current.empty()) {
        tokens.push_back(current);
    }

    return tokens;
}

Command CommandParser::parse(const std::string& input) {
    Command command;

    std::vector<std::string> tokens = tokenize(input);

    if (tokens.empty()) {
        return command;
    }

    command.action = tokens[0];

    for (std::size_t i = 1; i < tokens.size(); ++i) {
        command.arguments.push_back(tokens[i]);
    }

    return command;
}