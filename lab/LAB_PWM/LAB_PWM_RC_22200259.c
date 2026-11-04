/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 Keumjoon Myeong
Created          : 05-03-2021
Modified         : 11-02-2026
Language/ver     : C++ in VS Code

Description      : Stepper Motor LAB
/----------------------------------------------------------------*/

#include "ecSTM32F4v2.h"

#define BUTTON_PIN PC_13
#define TIM2_CH2 PA_1
#define PULSEWIDTH 500      //usec

volatile int angle = 0;
volatile int dir = 1;

void setup(void){
    RCC_PLL_init();

    GPIO_init(BUTTON_PIN, INPUT);
    GPIO_pupd(BUTTON_PIN, PULL_UP);

    PWM_init(TIM2_CH2, 20);
    PWM_pulsewidth_us(TIM2_CH2, PULSEWIDTH);

    TIM_UI_init(TIM3, 500);

    EXTI_init(BUTTON_PIN, FALL, 0);
}

int main(void){
    setup();
    while(1){}
    
}

void TIM3_IRQHandler(void){
    if(is_UIF(TIM3)){
        angle += dir;
        if(angle >= 18){
            dir = -1;
        }
        else if(angle <= 0){
            dir = 1;
        }
        PWM_pulsewidth_us(TIM2_CH2, PULSEWIDTH+(2000 * angle)/18);
        clear_UIF(TIM3);
    }
}

void EXTI15_10_IRQHandler(void){
    if(is_pending_EXTI(BUTTON_PIN)){
        angle = 0;
        dir = 1;
        PWM_pulsewidth_us(TIM2_CH2, PULSEWIDTH);
        clear_pending_EXTI(BUTTON_PIN);
    }
}