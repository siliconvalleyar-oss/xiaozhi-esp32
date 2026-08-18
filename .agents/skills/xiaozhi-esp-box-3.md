# XiaoZhi ESP-BOX-3 Board Guide

This skill covers the Espressif ESP-BOX-3 AIoT development kit setup for XiaoZhi AI.

## Hardware Specs

- **MCU**: ESP32-S3-WROOM-1 (16MB Flash, 8MB PSRAM)
- **Display**: 2.4" IPS LCD (320x240, ILI9341)
- **Audio**: ES8311 Codec + ES7210 dual-mic ADC
- **AEC**: Device-side acoustic echo cancellation supported
- **Button**: Boot button (click / double-click)
- **Power**: USB-C

## Build & Flash

```bash
# Set target
idf.py set-target esp32s3

# Configure
idf.py menuconfig

# Build
idf.py build

# Flash
idf.py flash
```

Or use release script:
```bash
python scripts/release.py esp-box-3
```

## menuconfig Settings

### Board Type
`Xiaozhi Assistant` → `Board Type` → `ESP BOX 3`

### Display Styles

ESP-BOX-3 supports multiple UI styles:

| Style | Config | Description |
|-------|--------|-------------|
| **Emote Animation** (recommended) | `USE_EMOTE_MESSAGE_STYLE` | Rich表情动画, 眼睛动画, 状态图标. Class: `emote::EmoteDisplay` |
| **Default Message** | `USE_DEFAULT_MESSAGE_STYLE` | Standard text + icon UI. Class: `SpiLcdDisplay` |
| **WeChat Message** | `USE_WECHAT_MESSAGE_STYLE` | WeChat-like bubble UI. Class: `SpiLcdDisplay` |

#### Emote Style Setup
Requires custom assets:
1. `Xiaozhi Assistant` → `Flash Assets` → `Flash Custom Assets`
2. `Xiaozhi Assistant` → `Custom Assets File` → enter URL:
   ```
   https://dl.espressif.com/AE/wn9_nihaoxiaozhi_tts-font_puhui_common_20_4-esp-box-3.bin
   ```

### AEC (Echo Cancellation)
`Xiaozhi Assistant` → `Enable Device-Side AEC` → Enable

- Eliminates speaker output interference with microphone
- Improves voice recognition accuracy during TTS playback
- Toggle at runtime: double-click Boot button

### Wake Word
`Xiaozhi Assistant` → `Wake Word Implementation Type` → recommended:
- **Wakenet model with AFE** (`USE_AFE_WAKE_WORD`) — supports AEC

## Button Functions

### Boot Button
- **Single click**:
  - Config mode → enter WiFi configuration
  - Idle → start conversation
  - In conversation → interrupt/stop current conversation
- **Double click** (requires AEC enabled):
  - Toggle AEC on/off

## Factory Reset

Long-press Boot button for 3+ seconds to clear all config and reboot.

## Troubleshooting

1. **Emote animation not displaying**: Ensure custom assets URL is configured and device can reach it
2. **AEC not working**: Check that device-side AEC is enabled in menuconfig; hardware must have good mic-speaker isolation
3. **No audio**: Verify I2S wiring and ES8311 codec I2C address
4. **Display wrong**: Check SPI config, mirroring, and color inversion settings

## Key Files

- Board class: `main/boards/esp-box-3/` directory
- Config: `main/boards/esp-box-3/config.h`
- Build config: `main/boards/esp-box-3/config.json`
