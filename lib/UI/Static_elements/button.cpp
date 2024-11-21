#include "button.hpp"

int16_t Button::m_LastButtonY2 = 0; // define static member vars outside class definition to allocate mem

Button::Button(TFT &tft, const String &text, int16_t x, int16_t y, uint16_t textColor, uint16_t rectColor)
    : m_TFT(tft),
      m_Label(tft, text, x, y, textColor),
      m_RoundedRectangleComponent(tft, x, y, m_TFT.GetTFT().textWidth(text) + 2),
      StaticUIElement(m_TFT, 0, 0, 0, 0)
{
    debug("Button created");
    m_LastButtonY2 = m_RoundedRectangleComponent.GetY2() + m_BUTTON_GAP;
}

Button::Button(TFT &tft, const String &text, uint16_t textColor, uint16_t rectColor)
    : m_TFT(tft),
      m_Label(tft, text, 0, m_LastButtonY2, textColor),
      m_RoundedRectangleComponent(tft, 0, m_LastButtonY2, m_TFT.GetTFT().textWidth(text) + 2),
      StaticUIElement(m_TFT, 0, 0, 0, 0)
{
    debug("Button created");
    m_LastButtonY2 = m_RoundedRectangleComponent.GetY2() + m_BUTTON_GAP;
}

Button::~Button()
{
    debug("Button destroyed");
}

void Button::Draw()
{
    m_RoundedRectangleComponent.DebugDraw();
    m_Label.DebugDraw();
}

bool Button::IsClicked(int16_t x, int16_t y)
{
    if ((x >= m_RoundedRectangleComponent.GetX1() && x <= m_RoundedRectangleComponent.GetX2()) && (y >= m_RoundedRectangleComponent.GetY1() && y <= m_RoundedRectangleComponent.GetY2()))
    {
        return true;
    }
    return false;
}