/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259_명금준
Created          : 05-03-2021
Modified         : 09-16-2026
Language/ver     : C++ in VS Code

Description      : [WRITE BRIEF DESCRIPTION] !!!!!!
/----------------------------------------------------------------*/

#include "ecPinNames.h"
#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED0_PIN PB_12
#define LED1_PIN PB_13
#define LED2_PIN PB_14
#define LED3_PIN PB_15

#define B1 PA_4

// Initialiization
PinName_t LEDs[4] = {LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN};

void setup(void) {
  RCC_HSI_init();

  // initialize the pushbutton pin as an input:
  GPIO_init(B1, INPUT);           //Digital IN
  GPIO_pupd(B1, 1);               //Pull-Up

  // initialize the LED pins as outputs:
  for (int i = 0; i < 4; i++) {
    GPIO_init(LEDs[i], OUTPUT);   //Digital OUT
    GPIO_otype(LEDs[i], 1);       //Open-Drain
    GPIO_pupd(LEDs[i], 1);        //Pull-Up
    GPIO_ospeed(LEDs[i], 1);      //Medium Speed
  }
  GPIO_write(LED0_PIN, HIGH);     //LED0 ON
}
int main(void) {
  setup();
  int buttonState = 1;
  int prevButtonState = 1;
  int LED_State = 0;

  while (1) {
    buttonState = GPIO_read(B1);
    if (prevButtonState == 1 && buttonState == 0) {
      LED_State = (LED_State + 1) % 4;
      for (int i = 0; i < 4; i++) {
        GPIO_write(LEDs[i], (i == LED_State));
      }
      delay_ms_HSI(50);
    }
    prevButtonState = buttonState;
  }
}
