#ifndef __stm_ADC_H
#define __stm_ADC_H

extern 	u16 Li_battery;

extern u8 Emg_Data_Sent[20];      //Emg_Data_Sent
extern u8 Imu_Data_Sent[30];      //Imu_Data_Sent
extern u8 if_imu_sent_flag;


void init_ADC(void);
void get_ADCdata(void);
void send_mpu();
void encryptCode(u8 *ptr,u16 len);
u8 deencyptCode(u8 input);
#endif
