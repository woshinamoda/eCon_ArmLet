#ifndef  _DRIVE_BAT_H_
#define  _DRIVE_BAT_H_

#include "stdio.h"
#include "nrf_gpio.h"
#include "main.h"
#include "nrf_drv_saadc.h"	

#include "pca10040.h"



uint8_t	User_Set_battery_level_10per(uint16_t mvolts, uint8_t las_level);
uint8_t	First_battery_level(uint16_t mvolts);
void BatdataBubbleSort(uint16_t * p_bat_data, uint8_t length);
uint16_t	GetBatAvr(uint16_t * p_bat_data, uint8_t length);


















#endif

