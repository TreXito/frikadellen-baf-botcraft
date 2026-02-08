#pragma once

#include <string>
#include <functional>
#include <memory>
#include <atomic>
#include <thread>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace baf {

struct BazaarFlipRecommendation {
    std::string item_name;
    std::string item_tag;
    int amount;
    double price_per_unit;
    double total_price;
    bool is_buy_order;
};

struct AuctionFlip {
    std::string uuid;
    std::string item_name;
    int64_t starting_bid;
    int64_t profit;
    std::string finder;
    bool is_bin;
    bool is_skin;
    int profit_percentage;
};

class CoflClient {
public:
    using MessageCallback = std::function<void(const std::string& type, const json& data)>;
    
    CoflClient(const std::string& websocket_url, const std::string& player_name, const std::string& version);
    ~CoflClient();
    
    bool connect();
    void disconnect();
    bool isConnected() const { return connected.load(); }
    
    void setMessageCallback(MessageCallback callback) { message_callback = callback; }
    
    void sendMessage(const std::string& type, const json& data);
    
private:
    void runWebsocket();
    void handleMessage(const std::string& message);
    
    std::string websocket_url;
    std::string player_name;
    std::string version;
    
    MessageCallback message_callback;
    
    std::atomic<bool> connected{false};
    std::atomic<bool> should_run{true};
    std::unique_ptr<std::thread> websocket_thread;
};

// Helper functions to parse messages
BazaarFlipRecommendation parseBazaarFlipJson(const json& data);
AuctionFlip parseAuctionFlip(const json& data);

} // namespace baf
