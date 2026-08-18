# XiaoZhi MCP IoT Control Usage

This skill covers how to implement IoT control for ESP32 devices using the MCP (Model Context Protocol).

## Overview

MCP uses JSON-RPC 2.0 to let the backend discover and invoke "tools" registered by the device.

## Typical Flow

1. Device boots and connects to backend (WebSocket or MQTT)
2. Backend sends `initialize` to start MCP session
3. Backend issues `tools/list` to discover tools and input schemas
4. Backend calls tools with `tools/call`

## Registering Tools

### Regular Tools (AI-callable)

```cpp
auto& mcp_server = McpServer::GetInstance();

// No arguments
mcp_server.AddTool("self.dog.forward",
    "Move the robot forward",
    PropertyList(),
    [this](const PropertyList&) -> ReturnValue {
        servo_dog_ctrl_send(DOG_STATE_FORWARD, NULL);
        return true;
    });

// With arguments
mcp_server.AddTool("self.light.set_rgb",
    "Set the RGB color of the light",
    PropertyList({
        Property("r", kPropertyTypeInteger, 0, 255),
        Property("g", kPropertyTypeInteger, 0, 255),
        Property("b", kPropertyTypeInteger, 0, 255)
    }),
    [this](const PropertyList& properties) -> ReturnValue {
        int r = properties["r"].value<int>();
        int g = properties["g"].value<int>();
        int b = properties["b"].value<int>();
        SetLedColor(r, g, b);
        return true;
    });
```

### User-only Tools (privileged, hidden from AI)

```cpp
mcp_server.AddUserOnlyTool("self.display.clear_cache",
    "Clear locally cached images. User-only action.",
    PropertyList(),
    [](const PropertyList&) -> ReturnValue {
        ClearLocalCache();
        return true;
    });
```

User-only tools only appear when `tools/list` is called with `params.withUserTools = true`.

## Tool Registration API

```cpp
void AddTool(
    const std::string& name,           // unique, e.g. self.dog.forward
    const std::string& description,    // for the AI model
    const PropertyList& properties,    // input params: bool, int, string
    std::function<ReturnValue(const PropertyList&)> callback
);
```

## Built-in Tools

### AI-callable (from AddCommonTools)

| Tool | Description |
|------|-------------|
| `self.get_device_status` | Returns volume, screen, battery, network, etc. |
| `self.audio_speaker.set_volume` | Set speaker volume (0-100) |
| `self.screen.set_brightness` | Set screen brightness (0-100) |
| `self.screen.set_theme` | Switch UI theme ("light" or "dark") |
| `self.camera.take_photo` | Take a picture and answer a question about it |

### User-only (from AddUserOnlyTools)

| Tool | Description |
|------|-------------|
| `self.get_system_info` | Return JSON system info |
| `self.reboot` | Reboot after delay |
| `self.upgrade_firmware` | Download and install firmware from URL |
| `self.screen.get_info` | Screen width/height info |
| `self.screen.snapshot` | Snapshot screen as JPEG and upload |
| `self.screen.preview_image` | Download and display image from URL |
| `self.assets.set_download_url` | Set assets partition download URL |

## JSON-RPC Examples

### Get tools list
```json
{
  "jsonrpc": "2.0",
  "method": "tools/list",
  "params": { "cursor": "", "withUserTools": false },
  "id": 1
}
```

### Call a tool
```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.chassis.go_forward",
    "arguments": {}
  },
  "id": 2
}
```

### Successful response
```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "result": {
    "content": [{ "type": "text", "text": "true" }],
    "isError": false
  }
}
```

### Error response
```json
{
  "jsonrpc": "2.0",
  "id": 2,
  "error": {
    "code": -32601,
    "message": "Unknown tool: self.non_existent_tool"
  }
}
```

## Best Practices

- Prefer MCP for any new IoT control
- Use `module.action` naming style for tool names
- Keep descriptions concise but clear for the AI model
- Use `AddUserOnlyTool` for privileged actions (reboot, firmware upgrade, snapshots)
- Use `AddTool` for regular device control (speaker, LED, servo, GPIO)
