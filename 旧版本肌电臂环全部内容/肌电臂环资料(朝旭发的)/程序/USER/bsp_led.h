#ifndef __BSP_LED_H
#define __BSP_LED_H	 

#include "stm_sys.h"
/*
#define LED_ON()                        PAout(8) = 1
#define LED_OFF()                       PAout(8) = 0*/

#define LED1_ON()                        PAout(15) = 1
#define LED1_OFF()                       PAout(15) = 0
#define LED1_TOG()											 PAout(15)=1-PAin(15)

#define LED2_ON()                        PBout(3) = 1
#define LED2_OFF()                       PBout(3) = 0
#define LED2_TOG()											 PBout(3)=1-PBin(3)

#define LED3_ON()                        PBout(4) = 1
#define LED3_OFF()                       PBout(4) = 0
#define LED3_TOG()											 PBout(4)=1-PBin(4)
enum col{red,blue,green};

void LED_Init(void);//≥ı ºªØ	
void LED_blink(void);
void LED_red(void);
void LED_blue(void);
void LED_green(void);
void LED_TOGGLE(enum col temp);
#endif

