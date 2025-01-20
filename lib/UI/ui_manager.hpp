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
    void AddLabel(const String &text, uint8_t x, uint8_t y, uint8_t id = 0, uint16_t textColor = TFT_BLACK, uint8_t lines = 1);
    void AddRect(int16_t width, int16_t x, int16_t y, uint16_t color = TFT_DARKGREEN);
    void SetLabelText(const uint8_t m_ID, const String &newText);
    void AddButton(const String &text, State state, std::function<void(State)> callbackFunc, int16_t x, int16_t y, uint16_t textColor = TFT_WHITE, uint16_t rectColor = TFT_DARKGREEN);
    void Update();
    void Clear();
    const std::vector<std::unique_ptr<Button>> &GetButtons();
    void OnButtonPressed(State &newState);

private:
    std::vector<std::unique_ptr<Label>> m_Labels;
    std::vector<std::unique_ptr<Button>> m_Buttons;
    std::vector<std::unique_ptr<RoundedRectangle>> m_Rects;
    TFT &m_TFT;
    FiniteStateMachine &m_StateManager;
};
