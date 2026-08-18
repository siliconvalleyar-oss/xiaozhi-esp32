/**
 * INMP441 Breadboard S3 Board
 *
 * A complete XiaoZhi AI chatbot board using:
 *   - ESP32-S3 DevKit
 *   - INMP441 I2S digital microphone
 *   - MAX98357 I2S Class D amplifier + speaker
 *   - SSD1306 OLED 128x64 display (I2C)
 *   - Boot button (toggle chat / WiFi config)
 *   - Push-to-talk button
 *   - Status LED
 *   - MCP IoT tools (lamp control)
 *
 * Wiring: see README.md for full pin diagram.
 */

#include "wifi_board.h"
#include "codecs/no_audio_codec.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"
#include "lamp_controller.h"
#include "led/single_led.h"
#include "display/oled_display.h"
#include "system_reset.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>

#define TAG "INMP441-Bread-S3"

// =============================================================================
// Board Class
// =============================================================================

class Inmp441BreadS3Board : public WifiBoard {
private:
    Button boot_button_;
    Button action_button_;

    // Display
    i2c_master_bus_handle_t display_i2c_bus_;
    esp_lcd_panel_io_handle_t panel_io_ = nullptr;
    esp_lcd_panel_handle_t panel_ = nullptr;
    Display* display_ = nullptr;

    // LED
    SingleLed* led_ = nullptr;

    // =========================================================================
    // Display
    // =========================================================================

    void InitializeDisplayI2c() {
        i2c_master_bus_config_t bus_config = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = DISPLAY_SDA_PIN,
            .scl_io_num = DISPLAY_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &display_i2c_bus_));
    }

    void InitializeSsd1306Display() {
        esp_lcd_panel_io_i2c_config_t io_config = {
            .dev_addr = 0x3C,
            .on_color_trans_done = nullptr,
            .user_ctx = nullptr,
            .control_phase_bytes = 1,
            .dc_bit_offset = 6,
            .lcd_cmd_bits = 8,
            .lcd_param_bits = 8,
            .flags = {
                .dc_low_on_data = 0,
                .disable_control_phase = 0,
            },
            .scl_speed_hz = 400 * 1000,
        };

        ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c_v2(display_i2c_bus_, &io_config, &panel_io_));

        ESP_LOGI(TAG, "Install SSD1306 driver");
        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = -1;
        panel_config.bits_per_pixel = 1;

        esp_lcd_panel_ssd1306_config_t ssd1306_config = {
            .height = static_cast<uint8_t>(DISPLAY_HEIGHT),
        };
        panel_config.vendor_config = &ssd1306_config;

        ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(panel_io_, &panel_config, &panel_));
        ESP_LOGI(TAG, "SSD1306 driver installed");

        ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_));
        if (esp_lcd_panel_init(panel_) != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize display");
            display_ = new NoDisplay();
            return;
        }

        ESP_LOGI(TAG, "Turning display on");
        ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_, true));

        display_ = new OledDisplay(panel_io_, panel_, DISPLAY_WIDTH, DISPLAY_HEIGHT,
                                   DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
    }

    // =========================================================================
    // LED
    // =========================================================================

    void InitializeLed() {
        // Configure LED GPIO as output
        gpio_config_t io_conf = {
            .pin_bit_mask = 1ULL << BUILTIN_LED_GPIO,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE
        };
        gpio_config(&io_conf);
        gpio_set_level(BUILTIN_LED_GPIO, 0);
    }

    void SetLed(bool on) {
        gpio_set_level(BUILTIN_LED_GPIO, on ? 1 : 0);
    }

    // =========================================================================
    // Buttons
    // =========================================================================

    void InitializeButtons() {
        // Boot button: click to toggle chat or enter WiFi config
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            SetLed(true);
            app.ToggleChatState();
        });

        // Boot button: long press to factory reset
        boot_button_.OnLongPress([this]() {
            ESP_LOGW(TAG, "Long press detected — factory reset");
            SystemReset().Reset();
        });

        // Push-to-talk button: press-and-hold to talk
        action_button_.OnPressDown([this]() {
            SetLed(true);
            Application::GetInstance().StartListening();
        });

        action_button_.OnPressUp([this]() {
            SetLed(false);
            Application::GetInstance().StopListening();
        });
    }

    // =========================================================================
    // MCP IoT Tools
    // =========================================================================

    void InitializeTools() {
        auto& mcp_server = McpServer::GetInstance();

        // Built-in lamp controller
        static LampController lamp(LAMP_GPIO);

        // Custom tool: control the status LED
        mcp_server.AddTool("self.led.set",
            "Set the onboard LED on or off",
            PropertyList({
                Property("on", kPropertyTypeBoolean)
            }),
            [this](const PropertyList& properties) -> ReturnValue {
                bool on = properties["on"].value<bool>();
                SetLed(on);
                return on;
            });

        // Custom tool: get board info
        mcp_server.AddTool("self.board.get_info",
            "Get information about this INMP441 breadboard setup",
            PropertyList(),
            [](const PropertyList&) -> ReturnValue {
                return std::string(
                    "Board: INMP441 Bread S3\n"
                    "MCU: ESP32-S3\n"
                    "Mic: INMP441 I2S\n"
                    "Speaker: MAX98357 I2S\n"
                    "Display: SSD1306 128x64 OLED\n"
                    "Audio mode: Simplex I2S"
                );
            });
    }

public:
    Inmp441BreadS3Board()
        : WifiBoard(),
          boot_button_(BOOT_BUTTON_GPIO),
          action_button_(ACTION_BUTTON_GPIO)
    {
        InitializeLed();
        InitializeDisplayI2c();
        InitializeSsd1306Display();
        InitializeButtons();
        InitializeTools();

        ESP_LOGI(TAG, "Board initialized — INMP441 + MAX98357 + SSD1306");
    }

    // =========================================================================
    // Audio Codec: I2S Simplex (INMP441 + MAX98357)
    // =========================================================================

    virtual AudioCodec* GetAudioCodec() override {
#ifdef AUDIO_I2S_METHOD_SIMPLEX
        static NoAudioCodecSimplex audio_codec(
            AUDIO_INPUT_SAMPLE_RATE,
            AUDIO_OUTPUT_SAMPLE_RATE,
            // Speaker pins
            AUDIO_I2S_SPK_GPIO_BCLK,
            AUDIO_I2S_SPK_GPIO_LRCK,
            AUDIO_I2S_SPK_GPIO_DOUT,
            // Microphone pins
            AUDIO_I2S_MIC_GPIO_SCK,
            AUDIO_I2S_MIC_GPIO_WS,
            AUDIO_I2S_MIC_GPIO_DIN);
#else
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

    // =========================================================================
    // Display
    // =========================================================================

    virtual Display* GetDisplay() override {
        return display_;
    }
};

DECLARE_BOARD(Inmp441BreadS3Board);
