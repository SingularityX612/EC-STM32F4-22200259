/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 Keumjoon Myeong
Created          : 10-08-2026
Modified         : 10-15-2026
Language/ver     : C in VS Code

Description      : LAB-USART-Bluetooth
/----------------------------------------------------------------*/

#include "ecGPIO2.h"
#include "ecSTM32F4v2.h"  // this should include #include "ecUART2.h"

#define STEP_MODE HALF		//modify step mode, FULL or HALF
#define RPM 15				//modify RPM

static volatile uint8_t bt_cmd = 0;

// ( A, B,  AN,  BN)
PinName_t pinStepper[4]    = {PB_10,PB_4,PB_5,PB_3};

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(PA_5, OUTPUT);
    GPIO_otype(PA_5, PUSH_PULL);
    GPIO_pupd(PA_5, NO_PUPD);
    GPIO_ospeed(PA_5, HIGH_SPEED);
    GPIO_write(PA_5, LOW);

	stepper_init(STEP_MODE, pinStepper); 				// Stepper GPIO pin initialization
	stepper_speed(RPM);                          		//  set stepper motor speed

    // USART2: USB serial init
    USART2_init();
    USART2_baud(BAUD_9600);
    // USART1: MCU2 serial init
    USART1_init();
    USART1_baud(BAUD_9600);
}

int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	printf("MCU Initialized\r\n");
	USART1_write((uint8_t*)"Bluetooth Initialized\r\n", 23);

	uint32_t rev=2;
	uint32_t steps= (STEP_MODE == HALF ? STEP_PER_REV * 2 : STEP_PER_REV) * rev;

    while(1){
        if(bt_cmd == 'J'){
            bt_cmd = 0;
            printf("Rotate Motor CW 2 Rev\r\n");
            stepper_step(steps, DIR_CW);
        }
        else if(bt_cmd == 'K'){
            bt_cmd = 0;
            printf("Rotate Motor CCW 2 Rev\r\n");
            stepper_step(steps, DIR_CCW);
        }
    }
}

void USART1_IRQHandler(){
    if(is_USART1_RXNE()){
        uint8_t bt_data = USART1_read();

        USART1_write(&bt_data, 1);  // Echo to Bluetooth Tera Term
        USART2_write(&bt_data, 1);  // Echo to USB Tera Term

        if(bt_data == 'H') {
            GPIO_write(PA_5, HIGH);
            printf("\r\nLED ON\r\n");
        }
        else if(bt_data == 'L'){
            GPIO_write(PA_5, LOW);
            printf("\r\nLED OFF\r\n");
        }
        else if(bt_data == 'P'){
            stepper_stop();
            bt_cmd = 0;
            printf("\r\nMotor Stopped\r\n");
        }
        else if(bt_data == 'J' || bt_data == 'K'){
            stepper_stop();
            bt_cmd = bt_data;
        }
    }
}
void USART2_IRQHandler(void){
    if(is_USART2_RXNE()){
        uint8_t pc_data = USART2_read();
        USART2_write(&pc_data, 1);  // Echo USB typing to check USB connection
    }
}
