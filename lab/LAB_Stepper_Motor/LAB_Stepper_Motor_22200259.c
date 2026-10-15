/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 명금준
Created          : 05-03-2021
Modified         : 10-14-2026
Language/ver     : C++ in VS Code

Description      : Stepper Motor LAB
/----------------------------------------------------------------*/

#include "ecGPIO2.h"
#include "ecSTM32F4v2.h"

#define BUTTON_PIN PC_13
#define STEP_MODE HALF		//modify step mode, FULL or HALF
#define RPM 1				//modify RPM

// ( A, B,  AN,  BN)
PinName_t pinStepper[4]    = {PB_10,PB_4,PB_5,PB_3};


// Initialiization 
void setup(void){	
	RCC_PLL_init();                                 // System Clock = 84MHz
	SysTick_init();                                 // Systick init
	
	GPIO_init(BUTTON_PIN, INPUT);           		// GPIOC pin13 initialization
	GPIO_pupd(BUTTON_PIN, PULL_UP);
    EXTI_init(BUTTON_PIN, FALL,0);           		// External Interrupt Setting

	stepper_init(STEP_MODE, pinStepper); 				// Stepper GPIO pin initialization
	stepper_speed(RPM);                          		//  set stepper motor speed
}


int main(void) { 
	// Initialiization --------------------------------------------------------
	setup();
	uint32_t rev=1;
	uint32_t steps= (STEP_MODE == HALF ? STEP_PER_REV * 2 : STEP_PER_REV) * rev;
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
		stepper_step(steps, DIR_CW);  // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
		delay_ms(2000);
		stepper_step(steps, DIR_CCW);  // (Step : 2048, Direction : 0 or 1, Mode : FULL or HALF)
		delay_ms(2000);
	}
}

extern volatile uint32_t is_paused;

void EXTI15_10_IRQHandler(void) {  
	if (is_pending_EXTI(BUTTON_PIN)) {
		is_paused ^= 1UL;  //  			// toggle pause state
		clear_pending_EXTI(BUTTON_PIN); // cleared by writing '1'
	}
}