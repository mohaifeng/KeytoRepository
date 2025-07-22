################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/M_Src/cmd.c \
../User/M_Src/ee_prom.c \
../User/M_Src/flash.c \
../User/M_Src/led.c \
../User/M_Src/m_usart.c \
../User/M_Src/motor_control.c \
../User/M_Src/objectdirectory.c \
../User/M_Src/ow.c \
../User/M_Src/protocol.c \
../User/M_Src/register.c \
../User/M_Src/verification.c 

OBJS += \
./User/M_Src/cmd.o \
./User/M_Src/ee_prom.o \
./User/M_Src/flash.o \
./User/M_Src/led.o \
./User/M_Src/m_usart.o \
./User/M_Src/motor_control.o \
./User/M_Src/objectdirectory.o \
./User/M_Src/ow.o \
./User/M_Src/protocol.o \
./User/M_Src/register.o \
./User/M_Src/verification.o 

C_DEPS += \
./User/M_Src/cmd.d \
./User/M_Src/ee_prom.d \
./User/M_Src/flash.d \
./User/M_Src/led.d \
./User/M_Src/m_usart.d \
./User/M_Src/motor_control.d \
./User/M_Src/objectdirectory.d \
./User/M_Src/ow.d \
./User/M_Src/protocol.d \
./User/M_Src/register.d \
./User/M_Src/verification.d 


# Each subdirectory must supply rules for building sources it contributes
User/M_Src/%.o User/M_Src/%.su User/M_Src/%.cyclo: ../User/M_Src/%.c User/M_Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"D:/GitHubRepository/STM32-G431RBT6/User/M_Inc" -I"D:/GitHubRepository/STM32-G431RBT6/User/App_Inc" -Os -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-M_Src

clean-User-2f-M_Src:
	-$(RM) ./User/M_Src/cmd.cyclo ./User/M_Src/cmd.d ./User/M_Src/cmd.o ./User/M_Src/cmd.su ./User/M_Src/ee_prom.cyclo ./User/M_Src/ee_prom.d ./User/M_Src/ee_prom.o ./User/M_Src/ee_prom.su ./User/M_Src/flash.cyclo ./User/M_Src/flash.d ./User/M_Src/flash.o ./User/M_Src/flash.su ./User/M_Src/led.cyclo ./User/M_Src/led.d ./User/M_Src/led.o ./User/M_Src/led.su ./User/M_Src/m_usart.cyclo ./User/M_Src/m_usart.d ./User/M_Src/m_usart.o ./User/M_Src/m_usart.su ./User/M_Src/motor_control.cyclo ./User/M_Src/motor_control.d ./User/M_Src/motor_control.o ./User/M_Src/motor_control.su ./User/M_Src/objectdirectory.cyclo ./User/M_Src/objectdirectory.d ./User/M_Src/objectdirectory.o ./User/M_Src/objectdirectory.su ./User/M_Src/ow.cyclo ./User/M_Src/ow.d ./User/M_Src/ow.o ./User/M_Src/ow.su ./User/M_Src/protocol.cyclo ./User/M_Src/protocol.d ./User/M_Src/protocol.o ./User/M_Src/protocol.su ./User/M_Src/register.cyclo ./User/M_Src/register.d ./User/M_Src/register.o ./User/M_Src/register.su ./User/M_Src/verification.cyclo ./User/M_Src/verification.d ./User/M_Src/verification.o ./User/M_Src/verification.su

.PHONY: clean-User-2f-M_Src

