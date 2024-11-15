#pragma once
#include <TFT_eSPI.h>
#include <cstdint>
#include "TFT_wrapper.hpp"
#include "text_component.hpp"
#include "button_component.hpp"

class Button
{
public:
    Button(TFT &tft, const char *text, int16_t x, int16_t y, uint16_t textColor = TFT_WHITE, uint16_t rectColor = TFT_DARKGREEN);
    Button(TFT &tft, const char *text, uint16_t textColor = TFT_WHITE, uint16_t rectColor = TFT_DARKGREEN);
    ~Button();
    bool IsClicked(const int16_t x, const int16_t y);
    void Draw();

private:
    TFT &m_TFT;
    ButtonComponent m_ButtonComponent;
    Text m_TextComponent;
    static int16_t m_LastButtonY2;
    const int8_t m_BUTTON_GAP = 4;
};