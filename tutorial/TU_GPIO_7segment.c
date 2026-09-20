#include "ecRCC2.h"
#include "ecGPIO2.h"
#include "ecPinNames.h"

#define ledA PA_8
#define ledB PB_10
#define ledC PA_7
#define ledD PA_6
#define ledE PA_5
#define ledF PC_7
#define ledG PA_9

PinName_t LEDs[7] = {ledA, ledB, ledC, ledD, ledE, ledF, ledG};


void setup(void);
	
int main(void) {	
	// Initialiization --------------------------------------------------------
	setup();
	
	// Inifinite Loop ----------------------------------------------------------
	while(1){
        for(int i = 0; i < 7; i++){
            GPIO_write(LEDs[i], LOW);
        }
	}
}

void setup(void){
	RCC_HSI_init();
    for(int i = 0; i < 7; i++){
        GPIO_init(LEDs[i], OUTPUT);
    }
}
