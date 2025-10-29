# 🚀 Quick Setup Guide - Get Running in 5 Minutes!

## 🎯 **SUPER EASY MODE** - One Command Setup!
```bash
./setup.sh
# Follow the prompts to configure your 4 values
```

## ⚡ Manual Setup - Only 4 Values Needed!

### 📋 Prerequisites (30 seconds)
- ESP32 board
- PIR motion sensor 
- 3 jumper wires
- USB cable

### 1️⃣ Create Telegram Bot (2 minutes)
1. Open Telegram and message `@BotFather`
2. Send `/newbot`
3. Choose name: **"Motion Alert Bot"**
4. Choose username: **"your_motion_alert_bot"** (must be unique)
5. **🔑 COPY THE TOKEN** (looks like: `1234567890:ABCdefGHIjklMNOpqrSTUvwxYZ123456789`)

### 2️⃣ Get Your Chat ID (1 minute)

**Choose your notification method:**

**📱 Personal Chat (Easiest):**
1. Send **any message** to your new bot
2. Open: `https://api.telegram.org/bot<YOUR_TOKEN>/getUpdates`
3. **🔑 COPY THE "id" NUMBER** (positive like: `123456789`)

**👥 Group Chat (Family Alerts):**
1. Create/open group → Add your bot → Make bot admin
2. Send message in group
3. Open: `https://api.telegram.org/bot<YOUR_TOKEN>/getUpdates`
4. **🔑 COPY THE "id" NUMBER** (negative like: `-123456789`)

**📢 Channel (Broadcast):**
1. Create/open channel → Add bot as admin → Post message
2. Open: `https://api.telegram.org/bot<YOUR_TOKEN>/getUpdates`
3. **🔑 COPY THE "id" NUMBER** (starts with `-100`)

### 🧪 **Test Your Configuration** (Optional but Recommended)
**[🔗 Use Our Online Telegram Tester](https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/)**
- Verify your bot works BEFORE uploading to ESP32
- Tests the exact same values used in your `secrets.h`
- Sends a test message to confirm everything is working

### 3️⃣ Configure Project (1 minute)
```bash
# Copy the template
cp include/secrets.h.template include/secrets.h

# Edit secrets.h - ONLY 4 VALUES NEEDED:
# Line 10: WIFI_SSID = "Your_WiFi_Name"
# Line 11: WIFI_PASSWORD = "Your_WiFi_Password"  
# Line 14: BOT_TOKEN = "1234567890:YOUR_ACTUAL_TOKEN"
# Line 15: CHAT_ID = "YOUR_ACTUAL_CHAT_ID"
```

**📝 Example secrets.h:**
```cpp
const char* WIFI_SSID = "MyHomeWiFi";
const char* WIFI_PASSWORD = "mypassword123";
const char* BOT_TOKEN = "1234567890:ABCdefGHIjklMNOpqrSTUvwxYZ123456789";
const char* CHAT_ID = "123456789";
```

### 4️⃣ Wire Hardware (1 minute)
```
PIR Sensor → ESP32
VCC (red)  → 3.3V
GND (black)→ GND  
OUT (white)→ GPIO 4
```

### 5️⃣ Upload & Test (1 minute)
```bash
# Upload code
pio run --target upload

# Watch for startup messages
pio device monitor

# Look for: "🚀 ESP32_Motion_Detector Online"
```

### ✅ Test Motion Detection
1. **Wave hand** in front of PIR sensor
2. **Check Telegram** for "🚨 MOTION DETECTED" message
3. **Send `/status`** to bot for system info

## 🎉 **DONE! Your ESP32 Motion Detector is LIVE!**

---

## �️ Troubleshooting (If Needed)

### ❌ No WiFi Connection?
- ✅ Use **2.4GHz WiFi** (not 5GHz)
- ✅ Check SSID and password spelling
- ✅ Ensure WiFi password is **8+ characters**

### ❌ No Telegram Messages?
- ✅ **Use our Telegram Tester**: [Test Configuration Online](https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/)
- ✅ Verify bot token has `:` in the middle
- ✅ Test manually: `https://api.telegram.org/bot<TOKEN>/sendMessage?chat_id=<CHAT_ID>&text=Hello`
- ✅ For group chats, CHAT_ID starts with `-100`

### ❌ No Motion Detection?
- ✅ Check PIR sensor has **3.3V power** (LED should be on)
- ✅ Wait **2 minutes** after power-on for PIR to stabilize
- ✅ Adjust PIR **sensitivity screw** (small potentiometer)

### ❌ Build Errors?
```bash
# Update PlatformIO
pio upgrade --dev

# Clean and rebuild
pio run --target clean
pio run
```

---

## 🎯 Quick Commands

### Test Your Bot
```bash
# Replace <TOKEN> and <CHAT_ID> with your values
curl "https://api.telegram.org/bot<TOKEN>/sendMessage?chat_id=<CHAT_ID>&text=Test"
```

### Monitor Serial Output
```bash
pio device monitor --baud 115200 --filter colorize
```

### Upload to Different ESP32 Board
```bash
# For ESP32-S2
pio run -e esp32-s2 --target upload

# For ESP32-S3  
pio run -e esp32-s3 --target upload

# For ESP32-C3
pio run -e esp32-c3 --target upload
```

---

## 📱 Bot Commands (Send to Your Bot)

- `/status` - System information
- `/ping` - Test connectivity  
- `/restart` - Restart device
- `/settings` - View current settings
- `/help` - List all commands

---

## 🔥 **Pro Tips**

1. **Multiple Chat IDs**: Add family members by getting their chat ID
2. **Motion Sensitivity**: Adjust the small screw on PIR sensor
3. **Range**: PIR sensor detects up to 7 meters (23 feet)
4. **Power**: Can run on battery with deep sleep mode
5. **Expansion**: Add temperature sensor, camera, or multiple PIR sensors

---

## 📚 **Next Steps**

- 📖 Read `README.md` for advanced features
- ⚙️ Check `ADVANCED_CONFIG.md` for 600+ configuration options
- 🔌 See `WIRING.md` for additional sensors
- 🧪 Use `TESTING.md` for comprehensive testing

**🎊 Congratulations! Your ESP32 Motion Detection System is Ready!**