/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 명금준
Created          : 05-03-2021
Modified         : 10-14-2026
Language/ver     : C++ in VS Code

Description      : SysTick Library. Distributed to Students for LAB_SYSTICK
/----------------------------------------------------------------*/


// #include "ecSysTick2.h"
#include "ecSysTick2.h"


volatile uint32_t msTicks;

// Initialize 1 ms tick of SysTick timer. 
// Must call AFTER RCC_HSI_init() (16 MHz) or RCC_PLL_init() (84 MHz)
// Updates  EC_SYSCLK either 84 MHz or 16 MHz
void SysTick_init(void){
	//  SysTick Control and Status Register
	// Disable SysTick IRQ and SysTick Counter
	SysTick->CTRL = 0;											
	
	// Select processor clock
	// 1 = processor clock;  0 = external clock
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;

	// SysTick Reload Value Register : (SYSCLK / 1000) - 1  ->  1 ms
	//   HSI 16 MHz :  15999      PLL 84 MHz : 83999 
	SysTick->LOAD = (uint32_t)EC_SYSCLK *1UL/ 1000UL - 1UL;   // 1[ms] = 1/1000[s] 

	// SysTick Current Value Register : Reset counter value to 0
	SysTick->VAL = 0;

	// Enables SysTick exception request
	// 1 = counting down to zero --> SysTick exception request
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	
	// Enable SysTick IRQ and SysTick Timer
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		
	// Enable interrupt in NVIC
	NVIC_SetPriority(SysTick_IRQn, 15);		// Set Priority to 1
	NVIC_EnableIRQ(SysTick_IRQn);			// Enable interrupt in NVIC
}


// SysTick_Handler() is called every 1 ms by the SysTick IRQ
void SysTick_Handler(void){
	SysTick_counter();	
}

// Increment msTicks every 1 ms
void SysTick_counter(void){
	msTicks++;
}	

// Blocking delay in milliseconds
void delay_ms (uint32_t msec){
  	uint32_t curTicks;
  	curTicks = msTicks;
	while ((msTicks - curTicks) < msec){;}		// unsigned math: safe across wrap-around
}

void delay_us(uint32_t usec) {
    if (usec == 0) return;

    // 1µs당 84클럭 (84MHz 기준: 84 * usec)
    uint32_t total_ticks = usec * (EC_SYSCLK / 1000000UL);
    uint32_t elapsed_ticks = 0;
    uint32_t old_val = SysTick->VAL;

    while (elapsed_ticks < total_ticks) {
        uint32_t cur_val = SysTick->VAL;

        // SysTick은 다운카운터(숫자가 줄어듦)
        if (cur_val <= old_val) {
            elapsed_ticks += (old_val - cur_val);
        } else {
            // 카운터가 0에 도달하여 SysTick->LOAD 값으로 리로드(Wrap-around)된 경우
            elapsed_ticks += (old_val + (SysTick->LOAD + 1 - cur_val));
        }
        old_val = cur_val;
    }
}


// Reset the SysTick counter to 0
void SysTick_reset(void)
{
	SysTick->VAL = 0;
}

// Return the current value of the SysTick counter
uint32_t SysTick_val(void) {
	return SysTick->VAL;
}



