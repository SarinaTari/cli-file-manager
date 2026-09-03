#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct HistoryEntry {
    std::string operation;
    std::string source;
    std::string destination;
};

class HistoryManager {
private:
    std::vector<HistoryEntry> entries;

    std::size_t maximum_entries = 100;

public:
    explicit HistoryManager(
        std::size_t max_entries = 100
    );

    void record(
        const std::string& operation,
        const std::string& source = "",
        const std::string& destination = ""
    );

    void show() const;

    void clear();

    bool empty() const;

    std::size_t size() const;

    const std::vector<HistoryEntry>& get_entries() const;
};