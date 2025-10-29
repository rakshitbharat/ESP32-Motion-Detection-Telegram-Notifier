#!/bin/bash
# setup.sh - One-command setup for ESP32 Motion Detection System

set -e

echo "🚀 ESP32 Motion Detection System - One-Command Setup"
echo "=================================================="

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    echo "❌ PlatformIO not found. Installing..."
    if command -v pip &> /dev/null; then
        pip install platformio
    elif command -v pip3 &> /dev/null; then
        pip3 install platformio
    else
        echo "❌ Python/pip not found. Please install Python first."
        exit 1
    fi
fi

echo "✅ PlatformIO found"

# Create secrets.h if it doesn't exist
if [ ! -f "include/secrets.h" ]; then
    echo "📋 Creating secrets.h from template..."
    cp include/secrets.h.template include/secrets.h
    echo "✅ secrets.h created"
    
    echo ""
    echo "🔧 CONFIGURATION NEEDED:"
    echo "Edit include/secrets.h and update these 4 values:"
    echo "  1. WIFI_SSID = \"Your_WiFi_Name\""
    echo "  2. WIFI_PASSWORD = \"Your_WiFi_Password\""
    echo "  3. BOT_TOKEN = \"Get_From_@BotFather\""
    echo "  4. CHAT_ID = \"Your_Chat_ID\""
    echo ""
    echo "🧪 TELEGRAM TESTER AVAILABLE:"
    echo "Test your bot configuration online before uploading:"
    echo "https://rakshitbharat.github.io/ESP32-Motion-Detection-Telegram-Notifier/"
    echo ""
    echo "📖 See QUICK_SETUP.md for detailed instructions"
    echo ""
    
    # Open file for editing
    if command -v code &> /dev/null; then
        echo "🎯 Opening secrets.h in VS Code..."
        code include/secrets.h
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "🎯 Opening secrets.h in default editor..."
        open include/secrets.h
    elif command -v nano &> /dev/null; then
        echo "🎯 Use: nano include/secrets.h to edit"
    fi
    
    echo "⏳ Configure secrets.h then run: ./setup.sh build"
    exit 0
else
    echo "✅ secrets.h already exists"
fi

# Build the project
if [ "$1" == "build" ] || [ "$1" == "" ]; then
    echo "🔨 Building project..."
    pio run -e esp32dev
    
    echo ""
    echo "🎉 BUILD SUCCESSFUL!"
    echo "================="
    echo ""
    echo "📋 Next steps:"
    echo "  1. Connect your ESP32 via USB"
    echo "  2. Upload: pio run --target upload"
    echo "  3. Monitor: pio device monitor"
    echo ""
    echo "🔌 Hardware wiring:"
    echo "  PIR VCC (red)   → ESP32 3.3V"
    echo "  PIR GND (black) → ESP32 GND"
    echo "  PIR OUT (white) → ESP32 GPIO 4"
    echo ""
    echo "📱 Test your bot by sending /status"
    echo "✨ Wave hand near PIR sensor to test motion detection"
fi

# Upload if requested
if [ "$1" == "upload" ]; then
    echo "📤 Uploading to ESP32..."
    pio run --target upload
    echo "🎉 Upload complete! Check your Telegram for startup message."
fi

# Monitor if requested  
if [ "$1" == "monitor" ]; then
    echo "👀 Starting serial monitor..."
    pio device monitor --baud 115200 --filter colorize
fi

echo ""
echo "🆘 Need help? Check QUICK_SETUP.md or README.md"