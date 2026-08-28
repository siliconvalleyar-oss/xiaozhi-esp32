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
| DIN | Serial Data In | GPIO20 |
| BCLK | Bit Clock | GPIO18 |
| LRC | Word Select | GPIO19 |
| SD | Shutdown | 3.3V |
| GAIN | Gain select | GND (15dB) |

### SSD1306 OLED (Display)

| SSD1306 Pin | Function | ESP32-C6 Pin |
|-------------|----------|--------------|
| VCC | Power | 3.3V |
| GND | Ground | GND |
| SDA | I2C Data | GPIO2 |
| SCL | I2C Clock | GPIO3 |

### ESP32-C6-DevKitC-1 Pinout (Expansion Header)

| Header Pin | GPIO | Function in this board |
|------------|------|------------------------|
| 1 | GPIO9 | Boot button (active low) |
| 2 | GPIO8 | Built-in LED |
| 3 | GPIO7 | — |
| 4 | GPIO6 | — |
| 5 | GPIO5 | INMP441 WS (I2S mic word select) |
| 6 | GPIO4 | INMP441 SCK (I2S mic clock) |
| 7 | GPIO3 | SSD1306 SCL (I2C clock) |
| 8 | GPIO2 | SSD1306 SDA (I2C data) |
| 9 | GPIO1 | — |
| 10 | GPIO0 | — |

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
