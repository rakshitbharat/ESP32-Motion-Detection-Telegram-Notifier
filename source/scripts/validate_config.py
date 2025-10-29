#!/usr/bin/env python3
"""
Configuration Validation Script for ESP32 Motion Detection System
Validates all configuration combinations and checks for potential issues
"""

import os
import re
import sys
from pathlib import Path

class ConfigValidator:
    def __init__(self):
        self.errors = []
        self.warnings = []
        self.config_values = {}
        
    def validate_all(self):
        """Run all validation checks"""
        print("🔍 Starting comprehensive configuration validation...")
        print("=" * 60)
        
        # Load configuration values
        self.load_config_values()
        
        # Run validation checks
        self.validate_pin_assignments()
        self.validate_timing_parameters()
        self.validate_memory_settings()
        self.validate_network_settings()
        self.validate_telegram_settings()
        self.validate_feature_compatibility()
        self.validate_build_settings()
        self.validate_secrets_file()
        
        # Report results
        self.print_results()
        
        return len(self.errors) == 0
    
    def load_config_values(self):
        """Load configuration values from config.h"""
        config_path = "include/config.h"
        if not os.path.exists(config_path):
            self.errors.append("config.h file not found")
            return
        
        with open(config_path, 'r') as f:
            content = f.read()
            
            # Extract #define values
            defines = re.findall(r'#define\s+(\w+)\s+(.+)', content)
            for name, value in defines:
                # Clean up the value
                value = value.split('//')[0].strip()  # Remove comments
                self.config_values[name] = value
    
    def validate_pin_assignments(self):
        """Validate GPIO pin assignments"""
        print("📌 Validating pin assignments...")
        
        # ESP32 pin constraints
        INPUT_ONLY_PINS = [34, 35, 36, 37, 38, 39]
        STRAPPING_PINS = [0, 2, 5, 12, 15]
        FLASH_PINS = [6, 7, 8, 9, 10, 11]
        
        pins_in_use = {}
        
        pin_configs = [
            ("LED_PIN", "LED"),
            ("MOTION_SENSOR_PIN", "Motion Sensor"),
            ("EXTERNAL_LED_PIN", "External LED"),
            ("BUZZER_PIN", "Buzzer"),
            ("RESET_BUTTON_PIN", "Reset Button"),
            ("CONFIG_BUTTON_PIN", "Config Button"),
            ("TEMPERATURE_SENSOR_PIN", "Temperature Sensor"),
            ("LIGHT_SENSOR_PIN", "Light Sensor"),
            ("BATTERY_VOLTAGE_PIN", "Battery Monitor")
        ]
        
        for pin_name, description in pin_configs:
            if pin_name in self.config_values:
                pin_value = self.config_values[pin_name]
                
                # Skip disabled pins (-1)
                if pin_value == "-1":
                    continue
                
                try:
                    pin_num = int(pin_value)
                    
                    # Check pin range
                    if pin_num < 0 or pin_num > 39:
                        self.errors.append(f"Invalid pin number for {description}: {pin_num}")
                        continue
                    
                    # Check for pin conflicts
                    if pin_num in pins_in_use:
                        self.errors.append(f"Pin conflict: GPIO{pin_num} used for both {description} and {pins_in_use[pin_num]}")
                    else:
                        pins_in_use[pin_num] = description
                    
                    # Check input-only pins for outputs
                    if pin_num in INPUT_ONLY_PINS and pin_name in ["LED_PIN", "EXTERNAL_LED_PIN", "BUZZER_PIN"]:
                        self.errors.append(f"GPIO{pin_num} is input-only, cannot be used for {description}")
                    
                    # Warn about strapping pins
                    if pin_num in STRAPPING_PINS:
                        self.warnings.append(f"GPIO{pin_num} ({description}) is a strapping pin - may cause boot issues")
                    
                    # Warn about flash pins
                    if pin_num in FLASH_PINS:
                        self.errors.append(f"GPIO{pin_num} ({description}) is connected to flash - do not use")
                
                except ValueError:
                    self.errors.append(f"Invalid pin value for {pin_name}: {pin_value}")
    
    def validate_timing_parameters(self):
        """Validate timing-related parameters"""
        print("⏱️  Validating timing parameters...")
        
        timing_checks = [
            ("NOTIFICATION_INTERVAL", 1000, 3600000, "Notification interval should be 1s-1h"),
            ("WIFI_TIMEOUT", 5000, 60000, "WiFi timeout should be 5s-60s"),
            ("BOT_MTBS", 500, 10000, "Bot scan interval should be 0.5s-10s"),
            ("SENSOR_STABILIZATION_TIME", 10000, 300000, "Sensor stabilization should be 10s-5min"),
            ("MOTION_DEBOUNCE_DELAY", 10, 1000, "Motion debounce should be 10ms-1s"),
            ("WATCHDOG_TIMEOUT", 1000, 30000, "Watchdog timeout should be 1s-30s"),
            ("LOOP_DELAY", 10, 1000, "Loop delay should be 10ms-1s")
        ]
        
        for param, min_val, max_val, message in timing_checks:
            if param in self.config_values:
                try:
                    value = int(self.config_values[param])
                    if value < min_val or value > max_val:
                        self.warnings.append(f"{param}={value}: {message}")
                except ValueError:
                    self.errors.append(f"Invalid numeric value for {param}: {self.config_values[param]}")
    
    def validate_memory_settings(self):
        """Validate memory-related settings"""
        print("💾 Validating memory settings...")
        
        if "MIN_FREE_MEMORY" in self.config_values:
            try:
                min_memory = int(self.config_values["MIN_FREE_MEMORY"])
                if min_memory < 5000:
                    self.warnings.append("MIN_FREE_MEMORY is very low - may cause instability")
                elif min_memory > 100000:
                    self.warnings.append("MIN_FREE_MEMORY is very high - may trigger false alarms")
            except ValueError:
                self.errors.append("Invalid MIN_FREE_MEMORY value")
    
    def validate_network_settings(self):
        """Validate network-related settings"""
        print("🌐 Validating network settings...")
        
        # Check WiFi timeout values
        if "WIFI_MAX_RETRIES" in self.config_values:
            try:
                retries = int(self.config_values["WIFI_MAX_RETRIES"])
                if retries > 10:
                    self.warnings.append("WIFI_MAX_RETRIES is high - may cause long startup delays")
            except ValueError:
                self.errors.append("Invalid WIFI_MAX_RETRIES value")
        
        # Check signal strength threshold
        if "MIN_WIFI_SIGNAL_STRENGTH" in self.config_values:
            try:
                rssi = int(self.config_values["MIN_WIFI_SIGNAL_STRENGTH"])
                if rssi > -30:
                    self.warnings.append("MIN_WIFI_SIGNAL_STRENGTH seems too high")
                elif rssi < -100:
                    self.warnings.append("MIN_WIFI_SIGNAL_STRENGTH seems too low")
            except ValueError:
                self.errors.append("Invalid MIN_WIFI_SIGNAL_STRENGTH value")
    
    def validate_telegram_settings(self):
        """Validate Telegram-related settings"""
        print("📱 Validating Telegram settings...")
        
        # Check message length limits
        if "BOT_MAX_MESSAGE_LENGTH" in self.config_values:
            try:
                length = int(self.config_values["BOT_MAX_MESSAGE_LENGTH"])
                if length > 4096:
                    self.errors.append("BOT_MAX_MESSAGE_LENGTH exceeds Telegram limit of 4096")
                elif length < 100:
                    self.warnings.append("BOT_MAX_MESSAGE_LENGTH is very small")
            except ValueError:
                self.errors.append("Invalid BOT_MAX_MESSAGE_LENGTH value")
    
    def validate_feature_compatibility(self):
        """Check for incompatible feature combinations"""
        print("🔧 Validating feature compatibility...")
        
        # Check debug level vs performance monitoring
        debug_level = self.config_values.get("DEBUG_LEVEL", "0")
        perf_monitoring = self.config_values.get("ENABLE_PERFORMANCE_MONITORING", "false")
        
        try:
            if int(debug_level) > 3 and perf_monitoring.lower() == "true":
                self.warnings.append("High debug level + performance monitoring may impact performance")
        except ValueError:
            pass
        
        # Check deep sleep vs always-on features
        deep_sleep = self.config_values.get("ENABLE_DEEP_SLEEP", "false")
        if deep_sleep.lower() == "true":
            if self.config_values.get("ENABLE_TELEGRAM_NOTIFICATIONS", "true").lower() == "true":
                self.warnings.append("Deep sleep enabled with Telegram notifications - may miss messages")
    
    def validate_build_settings(self):
        """Validate PlatformIO build settings"""
        print("🔨 Validating build settings...")
        
        if not os.path.exists("platformio.ini"):
            self.errors.append("platformio.ini not found")
            return
        
        with open("platformio.ini", 'r') as f:
            content = f.read()
            
            # Check for required libraries
            required_libs = [
                "ArduinoJson",
                "UniversalTelegramBot",
                "WiFiClientSecure",
                "NTPClient"
            ]
            
            for lib in required_libs:
                if lib not in content:
                    self.warnings.append(f"Library {lib} not found in platformio.ini")
    
    def validate_secrets_file(self):
        """Validate secrets.h file"""
        print("🔐 Validating secrets file...")
        
        if not os.path.exists("include/secrets.h"):
            self.warnings.append("secrets.h not found - copy from template and configure")
            return
        
        with open("include/secrets.h", 'r') as f:
            content = f.read()
            
            # Check for default values that should be changed
            defaults = [
                ("Your_WiFi_Network_Name", "WiFi SSID"),
                ("Your_WiFi_Password", "WiFi password"), 
                ("1234567890:ABCdefGHIjklMNOpqrSTUvwxYZ123456789", "Bot token"),
                ("-1001234567890", "Chat ID")
            ]
            
            for default_val, description in defaults:
                if default_val in content:
                    self.warnings.append(f"Default {description} found in secrets.h - please configure")
    
    def print_results(self):
        """Print validation results"""
        print("=" * 60)
        print("📊 VALIDATION RESULTS")
        print("=" * 60)
        
        if self.errors:
            print(f"\n❌ ERRORS ({len(self.errors)}):")
            for i, error in enumerate(self.errors, 1):
                print(f"  {i}. {error}")
        
        if self.warnings:
            print(f"\n⚠️  WARNINGS ({len(self.warnings)}):")
            for i, warning in enumerate(self.warnings, 1):
                print(f"  {i}. {warning}")
        
        if not self.errors and not self.warnings:
            print("\n✅ All validations passed! Configuration looks good.")
        elif not self.errors:
            print(f"\n✅ No critical errors found. {len(self.warnings)} warnings to review.")
        else:
            print(f"\n❌ {len(self.errors)} errors must be fixed before building.")
        
        print("=" * 60)

def main():
    """Main validation function"""
    validator = ConfigValidator()
    
    if validator.validate_all():
        print("🎉 Configuration validation completed successfully!")
        sys.exit(0)
    else:
        print("💥 Configuration validation failed!")
        sys.exit(1)

if __name__ == "__main__":
    main()