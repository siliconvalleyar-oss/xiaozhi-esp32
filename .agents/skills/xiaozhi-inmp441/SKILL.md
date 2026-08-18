---
name: xiaozhi-inmp441
description: INMP441 I2S microphone connection guide for XiaoZhi ESP32
---

# XiaoZhi INMP441 Microphone Connection Guide

This skill covers how to connect an INMP441 I2S digital microphone to ESP32 for XiaoZhi AI.

## INMP441 Pinout

| INMP441 Pin | Function | Connect to ESP32 |
|-------------|----------|------------------|
| **VDD** | Power | 3.3V |
| **GND** | Ground | GND |
| **SD** | Serial Data Out | GPIO (e.g. GPIO32) |
| **WS** | Word Select (LRCK) | GPIO (e.g. GPIO25) |
| **SCK** | Serial Clock (BCLK) | GPIO (e.g. GPIO26) |
| **L/R** | Left/Right select | GND (left) or 3.3V (right) |

## Speaker (MAX98357 I2S Amplifier)

| MAX98357 Pin | Function | Connect to ESP32 |
|--------------|----------|------------------|
| **VIN** | Power | 5V or 3.3V |
| **GND** | Ground | GND |
| **DIN** | Serial Data In | GPIO (e.g. GPIO33) |
| **BCLK** | Bit Clock | GPIO (e.g. GPIO14) |
| **LRC** | Word Select | GPIO (e.g. GPIO27) |
| **SD** | Shutdown | Leave floating or 3.3V |
| **GAIN** | Gain select | Leave floating (15dB) |

## Wiring Diagram (Simplex Mode)

Separate I2S buses for mic and speaker (recommended):

```
ESP32                    INMP441 (Microphone)
──────                    ────────────────────
GPIO25  ──────────────→  WS
GPIO26  ──────────────→  SCK
GPIO32  ←──────────────  SD
3.3V    ──────────────→  VDD, L/R
GND     ──────────────→  GND

ESP32                    MAX98357 (Speaker)
──────                    ────────────────
GPIO14  ──────────────→  BCLK
GPIO27  ──────────────→  LRC
GPIO33  ──────────────→  DIN
5V      ──────────────→  VIN
GND     ──────────────→  GND
```

## config.h (Simplex Mode)

```c
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// Use simplex mode (separate I2S for mic and speaker)
#define AUDIO_I2S_METHOD_SIMPLEX

#ifdef AUDIO_I2S_METHOD_SIMPLEX

// INMP441 Microphone pins
#define AUDIO_I2S_MIC_GPIO_WS   GPIO_NUM_25
#define AUDIO_I2S_MIC_GPIO_SCK  GPIO_NUM_26
#define AUDIO_I2S_MIC_GPIO_DIN  GPIO_NUM_32

// MAX98357 Speaker pins
#define AUDIO_I2S_SPK_GPIO_DOUT GPIO_NUM_33
#define AUDIO_I2S_SPK_GPIO_BCLK GPIO_NUM_14
#define AUDIO_I2S_SPK_GPIO_LRCK GPIO_NUM_27

#else
// Duplex mode (shared I2S bus) - only if mic and speaker share pins
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_4
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_5
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_6
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_7

#endif

#define BOOT_BUTTON_GPIO GPIO_NUM_0

#endif // _BOARD_CONFIG_H_
```

## Board Class Implementation

```cpp
#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "application.h"
#include "button.h"
#include "config.h"

class MyInmp441Board : public WifiBoard {
private:
    Button boot_button_;

public:
    MyInmp441Board() : boot_button_(BOOT_BUTTON_GPIO) {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
    }

    virtual AudioCodec* GetAudioCodec() override {
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        // Separate I2S buses for mic and speaker
        static NoAudioCodecSimplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_SPK_GPIO_BCLK,   // Speaker BCLK
            AUDIO_I2S_SPK_GPIO_LRCK,   // Speaker WS/LRCK
            AUDIO_I2S_SPK_GPIO_DOUT,   // Speaker DIN
            AUDIO_I2S_MIC_GPIO_SCK,    // Mic SCK
            AUDIO_I2S_MIC_GPIO_WS,     // Mic WS
            AUDIO_I2S_MIC_GPIO_DIN);   // Mic SD
#else
        // Shared I2S bus
        static NoAudioCodecDuplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_BCLK,
            AUDIO_I2S_GPIO_WS,
            AUDIO_I2S_GPIO_DOUT,
            AUDIO_I2S_GPIO_DIN);
#endif
        return &audio_codec;
    }
};

DECLARE_BOARD(MyInmp441Board);
```

## Duplex vs Simplex

| Mode | When to Use | Audio Codec Class |
|------|-------------|-------------------|
| **Simplex** | Separate mic (INMP441) and speaker (MAX98357) with different pins | `NoAudioCodecSimplex` |
| **Duplex** | Mic and speaker share the same I2S bus pins | `NoAudioCodecDuplex` |

**Simplex is recommended** for INMP441 + MAX98357 because:
- Independent clock domains for mic and speaker
- No audio conflicts between input and output
- Better echo cancellation support

## Sample Rates

- **Microphone (INMP441)**: 16000 Hz (recommended for speech)
- **Speaker (MAX98357)**: 24000 Hz (server may send at 24 kHz)

## Important Notes

1. **INMP441 L/R pin**: Connect to GND for left channel, 3.3V for right channel
2. **INMP441 is a microphone only** - it cannot output audio
3. **MAX98357 is a speaker only** - it cannot capture audio
4. **Power**: Both modules can run on 3.3V, but MAX98357 also accepts 5V
5. **No external codec needed** - INMP441 and MAX98357 handle I2S directly
6. **GPIO availability**: Choose pins that are not used by display, buttons, or other peripherals

## Reference

This configuration is used in `main/boards/bread-compact-esp32/` which is the official breadboard DIY example for XiaoZhi AI.
