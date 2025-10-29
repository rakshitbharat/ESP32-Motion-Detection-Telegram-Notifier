# Advanced Configuration Guide

## 🔧 Advanced Configuration Options

This guide covers advanced configuration options for power users who want to customize every aspect of the ESP32 Motion Detection System.

## 📋 Configuration Hierarchy

The system uses a layered configuration approach:

1. **`config.h`** - System-wide defaults and compile-time settings
2. **`secrets.h`** - User credentials and sensitive data
3. **Runtime Configuration** - Settings that can be changed during operation

## ⚙️ Detailed Configuration Sections

### Network Configuration

#### WiFi Settings
```cpp
// Multiple network support
const WiFiNetwork WIFI_NETWORKS[] = {
    {"Home_WiFi", "password123", 1, true},        // Priority 1 (highest)
    {"Office_WiFi", "officepass", 2, true},       // Priority 2
    {"Mobile_Hotspot", "mobilepass", 3, false},   // Priority 3 (disabled)
};
```

#### Static IP Configuration
```cpp
const StaticIPConfig STATIC_IP = {
    "192.168.1.100",    // IP address
    "192.168.1.1",      // Gateway
    "255.255.255.0",    // Subnet mask
    "8.8.8.8",          // Primary DNS
    "8.8.4.4",          // Secondary DNS
    true                // Enable static IP
};
```

#### Network Quality Settings
```cpp
#define MIN_WIFI_SIGNAL_STRENGTH -80    // Minimum acceptable RSSI (dBm)
#define WIFI_RECONNECT_INTERVAL 60000   // Check connection every 60s
#define NETWORK_TIMEOUT 10000           // General network timeout
```

### Telegram Configuration

#### Multiple Chat Support
```cpp
const TelegramChat TELEGRAM_CHATS[] = {
    // chat_id, name, motion_alerts, status_updates, error_alerts, admin_commands, enabled
    {"-1001234567890", "Main Channel", true, true, true, false, true},
    {"123456789", "Personal", true, false, true, true, true},
    {"-1009876543210", "Family", true, false, false, false, true},
};
```

#### Message Templates
```cpp
const char* MOTION_ALERT_TEMPLATE = "🚨 *MOTION DETECTED*\n📍 %s\n🕐 %s\n🔢 %s";
const char* STATUS_MESSAGE_TEMPLATE = "📊 *Status*\n📍 %s\n🔋 %s\n💾 %s\n📶 %s";
```

#### Bot Command Authorization
```cpp
const char* AUTHORIZED_USERS[] = {
    "123456789",      // Your Telegram user ID
    "987654321",      // Another authorized user
};
```

### Motion Detection Configuration

#### Sensor Settings
```cpp
#define MOTION_SENSOR_PIN 4             // PIR sensor pin
#define MOTION_ACTIVE_STATE HIGH        // HIGH or LOW for detection
#define MOTION_DEBOUNCE_DELAY 50        // Debounce delay (ms)
#define SENSOR_STABILIZATION_TIME 30000 // Stabilization time (ms)
```

#### Notification Control
```cpp
#define NOTIFICATION_INTERVAL 30000     // Min time between notifications
#define MAX_NOTIFICATIONS_PER_HOUR 10   // Limit notifications per hour
#define MOTION_COOLDOWN_PERIOD 5000     // Cooldown after motion stops
```

#### Quiet Hours
```cpp
#define QUIET_HOURS_ENABLED true        // Enable quiet hours
#define QUIET_START_HOUR 22             // Start time (24h format)
#define QUIET_END_HOUR 6                // End time (24h format)
```

### Hardware Configuration

#### Pin Assignments
```cpp
// Core pins
#define LED_PIN 2                       // Built-in LED
#define MOTION_SENSOR_PIN 4             // PIR sensor
#define BUZZER_PIN -1                   // Buzzer (-1 to disable)

// Optional pins
#define EXTERNAL_LED_PIN -1             // External LED
#define RESET_BUTTON_PIN -1             // Reset button
#define TEMPERATURE_SENSOR_PIN -1       // Temperature sensor
```

#### LED Patterns
```cpp
#define LED_BLINK_FAST 100             // Fast blink (ms)
#define LED_BLINK_SLOW 500             // Slow blink (ms)
#define LED_BLINK_MOTION 50            // Motion blink (ms)
#define LED_BLINK_ERROR 200            // Error blink (ms)
```

### System Monitoring

#### Performance Monitoring
```cpp
#define ENABLE_PERFORMANCE_MONITORING true
#define MONITOR_LOOP_TIME true         // Monitor main loop time
#define MONITOR_NETWORK_PERFORMANCE true // Monitor network requests
#define PERFORMANCE_LOG_INTERVAL 300000 // Log every 5 minutes
```

#### Memory Management
```cpp
#define MIN_FREE_MEMORY 10000           // Minimum free memory (bytes)
#define MEMORY_CHECK_INTERVAL 600000    // Check every 10 minutes
#define ENABLE_MEMORY_DEBUG true        // Show memory usage
```

#### System Health Checks
```cpp
#define SYSTEM_STATUS_INTERVAL 900000   // Status check every 15 minutes
#define HEARTBEAT_INTERVAL 3600000      // Heartbeat every hour
#define WATCHDOG_TIMEOUT 8000           // Watchdog timeout (ms)
```

### Debug and Logging

#### Debug Levels
```cpp
#define DEBUG_LEVEL 3                   // 0=None, 1=Error, 2=Warn, 3=Info, 4=Debug
#define ENABLE_MEMORY_DEBUG true        // Memory usage logging
#define ENABLE_WIFI_DEBUG true          // WiFi debug info
#define ENABLE_TELEGRAM_DEBUG true      // Telegram debug info
```

#### Logging Options
```cpp
#define LOG_MOTION_EVENTS true          // Log all motion events
#define LOG_NETWORK_EVENTS true         // Log network events
#define LOG_TELEGRAM_MESSAGES true      // Log message sending
#define LOG_SYSTEM_STATUS true          // Log system status
```

### Power Management

#### CPU and Power Settings
```cpp
#define CPU_FREQUENCY 240               // CPU frequency (80, 160, 240 MHz)
#define ENABLE_LIGHT_SLEEP false        // Enable automatic light sleep
#define ENABLE_DEEP_SLEEP false         // Enable deep sleep mode
```

#### Battery Monitoring
```cpp
#define BATTERY_VOLTAGE_PIN -1          // Battery voltage pin
#define LOW_BATTERY_THRESHOLD 3.3       // Low battery threshold (V)
#define BATTERY_CHECK_INTERVAL 1800000  // Check every 30 minutes
```

### Security Configuration

#### Network Security
```cpp
#define USE_SSL_VERIFICATION false      // SSL certificate verification
#define MAX_FAILED_ATTEMPTS 5           // Max failed attempts before lockout
#define LOCKOUT_DURATION 300000         // Lockout duration (ms)
```

#### Access Control
```cpp
#define ENABLE_COMMAND_WHITELIST false  // Restrict commands to authorized users
#define ADMIN_CHAT_ID ""                // Admin chat for restricted commands
```

## 🎛️ Runtime Configuration

Some settings can be changed during runtime through Telegram commands:

### Available Runtime Commands

- `/config show` - Show current configuration
- `/config set <parameter> <value>` - Change a setting
- `/config reset` - Reset to defaults
- `/config save` - Save current settings

### Example Runtime Changes
```
/config set notification_interval 60000
/config set debug_level 2
/config set quiet_hours_enabled true
```

## 🔧 Board-Specific Configurations

### ESP32 DevKit C
```cpp
#define BOARD_TYPE "ESP32-DevKitC"
#define BUILTIN_LED_PIN 2
#define HAS_BUILTIN_LED true
```

### ESP32-S3
```cpp
#define BOARD_TYPE "ESP32-S3"
#define BUILTIN_LED_PIN 48
#define BOARD_HAS_PSRAM true
```

### ESP32-C3
```cpp
#define BOARD_TYPE "ESP32-C3"
#define BUILTIN_LED_PIN 8
#define HAS_BUILTIN_LED true
```

## ⚡ Performance Tuning

### Memory Optimization
```cpp
// Reduce memory usage
#define BOT_MAX_MESSAGE_LENGTH 1024     // Reduce from 4096
#define SERIAL_TX_BUFFER_SIZE 128       // Reduce buffer size
#define WIFI_RECONNECT_INTERVAL 120000  // Less frequent checks
```

### Speed Optimization
```cpp
// Increase performance
#define CPU_FREQUENCY 240               // Maximum CPU speed
#define LOOP_DELAY 50                   // Faster main loop
#define BOT_MTBS 500                    // Faster bot response
```

### Battery Optimization
```cpp
// Optimize for battery operation
#define CPU_FREQUENCY 80                // Lower CPU speed
#define ENABLE_LIGHT_SLEEP true         // Enable power saving
#define WIFI_RECONNECT_INTERVAL 300000  // Less frequent WiFi checks
```

## 🛠️ Custom Feature Development

### Adding New Sensors
1. Define pin in `config.h`
2. Initialize in `initializeSystem()`
3. Read in main loop
4. Add to status messages

### Adding New Notification Channels
1. Add credentials to `secrets.h`
2. Create new client object
3. Add send function
4. Integrate with notification system

### Adding Web Interface
1. Enable web server in config
2. Add HTML templates to data folder
3. Implement configuration endpoints
4. Add OTA update capability

## 🔍 Troubleshooting Configuration

### Common Issues

1. **Compilation Errors**
   - Check pin assignments don't conflict
   - Verify all required includes are present
   - Check macro definitions are valid

2. **Runtime Issues**
   - Verify credentials in `secrets.h`
   - Check pin connections match configuration
   - Monitor serial output for error messages

3. **Performance Issues**
   - Reduce debug level
   - Increase loop delay
   - Optimize notification frequency

### Debug Configuration
```cpp
// Maximum debug output
#define DEBUG_LEVEL 4
#define ENABLE_MEMORY_DEBUG true
#define ENABLE_WIFI_DEBUG true
#define ENABLE_TELEGRAM_DEBUG true
#define LOG_MOTION_EVENTS true
#define LOG_NETWORK_EVENTS true
#define MONITOR_LOOP_TIME true
```

## 📚 Configuration Reference

For complete configuration options, see:
- `include/config.h` - All compile-time settings
- `include/secrets.h.template` - Credential template
- `platformio.ini` - Build configuration
- Board-specific examples in `examples/` folder

---

**Remember**: Always test configuration changes thoroughly before deploying to production!