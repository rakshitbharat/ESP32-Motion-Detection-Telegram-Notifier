# ESP32 Motion Detection - Circuit Diagram

## 📋 Components Needed
- ESP32 Development Board
- PIR Motion Sensor (HC-SR501)
- Jumper wires (3 pieces)
- Breadboard (optional)

## 🔌 Connections

### PIR Sensor to ESP32
```
PIR Sensor Pin    →    ESP32 Pin    →    Description
─────────────────────────────────────────────────────
VCC               →    3.3V         →    Power supply
GND               →    GND          →    Ground
OUT (Signal)      →    GPIO 4       →    Motion detection signal
```

## 📐 Visual Diagram

```
    ESP32 Board                           PIR Sensor (HC-SR501)
   ┌─────────────┐                       ┌──────────────────┐
   │             │                       │    ┌─────────┐   │
   │         3.3V├───────────────────────┤VCC │ Dome    │   │
   │             │                       │    │ Lens    │   │
   │          GND├───────────────────────┤GND │         │   │
   │             │                       │    └─────────┘   │
   │       GPIO 4├───────────────────────┤OUT              │
   │             │                       │                 │
   │      GPIO 2 │ (Built-in LED)        │ [Sensitivity]   │
   │             │                       │ [Time Delay]    │
   └─────────────┘                       └──────────────────┘
```

## 🎛️ PIR Sensor Settings

Most PIR sensors have two potentiometers:

### Sensitivity Adjustment (Sx)
- **Clockwise**: Increase sensitivity (3-7 meters)
- **Counter-clockwise**: Decrease sensitivity (3 meters)

### Time Delay (Tx)  
- **Clockwise**: Longer detection time (5s-300s)
- **Counter-clockwise**: Shorter detection time (0.3s-18s)

## ⚡ Power Requirements

- **ESP32**: 3.3V (via USB or external supply)
- **PIR Sensor**: 3.3V - 5V (ESP32's 3.3V output works fine)
- **Current Draw**: ~65mA (ESP32) + ~50µA (PIR) = ~65mA total

## 🔧 Alternative Pin Configurations

You can change the motion sensor pin in `include/config.h`:

```cpp
#define MOTION_SENSOR_PIN 4    // Default
// Other options: 5, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23
```

**Avoid these pins:**
- GPIO 0, 2: Used for boot mode
- GPIO 6-11: Connected to flash memory
- GPIO 34-39: Input only (no internal pull-up)

## 🚨 Safety Notes

1. **Double-check connections** before powering on
2. **Use proper voltage levels** (3.3V for ESP32)
3. **Avoid short circuits** between VCC and GND
4. **Handle boards carefully** to avoid static damage

## 🧪 Testing Steps

1. **Power on ESP32** (LED should blink during boot)
2. **Wait for WiFi connection** (LED stays on when connected)
3. **Move hand in front of PIR** (LED should blink rapidly)
4. **Check Telegram** for notification
5. **Monitor serial output** for debug information

## 🔍 Troubleshooting Connections

### No Motion Detection?
- Check PIR OUT pin connection
- Adjust sensitivity potentiometer
- Ensure PIR has power (VCC connected)
- Wait 30-60 seconds for PIR to stabilize after power-on

### ESP32 Not Starting?
- Check USB cable connection
- Verify power supply (5V via USB)  
- Press RESET button on ESP32
- Check for loose connections

### LED Not Working?
- LED is built-in on GPIO 2 (no external wiring needed)
- Check if `LED_PIN` matches your board (usually GPIO 2)
- Some boards use different pins for built-in LED