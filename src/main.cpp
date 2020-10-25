#include "mbed.h"

InterruptIn button(p5);
DigitalOut red_led(p6);
DigitalOut blue_led(p7);
PwmOut ledIntensity(p8);

Timeout timeout;
Ticker ticker;

enum STATE
{
  DOWN, UP, FALL, MIN, RISE, MAX
};

void transitionStatePress();
void transitionStateRelease();

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
    
    while(1) {
        wait(1);
        printf("Led %f\n",ledIntensity.read());
    }
}

void transitionStatePress(){
    ticker.attach(&transitionStatePress,1);
    timeout.attach(&ledBlink, 0.2);
    if(stateButton){
        switch (current_state){
            case UP:
                current_state = RISE;
                printf("RISE\n");
                // stateButton = false;
                break;
            case DOWN:
                current_state = FALL;
                printf("FALL\n");
                // stateButton = false;
                break;
            case FALL:
                if(ledIntensity==0){
                    current_state = MIN;
                    printf("MIN\n");
                }else{
                    ledIntensity = ledIntensity - 0.05;
                }
                break;
            case RISE:
                if(ledIntensity==1){
                    current_state = MAX;
                    printf("MAX\n");
                }else{
                    ledIntensity = ledIntensity + 0.05;
                }
                break;
            default:
                break;
        }
        if (ledIntensity >= 1) {
                current_state = MAX;
                printf("MAX\n");
                ledIntensity = 1;
        } else if (ledIntensity <= 0) {
                current_state = MIN;
                printf("MIN\n");
                ledIntensity = 0;
        }
    }
}

void transitionStateRelease(){
    if(current_state == MAX || current_state == UP ||current_state == FALL){
        if(ledIntensity > 0){
            blue_led = 0;
            red_led = 1;
            current_state = DOWN;
            printf("DOWN\n");
        }
    }
    else if(current_state == MIN || current_state == DOWN || current_state == RISE){
        if(ledIntensity < 1){
            blue_led = 1;
            red_led = 0;
            current_state = UP;
            printf("UP\n");
        }
    }
}

void init(){
    blue_led = 1;
    red_led = 0;
    ledIntensity = 0.5;
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
    transitionStateRelease();
}

void ledBlink(){
    switch(current_state){
        case FALL:
            frequency = (0.2 * (ledIntensity / 0.05 - 1));
            red_led = !red_led;
            break;
        case RISE:
            frequency = 0.2 * (1 + ledIntensity / 0.05);
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
