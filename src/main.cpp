#include <Arduino.h>
#include <vector>
#include <EEPROM.h>
#include <memory>
#include <HX711_ADC.h>
#include "TFT_wrapper.hpp"
#include "coffee_scale.hpp"
#include "text_component.hpp"
#include "button_component.hpp"
#include "button.hpp"
#include "DebugUtils.hpp"

namespace HX711
{
  constexpr uint8_t SCK_PIN = GPIO_NUM_33;
  constexpr uint8_t DOUT_PIN = GPIO_NUM_32;
};

namespace TFT_Properties
{
  constexpr int16_t WIDTH = 320;
  constexpr int16_t LENGTH = 240;
  constexpr uint8_t TEXT_SIZE = 3;
  constexpr uint16_t TEXT_COLOR = TFT_WHITE;
  constexpr uint16_t BG_COLOR = TFT_WHITE;
  constexpr gpio_num_t BACKLIGHT_PIN = GPIO_NUM_2;
  constexpr gpio_num_t IRQ_PIN = GPIO_NUM_0;
};

volatile bool g_NewDataReady = false;
volatile bool g_TimeoutTimerExpired = false;
constexpr gpio_num_t BUZZER_PIN = GPIO_NUM_22;

CoffeeScale g_Scale(HX711::DOUT_PIN, HX711::SCK_PIN);
TFT g_TFT(TFT_Properties::LENGTH, TFT_Properties::WIDTH, TFT_Properties::TEXT_COLOR, TFT_Properties::BG_COLOR, TFT_Properties::TEXT_SIZE);

void onDataReadyISR();
void handleButtonPress(std::vector<std::unique_ptr<Button>> &buttons);
void handleScaleUpdate();

/*
TODO:
set timeOut/oneShotTimer callback function to the ISR in main using extern "C".
when button pressed, set last interaction time
remember to profile application
?Expand button hit box to make it more forgiving/less annoying (already covered by padding?)
?Run buttons by interrupts vs polling
line: tft.drawWideLine(0, 0, 320 / 2, 240 / 2, 2, TFT_DARKGREY, TFT_BROWN);
*/

void setup()
{
  Serial.begin(115200);
  debug("Setup started");

  pinMode(TFT_Properties::BACKLIGHT_PIN, OUTPUT);
  pinMode(TFT_Properties::IRQ_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(TFT_Properties::BACKLIGHT_PIN, HIGH);

  esp_sleep_enable_ext0_wakeup(TFT_Properties::IRQ_PIN, LOW);
  attachInterrupt(digitalPinToInterrupt(HX711::DOUT_PIN), onDataReadyISR, FALLING);

  EEPROM.begin(512); // CoffeeScale::Data + other vars
  g_TFT.Init();
  g_Scale.init();

  // write test data
  // constexpr CoffeeScale::Data testData{1, 2, 466.97};
  // g_Scale.setData(testData);
  // g_Scale.saveData();

  RTC_DATA_ATTR static uint8_t bootCount = 0;
  debug("Boot count: " + bootCount++);
  debug("Setup done");
}

void loop()
{
  constexpr unsigned long timeoutThreshold = 210 * 1000000; // 210 s

  std::vector<std::unique_ptr<Button>> buttons;
  buttons.reserve(3);
  buttons.emplace_back(std::make_unique<Button>(g_TFT, "test"));
  buttons.emplace_back(std::make_unique<Button>(g_TFT, "test1"));

  while (!g_TimeoutTimerExpired)
  {
    handleButtonPress(buttons);
    handleScaleUpdate();
  }
  debug("Timeout timer expired!");
}

inline void onDataReadyISR()
{
  if (g_Scale.update())
  {
    g_NewDataReady = true;
  }
}

// mark by instruction RAM attribute
inline void IRAM_ATTR onTimerExpireISR(void *arg)
{
  g_TimeoutTimerExpired = true;
}

void handleButtonPress(std::vector<std::unique_ptr<Button>> &buttons)
{
  // handle button press
  Vector2 touchCoords = g_TFT.getTouchCoords();

  if (touchCoords.x != 0 || touchCoords.y != 0)
  {
    for (auto it = buttons.begin(); it != buttons.end(); it++)
    {
      if (!(*it)->IsClicked(touchCoords.x, touchCoords.y))
      {
        continue;
      }

      // debug("Button clicked!");
      // buttons.erase(it);
      // analogWrite(BUZZER_PIN, 255 / 4);
      // digitalWrite(TFT_Properties::BACKLIGHT_PIN, 0);
      // // update display
      // // g_TFT.Clear(); // clear display
      // // // re-draw all available buttons
      // // for (auto const &button : buttons)
      // // {
      // //   button->Draw();
      // // }
      // delay(50);
      // analogWrite(BUZZER_PIN, 0);

      // // save to eeprom, clean up, etc.
      // debug("Entering deep sleep");
      // g_Scale.saveData();
      // delay(2000); // artificial delay to make the device (backlight) seem turned off, but wait for a little while to prevent accidental instantaneous power on
      // esp_deep_sleep_start();
      // break; // can only press 1 button/point at a time, no need to check the rest

      debug("Button clicked");
      g_Scale.setMode(CoffeeScale::Mode::NORMAL);
      // analogWrite(BUZZER_PIN, 255 / 3);
      // delay(50);
      // analogWrite(BUZZER_PIN, 0);
      break;
    }
  }
}

void handleScaleUpdate()
{
  // handle scale actions
  if (!g_NewDataReady)
  {
    return;
  }

  g_Scale.updateMass();
  // display new data in according mode
  // (for auto mode) store last mass and compare current mass to last mass, then if greater than some pre - defined threshold, detect as brew started, start timer break;
  g_Scale.display();
  g_NewDataReady = false;
}