---
name: xiaozhi-mqtt-udp
description: MQTT+UDP hybrid communication protocol
---

# XiaoZhi MQTT + UDP Hybrid Communication Protocol

This skill covers the dual-channel protocol: MQTT for control, UDP for real-time audio.

## Architecture

- **MQTT**: control messages, state synchronization, JSON payloads
- **UDP**: real-time audio, encrypted with AES-CTR

## End-to-End Flow

1. Device connects to MQTT broker
2. Device sends hello via MQTT → server responds with UDP endpoint + encryption keys
3. Device establishes UDP connection
4. Audio streams via encrypted UDP; control messages via MQTT
5. Session ends with goodbye messages

## MQTT Hello Exchange

### Device → Server
```json
{
  "type": "hello",
  "version": 3,
  "transport": "udp",
  "features": { "mcp": true, "aec": true },
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
  "transport": "udp",
  "session_id": "xxx",
  "audio_params": { "format": "opus", "sample_rate": 24000, "channels": 1, "frame_duration": 60 },
  "udp": {
    "server": "192.168.1.100",
    "port": 8888,
    "key": "0123456789ABCDEF0123456789ABCDEF",
    "nonce": "0123456789ABCDEF0123456789ABCDEF"
  }
}
```

## MQTT JSON Messages

### Device → Server
- **Listen**: `{"type":"listen", "state":"start|stop", "mode":"auto|manual"}`
- **Abort**: `{"type":"abort", "reason":"wake_word_detected"}`
- **MCP**: `{"type":"mcp", "payload": {...}}`
- **Goodbye**: `{"type":"goodbye"}`

### Server → Device
Same as WebSocket: STT, TTS, LLM, MCP, System, Alert, Goodbye, Custom

## UDP Audio Packet Format

```
|type 1B|flags 1B|payload_len 2B|ssrc 4B|timestamp 4B|sequence 4B|
|payload payload_len bytes|
```

- `type`: always `0x01`
- Encryption: AES-CTR with 128-bit key and nonce
- Counter built from timestamp + sequence number

## Sequence Number Management

- Sender: `local_sequence_` incremented monotonically
- Receiver: validates continuity, drops stale packets
- Small gaps logged as warnings but accepted

## State Machine

```
Disconnected → MqttConnecting → MqttConnected → RequestingChannel
→ ChannelOpened → UdpConnected → AudioStreaming
```

## Configuration

### MQTT Settings (from NVS)
- `endpoint`, `client_id`, `username`, `password`
- `keepalive` (default 240s), `publish_topic`

### Audio Parameters
- Format: Opus
- Sample rate: 16 kHz (device) / 24 kHz (server)
- Channels: 1 (mono)
- Frame duration: 60 ms

## Security

- **MQTT**: TLS/SSL (port 8883), username/password auth
- **UDP**: AES-CTR encryption, keys distributed via MQTT
- **Anti-replay**: monotonically increasing sequence numbers

## Comparison with WebSocket

| Feature | MQTT + UDP | WebSocket |
|---------|------------|-----------|
| Control | MQTT | WebSocket |
| Audio | UDP (encrypted) | WebSocket (binary) |
| Latency | Low (UDP) | Medium |
| Complexity | High | Low |
| Firewall | Low friendly | High friendly |

## Error Handling

- MQTT: automatic reconnect on disconnect
- UDP: no auto-retry, depends on MQTT re-negotiation
- Timeout: 120s default, based on last incoming packet
