#pragma once
#include <cstdint>
#include <TFT_eSPI.h>
#include "DebugUtils.hpp"

class TFT; // forward declaration

class UIElement
{
public:
    virtual ~UIElement();
    virtual void Draw() = 0;

    int16_t GetX1() const;
    int16_t GetX2() const;
    int16_t GetY1() const;
    int16_t GetY2() const;
    TFT &GetTFT() const;
    bool IsOOB() const;

protected:
    UIElement(TFT &tft, int16_t x1, int16_t x2, int16_t y1, int16_t y2);

private:
    TFT &m_TFT;
    int16_t m_X1;
    int16_t m_X2;
    int16_t m_Y1;
    int16_t m_Y2;
};