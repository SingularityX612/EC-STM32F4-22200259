# [LAB] GPIO InOut #

**Date:** 2026-09-20

**Author:** 22200259 명금준

**Github:**

**Demo Video:**

## Introduction

## Requirement
### Hardware
* MCU: NUCLEO-F411RE
* Evaluation Board: JKIT-NUCLEO

### Software
* VS Code / PlatformIO
* Language: C/C++

## Problem 1: Create EC_HAL library
### Procedure
Prepare Library Files
Manage your library in the directory of `\repos\EC\include\`.

* **For VS.Code User:** Save your header library files in this directory. See here for detail.

* **For uVision User:** Save your header library files in this directory. See here for detail.

Download library files.

* `ecRCC2.h, ecRCC2.c`
* `ecPinNames.h, ecPinNames.c`
* `ecGPIO2_student.h, ecGPIO2_student.c`

Create your own library for GPIO by renaming the downloaded files (ecGPIO2_student.*) as

`ecGPIO2.c`, `ecGPIO2.h`

Open `ecGPIO2.c` and change from `#include "ecGPIO2_student.h"` to `#include "ecGPIO2.h"`

## Function Definitions
Fill-in the blanks in the header files to complete the definitions for the provided functions
### ecGPIO2.h
```c
void GPIO_init(PinName_t pinName, int mode);
void GPIO_write(PinName_t pinName, int Output);
int  GPIO_read(PinName_t pinName);
void GPIO_mode(PinName_t pinName, int mode);
void GPIO_ospeed(PinName_t pinName, int speed);
void GPIO_otype(PinName_t pinName, int type);
void GPIO_pupd(PinName_t pinName, int pupd);
```

#### `ecGPIO2.c` Function Implementation Details

* `GPIO_init`: Enables the peripheral clock (`RCC`) for the corresponding port and initializes the pin mode
```c
void GPIO_init(PinName_t pinName, uint32_t mode) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  // Enable peripheral clock for each port
  if (Port == GPIOA) RCC_GPIOA_enable();
  if (Port == GPIOB) RCC_GPIOB_enable();
  if (Port == GPIOC) RCC_GPIOC_enable();
  if (Port == GPIOD) RCC_GPIOD_enable();

  GPIO_mode(pinName, mode);
}
```

* `GPIO_mode`: Configures the pin operation mode (Input, Output, AF, Analog via 2-bit `MODER` control)
```c
void GPIO_mode(PinName_t pinName, uint32_t mode) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  Port->MODER &= ~(3UL << (2 * pin));
  Port->MODER |= mode << (2 * pin);
}
```

* `GPIO_ospeed`: Configures the pin output speed (Low, Medium, Fast, High via 2-bit `OSPEEDR` control)
```c
void GPIO_ospeed(PinName_t pinName, uint32_t speed) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  Port->OSPEEDR &= ~(3UL << (2 * pin));
  Port->OSPEEDR |= speed << (2 * pin);
}
```

* `GPIO_otype`: Configures the output circuit type to Push-Pull (0) or Open-Drain (1) via 1-bit `OTYPER` control
```c
void GPIO_otype(PinName_t pinName, uint32_t type) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  Port->OTYPER &= ~(1UL << pin);
  Port->OTYPER |= type << pin;
}
```

* `GPIO_pupd`: Configures internal pull-up (1) or pull-down (2) resistor connection via 2-bit `PUPDR` control
```c
void GPIO_pupd(PinName_t pinName, uint32_t pupd) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  Port->PUPDR &= ~(3UL << (2 * pin));
  Port->PUPDR |= pupd << (2 * pin);
}
```

* `GPIO_write`: Outputs a digital HIGH (1) or LOW (0) signal to the pin via 1-bit `ODR` control
```c
void GPIO_write(PinName_t pinName, uint32_t Output) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  Port->ODR &= ~(1UL << pin);
  Port->ODR |= Output << pin;
}
```

* `GPIO_read`: Reads the digital input voltage state (0 or 1) of the pin via 1-bit `IDR` extraction
```c
uint32_t GPIO_read(PinName_t pinName) {
  GPIO_TypeDef *Port;
  unsigned int pin;
  ecPinmap(pinName, &Port, &pin);

  return (Port->IDR >> pin) & 1UL;
}
```

## Problem 2: Toggle a single LED with Digital Sensor(Photodetector)
### Problem
Toggle the LED by covering the photodetector sensor.

* Dark (LED ON), Bright (LED OFF) and repeat

* Connect the Photodetector sensor to PA_0 pin on Nucleo Board.

### Procedure
1. Connect the evaluation board (JKIT-NUCLEO) to the MCU.
2. Make sure that your library ecGPIO2.h, ecGPIO2.c are in `EC\include\`.
3. Create a new project under the directory `EC\lab\`
* Environment: “LAB_GPIO_DIO_LED_Photosensor”.
* Source File: “LAB_GPIO_DIO_LED_Photosensor.c”

4. You must modify the `platformio.ini`, to add new environment.

### Configuration

| Function | Port - Pin | Configuration |
| :--- | :--- | :--- |
| **Photodetector on JKIT** | PA_0 | Digital IN, Pull-Up |
| **LED0** | PB_12 | Digital OUT, Pull-Up, Open-Drain, Medium Speed |

## Algorithm

### Overview
| Ambient Light Condition | Sensor Voltage (PA_0) | buttonState Value | Output Action (PB_12) |
| :--- | :--- | :--- | :--- |
| **Bright** | HIGH (Vdd) | `1` | `LOW` (LED OFF) |
| **Dark** | LOW (0V) | `0` | `HIGH` (LED ON) |

### Description with Code
* Lab source code: 

* Description 1: Initialize pins according to the configuration
```c
void setup(void) {
	RCC_HSI_init();
	// initialize the Photodetector as an input:
	GPIO_init(Photodetector, INPUT);		//Digital IN
	GPIO_pupd(Photodetector, 1);			//Pull_Up 
	// initialize the LED pin as an output:
	GPIO_init(LED_PIN, OUTPUT);				//Digital OUT
	GPIO_otype(LED_PIN, 1); 				//Open-Drain
	GPIO_pupd(LED_PIN, 1); 					//Pull-Up
	GPIO_ospeed(LED_PIN, 1); 				//Medium Speed
}
```
* Description 2: Read Photodetector state and toggle LED state
```c

int DetectorState=0;
	
while(1){
		DetectorState = GPIO_read(Photodetector);       //read Photodetector state
		if(DetectorState) 	GPIO_write(LED_PIN, LOW);   //Bright -> LED OFF
		else 				GPIO_write(LED_PIN, HIGH);  //Dark -> LED ON
		delay_ms_HSI(100);             				    // delay 100 ms
}
```
## Problem 3: Toggle multiple LEDs with a button
### Problem
Use the four LEDs (LED0 to LED3) on the JKIT evaluation board.

Each time Button B1 is pressed, turn on one LED at a time in a sequential loop

* e.g. (LED0 -> LED1 -> LED2 -> LED3 -> LED0)

### Procedure

Connect the evaluation board (JKIT-NUCLEO) to the MCU.

Make sure that your library ecGPIO2.h, ecGPIO2.c are in `EC\include\`.

Create a new project under the directory `EC\lab\`

* Environment: “LAB_GPIO_DIO_multiLED”

* Source File: ““LAB_GPIO_DIO_multiLED.c”

You must modify the `platformio.ini`, to add new environment.

### Configuration

| Function | Port - Pin | Configuration |
| :--- | :--- | :--- |
| **SW2 on JKIT** | PA_4 | Digital IN, Pull-Up |
| **LED_0~LED_3** | PB_12, PB_13, PB_14, PB_15 | Digital OUT, Pull-Up, Open-Drain, Medium Speed |

## Algorithm

### Overview
#### State Transition Table (Moore FSM)
| Current State ($S_t$) | Input Event (Button Press) | Next State ($S_{t+1}$) | Output (LED3, LED2, LED1, LED0) | Active LED |
| :---: | :---: | :---: | :---: | :---: |
| **State 0** | Falling Edge (`1 -> 0`)<br>No Press (Others) | **State 1**<br>**State 0** | `0, 0, 0, 1` | LED0 ON |
| **State 1** | Falling Edge (`1 -> 0`)<br>No Press (Others) | **State 2**<br>**State 1** | `0, 0, 1, 0` | LED1 ON |
| **State 2** | Falling Edge (`1 -> 0`)<br>No Press (Others) | **State 3**<br>**State 2** | `0, 1, 0, 0` | LED2 ON |
| **State 3** | Falling Edge (`1 -> 0`)<br>No Press (Others) | **State 0**<br>**State 3** | `1, 0, 0, 0` | LED3 ON |

### Description with Code
* Lab source code:

* Description 1: LED Pin Array Grouping & Initialization

	Grouped four LED pins into a single array (`LEDs[4]`) to:
	* Eliminate code redundancy by initializing all pins using a single `for` loop.
  * Directly map the FSM state variable (`LED_State` = 0, 1, 2, 3) to the array index for sequential control.
```c
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
```
