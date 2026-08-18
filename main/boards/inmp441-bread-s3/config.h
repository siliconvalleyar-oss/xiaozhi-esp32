#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// =============================================================================
// Audio Configuration
// =============================================================================

#define AUDIO_INPUT_SAMPLE_RATE  16000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

// Use Simplex I2S: separate buses for microphone and speaker
// This is required when using INMP441 (mic) + MAX98357 (speaker)
#define AUDIO_I2S_METHOD_SIMPLEX

#ifdef AUDIO_I2S_METHOD_SIMPLEX

// --- INMP441 Microphone pins ---
#define AUDIO_I2S_MIC_GPIO_WS   GPIO_NUM_5    // Word Select (LRCK)
#define AUDIO_I2S_MIC_GPIO_SCK  GPIO_NUM_6    // Serial Clock (BCLK)
#define AUDIO_I2S_MIC_GPIO_DIN  GPIO_NUM_4    // Serial Data Out (SD)

// --- MAX98357 Speaker pins ---
#define AUDIO_I2S_SPK_GPIO_DOUT GPIO_NUM_7    // Serial Data In (DIN)
#define AUDIO_I2S_SPK_GPIO_BCLK GPIO_NUM_15   // Bit Clock (BCLK)
#define AUDIO_I2S_SPK_GPIO_LRCK GPIO_NUM_16   // Word Select (LRC)

#else
// Duplex mode (shared bus) - not recommended for INMP441
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_5
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_6
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_4
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_7

#endif

// =============================================================================
// Display Configuration (SSD1306 OLED 128x64 via I2C)
// =============================================================================

#define DISPLAY_SDA_PIN GPIO_NUM_3
#define DISPLAY_SCL_PIN GPIO_NUM_2
#define DISPLAY_WIDTH   128
#define DISPLAY_HEIGHT  64

#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y true

// =============================================================================
// Button Configuration
// =============================================================================

#define BOOT_BUTTON_GPIO    GPIO_NUM_0   // Boot button (click = toggle chat)
#define ACTION_BUTTON_GPIO  GPIO_NUM_1   // Push-to-talk button

// =============================================================================
// LED Configuration
// =============================================================================

#define BUILTIN_LED_GPIO GPIO_NUM_48    // Onboard LED (status indicator)

// =============================================================================
// MCP IoT Tools
// =============================================================================

// GPIO for external lamp/relay control via MCP
#define LAMP_GPIO GPIO_NUM_17

#endif // _BOARD_CONFIG_H_
