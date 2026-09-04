#pragma once
#include <filesystem>
namespace fs=std::filesystem;
class FileExplainer{public:static void explain(const fs::path&);};
