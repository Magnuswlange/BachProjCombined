#pragma once
#include <SPI.h>
#include <TFT_eSPI.h>
#include "DebugUtils.hpp"

struct Vector2
{
    uint16_t x, y;
};

class TFT
{
public:
    TFT(int16_t width, int16_t height, int16_t textColor, int16_t bgColor, uint8_t textSize);
    void Init(bool calibrate = false);
    void Clear();
    Vector2 GetTouchCoords();
    void SetTouchActive();
    void SetTFTActive();
    TFT_eSPI &GetTFT();
    // SetBrightness
    // DrawButton
    // DrawRect
    // DrawText
    // CalibrateTouch
    // IsTouched
    // SetTouchThreshold
    // SetTextSize
    // SetBackgroundColor

private:
    TFT_eSPI m_TFT;
    int16_t m_BG_COLOR;
    const int8_t m_TEXT_PADDING = 2;
    const int8_t m_GAP = 2;
    uint16_t m_CalData[5]; // ?convert to C++ style array
};