#include "text_component.hpp"

Text::Text(TFT &tft, const char *text, int16_t x, int16_t y, int16_t textColor)
    : UIElement(tft, x, (x + tft.GetTFT().textWidth(text)), y, (y + tft.GetTFT().fontHeight() + 2 * m_TEXT_PADDING)),
      m_TextColor(textColor), m_Text(text)
{
    debug("Text component created");
    Draw();
}

Text::~Text()
{
    debug("Text component destroyed");
}

void Text::Draw()
{
    if (IsOOB())
    {
        return;
    }

    TFT_eSPI &_TFT = GetTFT().GetTFT();
    _TFT.setTextColor(m_TextColor);
    debug("Drawing text at: " + (GetX1() + m_TEXT_PADDING) + ", " + GetY1() + m_TEXT_PADDING);
    _TFT.setCursor(GetX1() + m_TEXT_PADDING, GetY1() + m_TEXT_PADDING);
    _TFT.println(m_Text);

    debug("X1: " + GetX1() + ", X2: " + GetX2() + ", Y1: " + GetY1() + ", Y2: " + GetY2());
}

const char *Text::GetText()
{
    return m_Text;
}