//
//  light_control.ino
//  
//
//  Created by Artem on 27/08/2022.
//

#include "light_control.h"

bool Light_control::reached_maximum_brightness(int state, int brightness) {
  return (state == INCREASE_BRIGHTNESS) and (brightness >= maximum_brightness);
};

bool Light_control::reached_minimum_brightness(int state, int brightness) {
  return (state == DECREASE_BRIGHTNESS) and (brightness <= 0);
};

void Light_control::set_brightness(int brightness){
  for (int led_num = 0; led_num < PIN_NUM; ++led_num){
    strip.setPixelColor(led_num, strip.Color(brightness, brightness, 0));
  }
  strip.show();
};

/*
  changing brightness if enough time passed
  returns new status (can match with current status)
*/
int Light_control::brightness_change(){
  int brightness_variation = (current_time - states_starts[state])/state_duration[state];
  if (brightness_variation > 0){
    brightness_variation *= state == DECREASE_BRIGHTNESS ? -1 : 1;
    brightness += brightness_variation;
    states_starts[state] = current_time;
    set_brightness(brightness + brightness_shift);
  }
        
  if (reached_maximum_brightness(state, brightness)){
    return LIGHT_ON_WAIT;
  }
  if (reached_minimum_brightness(state, brightness)){
    set_brightness(0);
    return WAIT_FOR_SOMEONE;
  }
  return state;
};

void Light_control::setup_script(){
  strip = Adafruit_NeoPixel(PIN_NUM, PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
}
    
void Light_control::update_current_time(){ current_time = millis(); }
    
int Light_control::waiting_movment(){
  int new_state = digitalRead(SENSOR_PIN);
  if (new_state == INCREASE_BRIGHTNESS){
    states_starts[INCREASE_BRIGHTNESS] = current_time;
    Serial.println("Somebody is in this area!");
  }
  return new_state;
}
    
int Light_control::smoothly_light_on(){
  int new_state = brightness_change();
  if (new_state == LIGHT_ON_WAIT){
    states_starts[LIGHT_ON_WAIT] = current_time;
    Serial.println("Maximum brightness, start waiting!");
  }
  return new_state;
}
    
int Light_control::light_turned_on_wait(){
  int new_state = LIGHT_ON_WAIT;
  if (current_time - states_starts[LIGHT_ON_WAIT] > state_duration[LIGHT_ON_WAIT]) {
    states_starts[DECREASE_BRIGHTNESS] = current_time;
    new_state = DECREASE_BRIGHTNESS;
    Serial.println("Time out, try to turn off light");
  }
  return new_state;
}
    
int Light_control::trying_2_off_light(){
  int new_state = brightness_change();
  int sensor_starus = digitalRead(SENSOR_PIN);
  if (sensor_starus == 1){
    new_state = INCREASE_BRIGHTNESS;
    states_starts[INCREASE_BRIGHTNESS] = current_time;
    Serial.println("Somebody is in this area! Don't turn off light");
  }
  return new_state;
}
