#pragma once

#include <string>
#include <fstream>
#include <mutex>
#include <sstream>

namespace baf {

enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR
};

class Logger {
public:
    static Logger& getInstance();
    
    void init(const std::string& filename = "baf.log");
    void log(const std::string& message, LogLevel level = LogLevel::INFO);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    
    // Print to both console and log file with Minecraft color codes
    void printMcChat(const std::string& message);
    
    void setLogLevel(LogLevel level) { min_log_level = level; }
    
private:
    Logger() = default;
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    
    std::ofstream log_file;
    std::mutex log_mutex;
    LogLevel min_log_level = LogLevel::INFO;
    
    std::string getLevelString(LogLevel level);
    std::string getCurrentTimestamp();
    std::string removeMinecraftColorCodes(const std::string& text);
};

} // namespace baf
