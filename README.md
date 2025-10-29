# ESP32 Motion Detection Telegram Notifier

A PlatformIO-based ESP32 project that detects motion using a PIR sensor and sends notifications to a Telegram channel or chat.

## ⚡ **Get Started in 5 Minutes!** 
� **[QUICK_SETUP.md](QUICK_SETUP.md)** - Ultra-fast setup with only 4 configuration values needed!

## �🚀 Features

- **🏃‍♂️ Ultra-Quick Setup**: Only 4 values needed to start (WiFi + Telegram)
- **🎯 Motion Detection**: Uses PIR sensor to detect movement with smart debouncing
- **📱 Telegram Integration**: Real-time notifications with rich formatting and emoji
- **📡 WiFi Management**: Auto-connection, reconnection, and multi-network support
- **💬 Multiple Chat Support**: Send notifications to multiple Telegram chats/channels
- **🤖 Bot Commands**: Interactive commands (`/status`, `/ping`, `/restart`, `/help`)
- **💡 LED Indicators**: Visual feedback for system status and motion detection
- **⚙️ 600+ Configuration Options**: Customize every aspect of behavior
- **🔒 Security**: Sensitive credentials stored separately with validation
- **🏗️ Multiple ESP32 Support**: ESP32, ESP32-S2, ESP32-S3, ESP32-C3 compatible
- **🔧 Production Ready**: Advanced build system with multiple environments

## � **Quick Start** 

### **Want to get running immediately?** 
📖 **[QUICK_SETUP.md](QUICK_SETUP.md)** - Complete setup in 5 minutes with only 4 configuration values!

### **Want all the details?** 
Continue reading below for comprehensive setup and advanced features.

---

## �📋 Hardware Requirements

- **ESP32 Development Board** (ESP32-DevKitC, ESP32-S2, ESP32-S3, ESP32-C3)
- **PIR Motion Sensor** (HC-SR501 or similar) 
- **3 Jumper Wires**
- **USB Cable** for programming
- **Breadboard** (optional)

## 🔌 Wiring Diagram

```
PIR Sensor    ESP32
---------     -----
VCC (red)   →  3.3V (or 5V if sensor supports it)
GND (black) →  GND
OUT (white) →  GPIO 4 (configurable in config.h)

Built-in LED is on GPIO 2 (no additional wiring needed)
```

**💡 That's it! Only 3 wires needed for basic motion detection.**

## 📱 Telegram Bot Setup

### 🧪 **Quick Configuration Test**
**[🔗 Use Our Online Telegram Tester](https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/)**

Before diving into manual setup, use our web-based tester to verify your Telegram configuration works correctly. This tool tests the exact same values you'll use in your ESP32 code.

### Step 1: Create a Telegram Bot

1. **Open Telegram** and search for `@BotFather`
2. **Start a chat** with BotFather by clicking "Start"
3. **Create a new bot** by sending: `/newbot`
4. **Choose a name** for your bot (e.g., "Motion Alert Bot")
5. **Choose a username** for your bot (must end with 'bot', e.g., "motion_alert_notify_bot")
6. **Save the Bot Token** - BotFather will provide a token like: `1234567890:ABCdefGHIjklMNOpqrSTUvwxYZ123456789`

### Step 2: Get Your Chat ID

Choose your notification method:

#### Option A: Personal Chat (Simplest)
1. **Send a message** to your bot (any message like "hello")
2. **Open this URL** in your browser: `https://api.telegram.org/bot<YOUR_BOT_TOKEN>/getUpdates`
3. **Copy the "id"** number (positive number like `123456789`)

#### Option B: Group Chat (Family/Team Notifications)
1. **Create or open** your Telegram group
2. **Add your bot to the group**:
   - Group Settings → Add Members → Search for your bot username → Add
3. **Make bot an administrator** (recommended):
   - Group Settings → Administrators → Add Administrator → Select your bot → Enable "Post Messages"
4. **Send any message** in the group (like "Testing setup")
5. **Open this URL**: `https://api.telegram.org/bot<YOUR_BOT_TOKEN>/getUpdates`
6. **Copy the "id"** number (negative number like `-123456789`)

#### Option C: Channel (Broadcast Alerts)
1. **Create or open** your Telegram channel
2. **Add your bot as administrator**:
   - Channel Settings → Administrators → Add Administrator → Search for your bot → Enable "Post Messages"
3. **Post any message** in the channel
4. **Open this URL**: `https://api.telegram.org/bot<YOUR_BOT_TOKEN>/getUpdates`
5. **Copy the "id"** number (starts with `-100` like `-1001234567890`)

> **💡 Pro Tip:** Use our [Interactive Telegram Tester](https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/) for guided setup with automatic Chat ID detection!

### Step 3: Test Your Bot
Send this URL in your browser to test:
```
https://api.telegram.org/bot<YOUR_BOT_TOKEN>/sendMessage?chat_id=<YOUR_CHAT_ID>&text=Test message
```

## ⚙️ Project Setup

### Prerequisites
- **PlatformIO IDE** or **PlatformIO Core**
- **Git** (optional)

### Installation Steps

1. **Clone or download** this project:
   ```bash
   git clone <repository-url>
   cd motion-detect-notification
   ```

2. **Create secrets file**:
   ```bash
   cp include/secrets.h.template include/secrets.h
   ```

3. **Configure your credentials** in `include/secrets.h`:
   ```cpp
   const char* WIFI_SSID = "Your_WiFi_Network_Name";
   const char* WIFI_PASSWORD = "Your_WiFi_Password";
   const char* BOT_TOKEN = "1234567890:ABCdefGHIjklMNOpqrSTUvwxYZ123456789";
   const char* CHAT_ID = "-1001234567890";  // Your chat/channel ID
   ```

4. **Optional**: Modify settings in `include/config.h`:
   - Change GPIO pins
   - Adjust timing intervals
   - Modify device name

5. **Build and upload**:
   ```bash
   pio run --target upload
   ```

6. **Monitor serial output**:
   ```bash
   pio device monitor
   ```

## 📁 Project Structure

```
motion-detect-notification/
├── platformio.ini          # PlatformIO configuration
├── src/
│   └── main.cpp            # Main program
├── include/
│   ├── config.h            # Configuration settings
│   ├── secrets.h.template  # Template for credentials
│   └── secrets.h           # Your actual credentials (not in git)
├── .gitignore              # Git ignore file
└── README.md               # This file
```

## ⚡ Configuration Options

### WiFi Settings (`secrets.h`)
```cpp
const char* WIFI_SSID = "YourWiFiName";
const char* WIFI_PASSWORD = "YourWiFiPassword";
```

### Telegram Settings (`secrets.h`)
```cpp
const char* BOT_TOKEN = "your-bot-token";
const char* CHAT_ID = "your-chat-id";

// Multiple recipients (optional)
const char* CHAT_IDS[] = {
    "-1001234567890",  // Main channel
    "123456789",       // Personal chat
};
```

### Hardware Settings (`config.h`)
```cpp
#define MOTION_SENSOR_PIN 4    // PIR sensor pin
#define LED_PIN 2              // LED indicator pin
```

### Timing Settings (`config.h`)
```cpp
#define NOTIFICATION_INTERVAL 30000  // Min time between notifications (ms)
#define WIFI_TIMEOUT 10000           // WiFi connection timeout (ms)
```

## 🤖 Bot Commands

Your bot will respond to these commands:

- `/status` - Show system status and information
- `/test` - Send a test notification
- `/help` - Show available commands

## 🔧 Troubleshooting

### Common Issues

#### WiFi Connection Problems
- **Check credentials** in `secrets.h`
- **Verify WiFi network** is 2.4GHz (ESP32 doesn't support 5GHz)
- **Check signal strength** - move ESP32 closer to router

#### Telegram Not Working
- **🧪 Use Our Telegram Tester**: [Test Configuration Online](https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/)
- **Verify bot token** is correct
- **Check chat ID** format (negative for groups/channels)
- **Ensure bot is added** to channel/group as admin
- **Test bot manually** using browser method above

#### Motion Sensor Issues
- **Check wiring** connections
- **Verify power supply** (3.3V or 5V depending on sensor)
- **Adjust sensor sensitivity** (potentiometers on PIR sensor)
- **Check pin configuration** in `config.h`

#### Build Errors
- **Update PlatformIO** to latest version
- **Check internet connection** for library downloads
- **Verify ESP32 board** is properly connected

### Serial Monitor Output
Use serial monitor to debug:
```bash
pio device monitor --baud 115200
```

Look for these status messages:
- `✅ WiFi connected successfully!`
- `✅ Notification sent successfully`
- `🚨 MOTION DETECTED!`

## 🔒 Security Considerations

1. **Never commit** `secrets.h` to version control
2. **Use strong WiFi passwords**
3. **Keep bot token private**
4. **Regularly update** firmware and libraries
5. **Monitor bot usage** for unauthorized access

## 📈 Customization Ideas

### Hardware Enhancements
- **Add multiple PIR sensors** for different rooms
- **Include temperature/humidity sensor** (DHT22)
- **Add buzzer** for local alerts
- **Use external antenna** for better WiFi range

### Software Features
- **Add time-based scheduling** (only alert during certain hours)
- **Implement location-based messages** for multiple sensors
- **Add photo capture** with ESP32-CAM
- **Create web interface** for configuration
- **Store logs** on SD card

### Advanced Telegram Features
- **Inline keyboards** for interactive responses
- **Photo/video sending** capability
- **Group management** commands
- **Custom notification sounds**

## 📚 Dependencies

This project uses these libraries (automatically installed by PlatformIO):

- **ArduinoJson** (^6.21.3) - JSON parsing and creation
- **WiFi** (^1.2.7) - ESP32 WiFi functionality
- **UniversalTelegramBot** (^1.3.0) - Telegram Bot API
- **ArduinoHttpClient** (^0.4.0) - HTTP client for API calls

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📄 License

This project is open source. Feel free to use and modify as needed.

## 🆘 Support

If you encounter issues:

1. **Check this README** for troubleshooting steps
2. **Review serial monitor** output for error messages
3. **Verify hardware connections**
4. **Test bot manually** using browser method
5. **Open an issue** in the repository with details

## 🎯 Version History

- **v1.0.0** - Initial release with basic motion detection and Telegram notifications

---

**Happy Making! 🚀**

*Remember to keep your credentials secure and never share your bot token publicly.*