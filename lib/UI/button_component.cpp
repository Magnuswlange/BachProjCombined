#include "button_component.hpp"

ButtonComponent::ButtonComponent(TFT &tft, int16_t x, int16_t y, int16_t textWidth, uint16_t color)
    : UIElement(tft, x, (x + textWidth), y, (y + tft.GetTFT().fontHeight() + 2 * 2)),
      m_Color(color)
{
    debug("Button component created");
    Draw();
}

ButtonComponent::~ButtonComponent()
{
    debug("Button component destroyed");
}

void ButtonComponent::Draw()
{
    if (IsOOB())
    {
        return;
    }

    debug("X1: " + GetX1() + ", X2: " + GetX2() + ", Y1: " + GetY1() + ", Y2: " + GetY2() + "");
    TFT_eSPI &_TFT = GetTFT().GetTFT();
    _TFT.fillRoundRect(GetX1(), GetY1(), GetX2() - GetX1(), GetY2() - GetY1(), 5, m_Color);
}