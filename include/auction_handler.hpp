#pragma once

#include <string>
#include <memory>
#include <botcraft/Game/BaseClient.hpp>
#include "cofl_client.hpp"

namespace Botcraft {
    class BehaviourClient;
}

namespace baf {

class AuctionHandler {
public:
    AuctionHandler(std::shared_ptr<Botcraft::BehaviourClient> bot);
    
    void handleAuctionFlip(const AuctionFlip& flip);
    
private:
    bool shouldSkipConfirmation(const AuctionFlip& flip);
    bool buyAuctionItem(const std::string& uuid, bool skip_confirmation);
    
    std::shared_ptr<Botcraft::BehaviourClient> bot;
    std::atomic<bool> is_busy{false};
};

} // namespace baf
