#pragma once
#include <filesystem>
#include <string>
namespace fs=std::filesystem;
class SafetyAnalyzer{public:static void preview_remove(const fs::path&);static bool is_dangerous(const fs::path&,const fs::path&);};
