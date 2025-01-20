#include "button.hpp"

Button::Button(TFT &tft, const String &text, State state, std::function<void(State)> callbackFunc, int16_t x, int16_t y, uint16_t textColor, uint16_t rectColor)
    : m_TFT(tft),
      m_Label(tft, text, x, y, textColor),
      m_RoundedRectangleComponent(tft, x, y, m_TFT.GetTFT().textWidth(text) + 2),
      m_State(state),
      m_OnPressCallback(callbackFunc),
      StaticUIElement(m_TFT, 0, 0, 0, 0)
{
    debug("Button created");
    Misc::LAST_ELEMENT_Y = m_RoundedRectangleComponent.GetY2() + m_BUTTON_GAP;
    // Draw();
}

Button::Button(TFT &tft, const String &text, State state, std::function<void(State)> callbackFunc, uint16_t textColor, uint16_t rectColor)
    : m_TFT(tft),
      m_Label(tft, text, 0, Misc::LAST_ELEMENT_Y, textColor),
      m_RoundedRectangleComponent(tft, 0, Misc::LAST_ELEMENT_Y, m_TFT.GetTFT().textWidth(text) + 2),
      m_State(state),
      m_OnPressCallback(callbackFunc),
      StaticUIElement(m_TFT, 0, 0, 0, 0)
{
    debug("Button created");
    Misc::LAST_ELEMENT_Y = m_RoundedRectangleComponent.GetY2() + m_BUTTON_GAP;
    // Draw();
}

Button::~Button()
{
    debug("Button destroyed");
}

void Button::Draw()
{
    m_RoundedRectangleComponent.DebugDraw();
    m_Label.DebugDraw();
}

bool Button::IsClicked(int16_t x, int16_t y)
{
    if ((x >= m_RoundedRectangleComponent.GetX1() && x <= m_RoundedRectangleComponent.GetX2()) && (y >= m_RoundedRectangleComponent.GetY1() && y <= m_RoundedRectangleComponent.GetY2()))
    {
        return true;
    }
    return false;
}

void Button::OnPress()
{
    debug("Button pressed: " + m_Label.GetLabel());

    if (m_OnPressCallback)
    {
        m_OnPressCallback(m_State); // Trigger callback with associated state
    }
}