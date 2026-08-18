# INMP441 Breadboard S3

A complete XiaoZhi AI chatbot built on an ESP32-S3 DevKit using the **INMP441** I2S microphone and **MAX98357** I2S amplifier — no external codec needed.

## Hardware

| Component | Module | Interface |
|-----------|--------|-----------|
| MCU | ESP32-S3 DevKit | — |
| Microphone | INMP441 | I2S (Simplex) |
| Amplifier | MAX98357 | I2S (Simplex) |
| Display | SSD1306 OLED 128×64 | I2C |
| Status LED | Any 3.3V LED | GPIO |

## Wiring Diagram

### INMP441 (Microphone)

| INMP441 Pin | Connect to | ESP32-S3 Pin |
|-------------|------------|--------------|
| VDD | 3.3V | 3.3V |
| GND | GND | GND |
| SD | Serial Data Out | GPIO 4 |
| WS | Word Select | GPIO 5 |
| SCK | Serial Clock | GPIO 6 |
| L/R | GND (left channel) | GND |

### MAX98357 (Speaker / Amplifier)

| MAX98357 Pin | Connect to | ESP32-S3 Pin |
|--------------|------------|--------------|
| VIN | 5V (or 3.3V) | 5V |
| GND | GND | GND |
| DIN | Serial Data In | GPIO 7 |
| BCLK | Bit Clock | GPIO 15 |
| LRC | Word Select | GPIO 16 |
| SD | Leave floating | — |
| GAIN | Leave floating (15 dB) | — |

### SSD1306 OLED (I2C)

| OLED Pin | Connect to | ESP32-S3 Pin |
|----------|------------|--------------|
| VCC | 3.3V | 3.3V |
| GND | GND | GND |
| SDA | I2C Data | GPIO 3 |
| SCL | I2C Clock | GPIO 2 |

### Buttons & LED

| Component | Connect to | ESP32-S3 Pin |
|-----------|------------|--------------|
| Boot button | Built-in | GPIO 0 |
| Push-to-talk | External button → GND | GPIO 1 |
| Status LED | LED + 330Ω → GND | GPIO 48 |

### Optional: Lamp / Relay (MCP)

| Component | Connect to | ESP32-S3 Pin |
|-----------|------------|--------------|
| Relay / Lamp | Via transistor/MOSFET | GPIO 17 |

## GPIO Summary

| GPIO | Function | Direction |
|------|----------|-----------|
| 0 | Boot button | Input |
| 1 | Push-to-talk button | Input |
| 2 | OLED SCL | Output (I2C) |
| 3 | OLED SDA | Bidirectional (I2C) |
| 4 | INMP441 SD | Input (I2S mic data) |
| 5 | INMP441 WS | Output (I2S mic WS) |
| 6 | INMP441 SCK | Output (I2S mic clock) |
| 7 | MAX98357 DIN | Output (I2S speaker data) |
| 15 | MAX98357 BCLK | Output (I2S speaker clock) |
| 16 | MAX98357 LRC | Output (I2S speaker WS) |
| 17 | Lamp / Relay | Output (MCP tool) |
| 48 | Status LED | Output |

## Build & Flash

### Quick build with release.py

```bash
python scripts/release.py inmp441-bread-s3
```

### Manual build with idf.py

```bash
# Set target
idf.py set-target esp32s3

# Configure board
idf.py menuconfig
# Navigate to: Xiaozhi Assistant → Board Type → INMP441 Breadboard S3

# Build
idf.py build

# Flash (connect ESP32-S3 via USB)
idf.py flash monitor
```

## Button Functions

| Button | Action | Function |
|--------|--------|----------|
| Boot (GPIO 0) | Single click | Toggle chat (start/stop) |
| Boot (GPIO 0) | Long press (3s) | Factory reset |
| PTT (GPIO 1) | Press & hold | Push-to-talk (listen while held) |

## MCP Tools

This board registers the following IoT tools for AI control:

| Tool | Description |
|------|-------------|
| `self.led.set` | Set the status LED on/off (`on`: bool) |
| `self.board.get_info` | Get board hardware info |
| `self.light.set_rgb` | (via LampController) Control lamp on GPIO 17 |

Plus all built-in tools (`self.get_device_status`, `self.audio_speaker.set_volume`, etc.)

## Audio Notes

- **Sample rates**: 16 kHz input (mic), 24 kHz output (speaker)
- **Mode**: Simplex — separate I2S buses for mic and speaker
- **INMP441 L/R pin**: Connect to GND for left channel, 3.3V for right
- **No codec chip needed** — INMP441 and MAX98357 handle I2S directly
