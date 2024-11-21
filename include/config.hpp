#pragma once
#include <cstdint>
#include <Arduino.h>
#include <TFT_eSPI.h>

namespace HX711
{
    constexpr uint8_t SCK_PIN = GPIO_NUM_12;
    constexpr uint8_t DOUT_PIN = GPIO_NUM_13;
};

namespace TFT_Properties
{
    constexpr int16_t WIDTH = 320;
    constexpr int16_t LENGTH = 240;
    constexpr uint8_t TEXT_SIZE = 3;
    constexpr uint16_t TEXT_COLOR = TFT_WHITE;
    constexpr uint16_t BG_COLOR = TFT_WHITE;
    constexpr gpio_num_t BACKLIGHT_PIN = GPIO_NUM_2;
    constexpr gpio_num_t IRQ_PIN = GPIO_NUM_0;
};

namespace Misc
{
    constexpr gpio_num_t BUZZER_PIN = GPIO_NUM_22;
};
