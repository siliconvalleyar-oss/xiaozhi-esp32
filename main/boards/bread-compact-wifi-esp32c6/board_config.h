/**
 * @file board_config.h
 * @brief Definición de pines y parámetros hardware para ESP32-C6-DevKitC-1
 *
 * ┌─────────────────────────────────────────────────────────────┐
 * │  MAPA DE PINES — ESP32-C6-DevKitC-1                        │
 * ├──────────┬──────────┬───────────────────────────────────────┤
 * │  GPIO   │  Función │  Notas                                │
 * ├──────────┼──────────┼───────────────────────────────────────┤
 * │  GPIO0   │ PWR MIC  │  INMP441 VDD  (HIGH = encendido)      │
 * │  GPIO1   │ L/R MIC  │  INMP441 canal (HIGH = derecho)       │
 * │  GPIO6   │ I2C SDA  │  OLED SSD1306 (y otros dispositivos) │
 * │  GPIO7   │ I2C SCL  │  OLED SSD1306 (y otros dispositivos) │
 * │  GPIO9   │ BOOT BTN │  Activo en bajo, pull-up interno      │
 * │  GPIO10  │ I2S DOUT │  → MAX98357A DIN                     │
 * │  GPIO20  │ I2S DIN  │  ← INMP441 SD                        │
 * │  GPIO22  │ I2S BCLK │  Compartido: MAX98357A BCLK + INMP441 SCK │
 * │  GPIO23  │ I2S WS   │  Compartido: MAX98357A LRC + INMP441 WS   │
 * ├──────────┼──────────┼───────────────────────────────────────┤
 * │  LED     │ GPIO18   │  LED de estado (config.)             │
 * └──────────┴──────────┴───────────────────────────────────────┘
 *
 * PINES A EVITAR en ESP32-C6-DevKitC-1:
 *   GPIO19, GPIO20 → USB D-/D+ (comunicación con PC)
 *   GPIO11, GPIO12, GPIO13, GPIO14, GPIO15, GPIO24, GPIO25 → Flash SPI
 */

#pragma once

// ════════════════════════════════════════════════════════════════
//  I2C — Display OLED SSD1306
// ════════════════════════════════════════════════════════════════
#define BOARD_I2C_PORT          I2C_NUM_0
#define BOARD_I2C_SDA           6       // GPIO6
#define BOARD_I2C_SCL           7       // GPIO7
#define BOARD_I2C_FREQ_HZ       400000  // 400 kHz (Fast Mode)

// Display SSD1306 128×64
#define BOARD_DISPLAY_I2C_ADDR  0x3C    // Dirección estándar SSD1306 (0x3C o 0x3D)
#define BOARD_DISPLAY_WIDTH     128
#define BOARD_DISPLAY_HEIGHT    64
#define BOARD_DISPLAY_MIRROR_X  false
#define BOARD_DISPLAY_MIRROR_Y  false

// ════════════════════════════════════════════════════════════════
//  Botones
// ════════════════════════════════════════════════════════════════
#define BOARD_BOOT_BUTTON_GPIO  9       // GPIO9 — Botón BOOT del DevKitC-1

// ════════════════════════════════════════════════════════════════
//  LED de estado
// ════════════════════════════════════════════════════════════════
// GPIO18 → LED RGB / WS2812 en el ESP32-C6-DevKitC-1
// Comentar si la revisión de la placa no tiene LED integrado
#define BOARD_STATUS_LED_GPIO   18

// ════════════════════════════════════════════════════════════════
//  Audio I2S full-duplex (ESP32-C6 tiene 1 solo I2S)
//  BCLK y WS son COMPARTIDOS entre el MAX98357A y el INMP441.
// ════════════════════════════════════════════════════════════════
#define BOARD_I2S_BCLK        GPIO_NUM_22   // MAX98357A BCLK + INMP441 SCK
#define BOARD_I2S_WS          GPIO_NUM_23   // MAX98357A LRC + INMP441 WS
#define BOARD_I2S_DOUT        GPIO_NUM_10   // → MAX98357A DIN
#define BOARD_I2S_DIN         GPIO_NUM_20   // ← INMP441 SD

// INMP441 — control por GPIO
#define BOARD_MIC_POWER_PIN   GPIO_NUM_0    // VDD (HIGH = encendido)
#define BOARD_MIC_LR_PIN      GPIO_NUM_1    // canal (HIGH = derecho)

#define BOARD_AUDIO_SAMPLE_RATE 24000       // reloj I2S compartido (24 kHz)

// ════════════════════════════════════════════════════════════════
//  Identificación de placa
// ════════════════════════════════════════════════════════════════
#define BOARD_NAME              "ESP32-C6-DevKitC-1"
#define BOARD_FLASH_SIZE_MB     8
