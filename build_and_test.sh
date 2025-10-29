#!/bin/bash

# ESP32 Motion Detection System - Build and Test Script
# This script validates configuration, builds the project, and runs tests

set -e  # Exit on any error

echo "🚀 ESP32 Motion Detection System - Build & Test"
echo "================================================"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if we're in the right directory
if [ ! -f "platformio.ini" ]; then
    print_error "platformio.ini not found. Are you in the project root directory?"
    exit 1
fi

# Check if Python is available for validation scripts
if ! command -v python3 &> /dev/null; then
    print_warning "Python3 not found. Skipping validation scripts."
    SKIP_VALIDATION=true
else
    SKIP_VALIDATION=false
fi

# Check if PlatformIO is installed
if ! command -v pio &> /dev/null; then
    print_error "PlatformIO not found. Please install PlatformIO first."
    print_error "Visit: https://platformio.org/install"
    exit 1
fi

print_success "PlatformIO found: $(pio --version)"

# Step 1: Validate configuration
if [ "$SKIP_VALIDATION" = false ]; then
    print_status "Step 1: Validating configuration..."
    python3 scripts/validate_config.py
    if [ $? -ne 0 ]; then
        print_error "Configuration validation failed!"
        exit 1
    fi
    print_success "Configuration validation passed!"
else
    print_warning "Skipping configuration validation (Python3 not available)"
fi

# Step 2: Check secrets file
print_status "Step 2: Checking secrets file..."
if [ ! -f "include/secrets.h" ]; then
    print_warning "secrets.h not found!"
    print_status "Creating secrets.h from template..."
    cp include/secrets.h.template include/secrets.h
    print_warning "Please edit include/secrets.h with your credentials before building!"
    print_warning "You need to configure:"
    print_warning "  - WiFi SSID and password"
    print_warning "  - Telegram bot token"
    print_warning "  - Telegram chat ID"
    read -p "Press Enter to continue or Ctrl+C to exit and configure secrets.h..."
fi

# Step 3: Clean previous build
print_status "Step 3: Cleaning previous build..."
pio run --target clean
print_success "Build cleaned!"

# Step 4: Check library dependencies
print_status "Step 4: Installing/updating library dependencies..."
pio lib install
print_success "Libraries installed!"

# Step 5: Build the project
print_status "Step 5: Building project..."
BUILD_ENV=${1:-esp32dev}  # Default to esp32dev, or use first argument
print_status "Building for environment: $BUILD_ENV"

pio run -e $BUILD_ENV

if [ $? -eq 0 ]; then
    print_success "Build completed successfully!"
else
    print_error "Build failed!"
    exit 1
fi

# Step 6: Analyze build output
print_status "Step 6: Analyzing build output..."
BUILD_DIR=".pio/build/$BUILD_ENV"

if [ -f "$BUILD_DIR/firmware.bin" ]; then
    FIRMWARE_SIZE=$(stat -f%z "$BUILD_DIR/firmware.bin" 2>/dev/null || stat -c%s "$BUILD_DIR/firmware.bin" 2>/dev/null)
    print_success "Firmware size: $FIRMWARE_SIZE bytes"
    
    # Check firmware size (ESP32 has ~1.2MB app space)
    if [ $FIRMWARE_SIZE -gt 1200000 ]; then
        print_warning "Firmware size is large (>1.2MB). Consider optimizing."
    fi
fi

# Step 7: Run static analysis (if available)
print_status "Step 7: Running code analysis..."
if command -v cppcheck &> /dev/null; then
    print_status "Running cppcheck..."
    cppcheck --enable=warning,style,performance --std=c++11 src/ include/ 2>/dev/null || true
fi

# Step 8: Optional - Upload to device
if [ "$2" = "upload" ]; then
    print_status "Step 8: Uploading to device..."
    pio run -e $BUILD_ENV --target upload
    
    if [ $? -eq 0 ]; then
        print_success "Upload completed successfully!"
        print_status "Starting serial monitor..."
        print_status "Press Ctrl+C to stop monitoring"
        pio device monitor --baud 115200
    else
        print_error "Upload failed!"
        exit 1
    fi
else
    print_status "Step 8: Skipping upload (use '$0 $BUILD_ENV upload' to upload)"
fi

# Summary
echo ""
echo "================================================"
print_success "Build process completed successfully!"
echo "================================================"
print_status "Next steps:"
echo "  1. Configure secrets.h with your credentials"
echo "  2. Upload to device: pio run --target upload"
echo "  3. Monitor output: pio device monitor"
echo "  4. Test motion detection with PIR sensor"
print_status "Documentation:"
echo "  - README.md - Complete setup guide"
echo "  - QUICK_SETUP.md - 5-minute setup"
echo "  - ADVANCED_CONFIG.md - Advanced configuration"
echo "  - WIRING.md - Hardware wiring guide"
echo "================================================"