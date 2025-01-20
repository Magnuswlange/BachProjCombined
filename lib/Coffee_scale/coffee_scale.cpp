#include <EEPROM.h>
#include "coffee_scale.hpp"

CoffeeScale::CoffeeScale(FiniteStateMachine &stateManager, uint8_t dOutPin, uint8_t sckPin)
    : HX711_ADC(dOutPin, sckPin),
      m_StateManager(stateManager)
{
}

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

void CoffeeScale::Init(unsigned long stabilizingTime, bool tareOnStart)
{
    begin(); // init pins and set gain
    start(stabilizingTime, tareOnStart);

    if (getTareTimeoutFlag())
    {
        debug("Error: taring operation timed out");
        // return;
    }

    if (getSignalTimeoutFlag())
    {
        debug("Error: signal timed out");
        // return;
    }

    LoadData(); // load Data struct from EEPROM
    debug("Calibration value: " + m_Data.calibrationValue);
    setCalFactor(m_Data.calibrationValue);

    esp_timer_create_args_t oneShotTimerArgs{.callback = OnOneShotExpireISR, // func to cb on timer end
                                             .arg = nullptr,
                                             .dispatch_method = ESP_TIMER_TASK,
                                             .name = "oneShotTimer"};
    esp_timer_create_args_t periodTimerArgs{.callback = OnPeriodISR, // func to cb on timer end
                                            .arg = nullptr,
                                            .dispatch_method = ESP_TIMER_TASK,
                                            .name = "periodTimer"};
    esp_timer_create(&oneShotTimerArgs, &m_OneShotTimer);

    esp_timer_create(&periodTimerArgs, &m_PeriodTimer);
}

const CoffeeScale::Data &CoffeeScale::GetDataStruct() const
{
    return m_Data;
}

void CoffeeScale::LoadData()
{
    EEPROM.get(m_EepromStartAddress, m_Data);
}

void CoffeeScale::SetData(const CoffeeScale::Data &data)
{
    m_Data = data;
}

void CoffeeScale::AddCurrentBrewToData()
{
    m_Data.totalVolume += m_Mass / 1000; // g to kg, aka volume L
    m_Data.totalBrews++;
}

void CoffeeScale::ResetData()
{
    m_Data.Reset();
}

void CoffeeScale::SaveData()
{
    EEPROM.put(m_EepromStartAddress, m_Data);
    EEPROM.commit();
}

void CoffeeScale::UpdateMass()
{
    m_Mass = getData();
}

float CoffeeScale::GetMass()
{
    return m_Mass;
}

void CoffeeScale::StartOneShotTimer(uint64_t timeMs)
{
    if (m_OneShotTimer != nullptr)
    {
        if (esp_timer_is_active(m_OneShotTimer))
        {
            esp_timer_stop(m_OneShotTimer);
        }
    }
    esp_timer_start_once(m_OneShotTimer, timeMs * 1000);
}

void CoffeeScale::StartPeriodicTimer(uint64_t periodMs)
{
    if (m_PeriodTimer != nullptr)
    {
        if (esp_timer_is_active(m_PeriodTimer))
        {
            esp_timer_stop(m_PeriodTimer);
        }
    }
    esp_timer_start_periodic(m_PeriodTimer, periodMs * 1000);
}

void CoffeeScale::StopPeriodicTimer()
{
    if (m_PeriodTimer != nullptr)
    {
        if (esp_timer_is_active(m_PeriodTimer))
        {
            esp_timer_stop(m_PeriodTimer);
        }
    }
}

bool CoffeeScale::IsPeriodTimerRunning()
{
    if (m_PeriodTimer != nullptr)
    {
        return esp_timer_is_active(m_PeriodTimer);
    }
    return false;
}