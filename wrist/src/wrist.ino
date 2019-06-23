#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <FastLED.h>
FASTLED_USING_NAMESPACE
#pragma GCC diagnostic pop

#include "Adafruit_MPR121.h"

#define LED_COUNT 8
#define LED_PIN D2

#define STATUS_LED_TOUCH 0
#define STATUS_LED_WIFI 1
#define STATUS_LED_PING 2

#define TOUCH_SENSOR_COUNT 12

SYSTEM_MODE(MANUAL);

struct TouchSensor
{
  bool is_touched;
  long last_changed;
};

CRGB leds[LED_COUNT];
Adafruit_MPR121 touch_sensor = Adafruit_MPR121();
TouchSensor touch_sensors[TOUCH_SENSOR_COUNT];

void setup() {
  Serial.begin();
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
  FastLED.setBrightness(50);
  for (uint8_t i = 0; i < LED_COUNT; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.show();

  init_touch_sensor();
  WiFi.connect();
  Serial.printf("Hello, world!");
}

void loop() {
  update_touch_sensor();
  if (check_dfu_triggered())
  {
    System.dfu();
  }
  else if (check_reboot_triggered())
  {
    System.reset();
  }
  
  if (WiFi.ready()) {
    leds[STATUS_LED_WIFI] = CRGB::Green;
  } else if (WiFi.connecting()) {
    leds[STATUS_LED_WIFI] = CRGB::Orange;
  } else {
    leds[STATUS_LED_WIFI] = CRGB::Red;
  }
  FastLED.show();
}

void set_status_led(uint8_t led, boolean okay) {
  leds[led] = okay ? CRGB::Green : CRGB::Red;
  FastLED.show();
}

void init_touch_sensor()
{
  set_status_led(STATUS_LED_TOUCH, touch_sensor.begin(0x5A));
  for (uint8_t s = 0; s < TOUCH_SENSOR_COUNT; s += 1)
  {
    touch_sensors[s].is_touched = false;
    touch_sensors[s].last_changed = millis();
  }
}

void update_touch_sensor()
{
  uint16_t touched = touch_sensor.touched();
  for (uint8_t i = 0; i < TOUCH_SENSOR_COUNT; i++)
  {
    bool is_touched = touched & _BV(i);
    if (is_touched != touch_sensors[i].is_touched)
    {
      touch_sensors[i].is_touched = is_touched;
      touch_sensors[i].last_changed = millis();
    }
  }
}

bool check_dfu_triggered()
{
  long ms = millis();
  for (uint8_t s = 0; s < 3; s += 1)
  {
    if (!touch_sensors[s].is_touched)
    {
      return false;
    }
    if (ms - touch_sensors[s].last_changed < 2000)
    {
      return false;
    }
  }
  return true;
}

bool check_reboot_triggered()
{
  long ms = millis();
  for (uint8_t s = 3; s < 6; s += 1)
  {
    if (!touch_sensors[s].is_touched)
    {
      return false;
    }
    if (ms - touch_sensors[s].last_changed < 2000)
    {
      return false;
    }
  }
  return true;
}

void connect_wifi() {
  WiFi.connect();
}