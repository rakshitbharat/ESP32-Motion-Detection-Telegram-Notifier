# Testing and Validation Guide

## 🧪 Comprehensive Testing Framework

This guide covers all testing procedures to ensure your ESP32 Motion Detection System works perfectly.

## 📋 Pre-Flight Checklist

Before uploading code to your ESP32, verify:

### ✅ Configuration Checklist
- [ ] `secrets.h` created from template
- [ ] WiFi credentials configured
- [ ] Telegram bot token set
- [ ] Chat ID configured  
- [ ] GPIO pins match your wiring
- [ ] All timing parameters reasonable
- [ ] Debug level appropriate

### ✅ Hardware Checklist
- [ ] ESP32 board connected via USB
- [ ] PIR sensor wired correctly (VCC, GND, OUT)
- [ ] LED indicators working (if external)
- [ ] Power supply adequate (USB or external)
- [ ] No loose connections

### ✅ Software Checklist
- [ ] PlatformIO installed and updated
- [ ] All libraries available
- [ ] Code compiles without errors
- [ ] Telegram bot responding to test messages

## 🔧 Automated Validation

### Run Configuration Validator
```bash
python3 scripts/validate_config.py
```

This checks:
- Pin assignment conflicts
- Timing parameter ranges
- Memory settings
- Feature compatibility
- Build configuration

### Run Build Script
```bash
./build_and_test.sh esp32dev
```

This performs:
- Configuration validation
- Dependency installation
- Code compilation
- Static analysis
- Size optimization check

## 🧪 Step-by-Step Testing

### Phase 1: Basic System Test

#### 1.1 Compile and Upload
```bash
pio run --target upload
pio device monitor --baud 115200
```

**Expected Output:**
```
================================================================
        ESP32 MOTION DETECTION SYSTEM v2.0.0
================================================================
Device: ESP32-Motion-Detector
Location: Your_Location
Build Date: 2024-XX-XX XX:XX:XX
================================================================

🌐 Initializing WiFi...
📡 Connecting to: Your_WiFi_Network
✅ WiFi connected successfully!
📱 Initializing Telegram Bot...
✅ Telegram Bot initialized successfully
🤖 Bot Name: YourBotName
🎯 Initializing motion sensor...
✅ Motion sensor initialized on GPIO 4
🚀 System initialization completed!
📡 Monitoring for motion events...
```

#### 1.2 WiFi Connection Test
- Device should connect to WiFi within 15 seconds
- IP address should be displayed
- WiFi signal strength should be reasonable (>-80 dBm)

#### 1.3 LED Indicator Test
- LED should blink 3 times on startup
- LED should stay solid when WiFi connected
- LED should blink slowly when WiFi disconnected

### Phase 2: Telegram Integration Test

#### 2.1 Bot Communication Test
Send these commands to your bot:

**Test `/status` command:**
- Send: `/status`
- Expected: System status with uptime, memory, WiFi info

**Test `/test` command:**
- Send: `/test`  
- Expected: Test message with device info

**Test `/help` command:**
- Send: `/help`
- Expected: List of available commands

#### 2.2 Startup Notification Test
- Restart the ESP32
- You should receive startup notification within 30 seconds
- Message should include device name, location, IP, firmware version

### Phase 3: Motion Detection Test

#### 3.1 Sensor Stabilization
- Wait 30 seconds after startup for sensor stabilization
- Monitor serial output for "Motion sensor stabilization completed"

#### 3.2 Motion Event Test
- Wave your hand in front of PIR sensor
- **Expected Serial Output:**
  ```
  🚨 Motion detected!
  Motion notification sent (Daily: 1)
  ✅ Notification sent successfully
  ```

- **Expected Telegram Message:**
  ```
  🚨 MOTION DETECTED
  📍 Location: Your_Location
  🕐 Time: XX:XX:XX
  🔢 Device: ESP32-Motion-Detector
  📊 Event #1
  📱 ESP32-Motion-Detector
  ```

#### 3.3 LED Response Test
- LED should blink rapidly (5 times) when motion detected
- LED should return to normal state after motion stops

#### 3.4 Notification Throttling Test
- Trigger motion multiple times quickly
- Only first motion should send notification
- Subsequent motions should be logged but not notified until interval passes

### Phase 4: Advanced Feature Tests

#### 4.1 Multiple Chat Test (if configured)
- Motion should trigger notifications to all enabled chats
- Status updates should go to designated chats only

#### 4.2 Command Authorization Test (if enabled)
- Unauthorized users should get "access denied" message
- Authorized users should get normal responses

#### 4.3 Quiet Hours Test (if enabled)
- Set quiet hours in configuration
- Motion during quiet hours should be logged but not notified

#### 4.4 Daily Counter Reset Test
- Check notification counter resets at midnight
- Verify statistics tracking

### Phase 5: Error Handling Tests

#### 5.1 WiFi Disconnection Test
- Disconnect WiFi (turn off router/hotspot)
- Device should detect disconnection
- Device should attempt reconnection
- LED should blink slowly during disconnection

#### 5.2 Telegram API Failure Test
- Block internet access temporarily
- Motion should still be detected and logged
- Notifications should retry when connection restored

#### 5.3 Memory Stress Test
- Let device run for several hours
- Monitor memory usage in status messages
- Check for memory leaks or crashes

#### 5.4 Power Cycle Test
- Power off and restart device multiple times
- Each startup should be successful
- Settings should persist (if using persistent storage)

## 📊 Performance Testing

### 6.1 Response Time Tests
- Measure time from motion to notification
- Should be under 5 seconds with good WiFi
- Monitor for delays during peak usage

### 6.2 Reliability Tests
- 24-hour continuous operation test
- Motion detection accuracy over time
- Network reconnection reliability

### 6.3 Resource Usage Tests
- Monitor CPU usage and temperature
- Check memory consumption patterns  
- Verify acceptable power consumption

## 🐛 Troubleshooting Tests

### Common Issues and Solutions

#### WiFi Connection Problems
**Symptoms:**
- "WiFi connection failed" messages
- Long connection delays
- Frequent disconnections

**Tests:**
```bash
# Check WiFi diagnostics
/info command in Telegram
```

**Solutions:**
- Verify 2.4GHz network (ESP32 doesn't support 5GHz)
- Check signal strength and proximity to router
- Verify credentials in secrets.h
- Try different WiFi channel on router

#### Telegram Not Working
**Symptoms:**
- Bot doesn't respond to commands
- No startup notification received
- "Failed to send notification" errors

**Tests:**
```bash
# Manual bot test in browser:
https://api.telegram.org/bot<TOKEN>/getMe
https://api.telegram.org/bot<TOKEN>/sendMessage?chat_id=<ID>&text=test
```

**Solutions:**
- Verify bot token format and validity
- Check chat ID (negative for groups/channels)
- Ensure bot added to channel as admin
- Test with personal chat first

#### Motion Detection Issues
**Symptoms:**
- No motion detected
- False positive detections
- Inconsistent sensitivity

**Tests:**
- Check PIR sensor LED indicator (if available)
- Test with different motion patterns
- Monitor raw sensor readings in debug mode

**Solutions:**
- Verify wiring connections (VCC, GND, OUT)
- Check GPIO pin configuration
- Adjust PIR sensitivity potentiometer
- Allow full stabilization time (30+ seconds)

#### Memory Issues
**Symptoms:**
- Device crashes or restarts
- "Low memory warning" messages
- Degraded performance over time

**Tests:**
- Monitor `/status` memory readings
- Check for memory leaks in long-term operation

**Solutions:**
- Reduce debug level
- Optimize message templates
- Increase minimum memory threshold

## 📈 Performance Benchmarks

### Target Performance Metrics
- **Boot Time:** < 30 seconds to full operation
- **Motion Response:** < 3 seconds from detection to notification
- **WiFi Reconnection:** < 15 seconds after network restoration
- **Memory Usage:** > 50KB free during normal operation
- **CPU Temperature:** < 80°C under normal conditions
- **Uptime:** > 7 days continuous operation

### Measuring Performance
```bash
# Enable performance monitoring in config.h
#define ENABLE_PERFORMANCE_MONITORING true
#define MONITOR_LOOP_TIME true

# Check performance stats via Telegram
/stats command
```

## 🔍 Debug Mode Testing

### Enable Maximum Debug Output
```cpp
// In config.h
#define DEBUG_LEVEL 4
#define ENABLE_MEMORY_DEBUG true
#define ENABLE_WIFI_DEBUG true  
#define ENABLE_TELEGRAM_DEBUG true
#define LOG_MOTION_EVENTS true
#define LOG_NETWORK_EVENTS true
```

### Debug Serial Output Analysis
Monitor for:
- Memory allocation/deallocation
- Network request timing
- Telegram API responses
- Motion sensor readings
- System health metrics

## ✅ Final Validation Checklist

Before considering the system production-ready:

### Functionality Tests
- [ ] All basic features working
- [ ] Motion detection accurate and responsive
- [ ] Telegram notifications reliable
- [ ] WiFi connection stable
- [ ] LED indicators functioning
- [ ] All commands responding correctly

### Reliability Tests  
- [ ] 24+ hour continuous operation
- [ ] Multiple power cycles successful
- [ ] Network disruption recovery
- [ ] Memory usage stable
- [ ] No crash/restart events

### Security Tests
- [ ] Unauthorized access properly blocked
- [ ] Credentials not exposed in logs
- [ ] SSL connections working (if enabled)
- [ ] Bot commands properly authenticated

### Performance Tests
- [ ] Response times within targets
- [ ] Memory usage acceptable  
- [ ] CPU temperature normal
- [ ] Power consumption reasonable

## 📝 Test Report Template

```
ESP32 Motion Detection System - Test Report
==========================================
Date: ___________
Tester: ___________
Firmware Version: ___________
Hardware: ___________

Configuration Tests:
[ ] Pin assignments validated
[ ] Timing parameters verified
[ ] Feature compatibility checked
[ ] Build configuration correct

Basic Functionality:
[ ] WiFi connection: ______ seconds
[ ] Telegram bot responding: Y/N
[ ] Motion detection working: Y/N  
[ ] LED indicators functioning: Y/N
[ ] All commands working: Y/N

Performance Metrics:
- Boot time: ______ seconds
- Motion response time: ______ seconds
- WiFi signal strength: ______ dBm
- Free memory: ______ bytes
- CPU temperature: ______ °C

24-Hour Test Results:
- Uptime achieved: ______ hours
- Motion events detected: ______
- Notifications sent: ______
- Network reconnections: ______
- System restarts: ______

Issues Found:
1. ________________________
2. ________________________
3. ________________________

Overall Status: PASS / FAIL
Recommendation: DEPLOY / FIX ISSUES / MORE TESTING

Signature: ___________
```

---

**Remember**: Thorough testing now prevents headaches later. Take time to validate all functionality before deploying your motion detection system!