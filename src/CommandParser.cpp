#include "CommandParser.h"

#include <cctype>

std::vector<std::string> CommandParser::tokenize(
    const std::string& input
) {
    std::vector<std::string> tokens;

    std::string current;
    bool inside_quotes = false;
    char quote_character = '\0';

    for (char character : input) {

        if ((character == '"' || character == '\'') && !inside_quotes) {
            inside_quotes = true;
            quote_character = character;
            continue;
        }

        if (inside_quotes && character == quote_character) {
            inside_quotes = false;
            quote_character = '\0';
            continue;
        }

        if (std::isspace(
                static_cast<unsigned char>(character)
            ) && !inside_quotes) {

            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }

            continue;
        }

        current += character;
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