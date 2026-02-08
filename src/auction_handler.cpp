#include "auction_handler.hpp"
#include "logger.hpp"
#include "config.hpp"
#include "webhook.hpp"
#include "utils.hpp"
#include <botcraft/Game/AssetsManager.hpp>
#include <botcraft/Game/Inventory/InventoryManager.hpp>

namespace baf {

AuctionHandler::AuctionHandler(std::shared_ptr<Botcraft::BehaviourClient> bot)
    : bot(bot) {
}

void AuctionHandler::handleAuctionFlip(const AuctionFlip& flip) {
    auto& config = Config::getInstance();
    
    // Check if AH flips are enabled
    if (!config.getEnableAHFlips()) {
        Logger::getInstance().warn("Auction flips are disabled in config");
        return;
    }
    
    // Check if busy
    if (is_busy.exchange(true)) {
        Logger::getInstance().warn("Handler is busy, will retry");
        sleep(1000);
        handleAuctionFlip(flip);
        return;
    }
    
    Logger::getInstance().info("===== AUCTION FLIP =====");
    Logger::getInstance().info("Item: " + flip.item_name);
    Logger::getInstance().info("UUID: " + flip.uuid);
    Logger::getInstance().info("Starting Bid: " + std::to_string(flip.starting_bid) + " coins");
    Logger::getInstance().info("Profit: " + std::to_string(flip.profit) + " coins");
    Logger::getInstance().info("Finder: " + flip.finder);
    
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §eAUCTION FLIP §7- §e" + flip.item_name);
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Price: §6" + std::to_string(flip.starting_bid) + " coins");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Profit: §a+" + std::to_string(flip.profit) + " coins");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    
    try {
        bool skip = shouldSkipConfirmation(flip);
        bool success = buyAuctionItem(flip.uuid, skip);
        
        if (success) {
            Logger::getInstance().info("===== AUCTION PURCHASED =====");
            Logger::getInstance().printMcChat("§f[§4BAF§f]: §aSuccessfully purchased auction!");
            
            // Send webhook notification
            Webhook::sendAuctionPurchased(config.getWebhookUrl(), flip.item_name,
                                        flip.starting_bid, flip.profit, flip.uuid);
        } else {
            Logger::getInstance().error("Failed to purchase auction");
            Logger::getInstance().printMcChat("§f[§4BAF§f]: §cFailed to purchase auction");
        }
    } catch (const std::exception& e) {
        Logger::getInstance().error("Exception: " + std::string(e.what()));
        Logger::getInstance().printMcChat("§f[§4BAF§f]: §cError: " + std::string(e.what()));
    }
    
    is_busy.store(false);
}

bool AuctionHandler::shouldSkipConfirmation(const AuctionFlip& flip) {
    auto& config = Config::getInstance();
    const auto& skip_config = config.getSkipConfig();
    
    // Check ALWAYS skip
    if (skip_config.always) {
        Logger::getInstance().info("Skip: ALWAYS enabled");
        return true;
    }
    
    // Check minimum profit
    if (flip.profit >= skip_config.min_profit) {
        Logger::getInstance().info("Skip: Profit " + std::to_string(flip.profit) + 
                                 " >= " + std::to_string(skip_config.min_profit));
        return true;
    }
    
    // Check profit percentage
    if (flip.profit_percentage >= skip_config.profit_percentage) {
        Logger::getInstance().info("Skip: Profit % " + std::to_string(flip.profit_percentage) + 
                                 " >= " + std::to_string(skip_config.profit_percentage));
        return true;
    }
    
    // Check minimum price
    if (flip.starting_bid >= skip_config.min_price) {
        Logger::getInstance().info("Skip: Price " + std::to_string(flip.starting_bid) + 
                                 " >= " + std::to_string(skip_config.min_price));
        return true;
    }
    
    // Check USER finder
    if (skip_config.user_finder && flip.finder == "USER") {
        Logger::getInstance().info("Skip: USER finder");
        return true;
    }
    
    // Check skins
    if (skip_config.skins && flip.is_skin) {
        Logger::getInstance().info("Skip: Is skin");
        return true;
    }
    
    return false;
}

bool AuctionHandler::buyAuctionItem(const std::string& uuid, bool skip_confirmation) {
    Logger::getInstance().info("Buying auction: " + uuid + (skip_confirmation ? " (SKIP)" : ""));
    
    // In a full Botcraft implementation, you would:
    // 1. Send command: /ah or navigate to AH NPC
    // 2. Search for item by UUID
    // 3. Click on the item
    // 4. If skip_confirmation:
    //    - Click green checkmark directly (fast path)
    // 5. Else:
    //    - Click "Confirm Purchase"
    // 6. Wait for purchase confirmation
    
    // Example pseudocode structure:
    /*
    auto inventory_manager = bot->GetInventoryManager();
    
    // Open auction house
    bot->SendChatMessage("/ah");
    sleep(300);
    
    // Navigate to search and enter UUID
    // This would require window interaction
    
    // Click buy button (with or without confirmation skip)
    if (skip_confirmation) {
        // Click slot for instant purchase
    } else {
        // Click confirm button
    }
    */
    
    // For this demonstration, log what would happen
    Logger::getInstance().info("Would execute: /ah");
    Logger::getInstance().info("Would search for UUID: " + uuid);
    Logger::getInstance().info("Would click: " + std::string(skip_confirmation ? "Direct purchase" : "Confirm purchase"));
    
    sleep(Config::getInstance().getFlipActionDelay());
    
    return true;
}

} // namespace baf
