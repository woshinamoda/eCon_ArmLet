################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MPU6050/inv_mpu.c \
../MPU6050/inv_mpu_dmp_motion_driver.c \
../MPU6050/mpu6050.c 

OBJS += \
./MPU6050/inv_mpu.o \
./MPU6050/inv_mpu_dmp_motion_driver.o \
./MPU6050/mpu6050.o 

C_DEPS += \
./MPU6050/inv_mpu.d \
./MPU6050/inv_mpu_dmp_motion_driver.d \
./MPU6050/mpu6050.d 


# Each subdirectory must supply rules for building sources it contributes
MPU6050/%.o: ../MPU6050/%.c MPU6050/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/Sok/Desktop/ARMlet/APP_User" -I"C:/Users/Sok/Desktop/ARMlet/HardWare" -I"C:/Users/Sok/Desktop/ARMlet/MPU6050" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

