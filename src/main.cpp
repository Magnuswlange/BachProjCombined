#include <Arduino.h>
#include <vector>
#include <EEPROM.h>
#include <memory>
#include <HX711_ADC.h>
#include "config.hpp"
#include "DebugUtils.hpp"
#include "TFT_wrapper.hpp"
#include "coffee_scale.hpp"
#include "button.hpp"
#include "label.hpp"
#include "ui_manager.hpp"
#include "finite_state_machine.hpp"
#include "BuzzerUtil.hpp"

void OnStateEnter(State newState);
void OnStateExit(State oldState);
void OnButtonPress(State newState);
void OnDataReadyISR();
bool HandleButtonPress();
void HandleScaleUpdate();
void Shutdown();

volatile bool g_NewDataReady = false;
volatile bool g_TimeoutTimerExpired = false;
volatile int g_TimerDurationS = 0;
uint8_t g_AutoStage = 0;
RTC_DATA_ATTR static uint8_t g_BootCount = 0;

FiniteStateMachine g_StateMachine(OnStateEnter, OnStateExit);
TFT g_TFT(TFT_Properties::LENGTH, TFT_Properties::WIDTH, TFT_Properties::TEXT_COLOR, TFT_Properties::BG_COLOR, TFT_Properties::TEXT_SIZE);
UIManager g_UIManager(g_TFT, 10, g_StateMachine);
CoffeeScale g_Scale(g_StateMachine, HX711::DOUT_PIN, HX711::SCK_PIN);

// mark by AM attribute. extern C to avoid cpp name mangling so compiler/linker can find the function across translation units
extern "C" void IRAM_ATTR OnOneShotExpireISR(void *arg)
{
  g_TimeoutTimerExpired = true;
}

extern "C" void IRAM_ATTR OnPeriodISR(void *arg)
{
  g_TimerDurationS++;
}

/*
TODO:
set period timer callback function to the ISR in main using extern "C" to keep track of current brewing time.
remember to profile application
?Expand button hit box to make it more forgiving/less annoying (already covered by padding?)
?Run buttons by interrupts vs polling vs callback
?divide Data struct to statistics and settings
*/

void setup()
{
  Serial.begin(115200);
  debug("\nSetup started");
  pinMode(TFT_Properties::BACKLIGHT_PIN, OUTPUT);
  pinMode(TFT_Properties::IRQ_PIN, INPUT_PULLUP);
  pinMode(Misc::BUZZER_PIN, OUTPUT);
  digitalWrite(TFT_Properties::BACKLIGHT_PIN, HIGH);
  esp_sleep_enable_ext0_wakeup(TFT_Properties::IRQ_PIN, LOW);

  EEPROM.begin(512); // CoffeeScale::Data + other EEPROM vars
  g_TFT.Init();
  g_Scale.Init(); // init pins before attaching interrupt to avoid time out
  attachInterrupt(digitalPinToInterrupt(HX711::DOUT_PIN), OnDataReadyISR, FALLING);

  // write test data
  // constexpr CoffeeScale::Data testData{5, 10, -995.98};
  // g_Scale.SetData(testData);
  // g_Scale.SaveData();

  g_StateMachine.SetState(State::MAIN_MENU);
  g_Scale.StartOneShotTimer(Misc::TIMEOUT_THRESHOLD_MS);
  debug("Boot count: " + g_BootCount++);
  debug("Setup done\n");
}

void loop()
{
  while (!g_TimeoutTimerExpired)
  {
    if (HandleButtonPress())
    {
      delay(100);
      continue;
    }

    HandleScaleUpdate();

    switch (g_StateMachine.GetState())
    {
    case State::IDLE:
    {
      break;
    }
    case State::NORMAL:
    {
      static float lastMass = 0;
      debug("Mass (g): " + (String)g_Scale.GetMass());

      if (g_Scale.GetMass() < lastMass - 0.1 || g_Scale.GetMass() > lastMass + 0.1)
      {
        debug("Significant mass change detected, updating display");
        g_Scale.StartOneShotTimer(Misc::TIMEOUT_THRESHOLD_MS);
        lastMass = g_Scale.GetMass();
        g_UIManager.SetLabelText(10, "Mass: " + (String)g_Scale.GetMass() + " g");
      }

      break;
    }
    case State::AUTO:
    {
      static float lastMass = 0;
      static float lastFlowRate = 0;
      static int lastDurationS = 0;

      float currMass = g_Scale.GetMass();
      debug("Mass (g): " + (String)currMass);

      float flowRate = currMass / g_TimerDurationS;

      if (flowRate != lastFlowRate)
      {
        lastFlowRate = flowRate;
        g_UIManager.SetLabelText(13, "Rate: " + (String)flowRate + " g/s");
      }

      if (currMass < lastMass - 0.1 || currMass > lastMass + 0.1)
      {
        debug("Significant mass change detected, updating display");
        g_Scale.StartOneShotTimer(Misc::TIMEOUT_THRESHOLD_MS);
        lastMass = currMass;
        g_UIManager.SetLabelText(10, "Mass: " + (String)currMass + " g");

        if (!g_Scale.IsPeriodTimerRunning())
        {
          g_Scale.StartPeriodicTimer(1000);
        }
      }

      if (g_TimerDurationS != lastDurationS)
      {
        g_UIManager.SetLabelText(11, "Time: " + String((int)floor(g_TimerDurationS / 60)) + "m" + String(g_TimerDurationS % 60) + ("s")); // s to m and clamp seconds to [0;59]
        lastDurationS = g_TimerDurationS;
      }

      if (g_TimerDurationS > 0 && g_TimerDurationS < 15 && g_AutoStage == 0)
      {
        g_AutoStage++;
        g_UIManager.SetLabelText(12, "0-15s, pour \nuntil 60g");
        BuzzerUtil::Play(2);
      }
      else if (g_TimerDurationS >= 15 && g_TimerDurationS < 45 && g_AutoStage == 1)
      {
        g_AutoStage++;
        g_UIManager.SetLabelText(12, "15-45s, blooming \nwait");
        BuzzerUtil::Play(2);
      }
      else if (g_TimerDurationS >= 45 && g_TimerDurationS < 75 && g_AutoStage == 2)
      {
        g_AutoStage++;
        g_UIManager.SetLabelText(12, "45s-1m15s, pour \nuntil 300g");
        BuzzerUtil::Play(2);
      }
      else if (g_TimerDurationS >= 75 && g_TimerDurationS < 105 && g_AutoStage == 3)
      {
        g_AutoStage++;
        g_UIManager.SetLabelText(12, "1m15s-1m45s, pour \nuntil 500g");
        BuzzerUtil::Play(2);
      }
      else if (g_TimerDurationS >= 105 && g_TimerDurationS < 210 && g_AutoStage == 4)
      {
        g_AutoStage++;
        g_UIManager.SetLabelText(12, "3m30s, aim for \ndrawdown");
        BuzzerUtil::Play(2);
      }
      else if (g_TimerDurationS >= 210)
      {
        g_Scale.AddCurrentBrewToData();
        g_Scale.SaveData();
        Shutdown();
      }

      break;
    }
    default:
    {
      break;
    }
    }
  }

  Shutdown();
}

inline void OnDataReadyISR()
{
  if (g_Scale.update())
  {
    g_NewDataReady = true;
  }
}

bool HandleButtonPress()
{
  Vector2 touchCoords = g_TFT.GetTouchCoords();

  if (touchCoords.x == 0 && touchCoords.y == 0)
  {
    return false;
  }

  BuzzerUtil::Play(1);
  g_Scale.StartOneShotTimer(Misc::TIMEOUT_THRESHOLD_MS);
  const std::vector<std::unique_ptr<Button>> &buttons = g_UIManager.GetButtons();

  for (const auto &button : buttons)
  {
    if (button == nullptr || !button->IsClicked(touchCoords.x, touchCoords.y))
    {
      continue;
    }

    button->OnPress();
    break; // can only press one button at a time
  }

  return true;
}

void HandleScaleUpdate()
{
  if (!g_NewDataReady)
  {
    return;
  }

  g_Scale.UpdateMass();
  // SetLabel where String starts with mass and set it to the new g_Scale.GetMass()
  g_NewDataReady = false;
}

void Shutdown()
{
  debug("\n\nShutting down");
  BuzzerUtil::Play(3);
  esp_deep_sleep_start();
}

void OnButtonPress(State newState)
{
  debug("\n\nOnButtonPress called!");
  g_StateMachine.SetState(newState);
}

void OnStateExit(State oldState)
{
  debug("\n\nOnStateExit called!");
  g_TFT.Clear();
  g_UIManager.Clear();
  Misc::LAST_ELEMENT_Y = 0;
  debug("\nLast Y pos set to: " + Misc::LAST_ELEMENT_Y);
}

void OnStateEnter(State newState)
{
  State pendingState = newState;

  debug("\n\nOnStateEnter called!");

  if (&g_UIManager == nullptr)
  {
    debug("UIManager is nullptr");
    return;
  }

  switch (newState)
  {
  case State::MAIN_MENU:
  {
    g_UIManager.AddButton("Manual mode", State::NORMAL, OnButtonPress, 0, Misc::LAST_ELEMENT_Y);
    g_UIManager.AddButton("Auto mode", State::AUTO, OnButtonPress, 0, Misc::LAST_ELEMENT_Y);
    g_UIManager.AddButton("Statistics", State::STATS, OnButtonPress, 0, Misc::LAST_ELEMENT_Y);
    break;
  }
  case State::NORMAL:
  {
    debug("Taring scale...");
    g_Scale.tare();
    g_UIManager.AddLabel("Mass: 0.0 g", 0, Misc::LAST_ELEMENT_Y, 10); // Can't use SetLabel as the mass would change (string comp)
    g_UIManager.AddButton("Reset", State::TARE, OnButtonPress, 0, 240 - (g_TFT.GetTFT().fontHeight() + 4));
    g_UIManager.AddButton("Back", State::MAIN_MENU, OnButtonPress, 320 - (g_TFT.GetTFT().textWidth("Back") + 4), 240 - (g_TFT.GetTFT().fontHeight() + 4), TFT_WHITE, TFT_RED);
    break;
  }
  case State::AUTO:
  {
    debug("Taring scale...");
    g_Scale.tare();
    debug("Stopping period timer");
    g_Scale.StopPeriodicTimer();
    debug("Stopped period timer successfully");
    g_TimerDurationS = 0;

    g_UIManager.AddLabel("Mass: 0.0 g", 0, Misc::LAST_ELEMENT_Y, 10);                     // Can't use SetLabel as the mass would change (string comp)
    g_UIManager.AddLabel("Time: 0m0s", 0, Misc::LAST_ELEMENT_Y, 11);                      // Can't use SetLabel as the mass would change (string comp)
    g_UIManager.AddLabel("Rate: 0 g/s", 0, Misc::LAST_ELEMENT_Y, 13, TFT_DARKGREEN);      // Can't use SetLabel as the mass would change (string comp)
    g_UIManager.AddLabel("Pour to begin", 0, Misc::LAST_ELEMENT_Y + 30, 12, TFT_BLUE, 2); // Can't use SetLabel as the mass would change (string comp)
    // g_UIManager.AddButton("Reset", State::TARE, OnButtonPress, 0, 240 - (g_TFT.GetTFT().fontHeight() + 4));
    // g_UIManager.AddButton("Back", State::MAIN_MENU, OnButtonPress, 320 - (g_TFT.GetTFT().textWidth("Back") + 4), 240 - (g_TFT.GetTFT().fontHeight() + 4), TFT_WHITE, TFT_RED);
    break;
  }
  case State::STATS:
  {
    CoffeeScale::Data scaleData = g_Scale.GetDataStruct();
    debug("Total volume: " + scaleData.totalVolume);
    debug("Total brews: " + scaleData.totalBrews);
    debug("Boot count: " + g_BootCount);
    debug("Calibration value: " + scaleData.calibrationValue);

    g_UIManager.AddLabel("Volume (L): " + (String)scaleData.totalVolume, 0, Misc::LAST_ELEMENT_Y, TFT_BLACK);
    g_UIManager.AddLabel("Brews: " + (String)scaleData.totalBrews, 0, Misc::LAST_ELEMENT_Y, TFT_BLACK);
    g_UIManager.AddLabel("Boot count: " + (String)g_BootCount, 0, Misc::LAST_ELEMENT_Y, TFT_BLACK);
    g_UIManager.AddLabel("Calibration value: " + (String)scaleData.calibrationValue, 0, Misc::LAST_ELEMENT_Y, TFT_BLACK);

    g_UIManager.AddButton("Reset", State::RESET_STATS, OnButtonPress, 0, 240 - (g_TFT.GetTFT().fontHeight() + 4));
    g_UIManager.AddButton("Back", State::MAIN_MENU, OnButtonPress, 320 - (g_TFT.GetTFT().textWidth("Back") + 4), 240 - (g_TFT.GetTFT().fontHeight() + 4), TFT_WHITE, TFT_RED);
    break;
  }
  case State::RESET_STATS:
  {
    debug("Resetting stats");
    g_Scale.ResetData();

    g_UIManager.AddLabel("Reset stats successfully", 0, (g_TFT.GetTFT().fontHeight() + 4) * 0, TFT_BLACK);
    g_UIManager.AddButton("Back", State::STATS, OnButtonPress, 320 - (g_TFT.GetTFT().textWidth("Back") + 4), 240 - (g_TFT.GetTFT().fontHeight() + 4), TFT_WHITE, TFT_RED);
    break;
  }
  case State::TARE:
  {
    State oldState = g_StateMachine.GetState();
    // debug("Taring...");
    // g_Scale.tare();
    g_UIManager.AddLabel("Reset successfully", 0, (g_TFT.GetTFT().fontHeight() + 4) * 0, TFT_BLACK);
    g_UIManager.AddButton("Back", oldState, OnButtonPress, 320 - (g_TFT.GetTFT().textWidth("Back") + 4), 240 - (g_TFT.GetTFT().fontHeight() + 4), TFT_WHITE, TFT_RED);
    break;
  }
  default:
  {
    debug("ERROR: unhandled state!");
    break;
  }
  }
}