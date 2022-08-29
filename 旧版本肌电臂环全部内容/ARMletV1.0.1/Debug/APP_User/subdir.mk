################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP_User/app_user.c \
../APP_User/exit_work.c 

OBJS += \
./APP_User/app_user.o \
./APP_User/exit_work.o 

C_DEPS += \
./APP_User/app_user.d \
./APP_User/exit_work.d 


# Each subdirectory must supply rules for building sources it contributes
APP_User/%.o: ../APP_User/%.c APP_User/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I"C:/Users/Sok/Desktop/ARMlet/APP_User" -I"C:/Users/Sok/Desktop/ARMlet/HardWare" -I"C:/Users/Sok/Desktop/ARMlet/MPU6050" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

