#include "light_control.h"

Light_control light_controler;

void setup(){   
  pinMode(SENSOR_PIN, INPUT);
  Serial.begin(9600);
  light_controler.setup_script();
}


void loop(){
  light_controler.update_current_time();
  switch (light_controler.get_state()) {
    case WAIT_FOR_SOMEONE:
      light_controler.waiting_movment();
      break;
    case INCREASE_BRIGHTNESS:
      light_controler.smoothly_light_on();
      break;
    case LIGHT_ON_WAIT:
      light_controler.light_turned_on_wait();
      break;
    case DECREASE_BRIGHTNESS:
      light_controler.trying_2_off_light();
      break;
    default:
      Serial.print("Something went wrong!\nUnknown system state: ");
      Serial.println(light_controler.get_state());
      break;
  }
}
