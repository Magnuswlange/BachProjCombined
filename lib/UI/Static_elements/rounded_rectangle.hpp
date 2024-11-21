#pragma once
#include <cstdint>
#include "tft_wrapper.hpp"
#include "ui_element.hpp"
#include "DebugUtils.hpp"
#include "static_ui_element.hpp"

class RoundedRectangle : public StaticUIElement
{
public:
    RoundedRectangle(TFT &tft, int16_t x, int16_t y, int16_t textWidth, uint16_t color = TFT_DARKGREEN);
    ~RoundedRectangle();
    void Draw() override;

private:
    uint16_t m_Color;
};