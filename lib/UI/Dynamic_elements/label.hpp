#pragma once
#include <cstdint>
#include "tft_wrapper.hpp"
#include "dynamic_ui_element.hpp"
#include "DebugUtils.hpp"

class Label : public DynamicUIElement
{
public:
    Label(TFT &tft, const String &label, int16_t x, int16_t y, uint8_t id = 0, int16_t labelColor = TFT_WHITE, uint8_t lines = 1);
    ~Label();
    void Draw() override;
    void Update() override;
    String GetLabel();
    uint8_t GetID();
    void SetLabel(const String &label);
    void SetColor(uint16_t color);

private:
    uint16_t m_LabelColor;
    String m_Label;
    const int8_t m_WHITESPACE_GAP = 4;
    uint8_t m_ID;
};