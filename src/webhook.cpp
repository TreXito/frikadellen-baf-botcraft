#include "webhook.hpp"
#include "logger.hpp"
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

namespace baf {

bool Webhook::sendWebhook(const std::string& webhook_url, const json& payload) {
    if (webhook_url.empty()) {
        return false;
    }
    
    try {
        // In production, use libcurl or similar to send HTTP POST
        // For now, just log that we would send
        Logger::getInstance().debug("Would send webhook to: " + webhook_url);
        Logger::getInstance().debug("Payload: " + payload.dump(2));
        
        // Placeholder for actual HTTP POST implementation
        // CURL* curl = curl_easy_init();
        // ... setup and send ...
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to send webhook: " + std::string(e.what()));
        return false;
    }
}

void Webhook::sendInitialized(const std::string& webhook_url, const std::string& username) {
    json embed = {
        {"title", "Bot Initialized"},
        {"description", "BAF bot has successfully connected to Hypixel"},
        {"color", 0x00FF00},
        {"fields", json::array({
            {{"name", "Username"}, {"value", username}, {"inline", true}},
            {{"name", "Status"}, {"value", "Online"}, {"inline", true}}
        })},
        {"timestamp", ""}
    };
    
    json payload = {
        {"embeds", json::array({embed})}
    };
    
    sendWebhook(webhook_url, payload);
}

void Webhook::sendBazaarOrderPlaced(const std::string& webhook_url, const std::string& item_name,
                                   int amount, double price_per_unit, double total_price, bool is_buy_order) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0);
    ss << total_price;
    
    json embed = {
        {"title", is_buy_order ? "Bazaar Buy Order Placed" : "Bazaar Sell Offer Placed"},
        {"description", "Successfully placed bazaar order"},
        {"color", is_buy_order ? 0x00FF00 : 0x0000FF},
        {"fields", json::array({
            {{"name", "Item"}, {"value", item_name}, {"inline", false}},
            {{"name", "Amount"}, {"value", std::to_string(amount) + "x"}, {"inline", true}},
            {{"name", "Price/Unit"}, {"value", std::to_string((int)price_per_unit) + " coins"}, {"inline", true}},
            {{"name", "Total"}, {"value", ss.str() + " coins"}, {"inline", true}}
        })},
        {"timestamp", ""}
    };
    
    json payload = {
        {"embeds", json::array({embed})}
    };
    
    sendWebhook(webhook_url, payload);
}

void Webhook::sendAuctionPurchased(const std::string& webhook_url, const std::string& item_name,
                                  int64_t price, int64_t profit, const std::string& uuid) {
    json embed = {
        {"title", "Auction Purchased"},
        {"description", "Successfully purchased auction item"},
        {"color", 0xFFD700},
        {"fields", json::array({
            {{"name", "Item"}, {"value", item_name}, {"inline", false}},
            {{"name", "Price"}, {"value", std::to_string(price) + " coins"}, {"inline", true}},
            {{"name", "Profit"}, {"value", std::to_string(profit) + " coins"}, {"inline", true}},
            {{"name", "UUID"}, {"value", uuid}, {"inline", false}}
        })},
        {"timestamp", ""}
    };
    
    json payload = {
        {"embeds", json::array({embed})}
    };
    
    sendWebhook(webhook_url, payload);
}

void Webhook::sendError(const std::string& webhook_url, const std::string& error_message) {
    json embed = {
        {"title", "Error"},
        {"description", error_message},
        {"color", 0xFF0000},
        {"timestamp", ""}
    };
    
    json payload = {
        {"embeds", json::array({embed})}
    };
    
    sendWebhook(webhook_url, payload);
}

} // namespace baf
