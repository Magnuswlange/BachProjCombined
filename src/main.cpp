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
void onDataReadyISR();
void handleButtonPress();
void handleScaleUpdate();
void shutdown();

volatile bool g_NewDataReady = false;
volatile bool g_TimeoutTimerExpired = false;
bool g_UIUpdateOnlyOnce = false;
RTC_DATA_ATTR static uint8_t g_BootCount = 0;

FiniteStateMachine g_StateMachine(OnStateEnter, OnStateExit);
TFT g_TFT(TFT_Properties::LENGTH, TFT_Properties::WIDTH, TFT_Properties::TEXT_COLOR, TFT_Properties::BG_COLOR, TFT_Properties::TEXT_SIZE);
UIManager g_UIManager(g_TFT, 10, g_StateMachine);
CoffeeScale g_Scale(g_StateMachine, HX711::DOUT_PIN, HX711::SCK_PIN);

// mark by instruction RAM attribute. extern C to avoid cpp name mangling so compiler/linker can find the function across translation units
extern "C" void IRAM_ATTR onTimerExpireISR(void *arg)
{
  g_TimeoutTimerExpired = true;
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
  attachInterrupt(digitalPinToInterrupt(HX711::DOUT_PIN), onDataReadyISR, FALLING);

  // write test data
  // constexpr CoffeeScale::Data testData{1, 2, 466.97};
  // g_Scale.setData(testData);
  // g_Scale.saveData();

  g_StateMachine.SetState(State::STATS);
  g_Scale.StartOneShotTimer(Misc::TIMEOUT_THRESHOLD_MS);
  debug("Boot count: " + g_BootCount++);
  debug("Setup done\n");
}

void loop()
{
  while (!g_TimeoutTimerExpired)
  {
    if (!g_UIUpdateOnlyOnce)
    {
      g_UIManager.Update();
    }

    handleButtonPress();
    handleScaleUpdate();
  }

  shutdown();
}

inline void onDataReadyISR()
{
  if (g_Scale.update())
  {
    g_NewDataReady = true;
  }
}

void handleButtonPress()
{
  Vector2 touchCoords = g_TFT.GetTouchCoords();

  if (touchCoords.x == 0 && touchCoords.y == 0)
  {
    return;
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
    break; // can only press 1 button/point at a time, no need to check the rest
  }
}

void handleScaleUpdate()
{
  if (!g_NewDataReady)
  {
    return;
  }

  g_Scale.UpdateMass();
  // SetLabel where String starts with mass and set it to the new g_Scale.GetMass()
  g_NewDataReady = false;
}

void shutdown()
{
  debug("Shutting down");
  g_Scale.SaveData();
  esp_deep_sleep_start();
}

void OnButtonPress(State newState)
{
  debug("OnButtonPress called!");
  g_StateMachine.SetState(newState);
}

void OnStateExit(State oldState)
{
  debug("OnStateExit called!");

  g_TFT.Clear();
  g_UIManager.Clear();
  g_UIUpdateOnlyOnce = false;
}

void OnStateEnter(State newState)
{
  debug("OnStateEnter called!");

  if (&g_UIManager == nullptr)
  {
    debug("UIManager is nullptr");
    return;
  }

  switch (newState)
  {
  case State::MAIN_MENU:
  {
    debug("Welcome to the main menu");

    // g_UIManager.AddButton("Manual mode", State::NORMAL, OnButtonPress);
    // g_UIManager.AddButton("Auto mode", State::AUTO, OnButtonPress);
    // g_UIManager.AddButton("Statistics", State::STATS, OnButtonPress);
    // debug("button created successfully");
    g_UIManager.Update();
    g_UIUpdateOnlyOnce = true;
    break;
  }
  case State::NORMAL:
  {
    g_UIManager.AddLabel("Mass: X.XX g", 0, 0, TFT_BLACK); // Can't use SetLabel as the mass would change (string comp)
    g_UIUpdateOnlyOnce = false;
    break;
  }
  case State::AUTO:
  {
    debug("Mass (g): " + g_Scale.GetMass());

    g_UIUpdateOnlyOnce = false;
    // const int elapsedTimeSeconds = getTimeSinceInteraction() / 1000000;
    // debug("Time (m:s): " + elapsedTimeSeconds / 60 + ":" + elapsedTimeSeconds % 60); // s to m and clamp seconds to [0;59]
    break;
  }
  case State::STATS:
  {
    CoffeeScale::Data scaleData = g_Scale.GetDataStruct();

    debug("Total volume: " + scaleData.totalVolume);
    debug("Total brews: " + scaleData.totalBrews);
    debug("Boot count: " + g_BootCount);
    debug("Calibration value: " + scaleData.calibrationValue);

    g_UIManager.AddLabel("Volume: " + (String)scaleData.totalVolume, 0, (g_TFT.GetTFT().fontHeight() + 4) * 0, TFT_BLACK);
    g_UIManager.AddLabel("Brews: " + (String)scaleData.totalBrews, 0, (g_TFT.GetTFT().fontHeight() + 4) * 1, TFT_BLACK);
    g_UIManager.AddLabel("Boot count: " + (String)g_BootCount, 0, (g_TFT.GetTFT().fontHeight() + 4) * 2, TFT_BLACK);
    g_UIManager.AddLabel("Calibration value: " + (String)scaleData.calibrationValue, 0, (g_TFT.GetTFT().fontHeight() + 4) * 3, TFT_BLACK);
    g_UIUpdateOnlyOnce = true;
    break;
  }
  }
}