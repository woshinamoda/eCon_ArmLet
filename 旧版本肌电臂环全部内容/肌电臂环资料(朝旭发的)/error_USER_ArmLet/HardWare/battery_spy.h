#ifndef _BATTERY_SPY_h_
#define _BATTERY_SPY_h_

#include "stm32f4xx_hal.h"
#include "main.h"

extern uint16_t LI_Old_state;

void Bat_Filter(void);
void Bat_Spy(void);

#endif //end if battery_spy.h



