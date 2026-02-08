# Frikadellen BAF - Botcraft Edition

## Bazaar Auction Flipper for Hypixel SkyBlock (Minecraft 1.21.11)

This is a C++ port of the original [frikadellen-baf](https://github.com/TreXito/frikadellen-baf) using [@adepierre/Botcraft](https://github.com/adepierre/Botcraft) for Minecraft 1.21.11 compatibility with Hypixel.

### ⚠️ Important Warning

**This bot is against Hypixel's Terms of Service.** Using it can result in a permanent ban at any time. Use at your own risk.

This is NOT a Remote Access Trojan (RAT). The source code is open and auditable. Authentication is handled securely through Botcraft's Microsoft authentication system.

## Features

- ✅ **Auction House Flips** - Automatically buy underpriced BIN auctions from Coflnet recommendations
- ✅ **Bazaar Flips** - Automated bazaar buy/sell order placement
- ✅ **Window Skip** - Fast confirmation skip for profitable flips
- ✅ **Smart Configuration** - Customizable profit thresholds, skip conditions, and flip filters
- ✅ **Webhook Notifications** - Discord webhook support for flip notifications
- ✅ **SOCKS5 Proxy Support** - Route connection through proxy servers
- ✅ **Account Switching** - Automatic rotation between multiple accounts
- ✅ **Auto Cookie Management** - Automatically purchase and consume booster cookies
- ✅ **Minecraft 1.21.11** - Full compatibility with latest Hypixel version

## Requirements

- **Operating System**: Windows, Linux, or macOS
- **Minecraft Account**: Microsoft account with Hypixel access
- **Booster Cookie**: Active booster cookie for auction house access outside hub
- **Coins**: Sufficient coins in purse (bot doesn't withdraw from bank)
- **Network**: Stable connection with ping under 200ms recommended
- **Cofl Subscription**: Paid plan from [sky.coflnet.com](https://sky.coflnet.com)

### Build Requirements

- CMake 3.15 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- Botcraft library (compiled for Minecraft 1.21.11)
- Dependencies:
  - nlohmann_json (JSON parsing)
  - websocketpp (WebSocket client)
  - OpenSSL (HTTPS/WSS)

## Building from Source

### 1. Install Botcraft

Follow the [Botcraft installation guide](https://github.com/adepierre/Botcraft) to build Botcraft for Minecraft 1.21.11:

```bash
# Clone Botcraft
git clone https://github.com/adepierre/Botcraft.git
cd Botcraft

# Build for Minecraft 1.21.11
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DBOTCRAFT_GAME_VERSION=1.21.1 ..
cmake --build . --config Release
sudo cmake --install .
```

### 2. Build BAF

```bash
# Clone this repository
git clone https://github.com/TreXito/frikadellen-baf-botcraft.git
cd frikadellen-baf-botcraft

# Create build directory
mkdir build && cd build

# Configure and build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release

# The executable will be in build/frikadellen-baf
```

## Configuration

On first run, the bot will create a `config.toml` file. Edit this file to configure the bot:

```toml
# Your Minecraft username
INGAME_NAME = "YourUsername"

# Optional Discord webhook for notifications
WEBHOOK_URL = "https://discord.com/api/webhooks/..."

# Cofl websocket (default should work)
WEBSOCKET_URL = "wss://sky.coflnet.com/modsocket"

# Enable/disable flip types
ENABLE_AH_FLIPS = true
ENABLE_BAZAAR_FLIPS = true

# Action delay in milliseconds
FLIP_ACTION_DELAY = 100

# Skip confirmation settings
[SKIP]
ALWAYS = false
MIN_PROFIT = 1000000
USER_FINDER = false
SKINS = false
PROFIT_PERCENTAGE = 50
MIN_PRICE = 10000000
```

### Proxy Configuration (Optional)

```toml
PROXY_ENABLED = true
PROXY = "127.0.0.1:8080"
PROXY_USERNAME = "user"
PROXY_PASSWORD = "pass"
```

### Account Switching (Optional)

```toml
ACCOUNTS = "Player1,Player2,Player3"
AUTO_SWITCHING = "Player1:8,Player2:8,Player3:8"
```

## Usage

1. **First Run**: The bot will prompt for Microsoft account authentication
2. **Connection**: Bot connects to Hypixel and joins SkyBlock
3. **Island Teleport**: Automatically teleports to your island
4. **Flip Detection**: Listens for Coflnet flip recommendations
5. **Auto Trading**: Buys/sells items automatically based on recommendations

```bash
./frikadellen-baf
```

### Console Commands

While the bot is running, you can use these commands:

- `/switch <username>` - Switch to a different account
- `/status` - Show current bot status
- `/config <setting> <value>` - Change configuration
- `/stop` - Stop the bot gracefully

## How It Works

1. **Authentication**: Microsoft account authentication via Botcraft
2. **Connection**: Connects to mc.hypixel.net
3. **SkyBlock Join**: Automatically joins SkyBlock server
4. **Cofl Integration**: WebSocket connection to Coflnet for flip recommendations
5. **Auction Flips**: 
   - Receives flip notification
   - Checks profit against skip criteria
   - Opens auction house
   - Searches for item UUID
   - Purchases instantly (with optional skip)
6. **Bazaar Flips**:
   - Receives bazaar recommendation
   - Opens bazaar menu
   - Navigates to item
   - Places buy/sell order at recommended price
   - Confirms order

## Differences from Original

This Botcraft version differs from the original Node.js version:

- **Language**: C++ instead of TypeScript/Node.js
- **Framework**: Botcraft instead of Mineflayer
- **Minecraft Version**: 1.21.11 instead of 1.8.9
- **Performance**: Lower memory usage, faster execution
- **Protocol**: Native C++ protocol implementation

All core features and functionality are preserved and working.

## System Requirements

- **RAM**: ~200MB (vs 500MB for Node.js version)
- **CPU**: 1 core
- **Network**: Stable ping under 200ms
- **Disk**: ~50MB for executable + logs

## Logging

Logs are saved to `baf.log`. Debug information includes:

- Connection status
- Flip recommendations
- Window navigation steps
- Purchase confirmations
- Error messages

## Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly on Hypixel
5. Submit a pull request

## License

Same license as the original frikadellen-baf project.

## Credits

- Original BAF concept: [Hannesimo](https://github.com/Hannesimo)
- Node.js version: [TreXito](https://github.com/TreXito)
- Botcraft framework: [adepierre](https://github.com/adepierre)
- Coflnet: [Coflnet](https://coflnet.com)

## Disclaimer

This tool is provided for educational purposes only. The authors are not responsible for any bans, data loss, or other consequences resulting from the use of this bot. Use at your own risk.

**Remember**: Botting is against Hypixel's Terms of Service and can result in a permanent ban.