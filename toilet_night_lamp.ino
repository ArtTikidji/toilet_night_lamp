#include <Adafruit_NeoPixel.h>
#include <time.h>

#define SENSOR_PIN  6
#define PIN 8   // light control pin
#define PIN_NUM 2 //count of leds

const int statesCount = 3;

/*
  current state of system:
  0 -- waiting to turn light
  1 -- turning on light
  2 -- waiting to turn off light
*/
int state = 0;
/*
  time shifts between iterations in each state in mills
  in state 0 we just waiting for sensor activation, that's why states_timers[0] = NULL
  in state 1 we have 255 brightness increesing with delay 100 mills
  in state 2 we one step with waiting to turn off light
*/
unsigned long states_timers[statesCount] = {NULL, 100, 120000};
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIN_NUM, PIN, NEO_GRB + NEO_KHZ800);
unsigned long start_turn_on;
unsigned long statr_waiting;

int brightness_increase(unsigned long current_time){
  int brightness = (current_time - start_turn_on)/states_timers[2];
  
  if (brightness > 255) return 3;
  
  strip.setPixelColor(0, strip.Color(brightness, brightness, 0));
  strip.show();
  return 2;
}

void turn_off_light(){
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
}

void setup(){   
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);
  strip.begin();
  
  // in start position turning off light
  turn_off_light();
}


void loop(){
  unsigned long current_time = millis();
  switch (state) {
    case 0:
      state = digitalRead(SENSOR_PIN);
      if (state == 1){
        start_turn_on = millis();
        Serial.println("Somebody is in this area!");
      }
      break;
    case 1:
      state = brightness_increase(current_time);
      if (state == 3){ statr_waiting = current_time;}
      break;
    case 3:
      if (current_time - statr_waiting > states_timers[state]) {
        turn_off_light();
        state = 0;
      }
      break;
    default:
      Serial.print("Something went wrong!\nUnknown system state: ");
      Serial.println(state);
      break;
  }
}
