#pragma once
#include <filesystem>
namespace fs=std::filesystem;
class HealthChecker{public:static void check(const fs::path&);};
