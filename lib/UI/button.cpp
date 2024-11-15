#include "button.hpp"

int16_t Button::m_LastButtonY2 = 0; // define static member vars outside class definition to allocate mem

Button::Button(TFT &tft, const char *text, int16_t x, int16_t y, uint16_t textColor, uint16_t rectColor)
    : m_TFT(tft),
      m_TextComponent(tft, text, x, y, textColor),
      m_ButtonComponent(tft, x, y, m_TFT.GetTFT().textWidth(text) + 2)
{
    debug("Button created");
    Draw();
}

Button::Button(TFT &tft, const char *text, uint16_t textColor, uint16_t rectColor)
    : m_TFT(tft),
      m_TextComponent(tft, text, 0, m_LastButtonY2, textColor),
      m_ButtonComponent(tft, 0, m_LastButtonY2, m_TFT.GetTFT().textWidth(text) + 2)
{
    debug("Button created");
    Draw();
    m_LastButtonY2 += m_ButtonComponent.GetY2() + m_BUTTON_GAP;
}

Button::~Button()
{
    debug("Button destroyed");
}

void Button::Draw()
{
    // if OOB
    m_ButtonComponent.Draw();
    m_TextComponent.Draw();

    // int32_t rectHeight = _TFT.fontHeight() + 2 * m_TEXT_PADDING;
    // m_YEnd = m_YStart + rectHeight;
    // m_XEnd = m_XStart + _TFT.textWidth(m_TextComponent.GetText());
}

bool Button::IsClicked(int16_t x, int16_t y)
{
    if ((x >= m_ButtonComponent.GetX1() && x <= m_ButtonComponent.GetX2()) && (y >= m_ButtonComponent.GetY1() && y <= m_ButtonComponent.GetY2()))
    {
        return true;
    }
    return false;
}