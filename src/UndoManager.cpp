#include "UndoManager.h"

#include <fstream>
#include <iostream>
#include <stdexcept>

UndoManager::UndoManager(
    HistoryManager& history_manager
)
    : history(history_manager) {
}

void UndoManager::undo(
    const fs::path& current_directory
) {
    if (history.empty()) {
        std::cout
            << "Nothing to undo.\n";

        return;
    }

    const std::vector<HistoryEntry>& entries =
        history.get_entries();

    const HistoryEntry& entry =
        entries.back();

    std::error_code ec;

    if (entry.operation == "create_file") {
        fs::path path =
            current_directory / entry.source;

        if (fs::exists(path, ec)) {
            fs::remove(
                path,
                ec
            );

            if (ec) {
                throw std::runtime_error(
                    "Unable to undo file creation: "
                    + ec.message()
                );
            }
        }

        std::cout
            << "Undone: file creation\n";
    }

    else if (entry.operation == "create_directory") {
        fs::path path =
            current_directory / entry.source;

        if (fs::exists(path, ec)) {
            fs::remove(
                path,
                ec
            );

            if (ec) {
                throw std::runtime_error(
                    "Unable to undo directory creation: "
                    + ec.message()
                );
            }
        }

        std::cout
            << "Undone: directory creation\n";
    }

    else if (entry.operation == "rename") {
        fs::path source =
            current_directory / entry.source;

        fs::path destination =
            current_directory / entry.destination;

        if (fs::exists(destination, ec)) {
            fs::rename(
                destination,
                source,
                ec
            );

            if (ec) {
                throw std::runtime_error(
                    "Unable to undo rename: "
                    + ec.message()
                );
            }
        }

        std::cout
            << "Undone: rename\n";
    }

    else if (entry.operation == "move") {
        fs::path source =
            current_directory / entry.source;

        fs::path destination =
            current_directory / entry.destination;

        if (fs::exists(destination, ec)) {
            fs::rename(
                destination,
                source,
                ec
            );

            if (ec) {
                throw std::runtime_error(
                    "Unable to undo move: "
                    + ec.message()
                );
            }
        }

        std::cout
            << "Undone: move\n";
    }

    else {
        std::cout
            << "This operation cannot currently be undone.\n";

        return;
    }

    /*
     * We deliberately do not call undo() recursively.
     *
     * Removing the last history entry prevents the
     * same operation from being undone repeatedly.
     */
    HistoryManager new_history;

    for (
        std::size_t i = 0;
        i + 1 < entries.size();
        ++i
    ) {
        new_history.record(
            entries[i].operation,
            entries[i].source,
            entries[i].destination
        );
    }

    history.clear();

    for (
        const HistoryEntry& old_entry :
        new_history.get_entries()
    ) {
        history.record(
            old_entry.operation,
            old_entry.source,
            old_entry.destination
        );
    }
}