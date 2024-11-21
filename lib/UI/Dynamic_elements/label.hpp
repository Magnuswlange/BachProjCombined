#pragma once
#include <cstdint>
#include "tft_wrapper.hpp"
#include "dynamic_ui_element.hpp"
#include "DebugUtils.hpp"

class Label : public DynamicUIElement
{
public:
    Label(TFT &tft, const String &label, int16_t x, int16_t y, int16_t labelColor = TFT_BLACK);
    ~Label();
    void Draw() override;
    void Update() override;
    String GetLabel();
    void SetLabel(const String &label);

private:
    uint16_t m_LabelColor;
    String m_Label;
    const int16_t m_LABEL_PADDING = 2;
};