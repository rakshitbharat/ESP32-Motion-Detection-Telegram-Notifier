# ESP32 Motion Detection System

A smart PIR motion sensor that sends Telegram notifications with configurable sensitivity settings.

## Overview

This project creates a smart motion detection system using an ESP32 and PIR sensor that sends instant Telegram notifications when motion is detected. Features configurable sensitivity settings and LED feedback for easy setup and tuning.

## 🎯 Key Features

- **🏃 Motion Detection** - PIR sensor with adjustable sensitivity
- **📱 Telegram Alerts** - Instant notifications to your phone
- **🔧 Easy Configuration** - Physical button + LED patterns for sensor tuning
- **🤖 Bot Commands** - Control via Telegram (`/status`, `/test_sensor`, `/sensitivity`)
- **💡 Visual Feedback** - LED shows system status and configuration mode

## 📦 Hardware Requirements

- ESP32 Development Board (ESP32-DevKitC or similar)
- PIR Motion Sensor (HC-SR501 or similar) 
- 3 Jumper Wires (Male-to-Male)
- USB Cable for programming

## 🔌 Wiring (3 wires only!)

```
PIR Sensor → ESP32
VCC (Red)  → 3.3V
GND (Black)→ GND  
OUT (White)→ GPIO 4
```

That's it! Built-in LED on GPIO 2 shows status.

## 📱 Quick Telegram Setup

### 1. Create Bot
1. Message `@BotFather` in Telegram
2. Send `/newbot` 
3. Choose name: "My Motion Detector"
4. Choose username: "my_motion_bot"
5. **Save the token** (looks like: `123456:ABCdef...`)

### 2. Get Chat ID
1. Send "hello" to your bot
2. Visit: `https://api.telegram.org/bot<TOKEN>/getUpdates`
3. **Copy the "id" number**

### 3. Test It
Visit: `https://api.telegram.org/bot<TOKEN>/sendMessage?chat_id=<ID>&text=Test`

## ⚙️ Software Setup

### 1. Install PlatformIO
Download [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) (free)

### 2. Configure Project
```bash
# Clone project
git clone https://github.com/rakshitbharat/ESP32-Motion-Detection-Telegram-Notifier.git
cd ESP32-Motion-Detection-Telegram-Notifier

# Copy template
cp include/secrets.h.template include/secrets.h
```

### 3. Add Your Settings
Edit `include/secrets.h`:
```cpp
#define WIFI_SSID_SECRET "YourWiFiName"
#define WIFI_PASSWORD_SECRET "YourWiFiPassword" 
#define BOT_TOKEN_SECRET "123456:ABCdef..."
#define CHAT_ID_SECRET "123456789"
```

### 4. Upload & Run
```bash
pio run --target upload --target monitor
```

## 🎛️ Sensor Configuration Mode

**NEW FEATURE**: Easy PIR sensor tuning!

### Physical Button Method
1. **Hold Boot Button** (3 seconds) → Enter config mode
2. **Press Button** → Cycle through: Sensitivity → Range → Test → Save
3. **Watch LED patterns** → Shows current setting level
4. **Auto-saves** when complete

### Telegram Commands
```
/sensor_config     - Enter config mode
/sensitivity 0-4   - Set sensitivity (0=lowest, 4=highest)
/range 0-2         - Set range (0=short, 2=long)  
/test_sensor       - Test for 10 seconds
/show_settings     - View current settings
```

### Sensitivity Levels
| Level | Response | Best For |
|-------|----------|----------|
| 0-1 | Very Slow | Outdoor (avoid false alarms) |
| 2 | Balanced | Indoor rooms |
| 3-4 | Very Fast | Close-range detection |

## 🚀 Usage

### First Time
1. Power on → LED blinks (connecting to WiFi)
2. LED solid → Connected and monitoring
3. Walk past sensor → LED flashes + Telegram message

### Daily Use
- Motion detected → Get Telegram alert
- Use `/status` → Check system health
- Hold button → Adjust sensitivity if needed

### Troubleshooting

**No WiFi connection?**
- Check 2.4GHz network (ESP32 doesn't support 5GHz)
- Verify credentials in `secrets.h`

**No Telegram messages?** 
- Test bot manually with browser method above
- Check bot token and chat ID format

**False alarms?**
- Lower sensitivity: `/sensitivity 1`
- Adjust PIR sensor knobs (if available)

**Missing motion?**
- Increase sensitivity: `/sensitivity 3`
- Check sensor positioning and power

## 📊 Commands Reference

| Command | Description |
|---------|-------------|
| `/status` | System information |
| `/test` | Send test message |
| `/sensor_config` | Enter sensor setup mode |
| `/sensitivity [0-4]` | Adjust motion sensitivity |
| `/range [0-2]` | Adjust detection range |
| `/test_sensor` | 10-second motion test |
| `/show_settings` | Current sensor config |
| `/help` | Show all commands |

## 🔧 Advanced Configuration

Edit `include/config.h` for:
- Different GPIO pins
- Notification timing
- LED behavior
- Debug output levels

Key settings:
```cpp
#define MOTION_SENSOR_PIN 4        // PIR sensor pin
#define NOTIFICATION_INTERVAL 10000 // Min time between alerts (ms)
#define PRODUCTION_MODE false      // Enable full features
```

## 💡 Pro Tips

1. **Sensor Placement**: 6-10 feet high, avoid heat sources
2. **Sensitivity Tuning**: Start with level 2, adjust based on results
3. **Range Setting**: Use "Short" for bathrooms, "Long" for living rooms
4. **Testing**: Use `/test_sensor` to verify settings before leaving
5. **Multiple Sensors**: Copy project folder for different rooms

## 🏗️ Project Structure

```
ESP32-Motion-Detection-Telegram-Notifier/
├── src/main.cpp              # Main program
├── include/
│   ├── config.h              # Settings
│   ├── secrets.h.template    # Credentials template
│   └── secrets.h             # Your credentials (keep private!)
├── platformio.ini            # Build configuration
└── SENSOR_CONFIG_GUIDE.md    # Detailed sensor tuning guide
```

## 🎯 Version 2.0 Features

- ✅ Physical button configuration mode
- ✅ LED pattern feedback system
- ✅ Telegram sensor controls
- ✅ Adjustable sensitivity (5 levels)
- ✅ Configurable range settings
- ✅ Real-time sensor testing
- ✅ Persistent settings storage

## 🆘 Getting Help

1. **Check Serial Monitor**: `pio device monitor`
2. **Test Components**: Use `/test_sensor` command
3. **Verify Wiring**: Only 3 wires needed
4. **Check Settings**: Use `/show_settings`

---

**Ready to get started?** Follow the setup steps above to configure your motion detection system.