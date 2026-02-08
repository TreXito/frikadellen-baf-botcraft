#include "config.hpp"
#include "logger.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace baf {

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().warn("Config file not found: " + filename + ", creating default");
        return save(filename);
    }
    
    // Simple TOML parser (for production, use a library like toml11 or cpptoml)
    std::string line;
    std::string current_section;
    
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        line = line.substr(start);
        
        // Check for section header
        if (line[0] == '[') {
            size_t end = line.find(']');
            if (end != std::string::npos) {
                current_section = line.substr(1, end - 1);
            }
            continue;
        }
        
        // Parse key = value
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;
        
        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);
        
        // Trim key and value
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        // Remove quotes from string values
        if (!value.empty() && value[0] == '"') {
            value = value.substr(1, value.length() - 2);
        }
        
        // Parse based on current section
        if (current_section == "SKIP") {
            if (key == "ALWAYS") skip_config.always = (value == "true");
            else if (key == "MIN_PROFIT") skip_config.min_profit = std::stoll(value);
            else if (key == "USER_FINDER") skip_config.user_finder = (value == "true");
            else if (key == "SKINS") skip_config.skins = (value == "true");
            else if (key == "PROFIT_PERCENTAGE") skip_config.profit_percentage = std::stoi(value);
            else if (key == "MIN_PRICE") skip_config.min_price = std::stoll(value);
        } else {
            // Main section
            if (key == "INGAME_NAME") ingame_name = value;
            else if (key == "WEBHOOK_URL") webhook_url = value;
            else if (key == "WEBSOCKET_URL") websocket_url = value;
            else if (key == "FLIP_ACTION_DELAY") flip_action_delay = std::stoi(value);
            else if (key == "ENABLE_AH_FLIPS") enable_ah_flips = (value == "true");
            else if (key == "ENABLE_BAZAAR_FLIPS") enable_bazaar_flips = (value == "true");
            else if (key == "USE_COFL_CHAT") use_cofl_chat = (value == "true");
            else if (key == "AUCTION_DURATION_HOURS") auction_duration_hours = std::stoi(value);
            else if (key == "AUTO_COOKIE") auto_cookie = std::stoi(value);
            else if (key == "BED_MULTIPLE_CLICKS_DELAY") bed_multiple_clicks_delay = std::stoi(value);
            else if (key == "BED_SPAM") bed_spam = (value == "true");
            else if (key == "BED_SPAM_CLICK_DELAY") bed_spam_click_delay = std::stoi(value);
            else if (key == "PROXY_ENABLED") proxy_config.enabled = (value == "true");
            else if (key == "PROXY") {
                size_t colon = value.find(':');
                if (colon != std::string::npos) {
                    proxy_config.host = value.substr(0, colon);
                    proxy_config.port = std::stoi(value.substr(colon + 1));
                }
            }
            else if (key == "PROXY_USERNAME") proxy_config.username = value;
            else if (key == "PROXY_PASSWORD") proxy_config.password = value;
            else if (key == "ACCOUNTS") accounts = value;
            else if (key == "AUTO_SWITCHING") auto_switching = value;
        }
    }
    
    file.close();
    Logger::getInstance().info("Configuration loaded from " + filename);
    return true;
}

bool Config::save(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().error("Failed to create config file: " + filename);
        return false;
    }
    
    file << "# Frikadellen BAF Configuration File for Minecraft 1.21.11 + Botcraft\n\n";
    file << "INGAME_NAME = \"" << ingame_name << "\"\n";
    file << "WEBHOOK_URL = \"" << webhook_url << "\"\n";
    file << "WEBSOCKET_URL = \"" << websocket_url << "\"\n";
    file << "FLIP_ACTION_DELAY = " << flip_action_delay << "\n";
    file << "ENABLE_AH_FLIPS = " << (enable_ah_flips ? "true" : "false") << "\n";
    file << "ENABLE_BAZAAR_FLIPS = " << (enable_bazaar_flips ? "true" : "false") << "\n";
    file << "USE_COFL_CHAT = " << (use_cofl_chat ? "true" : "false") << "\n";
    file << "AUCTION_DURATION_HOURS = " << auction_duration_hours << "\n";
    file << "AUTO_COOKIE = " << auto_cookie << "\n";
    file << "BED_MULTIPLE_CLICKS_DELAY = " << bed_multiple_clicks_delay << "\n";
    file << "BED_SPAM = " << (bed_spam ? "true" : "false") << "\n";
    file << "BED_SPAM_CLICK_DELAY = " << bed_spam_click_delay << "\n";
    
    file << "\n[SKIP]\n";
    file << "ALWAYS = " << (skip_config.always ? "true" : "false") << "\n";
    file << "MIN_PROFIT = " << skip_config.min_profit << "\n";
    file << "USER_FINDER = " << (skip_config.user_finder ? "true" : "false") << "\n";
    file << "SKINS = " << (skip_config.skins ? "true" : "false") << "\n";
    file << "PROFIT_PERCENTAGE = " << skip_config.profit_percentage << "\n";
    file << "MIN_PRICE = " << skip_config.min_price << "\n";
    
    file << "\nPROXY_ENABLED = " << (proxy_config.enabled ? "true" : "false") << "\n";
    if (!proxy_config.host.empty()) {
        file << "PROXY = \"" << proxy_config.host << ":" << proxy_config.port << "\"\n";
    } else {
        file << "PROXY = \"\"\n";
    }
    file << "PROXY_USERNAME = \"" << proxy_config.username << "\"\n";
    file << "PROXY_PASSWORD = \"" << proxy_config.password << "\"\n";
    
    file << "\nACCOUNTS = \"" << accounts << "\"\n";
    file << "AUTO_SWITCHING = \"" << auto_switching << "\"\n";
    
    file.close();
    Logger::getInstance().info("Configuration saved to " + filename);
    return true;
}

void Config::saveSession(const std::string& username, const std::string& token, int64_t expires) {
    sessions[username] = std::make_pair(token, expires);
}

bool Config::loadSession(const std::string& username, std::string& token, int64_t& expires) {
    auto it = sessions.find(username);
    if (it != sessions.end()) {
        token = it->second.first;
        expires = it->second.second;
        return true;
    }
    return false;
}

} // namespace baf
