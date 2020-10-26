#include "mbed.h"

InterruptIn button(p5);
DigitalOut red_led(p6);
DigitalOut blue_led(p7);
PwmOut intensity(p8);

Timeout timeout;
Ticker ticker;

enum STATE
{
  DOWN, UP, FALL, MIN, RISE, MAX
};

void init();
void PressButton();
void ReleaseButton();
void transitionStatePress();
void ledBlink();

int current_state;
bool stateButton;
float frequency;

int main() {
    init();
    button.rise(&PressButton);
    button.fall(&ReleaseButton);
}

void transitionStatePress(){
    timeout.attach(&ledBlink, 0.1);
    if(stateButton){
        switch (current_state){
            case UP:
                current_state = RISE;
                printf("RISE\n");
                break;
            case DOWN:
                current_state = FALL;
                printf("FALL\n");
                break;
            case FALL:
                if(intensity==0.0){
                    current_state = MIN;
                    printf("MIN\n");
                }else{
                    intensity = intensity - 0.05;
                    printf("intensity: %.2f\n",intensity.read());
                }
                break;
            case RISE:
                if(intensity==1.0){
                    current_state = MAX;
                    printf("MAX\n");
                }else{
                    intensity = intensity + 0.05;
                    printf("intensity: %.2f\n",intensity.read());
                }
                break;
            default:
                if (intensity >= 1.0) {
                    current_state = MAX;
                    printf("MAX\n");
                } else if (intensity <= 0.0) {
                        current_state = MIN;
                        printf("MIN\n");
                }
                break;
        }
    }
}

void init(){
    blue_led = 1.0;
    red_led = 0.0;
    intensity = 1.0;
    current_state = UP;
    printf("UP\n");
}

void PressButton()
{
    printf("Press Button\n");
    stateButton = true;
    ticker.attach(&transitionStatePress,1.0);
}

void ReleaseButton()
{
    printf("Release Button\n");
    stateButton = false;
    ticker.detach();
    if(current_state == MAX || current_state == UP || current_state == FALL){
        if(intensity > 0.0){
            blue_led = 0.0;
            red_led = 1.0;
            current_state = DOWN;
            printf("DOWN\n");
        }
    }
    else if(current_state == MIN || current_state == DOWN || current_state == RISE){
        if(intensity < 1.0){
            blue_led = 1.0;
            red_led = 0.0;
            current_state = UP;
            printf("UP\n");
        }
    }
}

void ledBlink(){
    switch(current_state){
        case FALL:
            frequency = 1;
            red_led = !red_led;
            break;
        case RISE:
            frequency = 1;
            blue_led = !blue_led;
            break;
        case MIN:
            frequency = 0.1;
            red_led = !red_led;
            break;
        case MAX:
            frequency = 0.1;
            blue_led = !blue_led;
            break;
        default:
            break;
    }
    timeout.attach(&ledBlink, frequency);
}
