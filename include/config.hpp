#pragma once

#include <string>
#include <map>
#include <memory>

namespace baf {

struct SkipConfig {
    bool always = false;
    int64_t min_profit = 1000000;
    bool user_finder = false;
    bool skins = false;
    int profit_percentage = 50;
    int64_t min_price = 10000000;
};

struct ProxyConfig {
    bool enabled = false;
    std::string host;
    int port = 0;
    std::string username;
    std::string password;
};

class Config {
public:
    static Config& getInstance();
    
    bool load(const std::string& filename = "config.toml");
    bool save(const std::string& filename = "config.toml");
    
    // Getters
    std::string getIngameName() const { return ingame_name; }
    std::string getWebhookUrl() const { return webhook_url; }
    std::string getWebsocketUrl() const { return websocket_url; }
    int getFlipActionDelay() const { return flip_action_delay; }
    bool getEnableAHFlips() const { return enable_ah_flips; }
    bool getEnableBazaarFlips() const { return enable_bazaar_flips; }
    bool getUseCoflChat() const { return use_cofl_chat; }
    int getAuctionDurationHours() const { return auction_duration_hours; }
    int getAutoCookie() const { return auto_cookie; }
    int getBedMultipleClicksDelay() const { return bed_multiple_clicks_delay; }
    bool getBedSpam() const { return bed_spam; }
    int getBedSpamClickDelay() const { return bed_spam_click_delay; }
    const SkipConfig& getSkipConfig() const { return skip_config; }
    const ProxyConfig& getProxyConfig() const { return proxy_config; }
    std::string getAccounts() const { return accounts; }
    std::string getAutoSwitching() const { return auto_switching; }
    
    // Setters
    void setIngameName(const std::string& name) { ingame_name = name; }
    void setWebhookUrl(const std::string& url) { webhook_url = url; }
    void setEnableAHFlips(bool enable) { enable_ah_flips = enable; }
    void setEnableBazaarFlips(bool enable) { enable_bazaar_flips = enable; }
    
    // Session management
    void saveSession(const std::string& username, const std::string& token, int64_t expires);
    bool loadSession(const std::string& username, std::string& token, int64_t& expires);
    
private:
    Config() = default;
    ~Config() = default;
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;
    
    std::string ingame_name;
    std::string webhook_url;
    std::string websocket_url = "wss://sky.coflnet.com/modsocket";
    int flip_action_delay = 100;
    bool enable_ah_flips = true;
    bool enable_bazaar_flips = true;
    bool use_cofl_chat = true;
    int auction_duration_hours = 24;
    int auto_cookie = 24;
    int bed_multiple_clicks_delay = 50;
    bool bed_spam = false;
    int bed_spam_click_delay = 5;
    SkipConfig skip_config;
    ProxyConfig proxy_config;
    std::string accounts;
    std::string auto_switching;
    
    std::map<std::string, std::pair<std::string, int64_t>> sessions;
};

} // namespace baf
