---
name: xiaozhi-websocket
description: WebSocket communication protocol
---

# XiaoZhi WebSocket Communication Protocol

This skill covers the WebSocket protocol between the device and server.

## Overall Flow

1. Device boots → initializes audio, display, LEDs → connects to network
2. On voice session start (wake-up/button), device opens WebSocket
3. Device sends `"type":"hello"` → server replies with `"type":"hello"`
4. Bidirectional exchange: binary Opus audio + text JSON messages
5. Session ends → WebSocket closes → device returns to idle

## Connection Setup

Device sends these headers:
- `Authorization`: `Bearer <token>`
- `Protocol-Version`: protocol version number
- `Device-Id`: physical MAC address
- `Client-Id`: software-generated UUID

## Hello Exchange

### Device → Server
```json
{
  "type": "hello",
  "version": 1,
  "features": { "mcp": true, "aec": true },
  "transport": "websocket",
  "audio_params": {
    "format": "opus",
    "sample_rate": 16000,
    "channels": 1,
    "frame_duration": 60
  }
}
```

### Server → Device
```json
{
  "type": "hello",
  "transport": "websocket",
  "session_id": "xxx",
  "audio_params": {
    "format": "opus",
    "sample_rate": 24000,
    "channels": 1,
    "frame_duration": 60
  }
}
```

## JSON Message Types

### Device → Server
- **Listen**: `{"type":"listen", "state":"start|stop|detect", "mode":"auto|manual|realtime"}`
- **Abort**: `{"type":"abort", "reason":"wake_word_detected"}`
- **MCP**: `{"type":"mcp", "payload": {...}}` — IoT control via JSON-RPC 2.0

### Server → Device
- **STT**: `{"type":"stt", "text":"..."}` — speech-to-text result
- **LLM**: `{"type":"llm", "emotion":"happy", "text":"😀"}` — emotion update
- **TTS**: `{"type":"tts", "state":"start|stop|sentence_start"}` — TTS lifecycle
- **MCP**: `{"type":"mcp", "payload": {...}}` — IoT commands
- **System**: `{"type":"system", "command":"reboot"}`
- **Alert**: `{"type":"alert", "status":"Warning", "message":"Battery low", "emotion":"sad"}`

## Binary Protocol Versions

### Version 1 (default)
Raw Opus frames, no metadata.

### Version 2
```c
struct BinaryProtocol2 {
    uint16_t version;
    uint16_t type;           // 0: OPUS, 1: JSON
    uint32_t reserved;
    uint32_t timestamp;      // ms (useful for server-side AEC)
    uint32_t payload_size;
    uint8_t payload[];
} __attribute__((packed));
```

### Version 3
```c
struct BinaryProtocol3 {
    uint8_t type;
    uint8_t reserved;
    uint16_t payload_size;
    uint8_t payload[];
} __attribute__((packed));
```

## Device States

- `kDeviceStateUnknown` → `kDeviceStateStarting` → `kDeviceStateIdle`
- `kDeviceStateIdle` → `kDeviceStateConnecting` → `kDeviceStateListening`
- `kDeviceStateListening` ↔ `kDeviceStateSpeaking`
- Any → `kDeviceStateIdle` (on abort/error)

## Audio

- Default: Opus, 16 kHz, mono, 60 ms frames
- Server may use 24 kHz on downlink
- Frames in `listening` state are dropped to avoid mic conflicts

## Error Handling

- Connection failure: `on_network_error_()` fires, device shows alert
- Server disconnect: `OnDisconnected()` → `on_audio_channel_closed_()` → idle
- Missing `type` field: logged and ignored
