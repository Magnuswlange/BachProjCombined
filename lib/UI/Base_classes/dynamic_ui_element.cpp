#include "dynamic_ui_element.hpp"

DynamicUIElement::DynamicUIElement(TFT &tft, int16_t x1, int16_t x2, int16_t y1, int16_t y2)
    : UIElement(tft, x1, x2, y1, y2)
{
    debug("DynamicUIElement created");
}

DynamicUIElement::~DynamicUIElement()
{
    debug("DynamicUIElement destroyed");
}