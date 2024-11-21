#include "static_ui_element.hpp"

StaticUIElement::StaticUIElement(TFT &tft, int16_t x1, int16_t x2, int16_t y1, int16_t y2)
    : UIElement(tft, x1, x2, y1, y2)
{
    debug("StaticUIElement created");
}

StaticUIElement::~StaticUIElement()
{
    debug("StaticUIElement destroyed");
}