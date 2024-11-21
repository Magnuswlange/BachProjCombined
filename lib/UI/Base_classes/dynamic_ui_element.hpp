#pragma once
#include "ui_element.hpp"

class DynamicUIElement : public UIElement
{
public:
    DynamicUIElement(TFT &tft, int16_t x1, int16_t x2, int16_t y1, int16_t y2);
    ~DynamicUIElement();
    virtual void Update() = 0;
    virtual void Draw() = 0; // reassert for readability
};