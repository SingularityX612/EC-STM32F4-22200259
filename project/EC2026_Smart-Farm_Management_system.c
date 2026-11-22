#include "ecSTM32F4v2.h"

#define VENT_FAN        PA_15
#define WINDOW_SERVO    PC_7
#define LED PB_7
#define LIGHT_SENSOR    PA_0
#define MOISTURE_SENSOR PA_1
#define DHT11           PA_6
#define LCD_SCL         PB_8
#define LCD_SDA         PB_9
#define BT_TX           PA_11
#define BT_RX           PA_12
#define DEBUG_TX        PA_2
#define DEBUG_RX        PA_3
#define BUTTON_PIN      PC_13
#define MANUAL          0
#define AUTO            1

#define NORMAL          0
#define DRY             1

#define LIGHT_DARK      4000
#define LIGHT_BRIGHT    500
#define LIGHT_HYST      100
#define LIGHT_MIN_TIM   2

#define MOIST_DRY       3000
#define MOIST_WET       2500

#define TEMP_HIGH       28
#define TEMP_LOW        26

#define HUMI_HIGH       70
#define HUMI_LOW        60

#define SERVO_CLOSE_US  1500
#define SERVO_OPEN_US   900

#define PRINT_DELAY_TICK 6

volatile uint32_t light_val = 0;
         uint32_t light_on_time = 0;
         uint32_t dark_th = LIGHT_DARK;
         uint32_t bright_th = LIGHT_BRIGHT;
volatile uint32_t moist_val = 0;
volatile uint32_t temp_val = 0;
volatile uint32_t humi_val = 0;
volatile uint32_t dht_err_cnt = 0;
volatile uint32_t dht_fault = 0;
volatile uint32_t mode = MANUAL;
volatile uint32_t alarm_state = NORMAL;
volatile uint32_t alarm_ack = 0;
volatile uint32_t LED_duty = 0;

volatile uint32_t flag_500ms = 0;
volatile uint32_t flag_dht11 = 0;
volatile uint32_t dht_tick_cnt = 0;
volatile uint32_t dry_cnt = 0;
volatile uint32_t wet_cnt = 0;
volatile uint32_t auto_delay_cnt = 0;
volatile uint32_t blink_toggle = 0;
volatile uint32_t fan_on = 0;
volatile uint32_t window_open = 0;
volatile uint32_t flag_bt_msg = 0;

void setup(void){
    RCC_PLL_init();
    SysTick_init();

    USART_init(USART2, DEBUG_TX, DEBUG_RX, 9600);
    printf("UART2 OK\r\n");
    USART_init(USART6, BT_TX, BT_RX, 9600);

    PWM_init(VENT_FAN, 1);
    PWM_duty(VENT_FAN, 0.0);

    PWM_init(WINDOW_SERVO, 20);
    PWM_pulsewidth_us(WINDOW_SERVO, SERVO_CLOSE_US);

    DHT11_init(DHT11);
    I2C1_init();
    LCD_init();

    PWM_init(LED, 1);
    PWM_duty(LED, 0.0);

    TIM_UI_init(TIM1, 500);

    JADC_init(LIGHT_SENSOR);
    JADC_init(MOISTURE_SENSOR);

    PinName_t seqCHn[2] = {LIGHT_SENSOR, MOISTURE_SENSOR};
    JADC_sequence(seqCHn, 2);
    JADC_trigger_init(TIM5, 125, ADC_RISE);

    printf("MCU INITIALIZED \r\n");
}

int main(void){

    static uint32_t prev_fan_on = 1;
    static uint32_t prev_window_open = 1;
    static uint32_t prev_LED_duty = 101;
    static uint32_t print_tick = 0;
    static uint32_t flag_usb_msg = 0;

    setup();
    static int err = 0;
    char lcd_line1[20];
    char lcd_line2[20];
	err = I2C1_write(LCD_ADDR, LCD_BACKLIGHT);
	if(err)	printf("LCD NOT found. Check the wiring\r\n");
	else	printf("LCD found at 0x%02X\r\n", LCD_ADDR);
    while(1){
        if(flag_500ms){
            flag_500ms = 0;
            //DHT
            if(flag_dht11){
                flag_dht11 = 0;
                int dht_err = DHT11_read();

                if(dht_err == DHT11_OK){
                    temp_val = DHT11_temperature();
                    humi_val = DHT11_humidity();   
                    dht_err_cnt = 0;
                    dht_fault = 0;
                }
                else{
                    dht_err_cnt++;
                    dht_fault = 1;
                    printf("DHT11 error: %d (cnt=%lu)\r\n", dht_err, dht_err_cnt);
                }
            }
                   
            if (mode == AUTO){
                if(auto_delay_cnt > 0){
                    auto_delay_cnt--;
                    if(auto_delay_cnt % 2 == 0){
                        printf("AUTO light sensing starts in %lus...\r\n", (auto_delay_cnt / 2) + 1);
                    }
                }
                else{
                    //LED
                    if(LED_duty > 0){
                        if(light_on_time < LIGHT_MIN_TIM){
                            light_on_time++;
                        }
                    }
                    else{
                        light_on_time = 0;
                    }


                    if(dark_th < light_val){
                        LED_duty = 100;
                        dark_th = LIGHT_DARK - LIGHT_HYST;
                    }
                    else if(bright_th > light_val){
                        if(LED_duty == 0 || light_on_time >= LIGHT_MIN_TIM){
                            LED_duty = 0;
                            bright_th = LIGHT_BRIGHT + LIGHT_HYST;
                        }
                    }
                    else{
                        dark_th = LIGHT_DARK;
                        bright_th = LIGHT_BRIGHT;
                        LED_duty = (uint32_t)((100.0f / (LIGHT_DARK - LIGHT_BRIGHT)) * (light_val - LIGHT_BRIGHT));
                    }
                }
                
                //Ventilation
                if(dht_fault || HUMI_HIGH <= humi_val || TEMP_HIGH <= temp_val){
                    fan_on = 1;
                    window_open = 1;
                }
                else if(!dht_fault && HUMI_LOW >= humi_val && TEMP_LOW >= temp_val){
                    fan_on = 0;
                    window_open = 0;
                }   
            }

            //alarm
            if(alarm_state == NORMAL){
                if(moist_val >= MOIST_DRY){
                    dry_cnt++;
                    if(dry_cnt >= 10){
                        alarm_state = DRY;
                        dry_cnt = 0;
                        wet_cnt = 0;
                        alarm_ack = 0;
                        blink_toggle = 1;
                        flag_bt_msg = 1;
                    }
                }
                else{
                    dry_cnt = 0;
                }
            }
            else if(alarm_state == DRY){
                blink_toggle ^= 1;

                if(moist_val <= MOIST_WET){
                    wet_cnt++;
                    if(wet_cnt >= 10){
                        alarm_state = NORMAL;
                        wet_cnt = 0;
                        dry_cnt = 0;
                        alarm_ack = 0;
                        flag_bt_msg = 1;
                    }
                }
                else{
                    wet_cnt = 0;
                }
            }
            
            // 3 s periodic status report timer
            // - USB (USART2): continuously prints every 3 s in both AUTO and MANUAL (paused during 5 s countdown)
            // - BT (USART6): stays quiet unless a command/alarm event occurs or an unacknowledged DRY alarm repeats
            if(auto_delay_cnt == 0){
                print_tick++;
                if(print_tick >= PRINT_DELAY_TICK){
                    print_tick = 0;
                    flag_usb_msg = 1;
                    if(alarm_state == DRY && alarm_ack == 0){
                        flag_bt_msg = 1;
                    }
                }
            }
            else{
                print_tick = 0;
            }

            //LCD
            if (!err){

                if(alarm_state == NORMAL){
                    snprintf(lcd_line1, sizeof(lcd_line1), "%-7s%-9s", 
                            ((mode == AUTO) ? "AUTO" : "MANUAL"), 
                            (fan_on || window_open) ? "VENT" : "");
                    snprintf(lcd_line2, 17, "%02lu\xDF,%02lu%% LED:%lu%%  ", temp_val, humi_val, LED_duty);
                }
                else if(alarm_state == DRY){
                    snprintf(lcd_line1, sizeof(lcd_line1), "%-7s%-9s", "ALARM",
                            (fan_on || window_open) ? "VENT" : "");
                    snprintf(lcd_line2, sizeof(lcd_line2), "%-16s", blink_toggle ? "ADD WATER" : "");
                }
                LCD_setCursor(0,0);
                LCD_print(lcd_line1);
                LCD_setCursor(0,1);
                LCD_print(lcd_line2);
            }
            uint32_t out_LED_duty = (alarm_state == DRY) ? (blink_toggle ? 15 : 0) : LED_duty;
            if(prev_LED_duty != out_LED_duty){
                PWM_duty(LED, out_LED_duty / 100.0f);
                prev_LED_duty = out_LED_duty;
            }
        }
        // 
        if (prev_fan_on != fan_on){
            PWM_duty(VENT_FAN, fan_on ? 0.8f : 0.0f);
            prev_fan_on = fan_on;
        }
        if (prev_window_open != window_open){
            PWM_pulsewidth_us(WINDOW_SERVO, window_open ? SERVO_OPEN_US : SERVO_CLOSE_US);
            prev_window_open = window_open;
        }

        // Serial / Bluetooth status reporting (USB periodic monitoring & BT command/alarm events)
        if (flag_bt_msg || flag_usb_msg){
            // Snapshot and clear flags before slow UART TX to avoid losing a new USART6 RX interrupt
            uint32_t send_bt  = flag_bt_msg;
            uint32_t send_usb = flag_usb_msg;
            flag_bt_msg  = 0;
            flag_usb_msg = 0;

            char bt_buf[160];
            snprintf(bt_buf, sizeof(bt_buf),
            "\r\n--------------------\r\n"
            "\r\nMODE: %s | LED: %lu%% | FAN: %s | WIN: %s%s%s\r\n"
                "LIGHT: %lu | TEMP: %luC | HUMI: %lu%% | MOIST: %lu\r\n",
                    (mode == AUTO) ? "AUTO" : "MANUAL",
                    LED_duty,
                    (fan_on == 0) ? "OFF" : "ON",
                    (window_open == 0) ? "CLOSED" : "OPENED",
                    (alarm_ack == 0 && alarm_state == DRY) ? " | ADD WATER" : "",
                    (dht_fault) ? " | DHT ERR" : "",
                    light_val, temp_val, humi_val, moist_val);

            // USB (USART2): always print on 3 s periodic ticks and on BT command/alarm events
            if (send_usb || send_bt){
                printf("%s", bt_buf);
            }
            // Bluetooth (USART6): transmit only on BT command, alarm transition, or unacknowledged DRY alert
            if (send_bt){
                USART_write(USART6, (uint8_t*)bt_buf, strlen(bt_buf));
            }
        }
    }
}
//light & moisture
void ADC_IRQHandler(void){
    if(is_ADC_JEOC()){      // one JEOC after the whole sequence
        light_val = JADC_read(1);
        moist_val = JADC_read(2);   
        clear_ADC_JEOC();
    }
    if(is_ADC_OVR()){
        clear_ADC_OVR();
    }
}

//bluetooth
void USART6_IRQHandler(void){
    if(is_USART_RXNE(USART6)){
        uint8_t bt_data = USART_read(USART6);

        USART_write(USART6, &bt_data, 1);  // Echo to Bluetooth Tera Term
        USART2_write(&bt_data, 1);         // Echo to USB Tera Term

        if(bt_data == 'M') {
            mode ^= 1;
            if(mode == AUTO){
                auto_delay_cnt = 10;
            }
            flag_bt_msg = 1;
        }
        else if(mode == MANUAL && bt_data >= '0' && bt_data <= '5'){
            LED_duty = (bt_data - '0') * 20;
            flag_bt_msg = 1;
        }
        else if(mode == MANUAL && bt_data == 'F'){
            fan_on ^= 1;
            flag_bt_msg = 1;
        }
        else if(mode == MANUAL && bt_data == 'W'){
            window_open ^= 1;
            flag_bt_msg = 1;
        }
        else if(bt_data == 'A'){
            alarm_ack = 1;
            flag_bt_msg = 1;
        }
    }
}
//500ms timer
void TIM1_UP_TIM10_IRQHandler(void){
    if(is_UIF(TIM1)){
        clear_UIF(TIM1);
        flag_500ms = 1;
        dht_tick_cnt++;
        if(dht_tick_cnt >= 5){ //2.5ms
            dht_tick_cnt = 0;
            flag_dht11 = 1;
        }
    }
}

void USART2_IRQHandler(void){
    if(is_USART2_RXNE()){
        uint8_t pc_data = USART2_read();
        USART2_write(&pc_data, 1);  // Echo USB typing to check USB connection
    }
}
