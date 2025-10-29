#ifndef CONFIG_H
#define CONFIG_H

// ===================================================================
// NETWORK CONFIGURATION
// ===================================================================

// WiFi Configuration (defaults - override in secrets.h)
#ifndef WIFI_SSID
#define WIFI_SSID "YOUR_WIFI_SSID"
#endif
#ifndef WIFI_PASSWORD  
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#endif
#define WIFI_TIMEOUT 15000              // WiFi connection timeout (ms)
#define WIFI_RETRY_DELAY 5000           // Delay between WiFi retry attempts (ms)
#define WIFI_MAX_RETRIES 5              // Maximum WiFi connection retries
#define WIFI_RECONNECT_INTERVAL 60000   // Check WiFi connection every 60s
#define WIFI_SIGNAL_CHECK_INTERVAL 300000 // Check signal strength every 5 minutes

// Network Quality Settings
#define MIN_WIFI_SIGNAL_STRENGTH -80    // Minimum acceptable RSSI (dBm)
#define NETWORK_TIMEOUT 10000           // General network timeout (ms)

// Production Mode (enables only essential motion notifications)
#define PRODUCTION_MODE false           // Enable full configuration features

// Sensor Testing Mode (disable notifications for physical testing)
#define SENSOR_TESTING_MODE true       // Set to true to disable Telegram notifications for sensor testing

// ===================================================================
// TELEGRAM BOT CONFIGURATION
// ===================================================================

// Basic Telegram Settings (using secrets.h values)
#ifndef BOT_TOKEN
#define BOT_TOKEN BOT_TOKEN_SECRET
#endif
#ifndef CHAT_ID
#define CHAT_ID CHAT_ID_SECRET
#endif
#define BOT_MTBS 1000                   // Mean time between scan messages (ms)
#define BOT_MAX_MESSAGE_LENGTH 4096     // Telegram message limit
#define BOT_RETRY_ATTEMPTS 3            // Retry failed messages
#define BOT_RETRY_DELAY 2000            // Delay between retries (ms)

// Advanced Telegram Features
#define ENABLE_BOT_COMMANDS (!PRODUCTION_MODE)      // Enable /status, /test, /help commands
#define ENABLE_MULTIPLE_CHATS true      // Support multiple chat destinations
#define ENABLE_MESSAGE_FORMATTING (!PRODUCTION_MODE)  // Use Markdown/HTML formatting
#define ENABLE_TIMESTAMP_IN_MESSAGES (!PRODUCTION_MODE) // Add timestamp to messages
#define ENABLE_DEVICE_INFO_IN_MESSAGES (!PRODUCTION_MODE) // Include device info in messages

// Message Templates
#define MESSAGE_PARSE_MODE (PRODUCTION_MODE ? "" : "Markdown")   // No formatting in production for speed
#define STARTUP_MESSAGE_ENABLED (!PRODUCTION_MODE)  // Disabled in production
#define SHUTDOWN_MESSAGE_ENABLED (!PRODUCTION_MODE) // Disabled in production
#define HEARTBEAT_MESSAGE_ENABLED false
#define HEARTBEAT_INTERVAL 3600000      // Send heartbeat every hour (ms)

// ===================================================================
// DEVICE CONFIGURATION
// ===================================================================

// Device Identity
#define DEVICE_NAME "ESP32-Motion-Detector"
#ifndef DEVICE_LOCATION
#define DEVICE_LOCATION "Home"          // Configurable location name
#endif
#define FIRMWARE_VERSION "2.0.0"
#define HARDWARE_VERSION "1.0"
#define MANUFACTURER "Custom"

// System Behavior
#define ENABLE_WATCHDOG true            // Enable hardware watchdog
#define WATCHDOG_TIMEOUT 15000          // Watchdog timeout (ms) - Increased for network operations
#define SYSTEM_STATUS_INTERVAL 900000   // System status check every 15 minutes
#define MEMORY_CHECK_INTERVAL 600000    // Check free memory every 10 minutes
#define MIN_FREE_MEMORY 10000           // Minimum free memory threshold (bytes)

// ===================================================================
// MOTION DETECTION CONFIGURATION
// ===================================================================

// Basic Motion Settings
#define MOTION_SENSOR_PIN 4             // PIR sensor pin
#define MOTION_SENSOR_TYPE "PIR"        // Sensor type for logging
#define MOTION_ACTIVE_STATE HIGH        // HIGH or LOW for motion detection
#define MOTION_DEBOUNCE_DELAY 1000      // Hardware debounce delay (ms)

// Motion Sensor Configuration Mode
#define ENABLE_SENSOR_CONFIG_MODE true  // Enable sensor configuration mode
#define CONFIG_BUTTON_PIN 0             // Button pin for entering config mode (Boot button)
#define CONFIG_BUTTON_HOLD_TIME 3000    // Hold time to enter config mode (ms)
#define CONFIG_MODE_TIMEOUT 60000       // Config mode timeout (ms)
#define CONFIG_EXIT_DELAY 2000          // Delay before exiting config mode (ms)

// PIR Sensor Sensitivity Levels (these map to different timing parameters)
#define SENSITIVITY_VERY_LOW 0          // Least sensitive - long delays
#define SENSITIVITY_LOW 1               // Low sensitivity
#define SENSITIVITY_MEDIUM 2            // Default/Medium sensitivity  
#define SENSITIVITY_HIGH 3              // High sensitivity
#define SENSITIVITY_VERY_HIGH 4         // Most sensitive - short delays
#define DEFAULT_SENSITIVITY SENSITIVITY_MEDIUM

// Range/Distance Settings (affects debounce and session timing)
#define RANGE_SHORT 0                   // Short range - quick response
#define RANGE_MEDIUM 1                  // Medium range - default
#define RANGE_LONG 2                    // Long range - delayed response
#define DEFAULT_RANGE RANGE_MEDIUM

// Current sensor settings (can be changed at runtime)
extern int current_sensitivity_level;
extern int current_range_setting;
extern bool sensor_config_mode_active;

// Smart Motion Session Control
#define MOTION_SESSION_TIMEOUT 60000    // Motion session ends after 60s of no motion
#define MOTION_DEBOUNCE_DELAY 2000      // Ignore motion for 2s after detection
#define MOTION_COOLDOWN_PERIOD 10000    // Wait 10s after motion stops before ending session
#define NOTIFICATION_INTERVAL 10000     // Minimum 10 sec between notifications (ms)
#define MAX_NOTIFICATIONS_PER_HOUR 60   // Limit to 60 notifications per hour
#define ENABLE_MOTION_COUNTER true      // Count total motion events
#define RESET_COUNTER_DAILY true        // Reset counters at midnight

// Motion Sensitivity & Timing
#define MOTION_DETECTION_ENABLED true   // Master switch for motion detection
#define QUIET_HOURS_ENABLED false       // Enable quiet hours (no notifications)
#define QUIET_START_HOUR 22             // Start of quiet hours (24h format)
#define QUIET_END_HOUR 6                // End of quiet hours (24h format)

// ===================================================================
// PIN CONFIGURATION
// ===================================================================

// Status Indicators
#define LED_PIN 2                       // Built-in LED pin
#define EXTERNAL_LED_PIN -1             // External LED pin (-1 to disable)
#define BUZZER_PIN -1                   // Buzzer pin (-1 to disable)
#define STATUS_LED_ENABLED true         // Enable status LED

// Input Pins
#define RESET_BUTTON_PIN -1             // Reset button pin (-1 to disable)
#define CONFIG_BUTTON_PIN -1            // Configuration button pin (-1 to disable)

// Additional Sensors (optional)
#define TEMPERATURE_SENSOR_PIN -1       // Temperature sensor pin (-1 to disable)
#define LIGHT_SENSOR_PIN -1             // Light sensor pin (-1 to disable)

// ===================================================================
// TIMING CONFIGURATION
// ===================================================================

// System Timing
#define LOOP_DELAY 100                  // Main loop delay (ms)
#define STARTUP_DELAY 2000              // Delay after startup (ms)
#define SENSOR_STABILIZATION_TIME 30000 // PIR sensor stabilization time (ms)

// LED Patterns
#define LED_BLINK_FAST 100             // Fast blink delay (ms)
#define LED_BLINK_SLOW 500             // Slow blink delay (ms)
#define LED_BLINK_MOTION 50            // Motion detection blink (ms)
#define LED_BLINK_ERROR 200            // Error indication blink (ms)

// Configuration Mode LED Patterns
#define LED_CONFIG_ENTER 80            // Rapid blinking when entering config mode
#define LED_CONFIG_ACTIVE 250          // Steady pulse during config mode
#define LED_CONFIG_SENSITIVITY 150     // Pattern for sensitivity adjustment
#define LED_CONFIG_RANGE 300           // Pattern for range adjustment  
#define LED_CONFIG_TEST 60             // Very fast blink during sensor testing
#define LED_CONFIG_SAVE 400            // Slow pulse when saving settings
#define LED_CONFIG_EXIT 120            // Exit confirmation pattern

// Network Timing
#define HTTP_TIMEOUT 10000             // HTTP request timeout (ms) - Reduced for better reliability
#define SSL_HANDSHAKE_TIMEOUT 10000    // SSL handshake timeout (ms)

// ===================================================================
// DEBUG AND LOGGING CONFIGURATION
// ===================================================================

// Serial Debug
#define DEBUG_SERIAL true              // Enable serial debugging
#define SERIAL_BAUD_RATE 115200        // Serial communication speed
#define DEBUG_LEVEL 3                  // 0=None, 1=Error, 2=Warning, 3=Info, 4=Debug
#define ENABLE_MEMORY_DEBUG true       // Show memory usage
#define ENABLE_WIFI_DEBUG true         // Show WiFi debug info
#define ENABLE_TELEGRAM_DEBUG true     // Show Telegram debug info

// Verbose Logging Options
#define LOG_MOTION_EVENTS true         // Log all motion events
#define LOG_NETWORK_EVENTS true        // Log network connect/disconnect
#define LOG_TELEGRAM_MESSAGES true     // Log Telegram message sending
#define LOG_SYSTEM_STATUS true         // Log periodic system status
#define LOG_TIMESTAMP_FORMAT "[%02d:%02d:%02d] " // HH:MM:SS format

// Performance Monitoring
#define ENABLE_PERFORMANCE_MONITORING true
#define MONITOR_LOOP_TIME true         // Monitor main loop execution time
#define MONITOR_NETWORK_PERFORMANCE true // Monitor network request times
#define PERFORMANCE_LOG_INTERVAL 300000 // Log performance stats every 5 minutes

// ===================================================================
// SECURITY CONFIGURATION
// ===================================================================

// Network Security
#define USE_SSL_VERIFICATION false     // SSL certificate verification (set true for production)
#define ENABLE_ENCRYPTION false        // Enable message encryption (future feature)
#define MAX_FAILED_ATTEMPTS 5          // Max failed connection attempts before lockout
#define LOCKOUT_DURATION 300000        // Lockout duration (ms)

// Access Control
#define ENABLE_COMMAND_WHITELIST false // Only allow commands from specific users
#define ADMIN_CHAT_ID ""               // Admin chat ID for restricted commands

// ===================================================================
// POWER MANAGEMENT CONFIGURATION
// ===================================================================

// Power Saving
#define ENABLE_DEEP_SLEEP false        // Enable deep sleep (not recommended for always-on)
#define SLEEP_DURATION 0               // Deep sleep duration (microseconds)
#define CPU_FREQUENCY 240              // CPU frequency in MHz (80, 160, 240)
#define ENABLE_LIGHT_SLEEP false       // Enable automatic light sleep

// Battery Monitoring (if applicable)
#define BATTERY_VOLTAGE_PIN -1         // Battery voltage monitoring pin (-1 to disable)
#define LOW_BATTERY_THRESHOLD 3.3      // Low battery voltage threshold
#define BATTERY_CHECK_INTERVAL 1800000 // Check battery every 30 minutes

// ===================================================================
// FEATURE TOGGLES
// ===================================================================

// Core Features
#define ENABLE_MOTION_DETECTION true
#define ENABLE_TELEGRAM_NOTIFICATIONS (!SENSOR_TESTING_MODE)  // Disabled during sensor testing
#define ENABLE_WIFI_CONNECTION true
#define ENABLE_STATUS_LED true

// Optional Features
#define ENABLE_WEB_SERVER false        // Enable web configuration interface
#define ENABLE_OTA_UPDATES false       // Enable over-the-air updates
#define ENABLE_NTP_TIME_SYNC true      // Sync time with NTP server
#define ENABLE_TIMEZONE_SUPPORT true   // Support for timezone conversion

// Experimental Features
#define ENABLE_MOTION_PREDICTION false // AI-based motion prediction
#define ENABLE_SOUND_DETECTION false   // Sound-based alerts
#define ENABLE_CAMERA_INTEGRATION false // Camera snapshot on motion

// ===================================================================
// HARDWARE SPECIFIC CONFIGURATION
// ===================================================================

// ESP32 Specific
#define ESP32_PARTITION_SCHEME "default" // Partition scheme
#define FLASH_FREQUENCY 80             // Flash frequency in MHz
#define PSRAM_ENABLED false            // Enable PSRAM if available

// Board Variants
#define BOARD_TYPE "ESP32-DevKitC"     // Board identifier
#define HAS_BUILTIN_LED true           // Board has built-in LED
#define BUILTIN_LED_ACTIVE_LOW false   // LED is active low

// ===================================================================
// TIME ZONE CONFIGURATION
// ===================================================================

// Time Settings
#define TIMEZONE_OFFSET 0              // Timezone offset in hours from UTC
#define DAYLIGHT_SAVING_ENABLED false // Enable daylight saving time
#define NTP_SERVER "pool.ntp.org"     // NTP server for time sync
#define TIME_SYNC_INTERVAL 86400000   // Sync time every 24 hours

// ===================================================================
// VALIDATION MACROS
// ===================================================================

// Compile-time validation
#if NOTIFICATION_INTERVAL < 1000
    #error "NOTIFICATION_INTERVAL must be at least 1000ms"
#endif

#if BOT_MTBS < 500
    #error "BOT_MTBS must be at least 500ms to avoid rate limiting"
#endif

#if WIFI_TIMEOUT < 5000
    #error "WIFI_TIMEOUT should be at least 5000ms"
#endif

#if DEBUG_LEVEL < 0 || DEBUG_LEVEL > 4
    #error "DEBUG_LEVEL must be between 0 and 4"
#endif

#endif // CONFIG_H