/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259_명금준
Created          : 05-03-2021
Modified         : 09-14-2026
Language/ver     : C++ in VS Code

Description      : GPIO Library. Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/

#include "ecGPIO2.h"
// #include "ecGPIO2.h"

/*----------------------------------------------------------------
                                [EXERCISE]
---------------------------------------------------------------- */
void GPIO_init(PinName_t pinName, uint32_t mode) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  // mode  : Input(0), Output(1), AlterFunc(2), Analog(3)
  if (Port == GPIOA)
    RCC_GPIOA_enable();
  if (Port == GPIOB)
    RCC_GPIOB_enable();

  // Make it for GPIOC, GPIOD..GPIOH
  // [TO-DO] YOUR CODE GOES HERE !!!
  if (Port == GPIOC)
    RCC_GPIOC_enable();
  if (Port == GPIOD)
    RCC_GPIOD_enable();
  // You can also make and use a more general function of
  // void RCC_GPIO_enable(GPIO_TypeDef *Port);

  GPIO_mode(pinName, mode);
}

// GPIO Mode          : Input(00), Output(01), AlterFunc(10), Analog(11)
void GPIO_mode(PinName_t pinName, uint32_t mode) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  Port->MODER &= ~(3UL << (2 * pin));
  Port->MODER |= mode << (2 * pin);
}

// GPIO Speed          : Low speed (00), Medium speed (01), Fast speed (10),
// High speed (11)
void GPIO_ospeed(PinName_t pinName, uint32_t speed) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  //[TO-DO] YOUR CODE GOES HERE
  Port->OSPEEDR &= ~(3UL << (2 * pin));
  Port->OSPEEDR |= speed << (2 * pin);
  //[TO-DO] YOUR CODE GOES HERE
}

// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
void GPIO_otype(PinName_t pinName, uint32_t type) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  //[TO-DO] YOUR CODE GOES HERE
  Port->OTYPER &= ~(1UL << pin);
  Port->OTYPER |= type << pin;
  //[TO-DO] YOUR CODE GOES HERE
}

// GPIO Push-Pull    : No pull-up, pull-down (00), Pull-up (01), Pull-down (10),
// Reserved (11)
void GPIO_pupd(PinName_t pinName, uint32_t pupd) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  //[TO-DO] YOUR CODE GOES HERE
  Port->PUPDR &= ~(3UL << (2 * pin));
  Port->PUPDR |= pupd << (2 * pin);
  //[TO-DO] YOUR CODE GOES HERE
}

void GPIO_write(PinName_t pinName, uint32_t Output) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  //[TO-DO] YOUR CODE GOES HERE
  Port->ODR &= ~(1UL << pin);
  Port->ODR |= Output << pin;
  //[TO-DO] YOUR CODE GOES HERE
}

uint32_t GPIO_read(PinName_t pinName) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  //[TO-DO] YOUR CODE GOES HERE
  return (Port->IDR >> pin) & 1UL;
  //[TO-DO] YOUR CODE GOES HERE
}

// FND - JKIT board
static PinName_t _fndPins[8];
static PinName_t _fndSel[4];

// Each led that has to light up gets a 1, every other led gets a 0
// its in order  { (a,b,c,d,e,f,g,dp) }
// 인덱스 직접 매핑(O(1)): numberFND[num][segment]
int numberFND[12][8] = {
    {1, 1, 1, 1, 1, 1, 0, 0}, // 0(zero): a, b, c, d, e, f
    {0, 1, 1, 0, 0, 0, 0, 0}, // 1(one): b, c
    {1, 1, 0, 1, 1, 0, 1, 0}, // 2(two): a, b, d, e, g
    {1, 1, 1, 1, 0, 0, 1, 0}, // 3(three): a, b, c, d, g
    {0, 1, 1, 0, 0, 1, 1, 0}, // 4(four): b, c, f, g
    {1, 0, 1, 1, 0, 1, 1, 0}, // 5(five): a, c, d, f, g
    {1, 0, 1, 1, 1, 1, 1, 0}, // 6(six): a, c, d, e, f, g
    {1, 1, 1, 0, 0, 0, 0, 0}, // 7(seven): a, b, c
    {1, 1, 1, 1, 1, 1, 1, 0}, // 8(eight): a, b, c, d, e, f, g
    {1, 1, 1, 1, 0, 1, 1, 0}, // 9(nine): a, b, c, d, f, g
    {0, 0, 0, 0, 0, 0, 0, 1}, // 10(dot): dp
    {0, 0, 0, 0, 0, 0, 0, 0}  // 11(blank): all OFF
};

// Initialize DOUT pins for 7 segment leds
void FND_init(PinName_t *pinFND) {
  for (int i = 0; i < 8; i++) {
    _fndPins[i] = pinFND[i];
    GPIO_init(_fndPins[i], OUTPUT);
  }
}

// Initialize select pins (FND0~FND3) as DOUT
void FND_select_init(PinName_t *selectFND) {
  // [YOUR CODE GOES HERE]
  for (int i = 0; i < 4; i++) {
    _fndSel[i] = selectFND[i];
    GPIO_init(_fndSel[i], OUTPUT);
    GPIO_otype(_fndSel[i], 0x00);
  }
  // [YOUR CODE GOES HERE]
}

// Display a number 0 - 9 only
void FND_display(uint8_t num) {
  // [YOUR CODE GOES HERE]
  for (int i = 0; i < 8; i++) {
    GPIO_write(_fndPins[i], numberFND[num][i]);
  }
  // [YOUR CODE GOES HERE]
}

// Select display: FND0 to FND3
void FND_select(uint8_t digit) {
  // [YOUR CODE GOES HERE]
  for (int i = 0; i < 4; i++) {
    GPIO_write(_fndSel[i], (i == digit) ? HIGH : LOW);
  }
  // [YOUR CODE GOES HERE]
}

////////////////////////////////////////////////////////////////////

/*----------------------------------------------------------------
                                [Optional EXERCISE]
---------------------------------------------------------------- */
// void pinMode(PinName_t pin, uint32_t mode){}
// void digitalWrite(PinName_t pin, uint32_t value){}
// uint32_t  digitalRead(PinName_t pin){}