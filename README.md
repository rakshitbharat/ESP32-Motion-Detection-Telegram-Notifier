# ESP32 Motion Detection System

A smart PIR motion sensor that sends Telegram notifications with configurable sensitivity settings.

## Overview

This project creates a smart motion detection system using an ESP32 and PIR sensor that sends instant Telegram notifications when motion is detected. Features configurable sensitivity settings and LED feedback for easy setup and tuning.

## Features

- Motion Detection with adjustable sensitivity
- Telegram notifications
- Physical button configuration with LED feedback
- Bot commands for remote control
- Real-time sensor testing

## Hardware Requirements

- ESP32 Development Board (ESP32-DevKitC or similar)
- PIR Motion Sensor (HC-SR501 or similar) 
- 3 Jumper Wires (Male-to-Male)
- USB Cable for programming

## Wiring

```
PIR Sensor → ESP32
VCC (Red)  → 3.3V
GND (Black)→ GND  
OUT (White)→ GPIO 4
```

Built-in LED on GPIO 2 provides status indication.

## Telegram Setup

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

## Software Setup

### 1. Install PlatformIO
Download [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)

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

## Sensor Configuration

### Physical Button Method
1. **Hold Boot Button** (3 seconds) → Enter config mode
2. **Press Button** → Cycle through: Sensitivity → Range → Test → Save
3. **Watch LED patterns** → Shows current setting level
4. **Auto-saves** when complete

## Usage

1. Power on - LED blinks while connecting to WiFi
2. LED becomes solid when connected and monitoring
3. Motion detection triggers LED flash and Telegram message
4. Hold boot button to adjust sensitivity

## Configuration

Edit `include/config.h` to modify GPIO pins, timing, or debug settings.

## Troubleshooting

- Check serial monitor: `pio device monitor`
- Verify 2.4GHz WiFi connection
- Test Telegram bot manually via browser
- Use `/test_sensor` to verify motion detection

---

**Ready to get started?** Follow the setup steps above to configure your motion detection system.