#pragma once
#include <Arduino.h>
#include "config.hpp"

namespace BuzzerUtil
{
    void Play(int times)
    {
        for (int i = 0; i < times; i++)
        {
            analogWrite(Misc::BUZZER_PIN, 255 / 4);
            delay(50);
            analogWrite(Misc::BUZZER_PIN, 0);
            delay(50);
        }
    }
}