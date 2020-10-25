#include "mbed.h"

InterruptIn button(p5);
DigitalOut red_led(p6);
DigitalOut blue_led(p7);
PwmOut ledIntensity(p8);

Timeout timeout;
Ticker ticker;

enum STATE
{
  DOWN, UP, FALL, MIN, RISE, MAX, PRESS, RELEASE
};

void transitionState();
void transitionStateRelease();

void init();
void PressButton();
void ReleaseButton();
void defaultState();
void transitionState();
void manual_count_fun();

int current_state;
int manual_count = 0;
int stateButton;

int main() {
    init();
    
    button.rise(&PressButton);
    button.fall(&ReleaseButton);
    
    while(1) {
        wait(1);
        printf("Led %f\n",ledIntensity.read());
        printf("count %d\n",manual_count);
    }
}

void transitionState(){
    if(stateButton == PRESS){
        switch (current_state){
            case UP:
                printf("UP\n");
                blue_led = 1;
                red_led = 0;
                if (stateButton == RELEASE && manual_count>0 && manual_count<10){
                    current_state = DOWN;
                }else if(stateButton == PRESS && manual_count>=1 && ledIntensity<=1.0){
                    current_state = RISE;
                }
                break;
            case DOWN:
                blue_led = 0;
                red_led = 1;
                printf("DOWN\n");
                if (stateButton == RELEASE && manual_count>0 && manual_count<10){
                    current_state = UP;
                }else if(stateButton == PRESS && manual_count>=1 && ledIntensity>0.0){
                    current_state = FALL;
                }
                break;
            case FALL:
                printf("FALL\n");
                if (stateButton == PRESS && ledIntensity>0.0){
                    ledIntensity = ledIntensity - 0.05f;
                }else if(stateButton == PRESS && ledIntensity==0.0){
                    current_state = MIN;
                }
                else if(stateButton == RELEASE && ledIntensity>0.0){
                    current_state = DOWN;
                }
                break;
            case RISE:
                printf("RISE\n");
                if(stateButton == PRESS && ledIntensity<1.0){
                    ledIntensity = ledIntensity + 0.05f;
                }else if(stateButton == PRESS && ledIntensity ==1.0){
                    current_state = MAX;
                }else if(stateButton == RELEASE && ledIntensity<1.0){
                    current_state = UP;
                }
                break;
        //     case MAX:
        //         printf("MAX\n");
        //         if(stateButton == RELEASE){
        //             current_state = DOWN;
        //             manual_count = 0;
        //         }
        //         break;
        //     case MIN:
        //         printf("MIN\n");
        //         if(stateButton == RELEASE){
        //             current_state = UP;
        //             manual_count = 0;
        //         }
        //         break;
        }
    }
}

void transitionStateRelease(){
    if(current_state == MAX){
        if(ledIntensity > 0){
            blue_led = 0;
            red_led = 1;
            state = DOWN;
        }
    }
    else if(current_state == MIN){
        if(ledIntensity < 1){
            blue_led = 1;
            red_led = 1;
            state = UP;
        }
    }
}

void init(){
    blue_led = 1;
    red_led = 0;
    ledIntensity = 0.5;
    current_state = UP;
}

void manual_count_fun(){
    manual_count++;
}

void PressButton()
{
    printf("Press Button\n");
    stateButton = PRESS;
    ticker.attach(&transitionState,1);
    timeout.attach(&manual_count_fun,0.1);
}

void ReleaseButton()
{
    printf("Release Button\n");
    timeout.detach();
    stateButton = RELEASE;
    transitionState();
    manual_count = 0;
}
