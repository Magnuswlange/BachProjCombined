#include "label.hpp"
#include "config.hpp"

Label::Label(TFT &tft, const String &label, int16_t x, int16_t y, int16_t labelColor)
    : DynamicUIElement(tft, x, (x + tft.GetTFT().textWidth(label)), y, (y + tft.GetTFT().fontHeight() + 2 * 2)),
      m_LabelColor(labelColor), m_Label(label)
{
    debug("Label component created");
    debug("Label padding as int: " + static_cast<int>(m_LABEL_PADDING));
    DebugDraw();
}

Label::~Label()
{
    debug("Label component destroyed");
}

void Label::Draw()
{
    TFT_eSPI &_TFT = GetTFT().GetTFT();
    _TFT.setTextColor(m_LabelColor);
    _TFT.setCursor(GetX1() + m_LABEL_PADDING, GetY1() + m_LABEL_PADDING);
    _TFT.println(m_Label);
}

String Label::GetLabel()
{
    return m_Label;
}

void Label::Update()
{
    TFT_eSPI &_TFT = GetTFT().GetTFT();
    _TFT.fillRoundRect(GetX1(), GetY1(), TFT_Properties::WIDTH, GetY2() - GetY1(), 3, TFT_WHITE); // clear whole line  by drawing BG_COLOR rect above text. white for now
    DebugDraw();
}

void Label::SetLabel(const String &label)
{
    m_Label = label;
}