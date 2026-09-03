#include "HistoryManager.h"

#include <iostream>

HistoryManager::HistoryManager(
    std::size_t max_entries
)
    : maximum_entries(max_entries) {
}

void HistoryManager::record(
    const std::string& operation,
    const std::string& source,
    const std::string& destination
) {
    entries.push_back({
        operation,
        source,
        destination
    });

    if (entries.size() > maximum_entries) {
        entries.erase(
            entries.begin()
        );
    }
}

void HistoryManager::show() const {
    if (entries.empty()) {
        std::cout
            << "No history entries.\n";

        return;
    }

    std::cout
        << "Operation history:\n\n";

    std::size_t number = 1;

    for (const HistoryEntry& entry : entries) {
        std::cout
            << number
            << ". "
            << entry.operation;

        if (!entry.source.empty()) {
            std::cout
                << " "
                << entry.source;
        }

        if (!entry.destination.empty()) {
            std::cout
                << " -> "
                << entry.destination;
        }

        std::cout << "\n";

        ++number;
    }
}

void HistoryManager::clear() {
    entries.clear();
}

bool HistoryManager::empty() const {
    return entries.empty();
}

std::size_t HistoryManager::size() const {
    return entries.size();
}

const std::vector<HistoryEntry>&
HistoryManager::get_entries() const {
    return entries;
}