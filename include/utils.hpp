#pragma once

#include <string>
#include <chrono>
#include <thread>

namespace baf {

// String utilities
std::string removeMinecraftColorCodes(const std::string& text);
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string& str, char delimiter);

// Time utilities
int64_t getCurrentTimestamp();
void sleep(int milliseconds);

// Botcraft utilities
std::string getWindowTitle(void* window);
bool clickWindow(void* bot, int slot);

} // namespace baf
