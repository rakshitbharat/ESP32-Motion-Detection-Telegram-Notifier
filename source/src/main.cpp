// ===================================================================
// ESP32 Motion Detection System with Telegram Notifications
// Version: 2.0.0
// Author: Advanced Configuration System
// ===================================================================

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_task_wdt.h>

#include "config.h"

// Include secrets file if it exists, otherwise use config.h defaults
#ifdef __has_include
    #if __has_include("secrets.h")
        #include "secrets.h"
        #define USE_SECRETS_FILE
    #endif
#endif

// ===================================================================
// GLOBAL OBJECTS AND CLIENTS
// ===================================================================

WiFiClientSecure client;
UniversalTelegramBot* bot = nullptr;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, TIMEZONE_OFFSET * 3600, TIME_SYNC_INTERVAL);

// ===================================================================
// STATE VARIABLES
// ===================================================================

// Timing variables
unsigned long lastTimeBotRan = 0;
unsigned long lastNotificationTime = 0;
unsigned long lastWiFiCheck = 0;
unsigned long lastSystemCheck = 0;
unsigned long lastMemoryCheck = 0;
unsigned long lastHeartbeat = 0;
unsigned long systemStartTime = 0;
unsigned long lastMotionTime = 0;
unsigned long sensorStabilizationStart = 0;
unsigned long motionSessionStart = 0;
unsigned long lastMotionEnd = 0;

// Sensor Configuration Mode Variables
int current_sensitivity_level = DEFAULT_SENSITIVITY;
int current_range_setting = DEFAULT_RANGE;
bool sensor_config_mode_active = false;
unsigned long config_mode_start_time = 0;
unsigned long last_button_press = 0;
unsigned long config_button_press_start = 0;
bool config_button_held = false;
int config_step = 0; // 0=sensitivity, 1=range, 2=test, 3=save

// Status flags
bool motionDetected = false;
bool motionSessionActive = false;
bool motionSessionNotified = false;
bool wifiConnected = false;
bool systemInitialized = false;
bool sensorStabilized = false;
bool timeInitialized = false;
int currentWiFiNetwork = 0;
int consecutiveFailures = 0;
int dailyNotificationCount = 0;
int totalMotionEvents = 0;

// Performance monitoring
unsigned long loopStartTime = 0;
unsigned long maxLoopTime = 0;
unsigned long avgLoopTime = 0;
unsigned long loopCount = 0;

// Error tracking
int wifiFailureCount = 0;
int telegramFailureCount = 0;
String lastError = "";
unsigned long lastErrorTime = 0;

// ===================================================================
// FUNCTION PROTOTYPES
// ===================================================================

// Core system functions
void initializeSystem();
void systemLoop();
void performSystemChecks();
void handleWatchdog();

// Network functions
void initializeWiFi();
bool connectToWiFi();
void checkWiFiConnection();
bool tryConnectToNetwork(int networkIndex);
void handleNetworkFailure();
void printNetworkInfo();

// Telegram functions
void initializeTelegram();
bool sendTelegramMessage(const char* chatId, const String& message);
void sendTelegramNotification(const String& message);
void handleTelegramCommands();
void processCommand(const String& chatId, const String& command, const String& fromName);
String formatMessage(const char* templateStr, const char* param1 = "", const char* param2 = "", const char* param3 = "");

// Motion detection functions
void initializeMotionSensor();
void handleMotionDetection();
bool isMotionDetected();
void processMotionEvent();
bool shouldSendNotification();
void updateMotionStatistics();

// Sensor configuration functions
void initializeConfigButton();
void handleSensorConfigMode();
void enterSensorConfigMode();
void exitSensorConfigMode();
void processSensorConfigStep();
void nextConfigStep();
void adjustSensitivity(int direction);
void adjustRange(int direction);
void testSensorSettings();
void saveSensorSettings();
void loadSensorSettings();
void showCurrentSettings();
void applySensorSettings();
unsigned long getSensorDebounceDelay();
unsigned long getMotionCooldownPeriod();
void configModeLEDPattern(int pattern_type, int count = 1);

// Time functions
void initializeTime();
String getCurrentTimeString();
String getUptimeString();
bool isQuietHours();

// LED and status functions
void initializeLED();
void updateStatusLED();
void blinkLED(int times, int delayMs = LED_BLINK_SLOW);
void showSystemStatus();

// System monitoring functions
void checkSystemHealth();
void checkMemoryUsage();
void logSystemPerformance();
void resetDailyCounters();

// Utility functions
void printSystemInfo();
void logMessage(int level, const String& message);
void handleSystemError(const String& error);
bool validateConfiguration();
void saveSystemState();
void loadSystemState();

// Debug functions
#if DEBUG_SERIAL
void debugPrint(const String& message);
void debugPrintln(const String& message);
void printMemoryInfo();
void printWiFiDiagnostics();
#endif

// ===================================================================
// SENSOR CONFIGURATION FUNCTIONS
// ===================================================================

void initializeConfigButton() {
    if (CONFIG_BUTTON_PIN >= 0) {
        pinMode(CONFIG_BUTTON_PIN, INPUT_PULLUP);
        Serial.println("✅ Config button initialized on GPIO " + String(CONFIG_BUTTON_PIN));
    }
    
    // Load saved sensor settings
    loadSensorSettings();
    applySensorSettings();
}

void handleSensorConfigMode() {
    if (CONFIG_BUTTON_PIN < 0) return;
    
    unsigned long currentTime = millis();
    bool buttonPressed = (digitalRead(CONFIG_BUTTON_PIN) == LOW);
    
    if (!sensor_config_mode_active) {
        // Check for button press to enter config mode
        if (buttonPressed && !config_button_held) {
            config_button_press_start = currentTime;
            config_button_held = true;
        } else if (!buttonPressed && config_button_held) {
            config_button_held = false;
        } else if (buttonPressed && config_button_held && 
                   (currentTime - config_button_press_start >= CONFIG_BUTTON_HOLD_TIME)) {
            enterSensorConfigMode();
            config_button_held = false;
        }
    } else {
        // Handle config mode interactions
        if (buttonPressed && (currentTime - last_button_press > 500)) {
            last_button_press = currentTime;
            nextConfigStep();
        }
        
        // Check for config mode timeout
        if (currentTime - config_mode_start_time > CONFIG_MODE_TIMEOUT) {
            exitSensorConfigMode();
        }
        
        // Update config mode LED pattern
        processSensorConfigStep();
    }
}

void enterSensorConfigMode() {
    sensor_config_mode_active = true;
    config_mode_start_time = millis();
    config_step = 0;
    
    Serial.println("\n🔧 ENTERING SENSOR CONFIG MODE");
    Serial.println("Button presses: Next step | Hold to save and exit");
    Serial.println("Steps: 1=Sensitivity, 2=Range, 3=Test, 4=Save");
    
    // LED feedback for entering config mode
    configModeLEDPattern(LED_CONFIG_ENTER, 5);
    
    if (wifiConnected) {
        sendTelegramNotification("🔧 *Sensor Config Mode*\nPress button to cycle through settings.\nHold button to save and exit.");
    }
    
    showCurrentSettings();
}

void exitSensorConfigMode() {
    sensor_config_mode_active = false;
    
    Serial.println("💾 Exiting sensor config mode - saving settings");
    saveSensorSettings();
    applySensorSettings();
    
    // LED feedback for exit
    configModeLEDPattern(LED_CONFIG_EXIT, 3);
    
    if (wifiConnected) {
        String message = "✅ *Config Saved*\n";
        message += "Sensitivity: " + String(current_sensitivity_level) + "/4\n";
        message += "Range: " + String(current_range_setting) + "/2";
        sendTelegramNotification(message);
    }
    
    delay(CONFIG_EXIT_DELAY);
}

void nextConfigStep() {
    config_step = (config_step + 1) % 4;
    
    Serial.println("📍 Config Step: " + String(config_step + 1) + "/4");
    
    switch(config_step) {
        case 0: 
            Serial.println("🎚️ Adjusting SENSITIVITY (0-4): " + String(current_sensitivity_level));
            adjustSensitivity(1);
            break;
        case 1:
            Serial.println("📏 Adjusting RANGE (0-2): " + String(current_range_setting)); 
            adjustRange(1);
            break;
        case 2:
            Serial.println("🧪 TESTING current settings...");
            testSensorSettings();
            break;
        case 3:
            Serial.println("💾 SAVE settings and exit");
            exitSensorConfigMode();
            break;
    }
}

void processSensorConfigStep() {
    static unsigned long lastLEDUpdate = 0;
    unsigned long currentTime = millis();
    
    if (currentTime - lastLEDUpdate < 100) return;
    lastLEDUpdate = currentTime;
    
    switch(config_step) {
        case 0: // Sensitivity adjustment
            configModeLEDPattern(LED_CONFIG_SENSITIVITY);
            break;
        case 1: // Range adjustment  
            configModeLEDPattern(LED_CONFIG_RANGE);
            break;
        case 2: // Testing
            configModeLEDPattern(LED_CONFIG_TEST);
            break;
        case 3: // Save
            configModeLEDPattern(LED_CONFIG_SAVE);
            break;
    }
}

void adjustSensitivity(int direction) {
    int old_sensitivity = current_sensitivity_level;
    current_sensitivity_level += direction;
    
    // Wrap around
    if (current_sensitivity_level < 0) current_sensitivity_level = SENSITIVITY_VERY_HIGH;
    if (current_sensitivity_level > SENSITIVITY_VERY_HIGH) current_sensitivity_level = SENSITIVITY_VERY_LOW;
    
    if (old_sensitivity != current_sensitivity_level) {
        Serial.println("🎚️ Sensitivity: " + String(current_sensitivity_level) + "/4");
        
        String levels[] = {"Very Low", "Low", "Medium", "High", "Very High"};
        Serial.println("   Level: " + levels[current_sensitivity_level]);
        
        // Apply new settings immediately for testing
        applySensorSettings();
        
        // LED feedback - blink count shows level
        configModeLEDPattern(LED_BLINK_FAST, current_sensitivity_level + 1);
    }
}

void adjustRange(int direction) {
    int old_range = current_range_setting;
    current_range_setting += direction;
    
    // Wrap around
    if (current_range_setting < 0) current_range_setting = RANGE_LONG;
    if (current_range_setting > RANGE_LONG) current_range_setting = RANGE_SHORT;
    
    if (old_range != current_range_setting) {
        Serial.println("📏 Range: " + String(current_range_setting) + "/2");
        
        String ranges[] = {"Short", "Medium", "Long"};
        Serial.println("   Range: " + ranges[current_range_setting]);
        
        // Apply new settings immediately for testing
        applySensorSettings();
        
        // LED feedback - blink count shows range setting
        configModeLEDPattern(LED_BLINK_SLOW, current_range_setting + 1);
    }
}

void testSensorSettings() {
    Serial.println("\n🧪 TESTING SENSOR with current settings:");
    showCurrentSettings();
    Serial.println("Move in front of sensor to test detection...");
    Serial.println("Testing for 10 seconds...");
    
    unsigned long testStart = millis();
    int detectionCount = 0;
    
    while (millis() - testStart < 10000 && sensor_config_mode_active) {
        if (isMotionDetected()) {
            detectionCount++;
            Serial.println("✅ Motion detected! (#" + String(detectionCount) + ")");
            configModeLEDPattern(LED_CONFIG_TEST, 3);
            delay(500);
        }
        
        // Fast LED pattern during testing
        configModeLEDPattern(LED_CONFIG_TEST);
        delay(100);
        
        // Check for button press to exit test early
        if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
            delay(200); // Debounce
            if (digitalRead(CONFIG_BUTTON_PIN) == LOW) {
                break;
            }
        }
    }
    
    Serial.println("🏁 Test completed. Detections: " + String(detectionCount));
    
    if (wifiConnected) {
        String message = "🧪 *Sensor Test Results*\n";
        message += "Sensitivity: " + String(current_sensitivity_level) + "/4\n";
        message += "Range: " + String(current_range_setting) + "/2\n"; 
        message += "Detections in 10s: " + String(detectionCount);
        sendTelegramNotification(message);
    }
}

void saveSensorSettings() {
    // Save to preferences/EEPROM (basic implementation for now)
    Serial.println("💾 Saving sensor settings:");
    Serial.println("   Sensitivity: " + String(current_sensitivity_level));
    Serial.println("   Range: " + String(current_range_setting));
    
    // In a real implementation, you'd save to EEPROM/Preferences
    // preferences.putInt("sensitivity", current_sensitivity_level);
    // preferences.putInt("range", current_range_setting);
}

void loadSensorSettings() {
    // Load from preferences/EEPROM (basic implementation for now)
    // current_sensitivity_level = preferences.getInt("sensitivity", DEFAULT_SENSITIVITY);
    // current_range_setting = preferences.getInt("range", DEFAULT_RANGE);
    
    Serial.println("📖 Loaded sensor settings:");
    Serial.println("   Sensitivity: " + String(current_sensitivity_level));
    Serial.println("   Range: " + String(current_range_setting));
}

void showCurrentSettings() {
    Serial.println("\n📊 Current Sensor Settings:");
    
    String levels[] = {"Very Low", "Low", "Medium", "High", "Very High"};
    String ranges[] = {"Short", "Medium", "Long"};
    
    Serial.println("   Sensitivity: " + String(current_sensitivity_level) + "/4 (" + levels[current_sensitivity_level] + ")");
    Serial.println("   Range: " + String(current_range_setting) + "/2 (" + ranges[current_range_setting] + ")");
    Serial.println("   Debounce: " + String(getSensorDebounceDelay()) + "ms");
    Serial.println("   Cooldown: " + String(getMotionCooldownPeriod()) + "ms");
}

void applySensorSettings() {
    // Apply settings immediately by updating the motion detection logic
    Serial.println("⚙️ Applying sensor settings...");
    
    // The actual timing changes are applied through getSensorDebounceDelay() 
    // and getMotionCooldownPeriod() functions that are called in motion detection
}

unsigned long getSensorDebounceDelay() {
    // Adjust debounce delay based on sensitivity
    // Higher sensitivity = shorter debounce (more responsive)
    switch(current_sensitivity_level) {
        case SENSITIVITY_VERY_LOW:  return 3000;  // 3 seconds
        case SENSITIVITY_LOW:       return 2000;  // 2 seconds  
        case SENSITIVITY_MEDIUM:    return 1000;  // 1 second (default)
        case SENSITIVITY_HIGH:      return 500;   // 0.5 seconds
        case SENSITIVITY_VERY_HIGH: return 200;   // 0.2 seconds
        default: return MOTION_DEBOUNCE_DELAY;
    }
}

unsigned long getMotionCooldownPeriod() {
    // Adjust cooldown based on range setting
    // Longer range = longer cooldown period
    switch(current_range_setting) {
        case RANGE_SHORT:  return 5000;   // 5 seconds
        case RANGE_MEDIUM: return 10000;  // 10 seconds (default)
        case RANGE_LONG:   return 20000;  // 20 seconds
        default: return MOTION_COOLDOWN_PERIOD;
    }
}

void configModeLEDPattern(int pattern_type, int count) {
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    unsigned long currentTime = millis();
    
    if (currentTime - lastBlink >= pattern_type) {
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
        lastBlink = currentTime;
        
        // Count-based blinking
        if (count > 1) {
            static int blinkCount = 0;
            if (!ledState) blinkCount++;
            if (blinkCount >= count * 2) {
                blinkCount = 0;
                delay(500); // Pause between sequences
            }
        }
    }
}

// ===================================================================
// ARDUINO SETUP FUNCTION
// ===================================================================

void setup() {
    // Record system start time
    systemStartTime = millis();
    
    // Initialize Serial communication
    Serial.begin(SERIAL_BAUD_RATE);
    delay(STARTUP_DELAY);
    
    // Print startup banner
    Serial.println();
    Serial.println("================================================================");
    Serial.println("        ESP32 MOTION DETECTION SYSTEM v" + String(FIRMWARE_VERSION));
    Serial.println("================================================================");
    Serial.println("Device: " + String(DEVICE_NAME));
    Serial.println("Location: " + String(DEVICE_LOCATION));
    Serial.println("Build Date: " __DATE__ " " __TIME__);
    Serial.println("================================================================");
    
    // Initialize all subsystems
    initializeSystem();
    
    Serial.println("\n🚀 System initialization completed!");
    Serial.println("� Monitoring for motion events...");
    Serial.println("================================================================\n");
}

// ===================================================================
// ARDUINO MAIN LOOP FUNCTION
// ===================================================================

void loop() {
    // Performance monitoring
    #if ENABLE_PERFORMANCE_MONITORING
    loopStartTime = micros();
    #endif
    
    // Main system loop processing
    systemLoop();
    
    // Performance tracking
    #if ENABLE_PERFORMANCE_MONITORING && MONITOR_LOOP_TIME
    unsigned long loopTime = micros() - loopStartTime;
    if (loopTime > maxLoopTime) maxLoopTime = loopTime;
    avgLoopTime = (avgLoopTime * loopCount + loopTime) / (loopCount + 1);
    loopCount++;
    #endif
    
    // Main loop delay
    delay(LOOP_DELAY);
    
    // Feed watchdog
    #if ENABLE_WATCHDOG
    handleWatchdog();
    #endif
}

// ===================================================================
// SYSTEM INITIALIZATION FUNCTIONS
// ===================================================================

void initializeSystem() {
    // Validate configuration
    if (!validateConfiguration()) {
        Serial.println("❌ Configuration validation failed!");
        while(1) delay(1000); // Halt system
    }
    
    // Initialize hardware
    initializeLED();
    initializeMotionSensor();
    initializeConfigButton();
    
    // Initialize network
    initializeWiFi();
    
    // Initialize time
    if (ENABLE_NTP_TIME_SYNC && wifiConnected) {
        initializeTime();
    }
    
    // Initialize Telegram
    if (ENABLE_TELEGRAM_NOTIFICATIONS && wifiConnected) {
        initializeTelegram();
    }
    
    // Initialize watchdog
    #if ENABLE_WATCHDOG
    esp_task_wdt_init(WATCHDOG_TIMEOUT / 1000, true);
    esp_task_wdt_add(NULL);
    #endif
    
    // Print system information
    printSystemInfo();
    
    // Send startup notification
    if (wifiConnected && STARTUP_MESSAGE_ENABLED) {
        String startupMsg = "🚀 *" + String(DEVICE_NAME) + " Online*\n";
        #ifdef USE_SECRETS_FILE
        startupMsg += "📍 " + String(DEVICE_LOCATION_SECRET) + "\n";
        #else
        startupMsg += "📍 " + String(DEVICE_LOCATION) + "\n";
        #endif
        startupMsg += "🌐 IP: " + WiFi.localIP().toString() + "\n";
        startupMsg += "⚡ Firmware: v" + String(FIRMWARE_VERSION);
        sendTelegramNotification(startupMsg);
    }
    
    // Start sensor stabilization period
    sensorStabilizationStart = millis();
    systemInitialized = true;
    
    logMessage(1, "System initialization completed successfully");
}

void systemLoop() {
    unsigned long currentTime = millis();
    
    // Feed watchdog at start of loop
    #if ENABLE_WATCHDOG
    esp_task_wdt_reset();
    #endif
    
    // Check if sensor stabilization period is complete
    if (!sensorStabilized && (currentTime - sensorStabilizationStart) >= SENSOR_STABILIZATION_TIME) {
        sensorStabilized = true;
        logMessage(2, "Motion sensor stabilization completed");
        blinkLED(2, LED_BLINK_FAST);
    }
    
    // Periodic system checks
    if (currentTime - lastSystemCheck >= SYSTEM_STATUS_INTERVAL) {
        performSystemChecks();
        lastSystemCheck = currentTime;
    }
    
    // WiFi connection monitoring
    if (currentTime - lastWiFiCheck >= WIFI_RECONNECT_INTERVAL) {
        checkWiFiConnection();
        lastWiFiCheck = currentTime;
    }
    
    // Memory usage monitoring
    if (currentTime - lastMemoryCheck >= MEMORY_CHECK_INTERVAL) {
        checkMemoryUsage();
        lastMemoryCheck = currentTime;
    }
    
    // Handle sensor configuration mode
    if (ENABLE_SENSOR_CONFIG_MODE) {
        handleSensorConfigMode();
    }
    
    // Handle motion detection (only after stabilization and not in config mode)
    if (sensorStabilized && ENABLE_MOTION_DETECTION && !sensor_config_mode_active) {
        handleMotionDetection();
    }
    
    // Handle Telegram bot commands
    if (wifiConnected && ENABLE_BOT_COMMANDS && (currentTime - lastTimeBotRan) >= BOT_MTBS) {
        #if ENABLE_WATCHDOG
        esp_task_wdt_reset(); // Reset before potentially long operation
        #endif
        handleTelegramCommands();
        lastTimeBotRan = currentTime;
        #if ENABLE_WATCHDOG
        esp_task_wdt_reset(); // Reset after potentially long operation
        #endif
    }
    
    // Send heartbeat message
    if (HEARTBEAT_MESSAGE_ENABLED && wifiConnected && 
        (currentTime - lastHeartbeat) >= HEARTBEAT_INTERVAL) {
        sendTelegramNotification("💓 System heartbeat - " + getUptimeString());
        lastHeartbeat = currentTime;
    }
    
    // Update status LED
    updateStatusLED();
    
    // Performance logging
    #if ENABLE_PERFORMANCE_MONITORING
    if ((currentTime % PERFORMANCE_LOG_INTERVAL) < LOOP_DELAY) {
        logSystemPerformance();
    }
    #endif
    
    // Reset daily counters at midnight
    static int lastDay = -1;
    if (timeInitialized) {
        int currentDay = timeClient.getDay();
        if (currentDay != lastDay && RESET_COUNTER_DAILY) {
            resetDailyCounters();
            lastDay = currentDay;
        }
    }
}

void performSystemChecks() {
    checkSystemHealth();
    
    #if LOG_SYSTEM_STATUS
    logMessage(3, "System check: Uptime=" + getUptimeString() + 
                  ", Memory=" + String(ESP.getFreeHeap()) + 
                  ", WiFi=" + String(WiFi.RSSI()) + "dBm");
    #endif
}

// ===================================================================
// NETWORK FUNCTIONS
// ===================================================================

void initializeWiFi() {
    Serial.println("🌐 Initializing WiFi...");
    
    WiFi.mode(WIFI_STA);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    #ifdef USE_SECRETS_FILE
    // Set hostname
    WiFi.setHostname(DEVICE_ID);
    
    // Configure static IP if enabled
    if (STATIC_IP.enabled) {
        IPAddress local_IP, gateway, subnet, primaryDNS, secondaryDNS;
        local_IP.fromString(STATIC_IP.ip);
        gateway.fromString(STATIC_IP.gateway);
        subnet.fromString(STATIC_IP.subnet);
        primaryDNS.fromString(STATIC_IP.dns1);
        secondaryDNS.fromString(STATIC_IP.dns2);
        
        if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
            Serial.println("⚠️ Static IP configuration failed");
        }
    }
    #endif
    
    // Attempt to connect to WiFi
    wifiConnected = connectToWiFi();
    
    if (wifiConnected) {
        printNetworkInfo();
    } else {
        handleNetworkFailure();
    }
}

bool connectToWiFi() {
    #ifdef USE_SECRETS_FILE
    // Try multiple networks if configured
    for (int attempt = 0; attempt < WIFI_MAX_RETRIES && !wifiConnected; attempt++) {
        for (int i = 0; i < WIFI_NETWORK_COUNT && !wifiConnected; i++) {
            if (WIFI_NETWORKS[i].enabled && strlen(WIFI_NETWORKS[i].ssid) > 0) {
                if (tryConnectToNetwork(i)) {
                    currentWiFiNetwork = i;
                    wifiConnected = true;
                    consecutiveFailures = 0;
                    return true;
                }
            }
        }
        delay(WIFI_RETRY_DELAY);
    }
    #else
    // Single network connection
    return tryConnectToNetwork(0);
    #endif
    
    return false;
}

bool tryConnectToNetwork(int networkIndex) {
    #ifdef USE_SECRETS_FILE
    const char* ssid = (networkIndex < WIFI_NETWORK_COUNT) ? 
                       WIFI_NETWORKS[networkIndex].ssid : WIFI_SSID_SECRET;
    const char* password = (networkIndex < WIFI_NETWORK_COUNT) ? 
                           WIFI_NETWORKS[networkIndex].password : WIFI_PASSWORD_SECRET;
    #else
    const char* ssid = WIFI_SSID;
    const char* password = WIFI_PASSWORD;
    #endif
    
    if (strlen(ssid) == 0) return false;
    
    Serial.println("📡 Connecting to: " + String(ssid));
    
    WiFi.begin(ssid, password);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime) < WIFI_TIMEOUT) {
        delay(500);
        Serial.print(".");
        blinkLED(1, LED_BLINK_FAST);
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ WiFi connected successfully!");
        wifiFailureCount = 0;
        return true;
    } else {
        Serial.println("\n❌ Failed to connect to: " + String(ssid));
        wifiFailureCount++;
        return false;
    }
}

void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED && wifiConnected) {
        Serial.println("⚠️ WiFi connection lost! Attempting to reconnect...");
        wifiConnected = false;
        blinkLED(3, LED_BLINK_ERROR);
        
        if (connectToWiFi()) {
            Serial.println("✅ WiFi reconnected successfully");
            sendTelegramNotification("🔄 WiFi reconnected - " + WiFi.localIP().toString());
        } else {
            handleNetworkFailure();
        }
    }
    
    // Check signal strength
    if (wifiConnected && WiFi.RSSI() < MIN_WIFI_SIGNAL_STRENGTH) {
        logMessage(2, "⚠️ Weak WiFi signal: " + String(WiFi.RSSI()) + " dBm");
    }
}

void handleNetworkFailure() {
    consecutiveFailures++;
    wifiConnected = false;
    
    String errorMsg = "❌ Network failure #" + String(consecutiveFailures);
    logMessage(1, errorMsg);
    
    if (consecutiveFailures >= MAX_FAILED_ATTEMPTS) {
        Serial.println("🚨 Maximum network failures reached. Entering lockout mode.");
        // Could implement lockout or restart logic here
        delay(LOCKOUT_DURATION);
        consecutiveFailures = 0;
    }
    
    blinkLED(5, LED_BLINK_ERROR);
}

void printNetworkInfo() {
    Serial.println("\n📶 Network Information:");
    Serial.println("SSID: " + WiFi.SSID());
    Serial.println("IP Address: " + WiFi.localIP().toString());
    Serial.println("Gateway: " + WiFi.gatewayIP().toString());
    Serial.println("DNS: " + WiFi.dnsIP().toString());
    Serial.println("MAC Address: " + WiFi.macAddress());
    Serial.println("Signal Strength: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("Channel: " + String(WiFi.channel()));
}

// ===================================================================
// TELEGRAM FUNCTIONS
// ===================================================================

void initializeTelegram() {
    Serial.println("📱 Initializing Telegram Bot...");
    
    // Configure SSL client
    client.setInsecure(); // For simplicity, not verifying SSL certificate
    // In production, you should use: client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
    
    // Initialize bot
    #ifdef USE_SECRETS_FILE
    bot = new UniversalTelegramBot(BOT_TOKEN_SECRET, client);
    #else
    bot = new UniversalTelegramBot(BOT_TOKEN, client);
    #endif
    
    if (bot) {
        Serial.println("✅ Telegram Bot initialized successfully");
        
        // Configure HTTP timeout for bot
        bot->longPoll = 0;  // Disable long polling to prevent timeouts
        
        // Test bot connection with a simple API call
        Serial.println("🤖 Bot initialized with token");
        logMessage(2, "Telegram bot connection ready");
    } else {
        Serial.println("❌ Failed to initialize Telegram Bot");
        telegramFailureCount++;
    }
}

bool sendTelegramMessage(const char* chatId, const String& message) {
    if (!wifiConnected || !bot || strlen(chatId) == 0) {
        return false;
    }
    
    #if LOG_TELEGRAM_MESSAGES
    logMessage(3, "Sending Telegram message to " + String(chatId));
    #endif
    
    for (int attempt = 0; attempt < BOT_RETRY_ATTEMPTS; attempt++) {
        #if MONITOR_NETWORK_PERFORMANCE
        unsigned long startTime = millis();
        #endif
        
        // Reset watchdog before potentially blocking HTTP call
        #if ENABLE_WATCHDOG
        esp_task_wdt_reset();
        #endif
        
        bool result = bot->sendMessage(String(chatId), message, MESSAGE_PARSE_MODE);
        
        // Reset watchdog after HTTP call
        #if ENABLE_WATCHDOG
        esp_task_wdt_reset();
        #endif
        
        #if MONITOR_NETWORK_PERFORMANCE
        unsigned long duration = millis() - startTime;
        if (duration > HTTP_TIMEOUT / 2) {
            logMessage(2, "Slow Telegram response: " + String(duration) + "ms");
        }
        #endif
        
        if (result) {
            telegramFailureCount = 0;
            return true;
        } else {
            telegramFailureCount++;
            logMessage(2, "Telegram send attempt " + String(attempt + 1) + " failed");
            
            if (attempt < BOT_RETRY_ATTEMPTS - 1) {
                delay(BOT_RETRY_DELAY);
            }
        }
    }
    
    logMessage(1, "Failed to send Telegram message after " + String(BOT_RETRY_ATTEMPTS) + " attempts");
    return false;
}

void sendTelegramNotification(const String& message) {
    if (!ENABLE_TELEGRAM_NOTIFICATIONS || !wifiConnected) {
        return;
    }
    
    String finalMessage = message;
    
    // Add timestamp if enabled
    #if ENABLE_TIMESTAMP_IN_MESSAGES
    if (timeInitialized) {
        finalMessage = "🕐 " + getCurrentTimeString() + "\n" + finalMessage;
    } else {
        finalMessage = "⏱️ " + String(millis() / 1000) + "s | " + finalMessage;
    }
    #endif
    
    // Add device info if enabled
    #if ENABLE_DEVICE_INFO_IN_MESSAGES
    finalMessage += "\n📱 " + String(DEVICE_NAME);
    #endif
    
    #ifdef USE_SECRETS_FILE
    // Send to multiple chats based on configuration
    bool sentToAny = false;
    for (int i = 0; i < TELEGRAM_CHAT_COUNT; i++) {
        if (TELEGRAM_CHATS[i].enabled && TELEGRAM_CHATS[i].motion_alerts) {
            if (sendTelegramMessage(TELEGRAM_CHATS[i].chat_id, finalMessage)) {
                sentToAny = true;
                logMessage(3, "Notification sent to " + String(TELEGRAM_CHATS[i].name));
            }
        }
    }
    
    if (sentToAny) {
        Serial.println("✅ Notification sent successfully");
    } else {
        Serial.println("❌ Failed to send notification to any chat");
    }
    #else
    // Single chat mode
    #ifdef USE_SECRETS_FILE
    if (sendTelegramMessage(CHAT_ID_SECRET, finalMessage)) {
    #else
    if (sendTelegramMessage(CHAT_ID, finalMessage)) {
    #endif
        Serial.println("✅ Notification sent successfully");
    } else {
        Serial.println("❌ Failed to send notification");
    }
    #endif
}

void handleTelegramCommands() {
    if (!bot || !wifiConnected || !ENABLE_BOT_COMMANDS) {
        return;
    }
    
    // Reset watchdog before potentially blocking HTTP call
    #if ENABLE_WATCHDOG
    esp_task_wdt_reset();
    #endif
    
    int numNewMessages = bot->getUpdates(bot->last_message_received + 1);
    
    // Reset watchdog after HTTP call
    #if ENABLE_WATCHDOG
    esp_task_wdt_reset();
    #endif
    
    for (int i = 0; i < numNewMessages; i++) {
        String chatId = String(bot->messages[i].chat_id);
        String text = bot->messages[i].text;
        String fromName = bot->messages[i].from_name;
        
        #if LOG_TELEGRAM_MESSAGES
        logMessage(3, "Command from " + fromName + " (" + chatId + "): " + text);
        #endif
        
        // Check authorization if enabled
        #ifdef USE_SECRETS_FILE
        bool authorized = false;
        for (int j = 0; j < AUTHORIZED_USER_COUNT; j++) {
            if (chatId == String(AUTHORIZED_USERS[j]) && strlen(AUTHORIZED_USERS[j]) > 0) {
                authorized = true;
                break;
            }
        }
        
        if (strlen(AUTHORIZED_USERS[0]) > 0 && !authorized) {
            bot->sendMessage(chatId, "❌ Unauthorized access denied", "");
            logMessage(2, "Unauthorized command attempt from " + fromName);
            continue;
        }
        #endif
        
        processCommand(chatId, text, fromName);
    }
}

void processCommand(const String& chatId, const String& command, const String& fromName) {
    String response = "";
    
    if (command == "/status" || command.startsWith("/status@")) {
        response = "📊 *System Status*\n";
        #ifdef USE_SECRETS_FILE
        response += "📍 " + String(DEVICE_LOCATION_SECRET) + "\n";
        #else
        response += "📍 " + String(DEVICE_LOCATION) + "\n";
        #endif
        response += "🔋 Uptime: " + getUptimeString() + "\n";
        response += "💾 Memory: " + String(ESP.getFreeHeap()) + " bytes\n";
        response += "📶 WiFi: " + String(WiFi.RSSI()) + " dBm";
        
        response += "\n🔢 Motion Events: " + String(totalMotionEvents);
        response += "\n📊 Daily Notifications: " + String(dailyNotificationCount);
        #ifdef SOC_TEMP_SENSOR_SUPPORTED
        response += "\n🌡️ CPU Temp: " + String(temperatureRead()) + "°C";
        #endif
        
    } else if (command == "/test" || command.startsWith("/test@")) {
        response = "🧪 *Test Message*\n";
        response += "Device: " + String(DEVICE_NAME) + "\n";
        response += "Location: " + String(DEVICE_LOCATION) + "\n";
        response += "Time: " + getCurrentTimeString();
        
    } else if (command == "/help" || command.startsWith("/help@")) {
        response = "🤖 *Available Commands:*\n";
        response += "/status - Show system status\n";
        response += "/test - Send test message\n";
        response += "/stats - Show statistics\n";
        response += "/reset - Reset counters\n";
        response += "/reboot - Restart device\n";
        response += "/sensor_config - Enter sensor config mode\n";
        response += "/sensitivity [0-4] - Set sensor sensitivity\n";
        response += "/range [0-2] - Set sensor range\n";
        response += "/test_sensor - Test current sensor settings\n";
        response += "/show_settings - Show current sensor settings\n";
        response += "/help - Show this help\n";
        
    } else if (command == "/stats" || command.startsWith("/stats@")) {
        response = "📈 *System Statistics:*\n";
        response += "Total Motion Events: " + String(totalMotionEvents) + "\n";
        response += "Daily Notifications: " + String(dailyNotificationCount) + "\n";
        response += "WiFi Failures: " + String(wifiFailureCount) + "\n";
        response += "Telegram Failures: " + String(telegramFailureCount) + "\n";
        response += "Free Memory: " + String(ESP.getFreeHeap()) + " bytes\n";
        response += "Max Loop Time: " + String(maxLoopTime) + " μs\n";
        response += "Avg Loop Time: " + String(avgLoopTime) + " μs";
        
    } else if (command == "/reset" || command.startsWith("/reset@")) {
        resetDailyCounters();
        response = "🔄 *Counters Reset*\nDaily statistics have been reset.";
        
    } else if (command == "/reboot" || command.startsWith("/reboot@")) {
        response = "🔄 *Rebooting System*\nDevice will restart in 5 seconds...";
        bot->sendMessage(chatId, response, MESSAGE_PARSE_MODE);
        delay(5000);
        ESP.restart();
        
    } else if (command == "/info" || command.startsWith("/info@")) {
        response = "ℹ️ *Device Information:*\n";
        response += "Model: " + String(ESP.getChipModel()) + "\n";
        response += "Revision: " + String(ESP.getChipRevision()) + "\n";
        response += "CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz\n";
        response += "Flash: " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB\n";
        response += "SDK: " + String(ESP.getSdkVersion()) + "\n";
        response += "MAC: " + WiFi.macAddress();
        
    } else if (command == "/sensor_config" || command.startsWith("/sensor_config@")) {
        if (!sensor_config_mode_active) {
            enterSensorConfigMode();
            response = "🔧 *Sensor Config Mode Activated*\nUse physical button or /sensitivity and /range commands to adjust settings.";
        } else {
            response = "⚠️ Sensor config mode already active.";
        }
        
    } else if (command.startsWith("/sensitivity") && !command.startsWith("/sensitivity@")) {
        String paramStr = command.substring(12);
        paramStr.trim();
        
        if (paramStr.length() > 0) {
            int newSensitivity = paramStr.toInt();
            if (newSensitivity >= SENSITIVITY_VERY_LOW && newSensitivity <= SENSITIVITY_VERY_HIGH) {
                current_sensitivity_level = newSensitivity;
                applySensorSettings();
                saveSensorSettings();
                
                String levels[] = {"Very Low", "Low", "Medium", "High", "Very High"};
                response = "🎚️ *Sensitivity Updated*\n";
                response += "Level: " + String(newSensitivity) + "/4 (" + levels[newSensitivity] + ")\n";
                response += "Debounce: " + String(getSensorDebounceDelay()) + "ms";
            } else {
                response = "❌ Invalid sensitivity level. Use 0-4.";
            }
        } else {
            String levels[] = {"Very Low", "Low", "Medium", "High", "Very High"};
            response = "🎚️ *Current Sensitivity*\n";
            response += "Level: " + String(current_sensitivity_level) + "/4 (" + levels[current_sensitivity_level] + ")\n";
            response += "Use `/sensitivity [0-4]` to change.";
        }
        
    } else if (command.startsWith("/range") && !command.startsWith("/range@")) {
        String paramStr = command.substring(6);
        paramStr.trim();
        
        if (paramStr.length() > 0) {
            int newRange = paramStr.toInt();
            if (newRange >= RANGE_SHORT && newRange <= RANGE_LONG) {
                current_range_setting = newRange;
                applySensorSettings();
                saveSensorSettings();
                
                String ranges[] = {"Short", "Medium", "Long"};
                response = "📏 *Range Updated*\n";
                response += "Setting: " + String(newRange) + "/2 (" + ranges[newRange] + ")\n";
                response += "Cooldown: " + String(getMotionCooldownPeriod()) + "ms";
            } else {
                response = "❌ Invalid range setting. Use 0-2.";
            }
        } else {
            String ranges[] = {"Short", "Medium", "Long"};
            response = "📏 *Current Range*\n";
            response += "Setting: " + String(current_range_setting) + "/2 (" + ranges[current_range_setting] + ")\n";
            response += "Use `/range [0-2]` to change.";
        }
        
    } else if (command == "/test_sensor" || command.startsWith("/test_sensor@")) {
        response = "🧪 *Starting Sensor Test*\nMove in front of sensor for 10 seconds...";
        bot->sendMessage(chatId, response, MESSAGE_PARSE_MODE);
        
        // Run the sensor test
        testSensorSettings();
        return; // Test function sends its own results
        
    } else if (command == "/show_settings" || command.startsWith("/show_settings@")) {
        String levels[] = {"Very Low", "Low", "Medium", "High", "Very High"};
        String ranges[] = {"Short", "Medium", "Long"};
        
        response = "⚙️ *Current Sensor Settings*\n";
        response += "🎚️ Sensitivity: " + String(current_sensitivity_level) + "/4 (" + levels[current_sensitivity_level] + ")\n";
        response += "📏 Range: " + String(current_range_setting) + "/2 (" + ranges[current_range_setting] + ")\n";
        response += "⏱️ Debounce: " + String(getSensorDebounceDelay()) + "ms\n";
        response += "🕐 Cooldown: " + String(getMotionCooldownPeriod()) + "ms\n";
        
        if (sensor_config_mode_active) {
            response += "\n🔧 Config mode is currently active";
        }
        
    } else {
        response = "❓ Unknown command: " + command + "\nSend /help for available commands.";
    }
    
    if (response.length() > 0) {
        sendTelegramMessage(chatId.c_str(), response);
    }
}

String formatMessage(const char* templateStr, const char* param1, const char* param2, const char* param3) {
    String formatted = String(templateStr);
    formatted.replace("%s", String(param1));
    if (strlen(param2) > 0) {
        formatted.replace("%s", String(param2));
    }
    if (strlen(param3) > 0) {
        formatted.replace("%s", String(param3));
    }
    return formatted;
}

// ===================================================================
// MOTION DETECTION FUNCTIONS
// ===================================================================

void initializeMotionSensor() {
    Serial.println("🎯 Initializing motion sensor...");
    
    pinMode(MOTION_SENSOR_PIN, INPUT);
    
    // Additional sensor pins if configured
    #if TEMPERATURE_SENSOR_PIN >= 0
    pinMode(TEMPERATURE_SENSOR_PIN, INPUT);
    #endif
    
    #if LIGHT_SENSOR_PIN >= 0
    pinMode(LIGHT_SENSOR_PIN, INPUT);
    #endif
    
    Serial.println("✅ Motion sensor initialized on GPIO " + String(MOTION_SENSOR_PIN));
    logMessage(2, "Motion sensor stabilizing for " + String(SENSOR_STABILIZATION_TIME/1000) + " seconds");
}

void handleMotionDetection() {
    if (!MOTION_DETECTION_ENABLED || !sensorStabilized) {
        return;
    }
    
    bool currentMotionState = isMotionDetected();
    unsigned long currentTime = millis();
    
    if (currentMotionState) {
        // Motion detected
        if (!motionDetected) {
            // New motion event
            motionDetected = true;
            lastMotionTime = currentTime;
            
            if (!motionSessionActive) {
                // Start new motion session
                motionSessionActive = true;
                motionSessionStart = currentTime;
                motionSessionNotified = false;
                
                #if LOG_MOTION_EVENTS
                logMessage(2, "🚨 Motion session started!");
                #endif
                
                // Send notification for new session
                if (shouldSendNotification()) {
                    processMotionEvent();
                    motionSessionNotified = true;
                }
            } else {
                // Continue existing session
                #if LOG_MOTION_EVENTS
                logMessage(3, "📍 Motion continues in session");
                #endif
            }
        }
        // Update last motion time
        lastMotionTime = currentTime;
        
    } else {
        // No motion detected
        if (motionDetected) {
            // Motion just stopped
            motionDetected = false;
            lastMotionEnd = currentTime;
            #if LOG_MOTION_EVENTS
            logMessage(3, "Motion stopped");
            #endif
        }
        
        // Check if motion session should end (using configurable cooldown period)
        if (motionSessionActive && (currentTime - lastMotionEnd >= getMotionCooldownPeriod())) {
            motionSessionActive = false;
            unsigned long sessionDuration = (currentTime - motionSessionStart) / 1000;
            #if LOG_MOTION_EVENTS
            logMessage(2, "🏁 Motion session ended (Duration: " + String(sessionDuration) + "s)");
            #endif
        }
        
        updateStatusLED();
    }
}

bool isMotionDetected() {
    int sensorValue = digitalRead(MOTION_SENSOR_PIN);
    return (sensorValue == MOTION_ACTIVE_STATE);
}

void processMotionEvent() {
    unsigned long currentTime = millis();
    
    #if LOG_MOTION_EVENTS
    logMessage(2, "🚨 Motion detected!");
    #endif
    
    // Update statistics
    updateMotionStatistics();
    
    // Visual indication
    blinkLED(5, LED_BLINK_MOTION);
    
    // Check if notification should be sent
    if (shouldSendNotification()) {
        // Minimal payload for fastest API call
        String motionMessage = ".";
        
        sendTelegramNotification(motionMessage);
        lastNotificationTime = currentTime;
        dailyNotificationCount++;
        
        #if LOG_MOTION_EVENTS
        logMessage(2, "Motion notification sent (Daily: " + String(dailyNotificationCount) + ")");
        #endif
    } else {
        #if LOG_MOTION_EVENTS
        logMessage(3, "Motion notification suppressed");
        #endif
    }
}

bool shouldSendNotification() {
    unsigned long currentTime = millis();
    
    // Don't send if session already notified
    if (motionSessionNotified) {
        return false;
    }
    
    // Check minimum interval between notifications (use configurable debounce)
    unsigned long notificationInterval = max((unsigned long)NOTIFICATION_INTERVAL, getSensorDebounceDelay());
    if (currentTime - lastNotificationTime < notificationInterval) {
        return false;
    }
    
    // Check daily notification limit
    if (dailyNotificationCount >= MAX_NOTIFICATIONS_PER_HOUR && MAX_NOTIFICATIONS_PER_HOUR > 0) {
        return false;
    }
    
    // Check quiet hours
    if (QUIET_HOURS_ENABLED && isQuietHours()) {
        return false;
    }
    
    // Check if WiFi is connected
    if (!wifiConnected) {
        return false;
    }
    
    return true;
}

void updateMotionStatistics() {
    totalMotionEvents++;
    
    #if ENABLE_MOTION_COUNTER
    // Could save to EEPROM/Flash for persistence across reboots
    // preferences.putInt("motionCount", totalMotionEvents);
    #endif
}

bool isQuietHours() {
    if (!timeInitialized || !QUIET_HOURS_ENABLED) {
        return false;
    }
    
    int currentHour = timeClient.getHours();
    
    if (QUIET_START_HOUR < QUIET_END_HOUR) {
        // Same day quiet hours (e.g., 22:00 to 06:00 next day)
        return (currentHour >= QUIET_START_HOUR || currentHour < QUIET_END_HOUR);
    } else {
        // Cross-day quiet hours (e.g., 10:00 to 14:00 same day)
        return (currentHour >= QUIET_START_HOUR && currentHour < QUIET_END_HOUR);
    }
}

// ===================================================================
// TIME FUNCTIONS
// ===================================================================

void initializeTime() {
    Serial.println("🕐 Initializing NTP time sync...");
    
    timeClient.begin();
    timeClient.setTimeOffset(TIMEZONE_OFFSET * 3600);
    
    int attempts = 0;
    while (!timeClient.update() && attempts < 5) {
        delay(1000);
        attempts++;
        Serial.print(".");
    }
    
    if (timeClient.isTimeSet()) {
        timeInitialized = true;
        Serial.println("\n✅ Time synchronized: " + getCurrentTimeString());
        logMessage(2, "NTP time sync successful");
    } else {
        Serial.println("\n⚠️ Failed to sync time with NTP server");
        logMessage(2, "NTP time sync failed");
    }
}

String getCurrentTimeString() {
    if (!timeInitialized) {
        unsigned long seconds = millis() / 1000;
        unsigned long hours = seconds / 3600;
        unsigned long minutes = (seconds % 3600) / 60;
        seconds = seconds % 60;
        
        char timeStr[32];
        sprintf(timeStr, "%02lu:%02lu:%02lu", hours, minutes, seconds);
        return String(timeStr);
    }
    
    timeClient.update();
    return timeClient.getFormattedTime();
}

String getUptimeString() {
    unsigned long uptime = millis() - systemStartTime;
    unsigned long days = uptime / 86400000;
    unsigned long hours = (uptime % 86400000) / 3600000;
    unsigned long minutes = (uptime % 3600000) / 60000;
    
    String uptimeStr = "";
    if (days > 0) {
        uptimeStr += String(days) + "d ";
    }
    if (hours > 0 || days > 0) {
        uptimeStr += String(hours) + "h ";
    }
    uptimeStr += String(minutes) + "m";
    
    return uptimeStr;
}

// ===================================================================
// LED AND STATUS FUNCTIONS
// ===================================================================

void initializeLED() {
    pinMode(LED_PIN, OUTPUT);
    
    #if EXTERNAL_LED_PIN >= 0
    pinMode(EXTERNAL_LED_PIN, OUTPUT);
    #endif
    
    #if BUZZER_PIN >= 0
    pinMode(BUZZER_PIN, OUTPUT);
    #endif
    
    // Initial LED test
    blinkLED(3, LED_BLINK_FAST);
    digitalWrite(LED_PIN, LOW);
    
    Serial.println("✅ LED initialized on GPIO " + String(LED_PIN));
}

void updateStatusLED() {
    if (!STATUS_LED_ENABLED) {
        return;
    }
    
    static unsigned long lastLEDUpdate = 0;
    static bool ledState = false;
    unsigned long currentTime = millis();
    
    if (!wifiConnected) {
        // Blink slowly when WiFi disconnected
        if (currentTime - lastLEDUpdate >= LED_BLINK_SLOW) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
            lastLEDUpdate = currentTime;
        }
    } else if (motionDetected) {
        // Blink fast during motion
        if (currentTime - lastLEDUpdate >= LED_BLINK_MOTION) {
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState);
            lastLEDUpdate = currentTime;
        }
    } else {
        // Solid on when connected and no motion
        digitalWrite(LED_PIN, HIGH);
    }
}

void blinkLED(int times, int delayMs) {
    bool originalState = digitalRead(LED_PIN);
    
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_PIN, HIGH);
        #if EXTERNAL_LED_PIN >= 0
        digitalWrite(EXTERNAL_LED_PIN, HIGH);
        #endif
        
        delay(delayMs);
        
        digitalWrite(LED_PIN, LOW);
        #if EXTERNAL_LED_PIN >= 0
        digitalWrite(EXTERNAL_LED_PIN, LOW);
        #endif
        
        delay(delayMs);
    }
    
    // Restore original state
    digitalWrite(LED_PIN, originalState);
}

// ===================================================================
// SYSTEM MONITORING FUNCTIONS
// ===================================================================

void checkSystemHealth() {
    // Check free memory
    size_t freeHeap = ESP.getFreeHeap();
    if (freeHeap < MIN_FREE_MEMORY) {
        String errorMsg = "⚠️ Low memory warning: " + String(freeHeap) + " bytes";
        logMessage(1, errorMsg);
        handleSystemError("LOW_MEMORY");
    }
    
    // Check WiFi signal strength
    if (wifiConnected && WiFi.RSSI() < MIN_WIFI_SIGNAL_STRENGTH) {
        logMessage(2, "⚠️ Weak WiFi signal: " + String(WiFi.RSSI()) + " dBm");
    }
    
    // Check error counts
    if (telegramFailureCount > 10) {
        handleSystemError("TELEGRAM_FAILURES");
    }
    
    if (wifiFailureCount > 5) {
        handleSystemError("WIFI_FAILURES");
    }
}

void checkMemoryUsage() {
    size_t freeHeap = ESP.getFreeHeap();
    size_t minFreeHeap = ESP.getMinFreeHeap();
    
    #if ENABLE_MEMORY_DEBUG
    logMessage(4, "Memory - Free: " + String(freeHeap) + ", Min: " + String(minFreeHeap));
    #endif
    
    if (freeHeap < MIN_FREE_MEMORY) {
        // Attempt garbage collection
        Serial.println("🧹 Attempting memory cleanup...");
        // Force garbage collection if available
        yield();
        delay(100);
    }
}

void logSystemPerformance() {
    #if ENABLE_PERFORMANCE_MONITORING
    Serial.println("\n📊 Performance Statistics:");
    Serial.println("Max Loop Time: " + String(maxLoopTime) + " μs");
    Serial.println("Avg Loop Time: " + String(avgLoopTime) + " μs");
    Serial.println("Total Loops: " + String(loopCount));
    Serial.println("Free Memory: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
    
    // Reset max loop time
    maxLoopTime = 0;
    #endif
}

void resetDailyCounters() {
    dailyNotificationCount = 0;
    wifiFailureCount = 0;
    telegramFailureCount = 0;
    
    logMessage(2, "� Daily counters reset");
    
    #if !PRODUCTION_MODE
    if (wifiConnected) {
        sendTelegramNotification("📅 Daily statistics reset - New day started!");
    }
    #endif
}

void handleWatchdog() {
    #if ENABLE_WATCHDOG
    esp_task_wdt_reset();
    #endif
}

// ===================================================================
// UTILITY FUNCTIONS
// ===================================================================

void printSystemInfo() {
    Serial.println("\n" + String('=', 60));
    Serial.println("                 SYSTEM INFORMATION");
    Serial.println(String('=', 60));
    Serial.println("Device: " + String(DEVICE_NAME));
    Serial.println("Location: " + String(DEVICE_LOCATION));
    Serial.println("Firmware: v" + String(FIRMWARE_VERSION));
    Serial.println("Hardware: v" + String(HARDWARE_VERSION));
    Serial.println("Build: " __DATE__ " " __TIME__);
    Serial.println();
    Serial.println("ESP32 Info:");
    Serial.println("  Chip: " + String(ESP.getChipModel()) + " Rev " + String(ESP.getChipRevision()));
    Serial.println("  CPU Freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
    Serial.println("  Flash: " + String(ESP.getFlashChipSize() / 1024 / 1024) + " MB");
    Serial.println("  Free Memory: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("  SDK: " + String(ESP.getSdkVersion()));
    Serial.println();
    Serial.println("Configuration:");
    Serial.println("  Motion Pin: GPIO " + String(MOTION_SENSOR_PIN));
    Serial.println("  LED Pin: GPIO " + String(LED_PIN));
    Serial.println("  Notification Interval: " + String(NOTIFICATION_INTERVAL/1000) + "s");
    Serial.println("  Debug Level: " + String(DEBUG_LEVEL));
    Serial.println(String('=', 60) + "\n");
}

void logMessage(int level, const String& message) {
    if (level > DEBUG_LEVEL) {
        return;
    }
    
    String levelStr = "";
    switch (level) {
        case 1: levelStr = "[ERROR] "; break;
        case 2: levelStr = "[WARN]  "; break;
        case 3: levelStr = "[INFO]  "; break;
        case 4: levelStr = "[DEBUG] "; break;
        default: levelStr = "[LOG]   "; break;
    }
    
    #if DEBUG_SERIAL
    String timestamp = "";
    #if ENABLE_TIMESTAMP_IN_MESSAGES && defined(LOG_TIMESTAMP_FORMAT)
    if (timeInitialized) {
        timeClient.update();
        char timeStr[32];
        sprintf(timeStr, LOG_TIMESTAMP_FORMAT, 
                timeClient.getHours(), 
                timeClient.getMinutes(), 
                timeClient.getSeconds());
        timestamp = String(timeStr);
    }
    #endif
    
    Serial.println(timestamp + levelStr + message);
    #endif
}

void handleSystemError(const String& error) {
    lastError = error;
    lastErrorTime = millis();
    
    logMessage(1, "System Error: " + error);
    
    // Send error notification if enabled
    if (wifiConnected && ENABLE_TELEGRAM_NOTIFICATIONS) {
        String errorMsg = formatMessage(
            #ifdef USE_SECRETS_FILE
            ERROR_MESSAGE_TEMPLATE,
            #else
            "❌ *System Error*\n📍 %s\n🔍 Error: %s\n🕐 Time: %s",
            #endif
            DEVICE_LOCATION,
            error.c_str(),
            getCurrentTimeString().c_str()
        );
        
        sendTelegramNotification(errorMsg);
    }
    
    // Handle specific errors
    if (error == "LOW_MEMORY") {
        // Attempt memory cleanup
        checkMemoryUsage();
    } else if (error == "TELEGRAM_FAILURES") {
        // Reset Telegram connection
        telegramFailureCount = 0;
        initializeTelegram();
    } else if (error == "WIFI_FAILURES") {
        // Reset WiFi connection
        wifiFailureCount = 0;
        initializeWiFi();
    }
}

bool validateConfiguration() {
    bool valid = true;
    
    // Validate pin assignments
    if (MOTION_SENSOR_PIN < 0 || MOTION_SENSOR_PIN > 39) {
        Serial.println("❌ Invalid motion sensor pin: " + String(MOTION_SENSOR_PIN));
        valid = false;
    }
    
    if (LED_PIN < 0 || LED_PIN > 39) {
        Serial.println("❌ Invalid LED pin: " + String(LED_PIN));
        valid = false;
    }
    
    // Validate timing parameters
    if (NOTIFICATION_INTERVAL < 1000) {
        Serial.println("❌ Notification interval too short: " + String(NOTIFICATION_INTERVAL));
        valid = false;
    }
    
    if (WIFI_TIMEOUT < 5000) {
        Serial.println("❌ WiFi timeout too short: " + String(WIFI_TIMEOUT));
        valid = false;
    }
    
    // Validate credentials (basic check)
    #ifdef USE_SECRETS_FILE
    if (!isValidBotToken(BOT_TOKEN)) {
        Serial.println("❌ Invalid bot token format");
        valid = false;
    }
    
    if (!isValidWiFiCredentials(WIFI_SSID, WIFI_PASSWORD)) {
        Serial.println("❌ Invalid WiFi credentials");
        valid = false;
    }
    #endif
    
    return valid;
}

#if DEBUG_SERIAL
void debugPrint(const String& message) {
    Serial.print(message);
}

void debugPrintln(const String& message) {
    Serial.println(message);
}

void printMemoryInfo() {
    Serial.println("💾 Memory Info:");
    Serial.println("  Free Heap: " + String(ESP.getFreeHeap()) + " bytes");
    Serial.println("  Min Free Heap: " + String(ESP.getMinFreeHeap()) + " bytes");
    Serial.println("  Heap Size: " + String(ESP.getHeapSize()) + " bytes");
}

void printWiFiDiagnostics() {
    Serial.println("📡 WiFi Diagnostics:");
    Serial.println("  Status: " + String(WiFi.status()));
    Serial.println("  SSID: " + WiFi.SSID());
    Serial.println("  RSSI: " + String(WiFi.RSSI()) + " dBm");
    Serial.println("  Channel: " + String(WiFi.channel()));
    Serial.println("  BSSID: " + WiFi.BSSIDstr());
}
#endif