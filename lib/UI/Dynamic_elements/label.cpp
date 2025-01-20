#include "label.hpp"
#include "config.hpp"

Label::Label(TFT &tft, const String &label, int16_t x, int16_t y, uint8_t id, int16_t labelColor, uint8_t lines)
    : DynamicUIElement(tft, x, (x + tft.GetTFT().textWidth(label)), y, (y + (tft.GetTFT().fontHeight() + 2 * 2) * lines)),
      m_LabelColor(labelColor), m_Label(label),
      m_ID(id)
{
    debug("Label created");
    // debug("Label padding as int: " + static_cast<int>(m_LABEL_PADDING));
    Misc::LAST_ELEMENT_Y = GetY2() + m_WHITESPACE_GAP;

    if (!IsOOB())
    {
        DebugDraw();
    }
}

Label::~Label()
{
    debug("Label component destroyed");
}

void Label::Draw()
{
    TFT_eSPI &_TFT = GetTFT().GetTFT();
    _TFT.setTextColor(m_LabelColor);
    _TFT.setCursor(GetX1() + m_WHITESPACE_GAP, GetY1() + m_WHITESPACE_GAP);
    _TFT.println(m_Label);
}

String Label::GetLabel()
{
    return m_Label;
}

void Label::Update()
{
    TFT_eSPI &_TFT = GetTFT().GetTFT();
    _TFT.fillRoundRect(GetX1(), GetY1(), TFT_Properties::WIDTH, GetY2() - GetY1(), 3, TFT_WHITE); // clear whole line by drawing BG_COLOR rect above text. white for now
    DebugDraw();
}

void Label::SetLabel(const String &label)
{
    m_Label = label;
}

void Label::SetColor(uint16_t color)
{
    GetTFT().GetTFT().setTextColor(m_LabelColor);
}

uint8_t Label::GetID()
{
    return m_ID;
}