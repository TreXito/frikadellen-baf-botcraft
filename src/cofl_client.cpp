#include "cofl_client.hpp"
#include "logger.hpp"
#include "utils.hpp"

namespace baf {

// Placeholder WebSocket implementation - in production, use websocketpp or similar
CoflClient::CoflClient(const std::string& websocket_url, const std::string& player_name, const std::string& version)
    : websocket_url(websocket_url), player_name(player_name), version(version) {
}

CoflClient::~CoflClient() {
    disconnect();
}

bool CoflClient::connect() {
    if (connected.load()) {
        return true;
    }
    
    Logger::getInstance().info("Connecting to Cofl websocket: " + websocket_url);
    
    // In production implementation, use actual WebSocket library
    // For now, simulate connection
    connected.store(true);
    should_run.store(true);
    
    websocket_thread = std::make_unique<std::thread>(&CoflClient::runWebsocket, this);
    
    Logger::getInstance().info("Connected to Cofl websocket");
    return true;
}

void CoflClient::disconnect() {
    if (!connected.load()) {
        return;
    }
    
    Logger::getInstance().info("Disconnecting from Cofl websocket");
    
    should_run.store(false);
    connected.store(false);
    
    if (websocket_thread && websocket_thread->joinable()) {
        websocket_thread->join();
    }
}

void CoflClient::runWebsocket() {
    // WebSocket event loop
    // In production, this would handle actual WebSocket messages
    Logger::getInstance().info("WebSocket thread started");
    
    while (should_run.load()) {
        sleep(100);
        
        // Simulate receiving messages (in production, handle real WebSocket events)
    }
    
    Logger::getInstance().info("WebSocket thread stopped");
}

void CoflClient::handleMessage(const std::string& message) {
    try {
        json j = json::parse(message);
        
        if (!j.contains("type") || !j.contains("data")) {
            Logger::getInstance().warn("Invalid message format");
            return;
        }
        
        std::string type = j["type"];
        json data;
        
        // Parse data field (might be stringified JSON)
        if (j["data"].is_string()) {
            data = json::parse(j["data"].get<std::string>());
        } else {
            data = j["data"];
        }
        
        Logger::getInstance().debug("[WebSocket] Received message type: " + type);
        
        if (message_callback) {
            message_callback(type, data);
        }
    } catch (const std::exception& e) {
        Logger::getInstance().error("Error parsing WebSocket message: " + std::string(e.what()));
    }
}

void CoflClient::sendMessage(const std::string& type, const json& data) {
    if (!connected.load()) {
        Logger::getInstance().warn("Cannot send message: not connected");
        return;
    }
    
    json message = {
        {"type", type},
        {"data", data.dump()}
    };
    
    // In production, send via actual WebSocket
    Logger::getInstance().debug("[WebSocket] Sent message type: " + type);
}

BazaarFlipRecommendation parseBazaarFlipJson(const json& data) {
    BazaarFlipRecommendation recommendation;
    
    try {
        Logger::getInstance().info("[BazaarDebug] Parsing bazaar flip JSON: " + data.dump());
        
        // Extract item name
        if (data.contains("itemName")) {
            recommendation.item_name = data["itemName"];
        } else if (data.contains("item")) {
            recommendation.item_name = data["item"];
        } else if (data.contains("name")) {
            recommendation.item_name = data["name"];
        } else {
            throw std::runtime_error("Missing item name");
        }
        
        // Extract item tag (optional)
        if (data.contains("itemTag")) {
            recommendation.item_tag = data["itemTag"];
        }
        
        // Extract amount
        if (data.contains("amount")) {
            recommendation.amount = data["amount"];
        } else if (data.contains("count")) {
            recommendation.amount = data["count"];
        } else if (data.contains("quantity")) {
            recommendation.amount = data["quantity"];
        } else {
            throw std::runtime_error("Missing amount");
        }
        
        // Extract price
        if (data.contains("pricePerUnit")) {
            recommendation.price_per_unit = data["pricePerUnit"];
        } else if (data.contains("unitPrice")) {
            recommendation.price_per_unit = data["unitPrice"];
        } else if (data.contains("price")) {
            // Coflnet sends per-piece price in "price" field
            recommendation.price_per_unit = data["price"];
        } else {
            throw std::runtime_error("Missing price");
        }
        
        // Calculate total price
        if (data.contains("totalPrice")) {
            recommendation.total_price = data["totalPrice"];
        } else {
            recommendation.total_price = recommendation.price_per_unit * recommendation.amount;
        }
        
        // Determine order type
        if (data.contains("isBuyOrder")) {
            recommendation.is_buy_order = data["isBuyOrder"];
        } else if (data.contains("isSell")) {
            recommendation.is_buy_order = !data["isSell"].get<bool>();
        } else if (data.contains("type")) {
            std::string type = data["type"];
            recommendation.is_buy_order = (type == "buy" || type == "BUY");
        } else {
            recommendation.is_buy_order = true; // Default to buy order
        }
        
        Logger::getInstance().info("[BazaarDebug] Successfully parsed: " + 
            std::to_string(recommendation.amount) + "x " + recommendation.item_name + 
            " @ " + std::to_string(recommendation.price_per_unit) + " coins (" +
            (recommendation.is_buy_order ? "BUY" : "SELL") + ")");
        
        return recommendation;
    } catch (const std::exception& e) {
        Logger::getInstance().error("[BazaarDebug] Failed to parse bazaar flip: " + std::string(e.what()));
        throw;
    }
}

AuctionFlip parseAuctionFlip(const json& data) {
    AuctionFlip flip;
    
    try {
        flip.uuid = data.value("uuid", "");
        flip.item_name = data.value("itemName", data.value("item", ""));
        flip.starting_bid = data.value("startingBid", data.value("price", 0));
        flip.profit = data.value("profit", 0);
        flip.finder = data.value("finder", "");
        flip.is_bin = data.value("bin", true);
        flip.is_skin = data.value("skin", false);
        flip.profit_percentage = data.value("profitPercentage", 0);
        
        Logger::getInstance().info("Parsed auction flip: " + flip.item_name + 
            " UUID: " + flip.uuid + " Profit: " + std::to_string(flip.profit));
        
        return flip;
    } catch (const std::exception& e) {
        Logger::getInstance().error("Failed to parse auction flip: " + std::string(e.what()));
        throw;
    }
}

} // namespace baf
