#include <Adafruit_NeoPixel.h>

#define SENSOR_PIN  6
#define PIN 8   // light control pin
#define PIN_NUM 2 //count of leds

Adafruit_NeoPixel strip = Adafruit_NeoPixel( PIN_NUM,PIN, NEO_GRB + NEO_KHZ800);

int state;
int r, g;
void setup()
{
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);
  strip.begin();
}
void loop()
{
  state = digitalRead(SENSOR_PIN);
  if (state == 1){
    Serial.println("Somebody is in this area!");
    for(r = 0; r < 255; r++){
      g = r;
      strip.setPixelColor(0, strip.Color(r, g, 0));//setting yellow colour
      strip.show();
      delay(100);
    }
    delay(120000);
  }
  else{
    strip.setPixelColor(0, strip.Color(0, 0, 0));//turn off light
    strip.show();
    delay(500);
  }
}
