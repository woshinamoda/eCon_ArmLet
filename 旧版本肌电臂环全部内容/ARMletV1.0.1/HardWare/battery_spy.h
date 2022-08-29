#ifndef _BATTERY_SPY_h_
#define _BATTERY_SPY_h_

#include "stm32f4xx_hal.h"
#include "main.h"

extern uint16_t LI_Old_state;
extern uint16_t	BatValueBuf[10];
extern uint16_t	sum;



void Bat_Filter(void);
void Bat_Spy(void);
void check_Charge_OK(void);
void check_Charge_BySTAT(void);
#endif //end if battery_spy.h



