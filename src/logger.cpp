#include "logger.hpp"
#include "utils.hpp"
#include <iostream>
#include <iomanip>

namespace baf {

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    if (log_file.is_open()) {
        log_file.close();
    }
}

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(log_mutex);
    
    if (log_file.is_open()) {
        log_file.close();
    }
    
    log_file.open(filename, std::ios::app);
    if (!log_file.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::removeMinecraftColorCodes(const std::string& text) {
    return baf::removeMinecraftColorCodes(text);
}

void Logger::log(const std::string& message, LogLevel level) {
    if (level < min_log_level) return;
    
    std::lock_guard<std::mutex> lock(log_mutex);
    
    std::string timestamp = getCurrentTimestamp();
    std::string level_str = getLevelString(level);
    std::string clean_message = removeMinecraftColorCodes(message);
    
    std::string log_line = "[" + timestamp + "] [" + level_str + "] " + clean_message;
    
    // Print to console
    std::cout << log_line << std::endl;
    
    // Write to file
    if (log_file.is_open()) {
        log_file << log_line << std::endl;
        log_file.flush();
    }
}

void Logger::debug(const std::string& message) {
    log(message, LogLevel::DEBUG);
}

void Logger::info(const std::string& message) {
    log(message, LogLevel::INFO);
}

void Logger::warn(const std::string& message) {
    log(message, LogLevel::WARN);
}

void Logger::error(const std::string& message) {
    log(message, LogLevel::ERROR);
}

void Logger::printMcChat(const std::string& message) {
    std::string clean = removeMinecraftColorCodes(message);
    
    std::lock_guard<std::mutex> lock(log_mutex);
    
    // Print colored message to console (with codes)
    std::cout << message << std::endl;
    
    // Write clean message to log file
    if (log_file.is_open()) {
        log_file << "[" << getCurrentTimestamp() << "] [CHAT] " << clean << std::endl;
        log_file.flush();
    }
}

} // namespace baf
