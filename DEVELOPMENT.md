# Development Notes

## Architecture

### Overview

Frikadellen BAF is structured as a modular C++ application with clear separation of concerns:

```
┌─────────────┐
│    main     │  Entry point, initialization
└──────┬──────┘
       │
       ├─────► Config      (TOML configuration)
       ├─────► Logger      (Logging system)
       ├─────► CoflClient  (WebSocket to Coflnet)
       ├─────► BazaarHandler (Bazaar flip logic)
       ├─────► AuctionHandler (AH flip logic)
       └─────► Webhook     (Discord notifications)
```

### Components

#### 1. Config (`config.hpp`, `config.cpp`)

- Singleton pattern
- TOML configuration file parsing
- Runtime configuration updates
- Session management for authentication

**Key Features**:
- Skip configuration for smart flip filtering
- Proxy support (SOCKS5)
- Account switching timers
- Persistent storage

#### 2. Logger (`logger.hpp`, `logger.cpp`)

- Thread-safe logging
- Multiple log levels (DEBUG, INFO, WARN, ERROR)
- Console + file output
- Minecraft color code handling

**Usage**:
```cpp
Logger::getInstance().info("Message");
Logger::getInstance().printMcChat("§aColored message");
```

#### 3. CoflClient (`cofl_client.hpp`, `cofl_client.cpp`)

- WebSocket client for sky.coflnet.com
- Message parsing (JSON)
- Callback-based event system
- Auto-reconnection

**Message Types**:
- `flip` - Auction house flips
- `bazaarFlip`, `bzRecommend`, `placeOrder` - Bazaar flips
- `chatMessage`, `writeToChat` - In-game chat
- `privacySettings` - Privacy configuration

#### 4. BazaarHandler (`bazaar_handler.hpp`, `bazaar_handler.cpp`)

Handles bazaar flip recommendations:

**Flow**:
1. Receive recommendation from Cofl
2. Check if enabled and not busy
3. Open bazaar GUI (`/bz <item>`)
4. Navigate menus:
   - Search results → Item detail
   - Click "Create Buy Order" or "Create Sell Offer"
   - Enter amount (sign)
   - Enter price (sign)
   - Confirm
5. Send webhook notification

**Features**:
- Atomic busy state
- Automatic retry on failure
- Detailed debug logging

#### 5. AuctionHandler (`auction_handler.hpp`, `auction_handler.cpp`)

Handles auction house flips:

**Flow**:
1. Receive flip from Cofl
2. Check skip criteria
3. Open auction house
4. Search by UUID
5. Purchase (with or without confirmation)
6. Send webhook notification

**Skip Criteria**:
- Always skip (if enabled)
- Minimum profit threshold
- Profit percentage
- Minimum price
- USER finder
- Skin items

#### 6. Webhook (`webhook.hpp`, `webhook.cpp`)

Discord webhook integration:

**Event Types**:
- Bot initialized
- Bazaar order placed
- Auction purchased
- Error notifications

**Format**: Discord embeds with colors and fields

### Botcraft Integration

The full Botcraft integration (when enabled) provides:

#### Connection
```cpp
auto bot = std::make_shared<Botcraft::BehaviourClient>(true);
bot->Connect("mc.hypixel.net", username, authentifier);
```

#### Chat
```cpp
bot->SendChatMessage("/play sb");
bot->SendChatMessage("/is");
```

#### Inventory Management
```cpp
auto inventory_manager = bot->GetInventoryManager();
auto window = inventory_manager->GetWindow(window_id);
```

#### Window Interaction
```cpp
// Click slot in window
inventory_manager->ClickSlot(slot_index);

// Sign editing (custom packet)
bot->SendCustomPacket(...);
```

### Protocol Details

#### Bazaar GUI Navigation (Minecraft 1.21.11)

**Step 1: Search Results**
- Window title: "Bazaar ➜ Search"
- Find item by name in slots
- Click item slot to open detail view

**Step 2: Item Detail**
- Window title: "Bazaar ➜ <ItemName>"
- Slot 15: "Create Buy Order" (green wool)
- Slot 16: "Create Sell Offer" (red wool)
- Click appropriate slot

**Step 3: Amount Selection** (buy orders only)
- Window title: "How many do you want..."
- Slot for "Custom Amount" - opens sign
- Write amount to sign line 1

**Step 4: Price Selection**
- Window title: "How much do you want..."
- Slot for "Custom Price" - opens sign
- Write price to sign line 1

**Step 5: Confirmation**
- Window title: "Confirm..."
- Slot 13: Confirm button
- Click to complete order

#### Sign Editing Protocol

```cpp
// open_sign_entity packet received
bot->_client->write("update_sign", {
    "location": {x, y, z},
    "text1": "\"<value>\"",  // Quoted string
    "text2": "{\"italic\":false,\"extra\":[\"^^^^^^^^^^^^^^^\"],\"text\":\"\"}",
    "text3": "{\"italic\":false,\"extra\":[\"\"],\"text\":\"\"}",
    "text4": "{\"italic\":false,\"extra\":[\"\"],\"text\":\"\"}"
});
```

**Note**: Line 1 contains the value, lines 2-4 are visual markers.

### Data Structures

#### BazaarFlipRecommendation
```cpp
struct BazaarFlipRecommendation {
    std::string item_name;      // Display name
    std::string item_tag;       // Internal ID (optional)
    int amount;                 // Quantity
    double price_per_unit;      // Price per piece
    double total_price;         // Total order cost
    bool is_buy_order;          // true = buy, false = sell
};
```

#### AuctionFlip
```cpp
struct AuctionFlip {
    std::string uuid;           // Auction UUID
    std::string item_name;      // Item display name
    int64_t starting_bid;       // BIN price
    int64_t profit;             // Expected profit
    std::string finder;         // "USER" or "FLIPPER"
    bool is_bin;                // Buy It Now
    bool is_skin;               // Is skin item
    int profit_percentage;      // Profit %
};
```

### Threading Model

- **Main Thread**: Bot game loop, window handling
- **WebSocket Thread**: Cofl message reception
- **Handler Threads**: Flip execution (atomic busy state prevents conflicts)

**Thread Safety**:
- Config: Read-only after init (thread-safe)
- Logger: Mutex-protected writes
- Handlers: Atomic busy flags
- CoflClient: Message queue (thread-safe)

### Error Handling

**Strategy**:
1. Catch exceptions at handler level
2. Log errors with context
3. Send webhook notification
4. Reset state for retry
5. Never crash the bot

**Example**:
```cpp
try {
    // Risky operation
} catch (const std::exception& e) {
    Logger::getInstance().error("Operation failed: " + std::string(e.what()));
    Webhook::sendError(config.getWebhookUrl(), e.what());
    is_busy.store(false);  // Reset state
}
```

## Minecraft Version Compatibility

### 1.21.11 Specifics

- **Protocol Version**: 767
- **Java Version**: 21+
- **Changes from 1.8.9**:
  - Different packet IDs
  - New chunk format
  - Updated NBT structure
  - Changed window IDs
  - New authentication (Microsoft)

### Hypixel Specifics

- Server version: 1.8.9 with 1.21 support
- Custom plugins (SkyBlock)
- Anti-cheat detection
- Rate limiting on actions
- Auction house UUID-based search

## Performance Considerations

### Optimization Targets

1. **Window Navigation**: < 300ms per step
2. **Flip Execution**: < 2 seconds total
3. **Memory Usage**: < 200MB
4. **CPU Usage**: < 5% (single core)

### Bottlenecks

- Network latency (Hypixel → client)
- Window open delays (server-side)
- Sign packet processing
- WebSocket message parsing

### Improvements

1. **Packet caching**: Reuse common packets
2. **Prediction**: Start next action before confirmation
3. **Parallel processing**: Multiple accounts
4. **Smart delays**: Ping-based timing

## Testing

### Unit Tests

Not yet implemented. Would test:
- Config parsing
- Message parsing
- Skip criteria logic
- Price calculations

### Integration Tests

Manual testing required:
1. Connect to Hypixel
2. Join SkyBlock
3. Receive Cofl flip
4. Execute bazaar order
5. Execute AH purchase
6. Verify webhooks

### Test Server

Not applicable - Hypixel testing only.

## Future Enhancements

### Priority 1 (Core Functionality)

- [ ] Full Botcraft window interaction
- [ ] Sign packet implementation
- [ ] Robust error recovery
- [ ] Connection stability

### Priority 2 (Features)

- [ ] Auto-sell system
- [ ] Cookie auto-buy
- [ ] Profile switching
- [ ] Trade handling
- [ ] Inventory management

### Priority 3 (Quality of Life)

- [ ] Web GUI (HTTP server)
- [ ] Performance metrics
- [ ] Flip history database
- [ ] Profit tracking
- [ ] Multi-account dashboard

### Priority 4 (Advanced)

- [ ] Machine learning flip prediction
- [ ] Market analysis tools
- [ ] API for external tools
- [ ] Plugin system

## Security Considerations

### Authentication

- Microsoft OAuth flow
- Token caching (encrypted)
- Automatic refresh
- No plain-text storage

### Network

- WSS for Cofl (encrypted)
- SOCKS5 proxy support
- IP rotation (manual)
- Rate limit handling

### Anti-Cheat

**Hypixel Detection Vectors**:
1. Impossible click speeds
2. Perfect timing (too consistent)
3. 24/7 operation
4. Repetitive patterns
5. Movement anomalies

**Mitigations**:
- Human-like delays (jitter)
- Random pauses
- AFK detection handling
- Cookie checks
- Session rotation

### Data Privacy

- No telemetry
- Local logs only
- Webhook opt-in
- Open source (auditable)

## Contributing Guidelines

### Code Style

- C++17 standard
- Google C++ Style Guide
- 4-space indentation
- Max 120 chars per line
- Header guards: `#pragma once`

### Commit Messages

Format: `<type>: <description>`

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `refactor`: Code restructuring
- `test`: Testing
- `chore`: Maintenance

Example: `feat: add SOCKS5 proxy support`

### Pull Request Process

1. Fork repository
2. Create feature branch
3. Implement changes
4. Test on Hypixel
5. Update documentation
6. Submit PR with description

### Testing Requirements

For PRs:
- [ ] Compiles without warnings
- [ ] Tested on Hypixel
- [ ] No crashes for 1+ hours
- [ ] Documentation updated
- [ ] Config backwards compatible

## Known Issues

### Current Limitations

1. **No Web GUI**: Terminal-only interface
2. **Manual Setup**: Requires Botcraft compilation
3. **Hypixel Only**: Not compatible with other servers
4. **No Tests**: Automated testing not implemented
5. **Demo Mode**: Full implementation requires Botcraft

### Planned Fixes

See "Future Enhancements" section above.

## References

- [Botcraft Documentation](https://adepierre.github.io/Botcraft/)
- [Coflnet API](https://sky.coflnet.com)
- [Hypixel API](https://api.hypixel.net)
- [Minecraft Protocol](https://wiki.vg/Protocol)
- [Original BAF](https://github.com/TreXito/frikadellen-baf)
