/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : [22200259-KimGyeongsik]
Created          : 05-03-2021
Modified         : 10-01-2026
Language/ver     : C++ in VS Code

Description      : [LAB] EXTI-Counter_0-60(challange)
/----------------------------------------------------------------*/

#include "ecSTM32F4v2.h"

#define SW1_PIN PD_2 // EVAL board JKIT SW1
#define SW2_PIN PA_4 // EVAL board JKIT SW2

// void led_toggle(PinName_t pinName);  // in ecGPIO2.h

// Initialiization
void setup(void) {
  RCC_PLL_init(); // System Clock = 84MHz
  SysTick_init(); // SysTick Timer Initialization

  // Initialize Input Buttons
  GPIO_init(SW1_PIN, INPUT);
  GPIO_pupd(SW1_PIN, PULL_UP);
  GPIO_init(SW2_PIN, INPUT);   // INPUT for EVAL board
  GPIO_pupd(SW2_PIN, PULL_UP); // PULL-UP for EVAL board

  FND_init(pinFND);
  FND_select_init(selectFND);

  // Initialize EXTI PA_4
  EXTI_init(SW1_PIN, FALL, 10);
  EXTI_init(SW2_PIN, FALL, 10);
}

uint8_t numDisplay1 = 0;
uint8_t numDisplay2 = 0;
uint8_t selectFND1 = 0;
uint8_t selectFND2 = 1;
uint16_t count_stack = 0;
volatile uint8_t count = 0;
volatile uint8_t Pause = 0;

// MAIN  ----------------------------------------
int main(void) {
  setup();

  while (1) {
    FND_select(selectFND1);
    FND_display(numDisplay1);
    delay_ms(2);

    FND_select(selectFND2);
    FND_display(numDisplay2);
    delay_ms(2);

    if (!Pause) {
      count_stack += 4;
    }

    if (count_stack >= 1000) {
      count = (count + 1) % 60;
      numDisplay1 = count % 10;
      numDisplay2 = count / 10;
      count_stack = 0;
    }
  }
}

void EXTI2_IRQHandler(void) {
  if (is_pending_EXTI(SW1_PIN)) {
    count = 0;
    count_stack = 0;
    numDisplay1 = 0;
    numDisplay2 = 0;
    clear_pending_EXTI(SW1_PIN);
  }
}

void EXTI4_IRQHandler(void) {
  if (is_pending_EXTI(SW2_PIN)) {
    Pause ^= 1UL;
    clear_pending_EXTI(SW2_PIN);
  }
}
