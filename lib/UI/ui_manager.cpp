#include "ui_manager.hpp"
#include "button.hpp"

UIManager::UIManager(TFT &tft, std::size_t vecSize)
    : m_TFT(tft)
{
    m_StaticUIElements.reserve(vecSize);
    m_DynamicUIElements.reserve(vecSize);
}

// void UIManager::AddLabel(const String &text, uint8_t x, uint8_t y, uint16_t textColor)
// {
//     m_DynamicUIElements.emplace_back(std::make_unique<Label>(m_TFT, text, x, y, textColor));
// }

void UIManager::AddButton(const String &text, uint16_t textColor, uint16_t rectColor)
{
    m_StaticUIElements.emplace_back(std::make_unique<Button>(m_TFT, text, textColor, rectColor));
}

void UIManager::AddLabel(std::shared_ptr<Label> label)
{
    m_DynamicUIElements.push_back(label);
}

void UIManager::Clear()
{
    m_StaticUIElements.clear();
    m_DynamicUIElements.clear();
}

void UIManager::Update()
{
    for (const auto &dynElem : m_DynamicUIElements)
    {
        dynElem->Update();
    }
}

const std::vector<std::unique_ptr<StaticUIElement>> &UIManager::GetStaticElements()
{
    return m_StaticUIElements;
}

const std::vector<std::shared_ptr<DynamicUIElement>> &UIManager::GetDynamicElements()
{
    return m_DynamicUIElements;
}
