/*----------------------------------------------------------------\
@ Embedded Controller by Young-Keun Kim - Handong Global University
Author           : 22200259 Keumjoon Myeong
Created          : 10-08-2026
Modified         : 10-15-2026
Language/ver     : C in VS Code

Description      : LAB-USART-LED
/----------------------------------------------------------------*/
#include "ecSTM32F4v2.h"  // this should include #include "ecUART2.h"

#define MAX_BUF 32
#define END_CHAR 13

static volatile uint8_t buffer[MAX_BUF]={0, };
static volatile uint8_t rx_buffer[MAX_BUF]={0, };
static volatile uint8_t PC_string[MAX_BUF]={0, };
static volatile uint8_t PC_data = 0;
static volatile uint8_t mcu_data = 0;
static volatile uint8_t rx_string[MAX_BUF] = {0, };
static volatile int idx = 0;
static volatile int rx_idx = 0;
static volatile int bReceive = 0;

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    GPIO_init(PA_5, OUTPUT);
    GPIO_otype(PA_5, PUSH_PULL);
    GPIO_pupd(PA_5, NO_PUPD);
    GPIO_ospeed(PA_5, HIGH_SPEED);
    GPIO_write(PA_5, LOW);

    // USART2: USB serial init
    USART2_init();
    //USART2_baud(BAUD_38400);
    // USART1: MCU2 serial init
    USART1_init();
    USART1_baud(BAUD_9600);
}

int main(void){
    setup();
    printf("MCU Initialized\r\n");

    while(1){
        if (bReceive == 1){
            printf("PC_string: %s\r\n", PC_string);
            bReceive = 0;
        }
    }
}

void USART2_IRQHandler(){       // USART2 RX Interrupt
    if(is_USART2_RXNE()){
        PC_data = USART2_read();     // RX from UART2 (PC)

        if(PC_data == '\0' || PC_data == '\n') return;

        if(PC_data == END_CHAR){
            USART2_write((uint8_t*)"\r", 1);
        }   
        else{
            USART2_write(&PC_data, 1);  // TX to USART2 (PC) Echo of keyboard typing
        }
        USART1_write(&PC_data, 1);      // TX to USART1 (MCU2)

        // Creates a String from received serial character
        if(PC_data != END_CHAR && (idx < MAX_BUF - 1)){
            buffer[idx] = PC_data;
            idx++;
        }
        else if (PC_data == END_CHAR) {
            buffer[idx] = '\0';
            // reset PC_string
            memset((void*)PC_string, 0, sizeof(char) * MAX_BUF);
            // copy to PC_string
            memcpy((void*)PC_string, (void*)buffer, sizeof(char) * (idx + 1));
            // reset buffer
            memset((void*)buffer, 0, sizeof(char) * MAX_BUF);
            idx = 0;
            bReceive = 1;
        }
        else{       // if(idx >= MAX_BUF - 1)
            idx = 0;
            // reset PC_string
            memset((void*)PC_string, 0, sizeof(char) * MAX_BUF);
            // reset buffer
            memset((void*)buffer, 0, sizeof(char) * MAX_BUF);
            printf("ERROR : Too long string\r\n");
        }
    }
}

// Added: receive a string from the other MCU through PA9/PA10.
void USART1_IRQHandler(){
    if(is_USART1_RXNE()){
        mcu_data = USART1_read();

        if(mcu_data == '\0' || mcu_data == '\n') return;

        if(mcu_data == 'H'){
            GPIO_write(PA_5, HIGH);
        }
        else if(mcu_data == 'L'){
            GPIO_write(PA_5, LOW);
        }

        // USART2_write(&mcu_data,1); // Echo received character to local PC

        if(mcu_data != END_CHAR && (rx_idx < MAX_BUF - 1)){
            rx_buffer[rx_idx] = mcu_data;
            rx_idx++;
        }
        else if (mcu_data == END_CHAR){
            rx_buffer[rx_idx] = '\0';

            memset((void*)PC_string, 0, sizeof(char) * MAX_BUF);
            memcpy((void*)PC_string, (void*)rx_buffer, sizeof(char) * (rx_idx + 1));

            memset((void*)rx_buffer, 0, sizeof(char) * MAX_BUF);
            rx_idx = 0;
            bReceive = 1;
        }
        else{
            rx_idx = 0;
            memset((void*)PC_string, 0, sizeof(char) * MAX_BUF);
            memset((void*)rx_buffer, 0, sizeof(char) * MAX_BUF);
            printf("ERROR : Too long string\r\n");
        }
    }
}
