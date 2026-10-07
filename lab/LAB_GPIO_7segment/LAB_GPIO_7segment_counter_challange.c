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
  uint8_t numDisplay2 = 0;
  uint8_t selectFND1 = 0;
  uint8_t selectFND2 = 1;

  uint8_t count = 0;

  int button_state = 1;
  int prev_button_state = 1;

  while (1) {
    FND_select(selectFND1);
    FND_display(numDisplay1);
    delay_ms_HSI(1);

    FND_select(selectFND2);
    FND_display(numDisplay2);
    delay_ms_HSI(1);

    button_state = GPIO_read(BUTTON_PIN);

    if (prev_button_state == 1 && button_state == 0) {
      numDisplay1++;
      // if (numDisplay1 > 9) {
      //   numDisplay1 = 0;
      //   numDisplay2++;
      //   if (numDisplay2 > 1) {
      //     numDisplay2 = 0;
      //   }
      // }
      count = (count + 1) % 20;
      numDisplay1 = count % 10; // segment 0
      numDisplay2 = count / 10; // segment 1

      delay_ms_HSI(70);
    }
    prev_button_state = button_state;
  }
}
