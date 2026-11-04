/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : Keumjoon Myeong
Student ID       : 22200259
Created          : 10-22-2026 
Language/ver     : C in VS Code

Description      : EC Test 1 (2026) Programming Test
                   
                   Rename this file to  EC_Test1_yourID.c
/----------------------------------------------------------------*/


#include "ecSTM32F4v2.h"

//==================================================================
//  EC Library Include
//==================================================================
// YOUR CODE GOES HERE
// YOUR CODE GOES HERE


//==================================================================
// User GPIO Pin definitions
//==================================================================
#define SW1 PD_2
#define SW2 PA_4
#define Photodetector PA_0
#define LED0 PB_12
#define STEP_MODE HALF		//modify step mode, FULL or HALF

// Stepper ( A, B, AN, BN )
// YOUR CODE GOES HERE
PinName_t pinStepper[4] = {PC_5, PC_6, PC_7, PC_8};

// FND segments
// YOUR CODE GOES HERE

// FND digit select : FND_0 ~ FND_3
// YOUR CODE GOES HERE

// Global Variables
// use   volatile uint32_t   instead of uint32_t if used in Interrupt ISR
volatile uint32_t motor_mode   = 0;	
volatile uint32_t motor_dir    = 0;	
// volatile uint32_t RPM  = 0;	
uint8_t DetectorState=0;
uint8_t selectFND1 = 0;
volatile uint8_t count = 0;

static volatile uint8_t dir_cmd = 0;
uint8_t tckcount = 0;
uint8_t ledstate = 1;
//==================================================================
//  Setup 
//==================================================================
void setup(void){
	RCC_PLL_init();					// System Clock = 84MHz
	SysTick_init();					// SysTick, 1 msec
	
	// USART2, 9600 baud
    USART2_init();
    USART2_baud(BAUD_9600);	
	USART_pins(USART2, PA_2, PA_3);	

	// GPIO Inputs
	GPIO_init(SW1, INPUT);
	GPIO_pupd(SW1, PULL_UP);
	GPIO_init(SW2, INPUT);
	GPIO_pupd(SW2, PULL_UP);
	GPIO_init(Photodetector, INPUT);		//Digital IN
	GPIO_pupd(Photodetector, PULL_UP);
	// EXTI 
  	EXTI_init(SW1, FALL, 10);
 	EXTI_init(SW2, FALL, 10);
	
	// GPIO Outputs	
	GPIO_init(LED0, OUTPUT);

	// Other Setup
	FND_init(pinFND);
  	FND_select_init(selectFND);
	stepper_init(FULL, pinStepper); 				// Stepper GPIO pin initialization
	// stepper_stop();    
}


//==================================================================
//  Main
//==================================================================
int main(void){
	setup();
	printf("MCU Initialized\r\n");
	// YOUR CODE GOES HERE
	// uint32_t steps= STEP_PER_REV;
	GPIO_write(LED0, ledstate);
	// stepper_speed(RPM);
	// uint32_t steps= (STEP_MODE == HALF ? STEP_PER_REV * 2 : STEP_PER_REV) * 1UL;

	while(1){
	// stepper_step(steps, motor_dir);  // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)

	if(motor_mode == 0){
		stepper_speed(0);
	}
	else if(motor_mode == 1){
		stepper_speed(2);
	}
	else if(motor_mode == 2){
		stepper_speed(6);
	}
	else if(motor_mode == 3){
		stepper_speed(10);
	}
	// 	// Check PAUSE/RESUME 
		// DetectorState = GPIO_read(Photodetector);       //read Photodetector state
		// if(DetectorState){
		// 	is_paused = 0;
		// 	tckcount = 0;
		// }
		// else{
		// 	is_paused = 1;
		// 	tckcount++;	
		// 	if(tckcount == 2000){
		// 		ledstate ^= 1UL;
		// 		tckcount -= 2000;
		// 	}	
		// }
		// delay_ms(100);             				    // delay 100 ms

		

	    if(dir_cmd == 'C'){
            dir_cmd = 0;
            printf("Rotate Motor CW\r\n");
			motor_dir = 0;
			// stepper_step(1, DIR_CW);
        }
        else if(dir_cmd == 'B'){
            dir_cmd = 0;
            printf("Rotate Motor CCW\r\n");
			// stepper_step(1, DIR_CCW);
			motor_dir = 1;
        }
	// 	// Related to controlling motor speed/dir etc
	// 	// HINT:  use  stepper_speed(vel);	only when vel>0
		stepper_step(1, motor_dir);	 // Keep step=1 or a low value to stop blocking other code
		FND_select(selectFND1);
    	FND_display(motor_mode);


	// 	// Print to the serial monitor every 2 sec
	// 	// PAUSE, blink at 2 sec
	// 	// printf("Mode=%lu Speed=%lu DIR=%lu\r\n",);
	// 	// HINT:  msTicks in ecSysTick2.h  is increasing at a 1 msec rate accurately.
	// 	// You can use msTicks  to count every 2000 msec 
	// 	// YOUR CODE GOES HERE
	// 	// YOUR CODE GOES HERE

	
	
	// delay_ms(1);  // use this delay
	}
}


//==================================================================
//  EXTI Interrupt Service Routines
//==================================================================

void EXTI2_IRQHandler(void) {
  if (is_pending_EXTI(SW1) && motor_mode < 3) {
	motor_mode++;
    clear_pending_EXTI(SW1);
  }
  else{
	clear_pending_EXTI(SW1);
  }
}

void EXTI4_IRQHandler(void) {
  if (is_pending_EXTI(SW2) && motor_mode > 0) {
    motor_mode--;
	clear_pending_EXTI(SW2);
  }
    else{
	clear_pending_EXTI(SW2);
  }
}


//==================================================================
//  UART2 Interrupt Service Routines
//==================================================================
// void USART2_IRQHandler(void){
// 	if(is_USART2_RXNE()){
	
// 	// HINT: 	comparing a character:  
// 	//  		uint8_t ch = USART2_read();		
// 	//			if(ch == 'C' || ch == 'c')	 etc..
// 	//
// 	// YOUR CODE GOES HERE
// 	// YOUR CODE GOES HERE
// 	}
// }
void USART2_IRQHandler(){
    if(is_USART2_RXNE()){
        uint8_t dir_data = USART2_read();

        USART2_write(&dir_data, 1);  // Echo to USB Tera Term

		if(dir_data == 'C' || dir_data == 'B'){
            stepper_stop();
            dir_cmd = dir_data;
        }
    }
}