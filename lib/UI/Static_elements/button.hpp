#pragma once
#include <TFT_eSPI.h>
#include <cstdint>
#include "TFT_wrapper.hpp"
#include "label.hpp"
#include "static_ui_element.hpp"
#include "rounded_rectangle.hpp"

class Button : public StaticUIElement
{
public:
    Button(TFT &tft, const String &label, int16_t x, int16_t y, uint16_t labelColor = TFT_WHITE, uint16_t rectColor = TFT_DARKGREEN);
    Button(TFT &tft, const String &label, uint16_t labelColor = TFT_WHITE, uint16_t rectColor = TFT_DARKGREEN);
    ~Button();
    bool IsClicked(const int16_t x, const int16_t y);
    void Draw() override;

private:
    TFT &m_TFT;
    RoundedRectangle m_RoundedRectangleComponent;
    Label m_Label;
    static int16_t m_LastButtonY2;
    const int8_t m_BUTTON_GAP = 4;
};