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

volatile bool g_NewDataReady = false;
volatile bool g_TimeoutTimerExpired = false;

CoffeeScale g_Scale(HX711::DOUT_PIN, HX711::SCK_PIN);
TFT g_TFT(TFT_Properties::LENGTH, TFT_Properties::WIDTH, TFT_Properties::TEXT_COLOR, TFT_Properties::BG_COLOR, TFT_Properties::TEXT_SIZE);
UIManager g_UIManager(g_TFT, 10);

constexpr unsigned long g_TimeoutThresholdMs = 60 * 1000;

// mark by instruction RAM attribute
extern "C" void IRAM_ATTR onTimerExpireISR(void *arg)
{
  g_TimeoutTimerExpired = true;
}
void onDataReadyISR();
void handleButtonPress();
void handleScaleUpdate();
void shutdown();

/*
TODO:
set period timer callback function to the ISR in main using extern "C" to keep track of current brewing time.
remember to profile application
?Expand button hit box to make it more forgiving/less annoying (already covered by padding?)
?Run buttons by interrupts vs polling
line: tft.drawWideLine(0, 0, 320 / 2, 240 / 2, 2, TFT_DARKGREY, TFT_BROWN);
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

  RTC_DATA_ATTR static uint8_t bootCount = 0;
  debug("Boot count: " + bootCount++);
  debug("Setup done\n");
}

void loop()
{
  std::vector<std::unique_ptr<Button>> buttons;
  g_Scale.StartOneShotTimer(g_TimeoutThresholdMs);

  g_UIManager.AddButton("Manual mode");
  g_UIManager.AddButton("Auto mode");
  g_UIManager.AddButton("Statistics");

  while (!g_TimeoutTimerExpired)
  {
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
  // handle button press
  Vector2 touchCoords = g_TFT.GetTouchCoords();

  if (touchCoords.x == 0 && touchCoords.y == 0)
  {
    return;
  }

  g_Scale.StartOneShotTimer(g_TimeoutThresholdMs);
  const std::vector<std::unique_ptr<StaticUIElement>> &_vec = g_UIManager.GetStaticElements();

  for (auto it = _vec.begin(); it != _vec.end(); it++)
  {
    Button *button = dynamic_cast<Button *>(it->get()); // cast UIElement to button obj

    // if not a Button obj
    if (button == nullptr)
    {
      continue;
    }

    // if not pressed
    if (!button->IsClicked(touchCoords.x, touchCoords.y))
    {
      continue;
    }

    debug("Button clicked");
    analogWrite(Misc::BUZZER_PIN, 255 / 4);
    delay(50);
    analogWrite(Misc::BUZZER_PIN, 0);

    debug(g_UIManager.GetDynamicElements().size());
    g_Scale.SetMode(CoffeeScale::Mode::NORMAL);
    break; // can only press 1 button/point at a time, no need to check the rest
  }
}

void handleScaleUpdate()
{
  // handle scale actions
  if (!g_NewDataReady)
  {
    return;
  }

  switch (g_Scale.GetMode())
  {
  case CoffeeScale::Mode::MENU:
  {
    debug("Welcome to the main menu");
    break;
  }
  case CoffeeScale::Mode::NORMAL:
  {
    g_TFT.Clear();
    g_UIManager.Clear();
    std::shared_ptr<Label> massLabel = std::make_shared<Label>(g_TFT, "Mass: " + (String)g_Scale.GetMass(), 0, 0, TFT_BLACK);
    g_UIManager.AddLabel(massLabel);

    while (1)
    {
      g_Scale.UpdateMass();
      massLabel->SetLabel("Mass: " + (String)g_Scale.GetMass());
      g_UIManager.Update();
      delay(10);
    }
    break;
  }
  case CoffeeScale::Mode::AUTO:
  {
    debug("Mass (g): " + g_Scale.GetMass());
    // const int elapsedTimeSeconds = getTimeSinceInteraction() / 1000000;
    // debug("Time (m:s): " + elapsedTimeSeconds / 60 + ":" + elapsedTimeSeconds % 60); // s to m and clamp seconds to [0;59]
    break;
  }
  case CoffeeScale::Mode::STATS:
  {
    CoffeeScale::Data scaleData = g_Scale.GetDataStruct();
    debug("Total volume: " + scaleData.totalVolume);
    debug("Total brews: " + scaleData.totalBrews);
    debug("Calibration value: " + scaleData.calibrationValue);
    break;
  }
  };

  g_NewDataReady = false;
}

void shutdown()
{
  debug("Shutting down");
  g_Scale.SaveData();
  esp_deep_sleep_start();
}