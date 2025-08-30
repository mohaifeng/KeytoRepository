################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Src/cmd.c \
../User/Src/dev.c \
../User/Src/led.c \
../User/Src/protocol.c \
../User/Src/tmc5160.c \
../User/Src/user_usart.c \
../User/Src/valve_control.c \
../User/Src/verification.c 

OBJS += \
./User/Src/cmd.o \
./User/Src/dev.o \
./User/Src/led.o \
./User/Src/protocol.o \
./User/Src/tmc5160.o \
./User/Src/user_usart.o \
./User/Src/valve_control.o \
./User/Src/verification.o 

C_DEPS += \
./User/Src/cmd.d \
./User/Src/dev.d \
./User/Src/led.d \
./User/Src/protocol.d \
./User/Src/tmc5160.d \
./User/Src/user_usart.d \
./User/Src/valve_control.d \
./User/Src/verification.d 


# Each subdirectory must supply rules for building sources it contributes
User/Src/%.o User/Src/%.su User/Src/%.cyclo: ../User/Src/%.c User/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"D:/GitHubRepository/STM32G431C8T6/User/Inc" -Os -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-Src

clean-User-2f-Src:
	-$(RM) ./User/Src/cmd.cyclo ./User/Src/cmd.d ./User/Src/cmd.o ./User/Src/cmd.su ./User/Src/dev.cyclo ./User/Src/dev.d ./User/Src/dev.o ./User/Src/dev.su ./User/Src/led.cyclo ./User/Src/led.d ./User/Src/led.o ./User/Src/led.su ./User/Src/protocol.cyclo ./User/Src/protocol.d ./User/Src/protocol.o ./User/Src/protocol.su ./User/Src/tmc5160.cyclo ./User/Src/tmc5160.d ./User/Src/tmc5160.o ./User/Src/tmc5160.su ./User/Src/user_usart.cyclo ./User/Src/user_usart.d ./User/Src/user_usart.o ./User/Src/user_usart.su ./User/Src/valve_control.cyclo ./User/Src/valve_control.d ./User/Src/valve_control.o ./User/Src/valve_control.su ./User/Src/verification.cyclo ./User/Src/verification.d ./User/Src/verification.o ./User/Src/verification.su

.PHONY: clean-User-2f-Src

