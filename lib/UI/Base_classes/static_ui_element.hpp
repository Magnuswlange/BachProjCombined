#pragma once
#include "ui_element.hpp"

class StaticUIElement : public UIElement
{
public:
    StaticUIElement(TFT &tft, int16_t x1, int16_t x2, int16_t y1, int16_t y2);
    ~StaticUIElement();
    virtual void Draw() = 0; // reassert for readability
};