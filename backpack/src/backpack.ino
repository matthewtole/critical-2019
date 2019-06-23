#include "Particle.h"
#include "neopixel.h"

SYSTEM_MODE(MANUAL);

#define PIXEL_PIN_LEFT D2
#define PIXEL_PIN_RIGHT D3
#define PIXEL_COUNT 50
#define PIXEL_TYPE WS2812B

Adafruit_NeoPixel stripLeft(PIXEL_COUNT, PIXEL_PIN_LEFT, PIXEL_TYPE);
Adafruit_NeoPixel stripRight(PIXEL_COUNT, PIXEL_PIN_RIGHT, PIXEL_TYPE);

TCPServer server = TCPServer(23);
TCPClient client;


Timer timer(5 * 1000, next_color);
Timer pattern_update(30, update_pattern);

void update_pattern() {

}

void next_color() {
  pattern_color = strip.Color(random(256), random(256), random(256));
    for (uint16_t l = 0; l < PIXEL_COUNT; l += 1)
  {
    strip.setPixelColor(l, pattern_color);
    strip2.setPixelColor(l, pattern_color);
  }
  strip.show();
  strip2.show();

  timer.start();
}

void setup()
{

  WiFi.connect();
  server.begin();

  // Make sure your Serial Terminal app is closed before powering your device
  Serial.begin(9600);
  // Wait for a USB serial connection for up to 30 seconds
  waitFor(Serial.isConnected, 30000);

  Serial.println(WiFi.localIP());
  Serial.println(WiFi.subnetMask());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.SSID());

  timer.start();
}

// // loop() runs over and over again, as quickly as it can execute.
void loop()
{
  if (client.connected())
  {
    // echo all available bytes back to the client
    if (client.available())
    {
      String str = client.readString();
      server.write(String::format("\nack: %d (%s)\n", str.length(), str.c_str()));

      if (str.length() != 6)
      {
        return;
      }
      String command = str.substring(0, 3);
      if (command == "DFU")
      {
        dfu_mode();
        return;
      }
      if (command == "PAT")
      {
        // uint8_t pattern_id = str.substring(3).toInt();
        // Serial.printf("%d", pattern_id);
        // set_pattern();
        return;
      }
    }
  }
  else
  {
    // if no client is yet connected, check for a new connection
    client = server.available();
  }
}

void set_pattern(uint8_t pattern_id) {
  pattern_color = (pattern_id == 0 ? strip.Color(255, 0, 0) : strip.Color(0, 0, 255));
}

void fadeToBlack(int ledNo, byte fadeValue) {
    uint32_t oldColor;
    uint8_t r, g, b;
    int value;
   
    oldColor = strip.getPixelColor(ledNo);
    r = (oldColor & 0x00ff0000UL) >> 16;
    g = (oldColor & 0x0000ff00UL) >> 8;
    b = (oldColor & 0x000000ffUL);

    r=(r<=10)? 0 : (int) r-(r*fadeValue/256);
    g=(g<=10)? 0 : (int) g-(g*fadeValue/256);
    b=(b<=10)? 0 : (int) b-(b*fadeValue/256);
   
    strip.setPixelColor(ledNo, r,g,b);
}


void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {  
  strip.clear();
  strip.show();
 
  for(int i = 0; i < PIXEL_COUNT+PIXEL_COUNT; i++) {
   
   
    // fade brightness all LEDs one step
    for(int j=0; j<PIXEL_COUNT; j++) {
      if( (!meteorRandomDecay) || (random(10)>5) ) {
        fadeToBlack(j, meteorTrailDecay );        
      }
    }
   
    // draw meteor
    for(int j = 0; j < meteorSize; j++) {
      if( ( i-j <PIXEL_COUNT) && (i-j>=0) ) {
        strip.setPixelColor(i-j, red, green, blue);
      }
    }
   
    strip.show();
    delay(SpeedDelay);
  }
}

void dfu_mode()
{
  System.dfu();
}
