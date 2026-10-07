/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : [ YOUR NAME GOES HERE !!!!!]
Created          : 05-03-2021
Modified         : 00-00-2026 [WRITE THE DATE!!!!]
Language/ver     : C++ in VS Code

Description      : GPIO Library. Distributed to Students for LAB_GPIO
/----------------------------------------------------------------*/

#ifndef __ECGPIO2_H
#define __ECGPIO2_H

#include "ecPinNames.h"
#include "ecRCC2.h"
#include "stm32f411xe.h"

#define INPUT 0x00
#define OUTPUT 0x01
#define AF 0x02
#define ANALOG 0x03

#define HIGH 1
#define LOW 0

#define NO_PUPD 0x00UL
#define PULL_UP 0x01UL
#define PULL_DOWN 0x02UL

#define PUSH_PULL 0x00UL
#define OPEN_DRAIN 0x01UL

#define LOW_SPEED 0x00UL
#define MEDIUM_SPEED 0x01UL
#define FAST_SPEED 0x02UL
#define HIGH_SPEED 0x03UL

/*----------------------------------------------------------------
                    [EXERCISE]
---------------------------------------------------------------- */
// ADD MORE  MACRO related to GPIO
// PU, PD, NO_PUPD, PUSH_PULL, LOW, MEDIUM, etc

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void GPIO_init(PinName_t pinName, uint32_t mode);
void GPIO_mode(PinName_t pinName, uint32_t mode);
void GPIO_ospeed(PinName_t pinName, uint32_t speed);
void GPIO_otype(PinName_t pinName, uint32_t type);
void GPIO_pupd(PinName_t pinName, uint32_t pupd);
void GPIO_write(PinName_t pinName, uint32_t Output);
uint32_t GPIO_read(PinName_t pinName);

// ARDUINO Syntax  (Optional)
void pinMode(PinName_t pin, uint32_t mode);
void digitalWrite(PinName_t pin, uint32_t value);
uint32_t digitalRead(PinName_t pin);

// FND for JKIT board
extern int numberFND[12][8];
void FND_init(PinName_t *pinFND);
void FND_select_init(PinName_t *selectFND);
void FND_display(uint8_t num);
void FND_select(uint8_t digit);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __ECGPIO2_H
