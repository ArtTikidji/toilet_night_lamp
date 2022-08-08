#include <Adafruit_NeoPixel.h>

#define SENSOR_PIN  6
#define PIN 8   // light control pin
#define PIN_NUM 2 //count of leds

int state;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIN_NUM, PIN, NEO_GRB + NEO_KHZ800);


void smoothly_turning_on_light(){
  for(int brightness = 0; brightness < 255; brightness++){
      strip.setPixelColor(0, strip.Color(brightness, brightness, 0));
      strip.show();
      delay(100);
    }
}

void turn_off_light(){
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
}

void setup()
{   
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);
  strip.begin();
  
  // in start position turning off light
  turn_off_light();
}


void loop()
{
  state = digitalRead(SENSOR_PIN);
  if (state == 1){
    Serial.println("Somebody is in this area!");

    smoothly_turning_on_light();

    delay(120000);
    
    // after some time turning off light
    turn_off_light();
  }
}
