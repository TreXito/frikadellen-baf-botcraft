#include <iostream>
#include <memory>
#include <string>
#include <csignal>
#include <atomic>

// Note: In production, uncomment these Botcraft includes
// #include <botcraft/Game/BehaviourClient.hpp>
// #include <botcraft/Game/ConnectionClient.hpp>
// #include <botcraft/Game/AssetsManager.hpp>
// #include <botcraft/Network/Authentifier.hpp>

#include "config.hpp"
#include "logger.hpp"
#include "cofl_client.hpp"
#include "bazaar_handler.hpp"
#include "auction_handler.hpp"
#include "webhook.hpp"

using namespace baf;

std::atomic<bool> should_run(true);

void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down..." << std::endl;
    should_run.store(false);
}

int main(int argc, char** argv) {
    // Set up signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Initialize logger
    Logger::getInstance().init("baf.log");
    Logger::getInstance().info("Starting Frikadellen BAF v1.0.0 for Minecraft 1.21.11");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §aStarting Frikadellen BAF v1.0.0");
    
    // Load configuration
    auto& config = Config::getInstance();
    if (!config.load("config.toml")) {
        Logger::getInstance().error("Failed to load configuration");
        return 1;
    }
    
    // Prompt for username if not set
    std::string username = config.getIngameName();
    if (username.empty()) {
        std::cout << "Enter your Minecraft username: ";
        std::getline(std::cin, username);
        config.setIngameName(username);
        config.save();
    }
    
    Logger::getInstance().info("Starting BAF for user: " + username);
    Logger::getInstance().info("AH Flips: " + std::string(config.getEnableAHFlips() ? "ENABLED" : "DISABLED"));
    Logger::getInstance().info("Bazaar Flips: " + std::string(config.getEnableBazaarFlips() ? "ENABLED" : "DISABLED"));
    
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Configuration loaded for §e" + username);
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7AH Flips: " + 
                                    std::string(config.getEnableAHFlips() ? "§aENABLED" : "§cDISABLED"));
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Bazaar Flips: " + 
                                    std::string(config.getEnableBazaarFlips() ? "§aENABLED" : "§cDISABLED"));
    
    // NOTE: In production, create actual Botcraft client here
    // For demonstration, we'll simulate the bot
    std::shared_ptr<void> bot_placeholder = nullptr; // Would be BehaviourClient in production
    
    /*
    // Production Botcraft initialization would look like:
    try {
        // Initialize Botcraft
        Botcraft::AssetsManager::getInstance().Init(true, true, true);
        
        // Create authentication
        auto authentifier = std::make_shared<Botcraft::Authentifier>();
        if (!authentifier->AuthMicrosoft()) {
            Logger::getInstance().error("Microsoft authentication failed");
            return 1;
        }
        
        // Create bot client
        auto bot = std::make_shared<Botcraft::BehaviourClient>(true);
        bot->SetAutoRespawn(true);
        
        // Set up proxy if enabled
        const auto& proxy_config = config.getProxyConfig();
        if (proxy_config.enabled) {
            // Configure SOCKS5 proxy
            Logger::getInstance().info("Using proxy: " + proxy_config.host + ":" + 
                                     std::to_string(proxy_config.port));
        }
        
        // Connect to Hypixel
        Logger::getInstance().info("Connecting to mc.hypixel.net...");
        Botcraft::ConnectionState state = bot->Connect("mc.hypixel.net", username, authentifier);
        
        if (state != Botcraft::ConnectionState::Play) {
            Logger::getInstance().error("Failed to connect to Hypixel");
            return 1;
        }
        
        Logger::getInstance().info("Connected to Hypixel!");
        Logger::getInstance().printMcChat("§f[§4BAF§f]: §aConnected to Hypixel!");
        
        // Join SkyBlock
        sleep(2000);
        bot->SendChatMessage("/play sb");
        
        // Wait for SkyBlock join
        sleep(5000);
        
        // Teleport to island
        Logger::getInstance().info("Teleporting to island...");
        bot->SendChatMessage("/is");
        sleep(2000);
        
        Logger::getInstance().printMcChat("§f[§4BAF§f]: §aReady to flip!");
        
    } catch (const std::exception& e) {
        Logger::getInstance().error("Botcraft initialization failed: " + std::string(e.what()));
        return 1;
    }
    */
    
    Logger::getInstance().info("Bot would connect to Hypixel here (Botcraft integration needed)");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §6[Demo Mode] Bot connection simulated");
    
    // Create handlers (pass nullptr for demo, would pass actual bot in production)
    auto bazaar_handler = std::make_shared<BazaarHandler>(nullptr);
    auto auction_handler = std::make_shared<AuctionHandler>(nullptr);
    
    // Create Cofl client
    CoflClient cofl_client(config.getWebsocketUrl(), username, "af-2.0.0-cpp");
    
    // Set up message callback
    cofl_client.setMessageCallback([&](const std::string& type, const json& data) {
        Logger::getInstance().debug("[WebSocket] Message type: " + type);
        
        try {
            if (type == "flip") {
                if (config.getEnableAHFlips()) {
                    AuctionFlip flip = parseAuctionFlip(data);
                    auction_handler->handleAuctionFlip(flip);
                }
            }
            else if (type == "bazaarFlip" || type == "bzRecommend" || type == "placeOrder") {
                if (config.getEnableBazaarFlips()) {
                    BazaarFlipRecommendation recommendation = parseBazaarFlipJson(data);
                    bazaar_handler->handleBazaarFlipRecommendation(recommendation);
                }
            }
            else if (type == "chatMessage" || type == "writeToChat") {
                // Handle chat messages
                if (data.is_array()) {
                    for (const auto& msg : data) {
                        if (msg.contains("text")) {
                            std::string text = msg["text"];
                            if (config.getUseCoflChat()) {
                                Logger::getInstance().printMcChat(text);
                            }
                        }
                    }
                } else if (data.contains("text")) {
                    std::string text = data["text"];
                    if (config.getUseCoflChat()) {
                        Logger::getInstance().printMcChat(text);
                    }
                }
            }
            else if (type == "getInventory") {
                // Handle inventory request
                Logger::getInstance().info("Inventory request received");
            }
            else if (type == "privacySettings") {
                Logger::getInstance().info("Privacy settings updated");
            }
            else {
                Logger::getInstance().warn("Unknown message type: " + type);
            }
        } catch (const std::exception& e) {
            Logger::getInstance().error("Error handling message: " + std::string(e.what()));
        }
    });
    
    // Connect to Cofl
    Logger::getInstance().info("Connecting to Cofl websocket...");
    if (!cofl_client.connect()) {
        Logger::getInstance().error("Failed to connect to Cofl");
        return 1;
    }
    
    Logger::getInstance().info("Connected to Cofl!");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §aConnected to Cofl!");
    
    // Send webhook notification
    if (!config.getWebhookUrl().empty()) {
        Webhook::sendInitialized(config.getWebhookUrl(), username);
    }
    
    // Main loop
    Logger::getInstance().info("Bot is now running. Press Ctrl+C to stop.");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §aBot is now running!");
    
    while (should_run.load()) {
        // In production, this would handle bot ticks and game events
        // For now, just sleep
        sleep(100);
        
        // Check if websocket is still connected
        if (!cofl_client.isConnected()) {
            Logger::getInstance().warn("Cofl connection lost, reconnecting...");
            cofl_client.connect();
        }
    }
    
    // Cleanup
    Logger::getInstance().info("Shutting down...");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §cShutting down...");
    
    cofl_client.disconnect();
    
    // In production, disconnect bot here
    // bot->Disconnect();
    
    Logger::getInstance().info("Goodbye!");
    Logger::getInstance().printMcChat("§f[§4BAF§f]: §7Goodbye!");
    
    return 0;
}
