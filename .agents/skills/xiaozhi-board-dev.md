# XiaoZhi Custom Board Development

This skill guides you through adding a new board to the XiaoZhi AI voice assistant project.

## Overview

XiaoZhi AI supports 70+ ESP32-series boards. Each board lives in its own directory under `main/boards/`.

> **WARNING**: Never overwrite an existing board's configuration. Always create a new board type or use the `builds` array in `config.json` for distinct firmware names. Overwriting risks OTA replacing your custom firmware with stock firmware.

## Directory Layout

A board directory typically contains:
- `xxx_board.cc` - board-level initialization and glue code
- `config.h` - pin assignments and board-level settings
- `config.json` - build configuration consumed by `scripts/release.py`
- `README.md` - board-specific notes

Boards can live directly under `main/boards/` or grouped by manufacturer under `main/boards/<manufacturer>/<board>/`.

## Steps

### 1. Create the Board Directory

```bash
mkdir main/boards/my-custom-board
```

### 2. Create config.h

Define all hardware settings:

```c
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// Audio
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_10
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_12
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_8
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_7
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_11

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_13
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_0
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_1
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR

// Buttons
#define BOOT_BUTTON_GPIO        GPIO_NUM_9

// Display
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_3
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_5
#define DISPLAY_DC_PIN          GPIO_NUM_6
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_4
#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY true
#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0
#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_2
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true

#endif // _BOARD_CONFIG_H_
```

### 3. Create config.json

```json
{
    "target": "esp32s3",
    "builds": [
        {
            "name": "my-custom-board",
            "sdkconfig_append": [
                "CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y",
                "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""
            ]
        }
    ]
}
```

**Fields**:
- `target`: `esp32`, `esp32s3`, `esp32c3`, `esp32c6`, `esp32p4`
- `name`: firmware package name (match directory name)
- `sdkconfig_append`: extra sdkconfig lines

**Common sdkconfig_append entries**:
- Flash: `CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y`, `8MB`, `16MB`
- Partition: `CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/4m.csv\"`, `8m.csv`, `16m.csv`
- Language: `CONFIG_LANGUAGE_EN_US=y`, `CONFIG_LANGUAGE_ZH_CN=y`
- AEC: `CONFIG_USE_DEVICE_AEC=y`
- Wake word: `CONFIG_WAKE_WORD_DISABLED=y`

### 4. Implement the Board Class

```cpp
#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"
#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>

#define TAG "MyCustomBoard"

class MyCustomBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    Button boot_button_;
    LcdDisplay* display_;

    void InitializeI2c() { /* I2C setup */ }
    void InitializeSpi() { /* SPI setup */ }
    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
    }
    void InitializeDisplay() { /* LCD setup with esp_lcd */ }
    void InitializeTools() {
        // Register MCP tools; see docs/mcp-usage.md
    }

public:
    MyCustomBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        InitializeSpi();
        InitializeDisplay();
        InitializeButtons();
        InitializeTools();
        GetBacklight()->SetBrightness(100);
    }

    virtual AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(
            codec_i2c_bus_, I2C_NUM_0,
            AUDIO_INPUT_SAMPLE_RATE, AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, AUDIO_I2S_GPIO_BCLK,
            AUDIO_I2S_GPIO_WS, AUDIO_I2S_GPIO_DOUT,
            AUDIO_I2S_GPIO_DIN, AUDIO_CODEC_PA_PIN,
            AUDIO_CODEC_ES8311_ADDR);
        return &audio_codec;
    }

    virtual Display* GetDisplay() override { return display_; }

    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN,
                                       DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }
};

DECLARE_BOARD(MyCustomBoard);
```

### 5. Hook Up the Build System

**Add Kconfig entry** in `main/Kconfig.projbuild`:
```kconfig
config BOARD_TYPE_MY_CUSTOM_BOARD
    bool "My Custom Board"
    depends on IDF_TARGET_ESP32S3
```

**Add branch in `main/CMakeLists.txt`**:
```cmake
elseif(CONFIG_BOARD_TYPE_MY_CUSTOM_BOARD)
    set(BOARD_TYPE "my-custom-board")
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)
```

**Font guidance**:
- Small (128x64 OLED): `font_puhui_basic_14_1` / `font_awesome_14_1`
- Small-medium (240x240): `font_puhui_basic_16_4` / `font_awesome_16_4`
- Medium (240x320): `font_puhui_basic_20_4` / `font_awesome_20_4`
- Large (480x320+): `font_puhui_basic_30_4` / `font_awesome_30_4`

### 6. Build and Flash

**Option A - idf.py**:
```bash
idf.py set-target esp32s3
idf.py fullclean
idf.py menuconfig  # select board
idf.py build
idf.py flash monitor
```

**Option B - release.py (recommended)**:
```bash
python scripts/release.py my-custom-board
```

## Board Class Hierarchy

- `Board` → `WifiBoard` (WiFi)
- `Board` → `Ml307Board` / `Nt26Board` (4G modem)
- `Board` → `DualNetworkBoard` (WiFi + 4G)
- `Board` → `RndisBoard` (RNDIS-over-USB)

## Audio Codecs Available

- `Es8311AudioCodec` (most common)
- `Es8374AudioCodec`, `Es8388AudioCodec`, `Es8389AudioCodec`
- `BoxAudioCodec` (ES7210 mic array + codec)
- `NoAudioCodec` (direct I2S)
- `DummyAudioCodec` (placeholder)

## Tips

1. Start from a similar board - copying is faster than starting from scratch
2. Bring up incrementally - display first, then audio, then full stack
3. Double check pin assignments against your schematic
4. Check hardware compatibility (codec / PMIC / touch controller)
