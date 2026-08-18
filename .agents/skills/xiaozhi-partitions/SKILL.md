---
name: xiaozhi-partitions
description: V2 partition table layout and configuration
---

# XiaoZhi V2 Partition Table

This skill covers the v2 partition layout and its improvements over v1.

## Key Changes from V1

1. **Added Assets Partition**: Network-loadable content (wake words, themes, fonts, audio, emojis)
2. **Replaced Model Partition**: Old 960KB `model` partition replaced with larger `assets`
3. **Reduced App Partitions**: Smaller OTA partitions to accommodate assets
4. **Dynamic Content**: Update without reflashing via HTTP downloads

## Assets Partition Contents

- Wake word models (customizable, network-loadable)
- Theme files (fonts, audio effects, backgrounds, UI elements)
- Custom emoji packs
- Language configuration files

## Partition Configurations

### 8MB Flash (`partitions/v2/8m.csv`)
- `nvs`: 16KB
- `otadata`: 8KB
- `phy_init`: 4KB
- `ota_0`: 3MB
- `ota_1`: 3MB
- `assets`: 2MB

### 16MB Flash (`partitions/v2/16m.csv`) - Standard
- `nvs`: 16KB
- `otadata`: 8KB
- `phy_init`: 4KB
- `ota_0`: 4MB
- `ota_1`: 4MB
- `assets`: 8MB

### 16MB Flash ESP32-C3 (`partitions/v2/16m_c3.csv`)
- Same as standard but `assets`: 4MB (limited mmap pages on C3)

### 32MB Flash (`partitions/v2/32m.csv`)
- `nvsfactory`: 200KB
- `nvs`: 840KB
- `otadata`: 8KB
- `phy_init`: 4KB
- `ota_0`: 4MB
- `ota_1`: 4MB
- `assets`: 16MB

## Technical Details

- Assets use SPIFFS filesystem subtype
- Memory-mapped for efficient runtime access
- Built-in checksum validation
- Progressive download with progress tracking
- Graceful fallback to defaults if network update fails

## Migration from V1

1. Backup important data from old `model` partition
2. Flash new v2 partition table for your flash size
3. Device auto-downloads required assets on first boot
4. Verify all features work correctly

## Usage in config.json

```json
{
  "sdkconfig_append": [
    "CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y",
    "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""
  ]
}
```
