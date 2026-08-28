# ESP32-C6-DevKitC-1

A XiaoZhi AI chatbot board for the ESP32-C6-DevKitC-1.

## Hardware

- **MCU**: ESP32-C6
- **Audio**: INMP441 I2S microphone + MAX98357A I2S amplifier (simplex)
- **Display**: SSD1306 OLED 128×64 (I2C)
- **Button**: Boot button (GPIO9)
- **LED**: Built-in LED (GPIO8)

## Wiring

### INMP441 (Microphone)

| INMP441 Pin | Function | ESP32-C6 Pin |
|-------------|----------|--------------|
| VDD | Power | 3.3V |
| GND | Ground | GND |
| SD | Serial Data Out | GPIO5 |
| WS | Word Select (LRCK) | GPIO6 |
| SCK | Serial Clock (BCLK) | GPIO4 |
| L/R | Left/Right select | GND (left) |

### MAX98357A (Speaker / Amplifier)

| MAX98357A Pin | Function | ESP32-C6 Pin |
|---------------|----------|--------------|
| VIN | Power | 5V or 3.3V |
| GND | Ground | GND |
| DIN | Serial Data In | GPIO7 |
| BCLK | Bit Clock | GPIO10 |
| LRC | Word Select | GPIO11 |
| SD | Shutdown | 3.3V |
| GAIN | Gain select | GND (15dB) |

### SSD1306 OLED (Display)

| SSD1306 Pin | Function | ESP32-C6 Pin |
|-------------|----------|--------------|
| VCC | Power | 3.3V |
| GND | Ground | GND |
| SDA | I2C Data | GPIO2 |
| SCL | I2C Clock | GPIO3 |

## Build

```bash
idf.py set-target esp32c6
idf.py menuconfig
# Select: Xiaozhi Assistant -> Board Type -> ESP32-C6-DevKitC-1
idf.py build
idf.py flash monitor
```

Or use release script:

```bash
python3 scripts/release.py esp32-c6-devkitc-1
```
