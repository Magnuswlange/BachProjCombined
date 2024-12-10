#include "rounded_rectangle.hpp"

RoundedRectangle::RoundedRectangle(TFT &tft, int16_t x, int16_t y, int16_t textWidth, uint16_t color)
    : StaticUIElement(tft, x, (x + textWidth), y, (y + tft.GetTFT().fontHeight() + 2 * 2)),
      m_Color(color)
{
    debug("Rounded rectangle created");

    if (!IsOOB())
    {
        DebugDraw();
    }
}

RoundedRectangle::~RoundedRectangle()
{
    debug("Rounded rectangle destroyed");
}

void RoundedRectangle::Draw()
{
    TFT_eSPI &_TFT = GetTFT().GetTFT();
    _TFT.fillRoundRect(GetX1(), GetY1(), GetX2() - GetX1(), GetY2() - GetY1(), 5, m_Color);
}