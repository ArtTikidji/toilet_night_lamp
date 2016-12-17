#include <Adafruit_NeoPixel.h>

#define  sensorPin  6
#define PIN 8   //led灯控制引脚
#define PIN_NUM 2 //允许接的led灯的个数

Adafruit_NeoPixel strip = Adafruit_NeoPixel( PIN_NUM,PIN, NEO_GRB + NEO_KHZ800);

int state;
int r, g;
void setup()
{
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
  strip.begin();
}
void loop()
{
  state = digitalRead(sensorPin);
  if (state == 1){
    Serial.println("Somebody is in this area!");
    for(r = 0; r < 255; r++){
      g = r;
      strip.setPixelColor(0, strip.Color(r, g, 0));//黄光
      strip.show();
      delay(100);
    }
    delay(120000);
  }
  else{
  strip.setPixelColor(0, strip.Color(0, 0, 0));//灭
  strip.show();
  delay(500);
  }
}
