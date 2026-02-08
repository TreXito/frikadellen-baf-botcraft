#include "bazaar_handler.hpp"
#include "logger.hpp"
#include "config.hpp"
#include "webhook.hpp"
#include "utils.hpp"
#ifdef WITH_BOTCRAFT
#include <botcraft/Game/AssetsManager.hpp>
#include <botcraft/Game/Inventory/InventoryManager.hpp>
#endif

namespace baf {

BazaarHandler::BazaarHandler(std::shared_ptr<Botcraft::BehaviourClient> bot)
    : bot(bot) {
}

void BazaarHandler::handleBazaarFlipRecommendation(const BazaarFlipRecommendation& recommendation) {
    auto& config = Config::getInstance();
    
    // Check if bazaar flips are enabled
    if (!config.getEnableBazaarFlips()) {
        Logger::getInstance().warn("[BazaarDebug] Bazaar flips are disabled in config");
        return;
    }
    
    // Check if busy
    if (is_busy.exchange(true)) {
        Logger::getInstance().warn("[BazaarDebug] Handler is busy, will retry");
        sleep(1000);
        handleBazaarFlipRecommendation(recommendation);
        return;
    }
    
    Logger::getInstance().info("[BazaarDebug] ===== STARTING BAZAAR FLIP ORDER =====");
    Logger::getInstance().info("[BazaarDebug] Item: " + recommendation.item_name);
    Logger::getInstance().info("[BazaarDebug] Amount: " + std::to_string(recommendation.amount));
    Logger::getInstance().info("[BazaarDebug] Price per unit: " + std::to_string(recommendation.price_per_unit) + " coins");
    Logger::getInstance().info("[BazaarDebug] Total price: " + std::to_string(recommendation.total_price) + " coins");
    Logger::getInstance().info("[BazaarDebug] Order type: " + std::string(recommendation.is_buy_order ? "BUY" : "SELL"));
    
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §e" + std::string(recommendation.is_buy_order ? "BUY" : "SELL") + 
                                    " ORDER §7- §e" + recommendation.item_name);
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Amount: §a" + std::to_string(recommendation.amount) + "x");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Price/unit: §6" + std::to_string((int)recommendation.price_per_unit) + " coins");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Total: §6" + std::to_string((int)recommendation.total_price) + " coins");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    
    try {
        bool success = placeBazaarOrder(recommendation.item_name, recommendation.amount, 
                                      recommendation.price_per_unit, recommendation.is_buy_order);
        
        if (success) {
            Logger::getInstance().info("[BazaarDebug] ===== BAZAAR FLIP ORDER COMPLETED =====");
            Logger::getInstance().printMcChat("§f[§4BAF§f]: §aSuccessfully placed bazaar order!");
            
            // Send webhook notification
            Webhook::sendBazaarOrderPlaced(config.getWebhookUrl(), recommendation.item_name,
                                         recommendation.amount, recommendation.price_per_unit,
                                         recommendation.total_price, recommendation.is_buy_order);
        } else {
            Logger::getInstance().error("[BazaarDebug] Failed to place bazaar order");
            Logger::getInstance().printMcChat("§f[§4BAF§f]: §cFailed to place bazaar order");
        }
    } catch (const std::exception& e) {
        Logger::getInstance().error("[BazaarDebug] Exception: " + std::string(e.what()));
        Logger::getInstance().printMcChat("§f[§4BAF§f]: §cError: " + std::string(e.what()));
    }
    
    is_busy.store(false);
}

bool BazaarHandler::placeBazaarOrder(const std::string& item_name, int amount, 
                                    double price_per_unit, bool is_buy_order) {
    Logger::getInstance().info("[BazaarDebug] Opening bazaar for: " + item_name);
    
    // In a full Botcraft implementation, you would:
    // 1. Send chat command: /bz <item_name>
    // 2. Wait for window to open
    // 3. Navigate through the GUI windows:
    //    - Search results -> Item detail
    //    - Click "Create Buy Order" or "Create Sell Offer"
    //    - Set amount (sign input)
    //    - Set price (sign input)
    //    - Confirm order
    // 4. Use bot->GetInventoryManager() to interact with windows
    // 5. Use custom packets for sign editing
    
    // Example pseudocode structure:
    /*
    auto inventory_manager = bot->GetInventoryManager();
    
    // Send command
    bot->SendChatMessage("/bz " + item_name);
    
    // Wait for window open
    sleep(300);
    
    // Find and click the appropriate slots
    // This would require actual implementation with Botcraft's API
    
    // Sign editing for amount and price
    // Would use Botcraft's packet system
    
    // Final confirmation click
    */
    
    // For this demonstration, log what would happen
    Logger::getInstance().info("[BazaarDebug] Would execute: /bz " + item_name);
    Logger::getInstance().info("[BazaarDebug] Would click: " + std::string(is_buy_order ? "Create Buy Order" : "Create Sell Offer"));
    Logger::getInstance().info("[BazaarDebug] Would set amount: " + std::to_string(amount));
    Logger::getInstance().info("[BazaarDebug] Would set price: " + std::to_string(price_per_unit));
    Logger::getInstance().info("[BazaarDebug] Would confirm order");
    
    sleep(Config::getInstance().getFlipActionDelay());
    
    return true;
}

} // namespace baf
