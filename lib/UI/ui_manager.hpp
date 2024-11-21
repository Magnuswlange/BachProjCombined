#pragma once
#include <vector>
#include <TFT_eSPI.h>
#include "ui_element.hpp"
#include "tft_wrapper.hpp"
#include "label.hpp"
#include "dynamic_ui_element.hpp"
#include "static_ui_element.hpp"

class UIManager
{
public:
    UIManager(TFT &tft, std::size_t vecSize);
    void AddLabel(std::shared_ptr<Label> label);
    void AddLabel(const String &text, uint8_t x, uint8_t y, uint16_t textColor = TFT_WHITE);
    void AddButton(const String &text, uint16_t textColor = TFT_WHITE, uint16_t rectColor = TFT_DARKGREEN);
    void Update();
    void Clear();
    const std::vector<std::unique_ptr<StaticUIElement>> &GetStaticElements();
    const std::vector<std::shared_ptr<DynamicUIElement>> &GetDynamicElements();

private:
    std::vector<std::unique_ptr<StaticUIElement>> m_StaticUIElements; // allow for polymorphic behavior, aka handling derived classes via base class pointers as compared to stored on the stack (raw UIElement objs)
    std::vector<std::shared_ptr<DynamicUIElement>> m_DynamicUIElements;
    TFT &m_TFT;
};
