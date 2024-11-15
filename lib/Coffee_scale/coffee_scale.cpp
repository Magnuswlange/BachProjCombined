#include <EEPROM.h>
#include "coffee_scale.hpp"

CoffeeScale::CoffeeScale(uint8_t dOutPin, uint8_t sckPin)
    : HX711_ADC(dOutPin, sckPin) {}

CoffeeScale::~CoffeeScale()
{
    if (m_OneShotTimer == nullptr)
    {
        if (esp_timer_is_active(m_OneShotTimer))
        {
            esp_timer_stop(m_OneShotTimer);
        }
        esp_timer_delete(m_OneShotTimer);
    }

    if (m_PeriodTimer != nullptr)
    {
        if (esp_timer_is_active(m_PeriodTimer))
        {
            esp_timer_stop(m_PeriodTimer);
        }
        esp_timer_delete(m_PeriodTimer);
    }
}

void CoffeeScale::init(unsigned long stabilizingTime, bool tareOnStart)
{
    begin();
    start(stabilizingTime, tareOnStart);

    if (getTareTimeoutFlag())
    {
        debug("Error: scale timed out");
        return;
    }

    // EEPROM.begin(sizeof(CoffeeScale::Data));
    loadData();
    setCalFactor(m_Data.calibrationValue);
    esp_timer_create_args_t oneShotTimerArgs{.callback = nullptr, // func to cb on timer end
                                             .arg = nullptr,
                                             .dispatch_method = ESP_TIMER_TASK,
                                             .name = "oneShotTimer"};
    esp_timer_create(&oneShotTimerArgs, &m_OneShotTimer);
    esp_timer_create(nullptr, &m_PeriodTimer);
}

void test(void *arg);

void CoffeeScale::setMode(Mode mode)
{
    m_Mode = mode;
}

CoffeeScale::Mode CoffeeScale::getMode() const
{
    return m_Mode;
}

void CoffeeScale::display()
{
    switch (m_Mode)
    {
    case Mode::MENU:
    {
        debug("Welcome to the main menu");
        break;
    }
    case Mode::NORMAL:
    {
        debug("Mass (g): " + m_Mass);
        break;
    }
    case Mode::AUTO:
    {
        debug("Mass (g): " + m_Mass);
        // const int elapsedTimeSeconds = getTimeSinceInteraction() / 1000000;
        // debug("Time (m:s): " + elapsedTimeSeconds / 60 + ":" + elapsedTimeSeconds % 60); // s to m and clamp seconds to [0;59]
        break;
    }
    case Mode::STATS:
    {
        debug("Total volume: " + m_Data.totalVolume);
        debug("Total brews: " + m_Data.totalBrews);
        debug("Calibration value: " + m_Data.calibrationValue);
        break;
    }
    };
}

const CoffeeScale::Data &CoffeeScale::getDataStruct() const
{
    return m_Data;
}

void CoffeeScale::loadData()
{
    EEPROM.get(m_EepromStartAddress, m_Data);
}

void CoffeeScale::setData(const CoffeeScale::Data &data)
{
    m_Data = data;
}

void CoffeeScale::addCurrentBrewToData()
{
    m_Data.totalVolume += m_Mass / 1000; // g to kg, aka volume L
    m_Data.totalBrews++;
}

void CoffeeScale::resetData()
{
    m_Data.reset();
}

void CoffeeScale::saveData()
{
    EEPROM.put(m_EepromStartAddress, m_Data);
    EEPROM.commit();
}

void CoffeeScale::updateMass()
{
    m_Mass = getData();
}

void CoffeeScale::startOneShotTimer(uint64_t timeMs)
{
    if (esp_timer_is_active(m_OneShotTimer))
    {
        esp_timer_stop(m_OneShotTimer);
    }
    esp_timer_start_once(m_OneShotTimer, timeMs * 1000);
}

void CoffeeScale::startPeriodicTimer(uint64_t periodMs)
{
    if (esp_timer_is_active(m_PeriodTimer))
    {
        esp_timer_stop(m_PeriodTimer);
    }
    esp_timer_start_periodic(m_PeriodTimer, periodMs * 1000);
}