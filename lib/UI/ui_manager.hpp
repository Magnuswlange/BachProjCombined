#pragma once
#include <vector>
#include <TFT_eSPI.h>
#include "ui_element.hpp"
#include "tft_wrapper.hpp"
#include "label.hpp"
#include "dynamic_ui_element.hpp"
#include "static_ui_element.hpp"
#include "finite_state_machine.hpp"
#include "button.hpp"
#include "states.hpp"

class UIManager
{
public:
    UIManager(TFT &tft, std::size_t vecSize, FiniteStateMachine &stateManager);
    void AddLabel(const String &text, uint8_t x, uint8_t y, uint16_t textColor = TFT_WHITE);
    void SetLabelText(const String &oldText, const String &newText);
    void AddButton(const String &text, State state, std::function<void(State)> callbackFunc, uint16_t textColor = TFT_WHITE, uint16_t rectColor = TFT_DARKGREEN);
    void Update();
    void Clear();
    const std::vector<std::unique_ptr<Button>> &GetButtons();
    void OnButtonPressed(State &newState);

private:
    std::vector<std::unique_ptr<Label>> m_Labels;
    std::vector<std::unique_ptr<Button>> m_Buttons;
    TFT &m_TFT;
    FiniteStateMachine &m_StateManager;
};
