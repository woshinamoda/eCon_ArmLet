#ifndef NRF_LSM6DSL_H
#define	NRF_LSM6DSL_H

//常规头文件引用
#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "nrf_gpio.h"
#include "boards.h"
#include "nrf_delay.h"
//twi头文件引用
#include "nrf_drv_twi.h"
#include "lsm6dsltr.h"
#include "main.h"

#define		LSM6D_ADDRESS_H									0x6B
#define		LSM6D_ADDRESS_L									0x6A
#define		LSM6D_WHO_AM_I									0x0F


void Nordic_TWI_init(void);
void TWI_uninit(void);
int32_t platform_write(void *handle, uint8_t Reg, uint8_t *Bufp,  uint16_t len);
int32_t platform_read(void *handle, uint8_t Reg, uint8_t *Bufp, uint16_t len);
void LSM6DSL_init(void);
uint8_t LSM6DSL_Write_Byte(uint8_t LSM6D_reg, uint8_t value);
uint8_t LSM6DSL_ReadOneByte(uint8_t LSM6D_reg);
void LSM6DSL_init_ByUser(void);
void Lsm_Get_Rawacc(void);
void Lsm_Get_Rawgryo(void);
void Get_LSM6D_Data();
void LSM6DSL_DataCut_InLine(void);
void LSM6DSL_DataCut_IMU(void);


void LSM6DSL_Sleep();
void LSM6DSL_WakeUp();























#endif




