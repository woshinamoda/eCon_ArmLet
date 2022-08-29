#ifndef __mpu9150_H
#define __mpu9150_H

extern signed short int init_gx;
extern u8 MPU_Sent[];

void init_mpu9150(void);
int get_mpu9150_data(u8 * mpudata);
void get_compass_bias(void);
void compass_calibration(void);

#endif
