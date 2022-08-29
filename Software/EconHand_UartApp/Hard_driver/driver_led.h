#ifndef  _DRIVE_LED_H_
#define  _DRIVE_LED_H_


#include "stdio.h"
#include "nrf_gpio.h"
#include "main.h"

#define	led_red_off					nrf_gpio_pin_set(LED_R)
#define	led_red_on					nrf_gpio_pin_clear(LED_R)

#define	led_green_off				nrf_gpio_pin_set(LED_G)
#define	led_green_on				nrf_gpio_pin_clear(LED_G)

#define	led_blue_off				nrf_gpio_pin_set(LED_B)
#define	led_blue_on					nrf_gpio_pin_clear(LED_B)






void EconHand_BSP_LED_gpio_init(void);
void LED_Reset_close(void);
void LED_Green_Open(void);
void LED_Blue_Slow_Blink(void);
void LED_Blue_Fast_Blink(void);
void LED_Red_Slow_Blink(void);
void LED_Blue_Open(void);
void APP_Order_Blink(void);

void LED_Indicate_Mode(void);








#endif

