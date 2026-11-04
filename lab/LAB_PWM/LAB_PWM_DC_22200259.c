/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 Keumjoon Myeong
Created          : 05-03-2021
Modified         : 11-02-2026
Language/ver     : C++ in VS Code

Description      : DC Motor LAB
/----------------------------------------------------------------*/

#include "ecSTM32F4v2.h"

#define BUTTON_PIN PC_13
#define DIR_PIN PC_2
#define PWM_TIMER PA_0
#define LED_PIN PA_5
#define IR_PIN PC_3
#define MODE_OFF 0
#define MODE_MID 1
#define MODE_HIGH 2

void setup(void){
    RCC_PLL_init();
    
    GPIO_init(BUTTON_PIN, INPUT);
    GPIO_pupd(BUTTON_PIN, PULL_UP);

    GPIO_init(DIR_PIN, OUTPUT);
    GPIO_otype(DIR_PIN, PUSH_PULL);
    GPIO_write(DIR_PIN, LOW);

    PWM_init(PWM_TIMER, 1);
    PWM_duty(PWM_TIMER, 0.0);
    
    TIM_UI_init(TIM3, 500);

    GPIO_init(LED_PIN, OUTPUT);
    GPIO_write(LED_PIN, LOW);

    GPIO_init(IR_PIN, INPUT);
    GPIO_pupd(IR_PIN, PULL_UP);

    USART2_init();

    EXTI_init(BUTTON_PIN, FALL, 0);

    printf("MCU Initialized\r\n");
}

volatile int count = 0;
volatile int pause = 0;
volatile int state = 0;
volatile float duty = 0.0;
int IR = 0;

typedef struct{
    int next;
    float pwm;
    int led;
} State_t;

const State_t FSM[3] = {
    {1, 0.0f, LOW},
    {2, 0.5f, HIGH},
    {0, 1.0f, HIGH}
};

int main(void){
    setup();
    while(1){
        IR = GPIO_read(IR_PIN);
        
        if(!IR){
            if(pause){
                pause = 0;
                duty = FSM[state].pwm;
                PWM_duty(PWM_TIMER, duty);
                GPIO_write(LED_PIN, FSM[state].led);
            }
        }
        else if(IR){
            if(!pause){
                pause = 1;
                duty = 0.0f;
                PWM_duty(PWM_TIMER, 0.0f);
            }
        }
    }
}

void TIM3_IRQHandler(void){
    if(is_UIF(TIM3)){
        count ++;
        if(count >= 4){
            count = 0;
            printf("State: %d, Pause: %c, Duty: %.2f\r\n", state, pause ? 'O' : 'X', duty);
        }

        if(pause){
            led_toggle(LED_PIN);
        }

        clear_UIF(TIM3);
    }
}

void EXTI15_10_IRQHandler(void){
    if(is_pending_EXTI(BUTTON_PIN)){
        if(pause == 0){
            state = FSM[state].next;
            duty = FSM[state].pwm;

            PWM_duty(PWM_TIMER, duty);
            GPIO_write(LED_PIN, FSM[state].led);
            }
        clear_pending_EXTI(BUTTON_PIN);
    }
}