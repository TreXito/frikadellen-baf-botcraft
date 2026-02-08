#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace baf {

class Webhook {
public:
    static void sendInitialized(const std::string& webhook_url, const std::string& username);
    static void sendBazaarOrderPlaced(const std::string& webhook_url, const std::string& item_name, 
                                     int amount, double price_per_unit, double total_price, bool is_buy_order);
    static void sendAuctionPurchased(const std::string& webhook_url, const std::string& item_name,
                                    int64_t price, int64_t profit, const std::string& uuid);
    static void sendError(const std::string& webhook_url, const std::string& error_message);
    
private:
    static bool sendWebhook(const std::string& webhook_url, const nlohmann::json& payload);
};

} // namespace baf
