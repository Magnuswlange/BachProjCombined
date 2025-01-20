#include "ui_manager.hpp"
#include "button.hpp"

UIManager::UIManager(TFT &tft, std::size_t vecSize, FiniteStateMachine &stateManager)
    : m_TFT(tft), m_StateManager(stateManager)
{
    m_Labels.reserve(vecSize);
    m_Buttons.reserve(vecSize);
}

void UIManager::AddLabel(const String &text, uint8_t x, uint8_t y, uint8_t id, uint16_t textColor, uint8_t lines)
{
    debug("\nLabel is being created");
    std::unique_ptr<Label> lbl = std::make_unique<Label>(m_TFT, text, x, y, id, textColor, lines);

    if (lbl->IsOOB())
    {
        debug("ERROR: label is OOB! disregarding");
        return;
    }

    m_Labels.push_back(std::move(lbl));
    debug("Label added to vec");
}

void UIManager::SetLabelText(const uint8_t id, const String &newText)
{
    for (const auto &label : m_Labels)
    {
        if (label->GetID() == id)
        {
            label->SetLabel(newText);
            label->Update();
            return;
        }
        debug("ERROR: no label matches ID " + id);
    }
}

void UIManager::AddRect(int16_t width, int16_t x, int16_t y, uint16_t color)
{
    debug("\nRect is being created");
    std::unique_ptr<RoundedRectangle> rect = std::make_unique<RoundedRectangle>(m_TFT, 0, Misc::LAST_ELEMENT_Y, 30, TFT_DARKGREEN);

    if (rect->IsOOB())
    {
        debug("ERROR: button is OOB! disregarding");
        return;
    }

    m_Rects.push_back(std::move(rect)); // transfer ownership
    debug("Rect added to vec");
}

void UIManager::AddButton(const String &text, State state, std::function<void(State)> callbackFunc, int16_t x, int16_t y, uint16_t textColor, uint16_t rectColor)
{
    debug("\nButton is being created");
    std::unique_ptr<Button> btn = std::make_unique<Button>(m_TFT, text, state, callbackFunc, x, y, textColor, rectColor);

    if (btn->IsOOB())
    {
        debug("ERROR: button is OOB! disregarding");
        return;
    }

    m_Buttons.push_back(std::move(btn)); // transfer ownership
    debug("Button added to vec");
}

void UIManager::Clear()
{
    m_Labels.clear();
    m_Buttons.clear();
    m_Rects.clear();
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