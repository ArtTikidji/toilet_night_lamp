#include <Adafruit_NeoPixel.h>
#include <time.h>

#define SENSOR_PIN  6
#define PIN 8   // light control pin
#define PIN_NUM 2 //count of leds

#define WAIT_FOR_SOMEONE 0
#define INCREASE_BRIGHTNESS 1
#define LIGHT_ON_WAIT 2
#define TRY_OFF_LIGHT 3

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
    unsigned long states_timers[statesCount] = {NULL, 100, 240000, 30000};
    unsigned long states_starts[statesCount] = {NULL, 0, 0, 0};
    Adafruit_NeoPixel strip;
    
    unsigned long start_checking;
    
    unsigned long current_time;
    
    int brightness_increase(){
        int brightness = (current_time - states_starts[INCREASE_BRIGHTNESS])/states_timers[INCREASE_BRIGHTNESS];
        if (brightness > 180) {return LIGHT_ON_WAIT;}
        brightness += 75;
        strip.setPixelColor(0, strip.Color(brightness, brightness, 0));
        strip.show();
        return INCREASE_BRIGHTNESS;
    }
    
    void turn_off_light(){
        strip.setPixelColor(0, strip.Color(0, 0, 0));
        strip.show();
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
        int result = brightness_increase();
        if (result == LIGHT_ON_WAIT){
            states_starts[LIGHT_ON_WAIT] = current_time;
            Serial.println("Maximum rightness, start waiting!");
        }
        return result;
    }
    
    int light_turned_on_wait(){
        int result = LIGHT_ON_WAIT;
        if (current_time - states_starts[LIGHT_ON_WAIT] > states_timers[LIGHT_ON_WAIT]) {
            states_starts[TRY_OFF_LIGHT] = current_time;
            result = TRY_OFF_LIGHT;
            Serial.println("Time out, try to turn off light");
        }
        return result;
    }
    
    int trying_2_off_light(){
        int result = TRY_OFF_LIGHT;
        int sensor_starus = digitalRead(SENSOR_PIN);
        if (sensor_starus == 1){
            result = LIGHT_ON_WAIT;
            states_starts[LIGHT_ON_WAIT] = current_time;
            Serial.println("Somebody is in this area! Don't turn off light");
        }
        if(current_time - states_starts[TRY_OFF_LIGHT] > states_timers[TRY_OFF_LIGHT]){
            turn_off_light();
            result = WAIT_FOR_SOMEONE;
            Serial.println("Turn off light");
        }
        return result;
    }
};
