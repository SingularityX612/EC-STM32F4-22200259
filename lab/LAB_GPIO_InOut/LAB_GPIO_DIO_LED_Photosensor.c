/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259_명금준
Created          : 05-03-2021
Modified         : 09-16-2026
Language/ver     : C++ in VS Code

Description      : [WRITE BRIEF DESCRIPTION] !!!!!!
/----------------------------------------------------------------*/

#include "ecRCC2.h"
#include "ecGPIO2.h"

#define LED_PIN PB_12
#define Photodetector PA_0

// Initialiization 
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
	
int main(void) { 
 	setup();
	int DetectorState=0;
	
	while(1){
		DetectorState = GPIO_read(Photodetector);       //read Photodetector state
		if(DetectorState) 	GPIO_write(LED_PIN, LOW);   //Bright -> LED OFF
		else 				GPIO_write(LED_PIN, HIGH);  //Dark -> LED ON
		delay_ms_HSI(100);             				    // delay 100 ms
	}
}
