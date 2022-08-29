#ifndef  _DRIVE_POWER_H_
#define  _DRIVE_POWER_H_


#include "stdio.h"
#include "nrf_gpio.h"
#include "main.h"

/*private power define ****************************************/
/*user begin code */
#define	AVDD_H					nrf_gpio_pin_set(AVDD_EN)
#define	AVDD_L					nrf_gpio_pin_clear(AVDD_EN)

#define	DVDD_H					nrf_gpio_pin_set(DVDD_EN)
#define	DVDD_L					nrf_gpio_pin_clear(DVDD_EN)

/*user stop code */




void EconHand_BSP_power_gpio_init(void);
void Control_DVDD(uint8_t state);
void Control_AVDD(uint8_t	state);


































#endif




