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
     in state 0 we just waiting for sensor activation, that's why state_duration[0] = NULL
     in state 1 we have 255 brightness increesing with delay 100 mills
     in state 2 we one iteration with waiting long time
     in state 3 we one iteration with checking sensor and turn off if time ends
     */
    const unsigned long state_duration[statesCount] = {NULL, 100, 240000, 100};
    unsigned long states_starts[statesCount] = {NULL, 0, 0, 0};
    Adafruit_NeoPixel strip;
    const int brightness_shift = 75;
    const int maximum_brightness = 180; // 255 - brightness_shift - 10 (just in case)
    int brightness;
    unsigned long current_time;
    
    bool reached_maximum_brightness(int state, int brightness) {
        return (state == INCREASE_BRIGHTNESS) and (brightness >= maximum_brightness);
    }
    
    bool reached_minimum_brightness(int state, int brightness) {
        return (state == DECREASE_BRIGHTNESS) and (brightness <= 0);
    }
    
    void set_brightness(int brightness){
        strip.setPixelColor(0, strip.Color(brightness, brightness, 0));
        strip.show();
    }
    
    /*
     changing brightness if enough time passed
     returns new status (can match with current status)
     */
    int brightness_change(){
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
    }
    
public:
    void setup_script(){
        strip = Adafruit_NeoPixel(PIN_NUM, PIN, NEO_GRB + NEO_KHZ800);
        strip.begin();
    }
    
    void update_current_time(){ current_time = millis(); }
    
    int waiting_movment(){
        int new_state = digitalRead(SENSOR_PIN);
        if (new_state == INCREASE_BRIGHTNESS){
            states_starts[INCREASE_BRIGHTNESS] = current_time;
            Serial.println("Somebody is in this area!");
        }
        return new_state;
    }
    
    int smoothly_light_on(){
        int new_state = brightness_change();
        if (new_state == LIGHT_ON_WAIT){
            states_starts[LIGHT_ON_WAIT] = current_time;
            Serial.println("Maximum brightness, start waiting!");
        }
        return new_state;
    }
    
    int light_turned_on_wait(){
        int new_state = LIGHT_ON_WAIT;
        if (current_time - states_starts[LIGHT_ON_WAIT] > state_duration[LIGHT_ON_WAIT]) {
            states_starts[DECREASE_BRIGHTNESS] = current_time;
            new_state = DECREASE_BRIGHTNESS;
            Serial.println("Time out, try to turn off light");
        }
        return new_state;
    }
    
    int trying_2_off_light(){
        int new_state = brightness_change();
        int sensor_starus = digitalRead(SENSOR_PIN);
        if (sensor_starus == 1){
            new_state = INCREASE_BRIGHTNESS;
            states_starts[INCREASE_BRIGHTNESS] = current_time;
            Serial.println("Somebody is in this area! Don't turn off light");
        }
        return new_state;
    }
};
