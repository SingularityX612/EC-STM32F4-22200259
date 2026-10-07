/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 명금준
Created          : 05-03-2021
Modified         : 09-28-2026
Language/ver     : C++ in VS Code

Description      : Tutorial - [Your Description GOES HERE !!]
/----------------------------------------------------------------*/

#include "ecSTM32F4v2.h"

#define LED_JK PB_12 // LED for Eval Board	JKIT

void setup(void) {
  RCC_PLL_init(); // System Clock = 84MHz
  SysTick_init();
  GPIO_init(LED_JK, OUTPUT); // LED for Eval Board	JKIT
}

int main(void) {
  setup();
  while (1) {
    GPIO_write(LED_JK, HIGH);
    delay_ms(1000);
    GPIO_write(LED_JK, LOW);
    delay_ms(1000);
  }
}
