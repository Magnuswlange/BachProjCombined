#include "ui_element.hpp"
#include "TFT_wrapper.hpp"

UIElement::UIElement(TFT &tft, int16_t x1, int16_t x2, int16_t y1, int16_t y2)
    : m_TFT(tft), m_X1(x1), m_X2(x2), m_Y1(y1), m_Y2(y2)
{
    debug("UIElement created");
}

UIElement::~UIElement()
{
    debug("UIElement destroyed");
}

int16_t UIElement::GetX1() const
{
    return m_X1;
}

int16_t UIElement::GetX2() const
{
    return m_X2;
}

int16_t UIElement::GetY1() const
{
    return m_Y1;
}

int16_t UIElement::GetY2() const
{
    return m_Y2;
}

TFT &UIElement::GetTFT() const
{
    return m_TFT;
}

bool UIElement::IsOOB() const
{
    // Only check Y axis for now. Label can go beyond width and then auto wrap to next line
    if (/*(m_X1 < 0 || m_X2 > m_TFT.GetTFT().getViewportWidth()) || */ (m_Y1 < 0 || m_Y2 > m_TFT.GetTFT().getViewportHeight()))
    {
        debug("ERROR: UIElement is OOB");
        debug("X1: " + m_X1 + ", X2: " + m_X2 + ", Y1: " + m_Y1 + ", Y2: " + m_Y2);
        return true;
    }
    return false;
}

void UIElement::DebugDraw()
{
    debug("DebugDraw: X1: " + m_X1 + ", X2: " + m_X2 + ", Y1: " + m_Y1 + ", Y2: " + m_Y2);
    Draw();
}