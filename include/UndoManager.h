#pragma once

#include "HistoryManager.h"

#include <filesystem>

namespace fs = std::filesystem;

class UndoManager {
private:
    HistoryManager& history;

public:
    explicit UndoManager(
        HistoryManager& history_manager
    );

    void undo(
        const fs::path& current_directory
    );
};