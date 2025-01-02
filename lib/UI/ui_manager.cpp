#include "ui_manager.hpp"
#include "button.hpp"

UIManager::UIManager(TFT &tft, std::size_t vecSize, FiniteStateMachine &stateManager)
    : m_TFT(tft), m_StateManager(stateManager)
{
    m_Labels.reserve(vecSize);
    m_Buttons.reserve(vecSize);
}

void UIManager::AddLabel(const String &text, uint8_t x, uint8_t y, uint16_t textColor)
{
    std::unique_ptr<Label> lbl = std::make_unique<Label>(m_TFT, text, x, y, textColor);
    if (lbl->IsOOB())
    {
        return;
    }

    m_Labels.push_back(std::move(lbl));
}

void UIManager::SetLabelText(const String &oldText, const String &newText)
{
    for (const auto &label : m_Labels)
    {
        if (label->GetLabel() == oldText)
        {
            label->SetLabel(newText);
            return;
        }
    }
}

void UIManager::AddButton(const String &text, State state, std::function<void(State)> callbackFunc, uint16_t textColor, uint16_t rectColor)
{
    debug("Button is being created");
    std::unique_ptr<Button> btn = std::make_unique<Button>(m_TFT, text, state, callbackFunc, textColor, rectColor);
    if (btn->IsOOB())
    {
        return;
    }

    m_Buttons.push_back(std::move(btn)); // transfer ownership
    debug("Button added to vec");
}

void UIManager::Clear()
{
    m_Labels.clear();
    m_Buttons.clear();
}

void UIManager::Update()
{
    for (const auto &button : m_Buttons)
    {
        button->DebugDraw();
    }

    for (const auto &label : m_Labels)
    {
        label->Update();
    }
}

const std::vector<std::unique_ptr<Button>> &UIManager::GetButtons()
{
    return m_Buttons;
}

void UIManager::OnButtonPressed(State &newState)
{
    debug("State change request: " + (int)newState);
    m_StateManager.SetState(newState);
}