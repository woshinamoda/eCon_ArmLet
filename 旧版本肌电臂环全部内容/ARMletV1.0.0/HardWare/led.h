#ifndef _LED_h_
#define _LED_h_

#include "stm32f4xx_hal.h"
#include "main.h"

enum col{green,red,blue};


void Open_LED(uint8_t color);
void Close_LED(uint8_t color);
void Booth_LED(uint8_t state);






#endif 

//[] END OF FILE

