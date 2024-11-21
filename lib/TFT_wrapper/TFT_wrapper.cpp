#include <EEPROM.h>
#include "TFT_wrapper.hpp"

TFT::TFT(int16_t width, int16_t height, int16_t textColor, int16_t bgColor, uint8_t textSize)
    : m_TFT(width, height), m_BG_COLOR(bgColor)
{
    m_TFT.setTextSize(textSize);
    m_TFT.setTextColor(textColor);
}

void TFT::Init(bool calibrate)
{
    pinMode(TFT_CS, OUTPUT);
    pinMode(TOUCH_CS, OUTPUT);
    SetTFTActive();

    m_TFT.init();
    m_TFT.setRotation(1);
    int i = 64;

    // calibrate and save to EEPROM
    if (calibrate)
    {
        m_TFT.calibrateTouch(m_CalData, TFT_WHITE, TFT_BLACK, 15);

        for (const auto &data : m_CalData)
        {
            EEPROM.put(i, data);
            debug("Writing " + data + " at " + i);
            i += sizeof(uint16_t);
        }
        EEPROM.commit();
    }

    // load calibration values from EEPROM into calData
    i = 64;
    for (auto &data : m_CalData)
    {
        EEPROM.get(i, data);
        // debug("Reading " + data + " at " + i);
        i += sizeof(uint16_t);
    }

    m_TFT.setTouch(m_CalData); // load from EEPROM
    m_TFT.fillScreen(m_BG_COLOR);
    m_TFT.setCursor(0, 0);
}

TFT_eSPI &TFT::GetTFT()
{
    return m_TFT;
}

void TFT::Clear()
{
    m_TFT.fillScreen(m_BG_COLOR);
}

Vector2 TFT::GetTouchCoords()
{
    Vector2 touchCoords{0, 0};
    SetTouchActive();

    if (m_TFT.getTouch(&touchCoords.x, &touchCoords.y))
    {
        debug("Touch detected: (" + touchCoords.x + ", " + touchCoords.y);
        SetTFTActive();
        m_TFT.fillCircle(touchCoords.x, touchCoords.y, 2, TFT_RED);
    }
    return touchCoords;
}

void TFT::SetTouchActive()
{
    digitalWrite(TFT_CS, HIGH);
    digitalWrite(TOUCH_CS, LOW);
}

void TFT::SetTFTActive()
{
    digitalWrite(TFT_CS, LOW);
    digitalWrite(TOUCH_CS, HIGH);
}