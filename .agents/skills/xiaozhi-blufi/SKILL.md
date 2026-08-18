---
name: xiaozhi-blufi
description: BluFi BLE-based WiFi provisioning for XiaoZhi
---

# XiaoZhi BluFi Provisioning

This skill covers BluFi (BLE-based WiFi provisioning) in the XiaoZhi firmware.

## Overview

BluFi allows a phone to configure WiFi credentials on the ESP32 device over Bluetooth Low Energy.

## Prerequisites

- Chip and firmware configuration supporting BLE
- Enable in `idf.py menuconfig`: `WiFi Configuration Method -> Esp Blufi` (`CONFIG_USE_ESP_BLUFI_WIFI_PROVISIONING=y`)
- Disable Hotspot option if using BluFi (they are mutually exclusive)
- Exactly one of `CONFIG_BT_BLUEDROID_ENABLED` / `CONFIG_BT_NIMBLE_ENABLED` must be selected

## Workflow

1. Phone connects to device via BLE using EspBlufi app
2. Phone sends WiFi SSID/password (can also scan for networks)
3. Device stores credentials in `SsidManager` (NVS via `esp-wifi-connect` component)
4. Device scans and connects to WiFi
5. Success/failure reported back over BluFi

## Steps

1. **Configure**: Enable `Esp Blufi` in menuconfig, build and flash
2. **Trigger**: First boot with no stored WiFi credentials enters provisioning automatically
3. **Phone**: Open EspBlufi app, scan/connect, optionally enable encryption, enter WiFi credentials
4. **Result**: Success → device connects to WiFi; Failure → retry or check router

## Important Notes

- BluFi and hotspot provisioning are mutually exclusive
- Clear stored SSID (`wifi` NVS namespace) between tests
- IDF 5.5.2: Bluetooth name is `"Xiaozhi-Blufi"`; IDF 5.5.1: `"BLUFI_DEVICE"`
- Follow official protocol frame format if writing custom BluFi client
