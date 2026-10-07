/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259_명금준
Created          : 05-03-2021
Modified         : 09-21-2026
Language/ver     : C++ in VS Code

Description      : [WRITE BRIEF DESCRIPTION] !!!!!!
/----------------------------------------------------------------*/
// #include "ecGPIO2.h"
// #include "ecRCC2.h"
// #include "stm32f4xx.h"

#include "ecSTM32F4v2.h"

// [YOUR CODE GOES HERE]

#define BUTTON_PIN PA_4

void setup(void) {
  // Intialize System Clock
  RCC_HSI_init();
  GPIO_init(BUTTON_PIN, INPUT);
  GPIO_pupd(BUTTON_PIN, 1);
  // Intialize FND pins and Others
  FND_init(pinFND);
  FND_select_init(selectFND);
  // Intialize  Others
  // [YOUR CODE GOES HERE]
};

int main(void) {
  setup();
  uint8_t numDisplay1 = 0;
  uint8_t selectFND1 = 0;

  int button_state = 1;
  int prev_button_state = 1;

  while (1) {
    FND_select(selectFND1);
    FND_display(numDisplay1);
    delay_ms_HSI(1);

    button_state = GPIO_read(BUTTON_PIN);

    if (prev_button_state == 1 && button_state == 0) {
      numDisplay1 = (numDisplay1 + 1) % 10;
      delay_ms_HSI(50);
    }
    prev_button_state = button_state;
  }
}
