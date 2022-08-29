#ifndef nrf_ADS1299_H
#define nrf_ADS1299_H

#include "main.h"
#include <stdbool.h>
#include <stdint.h>
#include "nrf_drv_spi.h"

/* private ads1299 user define ----------------------------------------*/
/* define begin */
#define	CS_H	nrf_gpio_pin_set(SPI_SS_PIN)					//CS拉高
#define	CS_L	nrf_gpio_pin_clear(SPI_SS_PIN)				//CS拉低

#define	START_H	nrf_gpio_pin_set(ADS_START)					//START拉高
#define	START_L	nrf_gpio_pin_clear(ADS_START)				//START拉低

#define	RESET_H	nrf_gpio_pin_set(ADS_RESET)					//RESET拉高
#define	RESET_L	nrf_gpio_pin_clear(ADS_RESET)				//PRESET拉低

#define	PWDN_H	nrf_gpio_pin_set(ADS_PWDN)					//PWDN拉高
#define	PWDN_L	nrf_gpio_pin_clear(ADS_PWDN)				//PWDN拉低

//ADS1299 SPI Command Definition Byte Assignments
#define _WAKEUP 		0x02 	// Wake-up from standby mode
#define _STANDBY		0x04 	// Enter Standby mode
#define _RESET 			0x06 	// Reset the device registers to default
#define _START 			0x08 	// Start and restart (synchronize) conversions
#define _STOP 			0x0A 	// Stop conversion
#define _RDATAC 		0x10 	// Enable Read Data Continuous mode (default mode at power-up)
#define _SDATAC 		0x11 	// Stop Read Data Continuous mode
#define _RDATA 			0x12 	// Read data by command supports multiple read back

//ASD1299 Register Addresses
#define ADS_ID			0x3E	// product ID for ADS1299
#define ID_REG  		0x00	// this register contains ADS_ID
#define CONFIG1 		0x01
#define CONFIG2 		0x02
#define CONFIG3 		0x03
#define LOFF				0x04
#define CH1SET 			0x05
#define CH2SET 			0x06
#define CH3SET 			0x07
#define CH4SET 			0x08
#define CH5SET 			0x09
#define CH6SET 			0x0A
#define CH7SET 			0x0B
#define CH8SET 			0x0C
#define BIAS_SENSP 	0x0D
#define BIAS_SENSN 	0x0E
#define LOFF_SENSP 	0x0F
#define LOFF_SENSN 	0x10
#define LOFF_FLIP 	0x11
#define LOFF_STATP 	0x12
#define LOFF_STATN 	0x13
#define GPIO 				0x14
#define MISC1 			0x15
#define MISC2 			0x16
#define CONFIG4 		0x17


/* define end */




/* private function typedef ----------------------------------------*/
/* void (fun) begin */
void 			ADS1292_gpio_config();
void 			spi_event_handler(nrf_drv_spi_evt_t const * p_event,	void *	p_context);
void			SPI_User_init();
void			SPI_uninit();
uint8_t		ADS_xfer(uint8_t byte);
uint8_t		ADS_WREG(uint8_t	address,	uint8_t	cmd);
uint8_t 	ADS_RREG(uint8_t address);
void			ADS1299_RESET(void);
void 			ADS_SDATAC(void);
uint8_t		ADS_getDeviceID(void);
void 			ADS1299_START(void);
void    	ADS_RDATAC(void);
void			ADS_Enter_StandBY();
void			ADS_Exit_StandBY();
void 			initialize_ads(void);
void 			updateBoardData(void);
void			ADS1299_DataCut_InLine();
void			ADS1299_Sleep_mode();


/* void (fun) stop */

#endif