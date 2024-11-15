#pragma once
#include <cstdint>
#include "tft_wrapper.hpp"
#include "ui_element.hpp"
#include "DebugUtils.hpp"

class Text : public UIElement
{
public:
    Text(TFT &tft, const char *text, int16_t x, int16_t y, int16_t textColor = TFT_BLACK);
    ~Text();
    void Draw() override;
    const char *GetText();

private:
    uint16_t m_TextColor;
    const char *m_Text;
    const int8_t m_TEXT_PADDING = 2;
};