################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/GFX_FUNCTIONS.c \
../Core/Src/OBD2.c \
../Core/Src/SDlog.c \
../Core/Src/UI.c \
../Core/Src/button.c \
../Core/Src/ds1307.c \
../Core/Src/fatfs_sd_card.c \
../Core/Src/fonts.c \
../Core/Src/main.c \
../Core/Src/rtime.c \
../Core/Src/st7735.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/GFX_FUNCTIONS.o \
./Core/Src/OBD2.o \
./Core/Src/SDlog.o \
./Core/Src/UI.o \
./Core/Src/button.o \
./Core/Src/ds1307.o \
./Core/Src/fatfs_sd_card.o \
./Core/Src/fonts.o \
./Core/Src/main.o \
./Core/Src/rtime.o \
./Core/Src/st7735.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/GFX_FUNCTIONS.d \
./Core/Src/OBD2.d \
./Core/Src/SDlog.d \
./Core/Src/UI.d \
./Core/Src/button.d \
./Core/Src/ds1307.d \
./Core/Src/fatfs_sd_card.d \
./Core/Src/fonts.d \
./Core/Src/main.d \
./Core/Src/rtime.d \
./Core/Src/st7735.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/GFX_FUNCTIONS.cyclo ./Core/Src/GFX_FUNCTIONS.d ./Core/Src/GFX_FUNCTIONS.o ./Core/Src/GFX_FUNCTIONS.su ./Core/Src/OBD2.cyclo ./Core/Src/OBD2.d ./Core/Src/OBD2.o ./Core/Src/OBD2.su ./Core/Src/SDlog.cyclo ./Core/Src/SDlog.d ./Core/Src/SDlog.o ./Core/Src/SDlog.su ./Core/Src/UI.cyclo ./Core/Src/UI.d ./Core/Src/UI.o ./Core/Src/UI.su ./Core/Src/button.cyclo ./Core/Src/button.d ./Core/Src/button.o ./Core/Src/button.su ./Core/Src/ds1307.cyclo ./Core/Src/ds1307.d ./Core/Src/ds1307.o ./Core/Src/ds1307.su ./Core/Src/fatfs_sd_card.cyclo ./Core/Src/fatfs_sd_card.d ./Core/Src/fatfs_sd_card.o ./Core/Src/fatfs_sd_card.su ./Core/Src/fonts.cyclo ./Core/Src/fonts.d ./Core/Src/fonts.o ./Core/Src/fonts.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/rtime.cyclo ./Core/Src/rtime.d ./Core/Src/rtime.o ./Core/Src/rtime.su ./Core/Src/st7735.cyclo ./Core/Src/st7735.d ./Core/Src/st7735.o ./Core/Src/st7735.su ./Core/Src/stm32f1xx_hal_msp.cyclo ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.cyclo ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.cyclo ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

