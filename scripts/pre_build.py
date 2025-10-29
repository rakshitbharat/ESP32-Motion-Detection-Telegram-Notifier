#!/usr/bin/env python3
"""
Pre-build script for ESP32 Motion Detection System
Validates configuration and prepares build environment
"""

import os
import sys
from datetime import datetime

def validate_secrets_file():
    """Check if secrets.h exists and is properly configured"""
    secrets_path = "include/secrets.h"
    template_path = "include/secrets.h.template"
    
    if not os.path.exists(secrets_path):
        print("❌ ERROR: secrets.h not found!")
        print("📝 Please copy secrets.h.template to secrets.h and configure your credentials:")
        print(f"   cp {template_path} {secrets_path}")
        print("   Then edit secrets.h with your WiFi and Telegram credentials")
        return False
    
    # Basic validation of secrets.h content
    with open(secrets_path, 'r') as f:
        content = f.read()
        
        if 'Your_WiFi_Network_Name' in content:
            print("⚠️  WARNING: Default WiFi SSID found in secrets.h")
            print("    Please update your WiFi credentials")
        
        if '1234567890:ABCdefGHIjklMNOpqrSTUvwxYZ123456789' in content:
            print("⚠️  WARNING: Default bot token found in secrets.h")
            print("    Please update your Telegram bot token")
        
        if '-1001234567890' in content:
            print("⚠️  WARNING: Default chat ID found in secrets.h")
            print("    Please update your Telegram chat ID")
    
    print("✅ secrets.h file exists and appears configured")
    return True

def check_build_environment():
    """Validate build environment"""
    print("🔍 Checking build environment...")
    
    # Check if we're in the right directory
    if not os.path.exists("platformio.ini"):
        print("❌ ERROR: platformio.ini not found. Are you in the project root?")
        return False
    
    if not os.path.exists("src/main.cpp"):
        print("❌ ERROR: main.cpp not found in src/ directory")
        return False
    
    print("✅ Build environment looks good")
    return True

def generate_build_info():
    """Generate build information header"""
    build_info = f"""// Auto-generated build information
// Generated at: {datetime.now().isoformat()}

#ifndef BUILD_INFO_H
#define BUILD_INFO_H

#define BUILD_TIMESTAMP "{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
#define BUILD_DATE "{datetime.now().strftime('%Y-%m-%d')}"
#define BUILD_TIME "{datetime.now().strftime('%H:%M:%S')}"
#define BUILD_YEAR {datetime.now().year}
#define BUILD_MONTH {datetime.now().month}
#define BUILD_DAY {datetime.now().day}

// Git information (if available)
#ifdef GIT_COMMIT_HASH
#define BUILD_GIT_HASH GIT_COMMIT_HASH
#else
#define BUILD_GIT_HASH "unknown"
#endif

#ifdef GIT_BRANCH
#define BUILD_GIT_BRANCH GIT_BRANCH
#else
#define BUILD_GIT_BRANCH "unknown"
#endif

#endif // BUILD_INFO_H
"""
    
    os.makedirs("include", exist_ok=True)
    with open("include/build_info.h", "w") as f:
        f.write(build_info)
    
    print("✅ Generated build information")

def main():
    """Main pre-build validation"""
    print("🚀 Pre-build validation starting...")
    print("=" * 50)
    
    success = True
    
    # Validate build environment
    if not check_build_environment():
        success = False
    
    # Validate secrets file
    if not validate_secrets_file():
        success = False
    
    # Generate build info
    generate_build_info()
    
    print("=" * 50)
    if success:
        print("✅ Pre-build validation completed successfully")
        sys.exit(0)
    else:
        print("❌ Pre-build validation failed")
        sys.exit(1)

if __name__ == "__main__":
    main()