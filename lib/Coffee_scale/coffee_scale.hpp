#pragma once
#include <HX711_ADC.h>
#include <memory>
#include "DebugUtils.hpp"
#include "states.hpp"
#include "finite_state_machine.hpp"

// Declare the function pointer type for shutdown callback
extern "C" void IRAM_ATTR onTimerExpireISR(void *arg);

class CoffeeScale : public HX711_ADC
{
public:
    CoffeeScale(FiniteStateMachine &stateManager, uint8_t dOutPin, uint8_t sckPin);
    ~CoffeeScale();

    struct Data
    {
        uint64_t totalVolume = 0;
        uint64_t totalBrews = 0;
        float calibrationValue = 0;

        void Reset()
        {
            *this = {};
        }
    };

    void Init(unsigned long stabilizingTime = 2000, bool tareOnStart = true);
    const CoffeeScale::Data &GetDataStruct() const;
    void LoadData();
    void SetData(const CoffeeScale::Data &data);
    void AddCurrentBrewToData();
    void ResetData();
    void SaveData();
    void UpdateMass();
    float GetMass();
    void StartOneShotTimer(uint64_t timeMs);
    void StartPeriodicTimer(uint64_t periodMs);

private:
    FiniteStateMachine &m_StateManager;
    float m_Mass;
    int64_t m_LastInteractionTime;
    Data m_Data;
    const int m_EepromStartAddress = 0;
    esp_timer *m_OneShotTimer;
    esp_timer *m_PeriodTimer;
};