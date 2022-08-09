#include <Adafruit_NeoPixel.h>
#include <time.h>

#define SENSOR_PIN  6
#define PIN 8   // light control pin
#define PIN_NUM 2 //count of leds

const int statesCount = 4;

/*
  current state of system:
  0 -- waiting to turn light
  1 -- turning on light
  2 -- waiting to turn off light
  3 -- checking sensor and keep illuminate if something detected
*/
int state = 0;
/*
  time shifts between iterations in each state in mills
  in state 0 we just waiting for sensor activation, that's why states_timers[0] = NULL
  in state 1 we have 255 brightness increesing with delay 100 mills
  in state 2 we one iteration with waiting long time 
  in state 3 we one iteration with checking sensor and turn off if time ends
*/
unsigned long states_timers[statesCount] = {NULL, 100, 240000, 30000};
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIN_NUM, PIN, NEO_GRB + NEO_KHZ800);
unsigned long start_turn_on;
unsigned long statr_waiting;
unsigned long start_checking;

int brightness_increase(unsigned long current_time){
  int brightness = (current_time - start_turn_on)/states_timers[1];
  if (brightness > 255) {
    return 2;
  }
  strip.setPixelColor(0, strip.Color(brightness, brightness, 0));
  strip.show();
  return 1;
}

void turn_off_light(){
  strip.setPixelColor(0, strip.Color(0, 0, 0));
  strip.show();
}

int waiting_movment(unsigned long current_time){
  int result = digitalRead(SENSOR_PIN);
  if (result == 1){
    start_turn_on = current_time;
    Serial.println("Somebody is in this area!");
  }
  return result;
}

int smoothly_light_on(unsigned long current_time){
  int result = brightness_increase(current_time);
  if (result == 2){ 
    statr_waiting = current_time;
    Serial.println("Maximum rightness, start waiting!");
  }
  return result;
}

int light_turned_on_wait(unsigned long current_time){
  int result = 2;
  if (current_time - statr_waiting > states_timers[2]) {
    start_checking = current_time;
    result = 3;
    Serial.println("Time out, try to turn off light");
  }
  return result;
}

int trying_2_off_light(unsigned long current_time){
  int result = 3;
  int sensor_starus = digitalRead(SENSOR_PIN);
  if (sensor_starus == 1){
    result = 2;
    statr_waiting = current_time;
    Serial.println("Somebody is in this area! Don't turn off light");
  }
  if(current_time - start_checking > states_timers[3]){
    turn_off_light();
    result = 0;
    Serial.println("Turn off light");
  }
  return result; 
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
      state = waiting_movment(current_time);
      break;
    case 1:
      state = smoothly_light_on(current_time);
      break;
    case 2:
      state = light_turned_on_wait(current_time);
      break;
    case 3:
      state = trying_2_off_light(current_time);
      break;
    default:
      Serial.print("Something went wrong!\nUnknown system state: ");
      Serial.println(state);
      break;
  }
}
