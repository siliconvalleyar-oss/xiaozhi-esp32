# XiaoZhi ESP32 — Full Architecture Overview

This skill provides a comprehensive overview of the XiaoZhi AI chatbot project architecture.

## Project Summary

XiaoZhi is an open-source MCP-based AI chatbot for ESP32. It uses voice interaction (ASR + LLM + TTS) and controls IoT devices via the MCP protocol. Supports 70+ boards across ESP32, ESP32-S3, ESP32-C3, ESP32-C6, and ESP32-P4.

## High-Level Architecture

```
┌─────────────────────────────────────────────────┐
│                   Cloud Backend                  │
│  ┌──────────┐  ┌──────────┐  ┌──────────────┐  │
│  │ ASR/STT  │  │   LLM    │  │     TTS      │  │
│  │ (Qwen/   │  │ (Qwen/   │  │              │  │
│  │ DeepSeek)│  │ DeepSeek)│  │              │  │
│  └────┬─────┘  └────┬─────┘  └──────┬───────┘  │
│       └──────────────┼───────────────┘          │
│                ┌─────┴─────┐                    │
│                │ MCP Client│←── IoT Control      │
│                └─────┬─────┘                    │
└──────────────────────┼──────────────────────────┘
                       │ WebSocket / MQTT+UDP
┌──────────────────────┼──────────────────────────┐
│                ┌─────┴─────┐   ESP32 Device      │
│                │ MCP Server│→── Device Control    │
│                └─────┬─────┘                     │
│       ┌──────────────┼───────────────┐          │
│  ┌────┴─────┐  ┌─────┴────┐  ┌──────┴───────┐  │
│  │  Audio   │  │ Display  │  │   Buttons/   │  │
│  │ (Opus)   │  │ (LCD/    │  │   LEDs/GPIO  │  │
│  │          │  │  OLED)   │  │              │  │
│  └──────────┘  └──────────┘  └──────────────┘  │
└─────────────────────────────────────────────────┘
```

## Core Components

### 1. Application Layer (`main/application.cc`)
- Central state machine managing device lifecycle
- States: Unknown → Starting → Idle → Connecting → Listening → Speaking
- Coordinates audio, display, protocol, and MCP

### 2. Audio Pipeline
- **Input**: Microphone → AEC/NR/AGC → OPUS encode → send to server
- **Output**: Receive OPUS → decode → resample → speaker output
- **Codec**: ES8311 (most common), ES8374, ES8388, ES8389, BoxAudioCodec, NoAudioCodec
- **Sample rates**: 16 kHz (device uplink), 24 kHz (server downlink)
- **Frame duration**: 60 ms OPUS frames

### 3. Communication Protocols

#### WebSocket (`main/protocols/websocket_protocol.cc`)
- Single connection for control + audio
- Binary frames for Opus audio, text frames for JSON messages
- Simple, firewall-friendly

#### MQTT + UDP (`main/protocols/mqtt_udp_protocol.cc`)
- MQTT for control messages, UDP for real-time audio
- UDP audio encrypted with AES-CTR
- Lower latency, higher complexity

### 4. MCP (Model Context Protocol)
- JSON-RPC 2.0 over WebSocket or MQTT
- Device registers tools via `McpServer::AddTool`
- Backend discovers tools via `tools/list`, invokes via `tools/call`
- Two tool types: regular (AI-callable) and user-only (privileged)

### 5. Display System
- **LVGL-based**: SPI/RGB LCD panels (ST7789, ILI9341, SH8601, etc.)
- **OLED**: SSD1306, SH1107
- **Emote display**: Custom animation system for expressions
- **UI styles**: Default message, WeChat-style, Emote animation

### 6. Wake Word Detection
- **ESP-SR**: On-device neural wake word engine
- **Wakenet + AFE**: With acoustic front-end for AEC
- Configurable wake words via custom assets

### 7. Board System (`main/boards/`)
- Each board: `xxx_board.cc` + `config.h` + `config.json`
- Inheritance: `Board` → `WifiBoard` / `Ml307Board` / `DualNetworkBoard`
- `DECLARE_BOARD()` macro registers the board class

## Directory Structure

```
xiaozhi-esp32/
├── main/
│   ├── application.cc          # Central app logic
│   ├── device_state.h          # State machine definitions
│   ├── mcp_server.cc           # MCP tool registration
│   ├── audio/                  # Audio pipeline (codec, opus, AEC)
│   ├── boards/                 # 70+ board implementations
│   │   ├── common/             # Shared components (displays, codecs, buttons)
│   │   ├── esp-box-3/          # Example board
│   │   └── waveshare/          # Manufacturer grouping
│   ├── display/                # Display drivers and UI
│   ├── protocols/              # WebSocket, MQTT+UDP protocols
│   ├── codecs/                 # Audio codec drivers
│   └── Kconfig.projbuild       # Board type selection
├── docs/                       # Developer documentation
├── scripts/
│   ├── release.py              # Build & package firmware
│   └── spiffs_assets/          # Asset generation tools
├── partitions/v2/              # Partition tables (8m, 16m, 32m)
└── .clang-format               # Code style (Google C++ base)
```

## Build System

### Quick Build
```bash
idf.py set-target esp32s3
idf.py menuconfig  # select board type
idf.py build
idf.py flash monitor
```

### Release Build
```bash
python scripts/release.py <board-directory>
```

## Key Config Files per Board

| File | Purpose |
|------|---------|
| `config.h` | Pin assignments, sample rates, display params |
| `config.json` | Target chip, flash size, partition table, sdkconfig |
| `xxx_board.cc` | Board class implementation |
| `README.md` | Board-specific documentation |

## Supported Chips

| Chip | Use Case |
|------|----------|
| ESP32 | Basic boards, cost-optimized |
| ESP32-S3 | Most boards, AI features, PSRAM |
| ESP32-C3 | Low-cost, RISC-V |
| ESP32-C6 | WiFi 6, Thread/Zigbee |
| ESP32-P4 | High-performance, display-focused |

## Networking

- **WiFi**: Default for most boards
- **ML307 Cat.1 4G**: Cellular connectivity (`Ml307Board`)
- **Dual**: WiFi + 4G switchable (`DualNetworkBoard`)
- **RNDIS**: USB networking for ESP32-S3/P4 (`RndisBoard`)
- **BluFi**: BLE-based WiFi provisioning

## Power Management

- `Axp2101`: PMIC helper
- `Sy6970`: Battery charger
- `AdcBatteryMonitor`: ADC-based battery voltage
- `PowerSaveTimer` / `SleepTimer`: Light-sleep scheduling

## Customization

- **Wake words**: Custom via [xiaozhi-assets-generator](https://github.com/78/xiaozhi-assets-generator)
- **Fonts**: Multiple sizes (14px to 30px), Chinese/English/Japanese
- **Emojis**: twemoji_32 (small) / twemoji_64 (large)
- **Backgrounds**: Custom chat backgrounds
- **Languages**: Chinese, English, Japanese

## V2 Partition Layout

- `ota_0` + `ota_1`: Dual OTA app partitions
- `assets`: SPIFFS partition for network-loadable content
- Sizes: 8MB (2MB assets) → 32MB (16MB assets)

## Code Style

- Google C++ style with clang-format
- 4-space indent, 100-char line width
- Format before commit: `find main -iname '*.h' -o -iname '*.cc' | xargs clang-format -i`
