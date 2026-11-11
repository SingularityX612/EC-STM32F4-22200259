/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 Keumjoon Myeong
Created          : 10-16-2026
Modified         : 11-10-2026
Language/ver     : C in VS Code

Description      : LAB_TIMER_ICAP
/----------------------------------------------------------------*/

#include "ecSTM32F4v2.h"

#define ECHO_PIN    PB_6        // TIM4_CH1 : capture input
#define TRIG_PIN    PA_6        // TIM3_CH1 : test signal (jumper PA_6 -> PB_6)

uint32_t pwm_period_ms=100;
uint32_t uTick=10;
uint32_t timer_arr  = 65536;    // ticks per counter wrap (ARR+1)

volatile uint32_t ovf_cnt = 0;      //counts overflows
volatile uint32_t cap_prev   = 0;   // CCR of the previous rising edge
volatile uint32_t cap_curr    = 0;  // CCR of the current rising edge
volatile float timeInterval = 0;    // [usec]
volatile float distance = 0;


void setup(void){
    RCC_PLL_init();
    SysTick_init();

    //USART2_init();                    
    USART_init(USART2, PA_2, PA_3, 9600);

    // PWM: Test signal of period_ms of 50% duty 
    PWM_init(TRIG_PIN, pwm_period_ms);
    PWM_pulsewidth_us(TRIG_PIN, 10);

    // Input capture  rising edge TIn=ICn, tick = uTick usec    
    ICAP_init(ECHO_PIN, IC_RISE);
    ICAP_ICn_init(ECHO_PIN, IC_FALL, IC_2);     //TI1 -> IC2, falling
    ICAP_tick(ECHO_PIN, uTick);         

    timer_arr=TIM4->ARR + 1;

    printf("MCU INITIALIZED \r\n");
}

int main(void){
    setup();

    while(1){
        printf("distance = %.2f cm\r\n", distance);  

        delay_ms(500);
    }
}


void TIM4_IRQHandler(void){
    // Counter overflow 
    if(is_UIF(TIM4)){
        ovf_cnt++;
        clear_UIF(TIM4);
    }

    // Rising edge captured on IC1
    if(is_CCIF(TIM4, IC_1)){                    // IC1 : rising edge, SAME pin (TI1)
        ovf_cnt = 0;
        cap_prev = ICAP_read(ECHO_PIN, IC_1);
        clear_CCIF(TIM4,IC_1);                                
    }
    // Falling edge captured on IC2
    if(is_CCIF(TIM4, IC_2)){                // IC2 : falling edge, SAME pin (TI1)       
        cap_curr = ICAP_read(ECHO_PIN, IC_2);

        // Ticks between the two edges, then scaled to usec.        
        distance = ((cap_curr-cap_prev) + (ovf_cnt*timer_arr))*uTick * 0.017f;

        ovf_cnt    = 0; 
        clear_CCIF(TIM4, IC_2);
    }   
}