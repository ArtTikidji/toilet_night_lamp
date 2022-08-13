#include "light_control.h"

Light_control light_controler;

void setup(){   
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);
  light_controler.setup_script();
}


void loop(){
  light_controler.update_current_time();
  switch (state) {
    case WAIT_FOR_SOMEONE:
      state = light_controler.waiting_movment();
      break;
    case INCREASE_BRIGHTNESS:
      state = light_controler.smoothly_light_on();
      break;
    case LIGHT_ON_WAIT:
      state = light_controler.light_turned_on_wait();
      break;
    case TRY_OFF_LIGHT:
      state = light_controler.trying_2_off_light();
      break;
    default:
      Serial.print("Something went wrong!\nUnknown system state: ");
      Serial.println(state);
      break;
  }
}
