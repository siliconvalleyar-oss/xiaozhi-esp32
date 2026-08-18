---
name: xiaozhi-mcp-protocol
description: MCP protocol interaction flow and wire format
---

# XiaoZhi MCP Protocol Interaction Flow

This skill covers the MCP (Model Context Protocol) wire protocol between the backend API (MCP client) and the ESP32 device (MCP server).

## Message Format

MCP messages are wrapped in WebSocket or MQTT transport. Inner payload follows JSON-RPC 2.0:

```json
{
  "session_id": "...",
  "type": "mcp",
  "payload": {
    "jsonrpc": "2.0",
    "method": "...",
    "params": { ... },
    "id": ...,
    "result": { ... },
    "error": { ... }
  }
}
```

## Interaction Flow

### 1. Connection & Capability Announcement

After device boots and connects, it sends transport hello:

```json
{
  "type": "hello",
  "version": 1,
  "features": { "mcp": true },
  "transport": "websocket",
  "audio_params": { ... },
  "session_id": "..."
}
```

### 2. Initialize MCP Session

Backend sends `initialize`:

```json
{
  "jsonrpc": "2.0",
  "method": "initialize",
  "params": {
    "capabilities": {
      "vision": {
        "url": "http://...",
        "token": "..."
      }
    }
  },
  "id": 1
}
```

Device responds:

```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "protocolVersion": "2024-11-05",
    "capabilities": { "tools": {} },
    "serverInfo": {
      "name": "BOARD_NAME",
      "version": "firmware_version"
    }
  }
}
```

### 3. Discover Tools

```json
{
  "jsonrpc": "2.0",
  "method": "tools/list",
  "params": {
    "cursor": "",
    "withUserTools": false
  },
  "id": 2
}
```

Response includes `tools` array and optional `nextCursor` for pagination.

### 4. Call a Tool

```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.audio_speaker.set_volume",
    "arguments": { "volume": 50 }
  },
  "id": 3
}
```

### 5. Device Notifications

Device-to-server notifications (no `id` field):

```json
{
  "jsonrpc": "2.0",
  "method": "notifications/state_changed",
  "params": {
    "newState": "idle",
    "oldState": "connecting"
  }
}
```

## User-only Tools

- **Regular tools**: `McpServer::AddTool` — visible in default `tools/list`
- **User-only tools**: `McpServer::AddUserOnlyTool` — hidden, requires `withUserTools=true`

## Key Points

- All IoT capability discovery and control flows through MCP (`type: "mcp"`)
- The legacy `type: "iot"` protocol is deprecated
- MCP works over both WebSocket and MQTT
- Pagination: use `nextCursor` from `tools/list` response
- Cross-check implementation details against `main/mcp_server.cc` and per-board `InitializeTools()`
