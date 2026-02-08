#pragma once

#include <string>
#include <memory>
#include <botcraft/Game/BaseClient.hpp>
#include "cofl_client.hpp"

namespace Botcraft {
    class BehaviourClient;
}

namespace baf {

class BazaarHandler {
public:
    BazaarHandler(std::shared_ptr<Botcraft::BehaviourClient> bot);
    
    void handleBazaarFlipRecommendation(const BazaarFlipRecommendation& recommendation);
    
private:
    bool placeBazaarOrder(const std::string& item_name, int amount, double price_per_unit, bool is_buy_order);
    
    std::shared_ptr<Botcraft::BehaviourClient> bot;
    std::atomic<bool> is_busy{false};
};

} // namespace baf
