# PIR Motion Sensor Configuration Guide

## Overview
The ESP32 Motion Detection System now includes an advanced sensor configuration mode that allows you to easily adjust PIR sensor sensitivity, range, and detection parameters for optimal performance in your specific environment.

## Features Added

### 1. Physical Button Configuration Mode
- **Button**: GPIO 0 (Boot button on most ESP32 boards)
- **Enter Config Mode**: Hold the button for 3 seconds
- **Navigate Settings**: Single button press to cycle through options
- **Save & Exit**: The system automatically saves after cycling through all options

### 2. LED Feedback Patterns
Different LED blink patterns indicate different configuration states:

- **Fast blinking (80ms)**: Entering config mode
- **Medium pulse (250ms)**: Config mode active
- **Sensitivity adjustment (150ms)**: Number of blinks = sensitivity level
- **Range adjustment (300ms)**: Number of blinks = range setting
- **Very fast blink (60ms)**: Sensor testing active
- **Slow pulse (400ms)**: Saving settings
- **Exit pattern (120ms)**: Confirming exit

### 3. Telegram Bot Commands
New commands for remote sensor configuration:

```
/sensor_config - Enter sensor configuration mode
/sensitivity [0-4] - Set sensor sensitivity level
/range [0-2] - Set sensor range setting
/test_sensor - Test current sensor settings for 10 seconds
/show_settings - Display current sensor configuration
```

## Sensitivity Levels

| Level | Setting | Debounce Delay | Description |
|-------|---------|----------------|-------------|
| 0 | Very Low | 3000ms | Least sensitive - requires sustained motion |
| 1 | Low | 2000ms | Low sensitivity - filters out small movements |
| 2 | Medium | 1000ms | Default balanced setting |
| 3 | High | 500ms | High sensitivity - quick response |
| 4 | Very High | 200ms | Most sensitive - detects minimal movement |

## Range Settings

| Setting | Range | Cooldown Period | Description |
|---------|-------|-----------------|-------------|
| 0 | Short | 5000ms | Quick reset for close-range detection |
| 1 | Medium | 10000ms | Balanced setting for general use |
| 2 | Long | 20000ms | Extended monitoring for large areas |

## Usage Instructions

### Method 1: Physical Button Configuration

1. **Enter Config Mode**:
   - Hold the boot button (GPIO 0) for 3 seconds
   - LED will blink rapidly to confirm entry
   - Telegram notification will be sent (if connected)

2. **Adjust Sensitivity**:
   - First step in config mode
   - Press button to cycle through sensitivity levels (0-4)
   - LED blinks show current level (1-5 blinks)
   - Each press increases sensitivity level

3. **Adjust Range**:
   - Second step in config mode
   - Press button to cycle through range settings (0-2)
   - LED blinks show current setting (1-3 blinks)

4. **Test Settings**:
   - Third step - automatic 10-second sensor test
   - Move in front of sensor to test detection
   - LED blinks rapidly during test
   - Detection count is logged

5. **Save & Exit**:
   - Fourth step - automatic save and exit
   - Settings are saved to memory
   - LED confirms with exit pattern

### Method 2: Telegram Commands

1. **Check Current Settings**:
   ```
   /show_settings
   ```

2. **Quick Sensitivity Adjustment**:
   ```
   /sensitivity 3    (sets to High sensitivity)
   ```

3. **Quick Range Adjustment**:
   ```
   /range 1         (sets to Medium range)
   ```

4. **Test Current Settings**:
   ```
   /test_sensor
   ```

5. **Interactive Mode**:
   ```
   /sensor_config   (enters interactive mode)
   ```

## Troubleshooting

### Problem: Sensor too sensitive (false alarms)
**Solutions**:
- Lower sensitivity level: `/sensitivity 1` or `/sensitivity 0`
- Increase range setting for longer cooldown: `/range 2`
- Use physical config mode to fine-tune

### Problem: Sensor not detecting motion
**Solutions**:
- Increase sensitivity level: `/sensitivity 3` or `/sensitivity 4`
- Decrease range setting for quicker reset: `/range 0`
- Run sensor test: `/test_sensor`
- Check PIR sensor positioning and power

### Problem: Too many notifications
**Solutions**:
- Lower sensitivity: `/sensitivity 1`
- Increase range for longer cooldown: `/range 2`
- The system already has built-in rate limiting

### Problem: Delayed detection
**Solutions**:
- Increase sensitivity for faster response: `/sensitivity 4`
- Use short range setting: `/range 0`
- Check sensor stabilization (30-second warmup period)

## Technical Details

### Configuration Storage
- Settings are saved to ESP32 memory (persistent across reboots)
- Default values are restored if no saved settings exist
- Settings can be reset via `/reset` command

### Motion Detection Logic
- Higher sensitivity = shorter debounce delays = more responsive
- Longer range = longer cooldown periods = fewer repeat notifications
- Session-based detection prevents spam notifications
- Configurable timing integrates with existing notification limits

### Safety Features
- Config mode automatically exits after 60 seconds of inactivity
- Settings are validated before applying
- Telegram notifications confirm all changes
- LED feedback provides visual confirmation without needing connectivity

## Example Usage Scenarios

### Indoor Small Room (High Sensitivity)
```
/sensitivity 4
/range 0
```
- Very responsive detection
- Quick reset for continuous monitoring

### Outdoor Large Area (Balanced)
```
/sensitivity 2
/range 2
```
- Moderate sensitivity to avoid false alarms from wind/animals
- Long cooldown to prevent notification spam

### Security Monitoring (Conservative)
```
/sensitivity 1
/range 1
```
- Lower sensitivity to reduce false positives
- Balanced cooldown period

## Default Settings
- Sensitivity: 2 (Medium)
- Range: 1 (Medium)
- These provide balanced performance for most environments

The system maintains all existing functionality while adding these powerful configuration options for optimal PIR sensor tuning.