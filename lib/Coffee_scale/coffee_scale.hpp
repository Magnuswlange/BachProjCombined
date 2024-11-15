#pragma once
#include <HX711_ADC.h>
#include "DebugUtils.hpp"

class CoffeeScale : public HX711_ADC
{
public:
    CoffeeScale(uint8_t dOutPin, uint8_t sckPin);
    ~CoffeeScale();

    enum class Mode : uint8_t
    {
        MENU = 0,
        NORMAL,
        AUTO,
        STATS
    };

    struct Data
    {
        uint64_t totalVolume = 0;
        uint64_t totalBrews = 0;
        float calibrationValue = 0;

        void reset()
        {
            *this = {};
        }
    };

    void init(unsigned long stabilizingTime = 2000, bool tareOnStart = true);
    Mode getMode() const;
    void setMode(Mode mode);
    void display();
    const CoffeeScale::Data &getDataStruct() const;
    void loadData();
    void setData(const CoffeeScale::Data &data);
    void addCurrentBrewToData();
    void resetData();
    void saveData();
    void updateMass();
    void startOneShotTimer(uint64_t timeMs);
    void startPeriodicTimer(uint64_t periodMs);

private:
    Mode m_Mode;
    float m_Mass;
    int64_t m_LastInteractionTime;
    Data m_Data;
    const int m_EepromStartAddress = 0;
    esp_timer *m_OneShotTimer;
    esp_timer *m_PeriodTimer;
};