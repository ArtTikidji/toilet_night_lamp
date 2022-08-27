#ifndef light_control_h
#define light_control_h

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

class Light_control{
private:
     /*
     current state of system:
     0 -- waiting to turn light
     1 -- turning on light
     2 -- waiting to turn off light
     3 -- checking sensor and keep illuminate if something detected
     */
    int state = WAIT_FOR_SOMEONE;
    /*
     time shifts between iterations in each state in mills
     in state 0 we just waiting for sensor activation, that's why state_duration[0] = NULL
     in state 1 we have 255 brightness increesing with delay 100 mills
     in state 2 we one iteration with waiting long time
     in state 3 we one iteration with checking sensor and turn off if time ends
     */
    const unsigned long state_duration[statesCount] = {0, 100, 240000, 100};
    unsigned long states_starts[statesCount] = {0, 0, 0, 0};
    Adafruit_NeoPixel strip;
    const int brightness_shift = 75;
    const int maximum_brightness = 180; // 255 - brightness_shift - 10 (just in case)
    int brightness;
    unsigned long current_time;
    
    bool reached_maximum_brightness(int, int);
    
    bool reached_minimum_brightness(int, int);
    
    void set_brightness(int);
    
    /*
     changing brightness if enough time passed
     returns new status (can match with current status)
     */
    int brightness_change();
    
public:
    void setup_script();
    
    int get_state();
    void update_current_time();
    
    void waiting_movment();
    void smoothly_light_on();
    void light_turned_on_wait();
    void trying_2_off_light();
};

#endif /* light_control_h */
