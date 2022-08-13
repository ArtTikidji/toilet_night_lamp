#include <Adafruit_NeoPixel.h>
#include <time.h>

#define SENSOR_PIN  6
#define PIN 8   // light control pin
#define PIN_NUM 2 //count of leds

#define WAIT_FOR_SOMEONE 0
#define INCREASE_BRIGHTNESS 1
#define LIGHT_ON_WAIT 2
#define DECREASE_BRIGHTNESS 3

const int statesCount = 4;

/*
 current state of system:
 0 -- waiting to turn light
 1 -- turning on light
 2 -- waiting to turn off light
 3 -- checking sensor and keep illuminate if something detected
 */
int state = WAIT_FOR_SOMEONE;


class Light_control{
private:
    /*
     time shifts between iterations in each state in mills
     in state 0 we just waiting for sensor activation, that's why states_timers[0] = NULL
     in state 1 we have 255 brightness increesing with delay 100 mills
     in state 2 we one iteration with waiting long time
     in state 3 we one iteration with checking sensor and turn off if time ends
     */
    const unsigned long states_timers[statesCount] = {NULL, 100, 2400, 100};
    unsigned long states_starts[statesCount] = {NULL, 0, 0, 0};
    Adafruit_NeoPixel strip;
    const int brightness_shift = 75;
    int brightness;
    unsigned long current_time;
    
    int brightness_change(bool upDown){
        int result = upDown ? INCREASE_BRIGHTNESS : DECREASE_BRIGHTNESS;
        int change_val = (current_time - states_starts[result])/states_timers[result];
        if (!(change_val >= 1)){
            return result;
        }
        brightness += upDown ? change_val : -1 * change_val;
        states_starts[result] = current_time;
        int new_val = brightness + brightness_shift;
        strip.setPixelColor(0, strip.Color(new_val, new_val, 0));
        strip.show();
        if (upDown){
            if (brightness > 170) { return LIGHT_ON_WAIT; }
        } else {
            if (brightness <= 0) {
                strip.setPixelColor(0, strip.Color(0, 0, 0));
                strip.show();
                return WAIT_FOR_SOMEONE;
            }
        }
        return result;
    }
    
public:
    void setup_script(){
        strip = Adafruit_NeoPixel(PIN_NUM, PIN, NEO_GRB + NEO_KHZ800);
        strip.begin();
    }
    
    void update_current_time(){ current_time = millis(); }
    
    int waiting_movment(){
        int result = digitalRead(SENSOR_PIN);
        if (result == INCREASE_BRIGHTNESS){
            states_starts[INCREASE_BRIGHTNESS] = current_time;
            Serial.println("Somebody is in this area!");
        }
        return result;
    }
    
    int smoothly_light_on(){
        int result = brightness_change(true);
        if (result == LIGHT_ON_WAIT){
            states_starts[LIGHT_ON_WAIT] = current_time;
            Serial.println("Maximum brightness, start waiting!");
        }
        return result;
    }
    
    int light_turned_on_wait(){
        int result = LIGHT_ON_WAIT;
        if (current_time - states_starts[LIGHT_ON_WAIT] > states_timers[LIGHT_ON_WAIT]) {
            states_starts[DECREASE_BRIGHTNESS] = current_time;
            result = DECREASE_BRIGHTNESS;
            Serial.println("Time out, try to turn off light");
        }
        return result;
    }
    
    int trying_2_off_light(){
        int result = brightness_change(false);
        int sensor_starus = digitalRead(SENSOR_PIN);
        if (sensor_starus == 1){
            result = INCREASE_BRIGHTNESS;
            states_starts[INCREASE_BRIGHTNESS] = current_time;
            Serial.println("Somebody is in this area! Don't turn off light");
        }
        return result;
    }
};
