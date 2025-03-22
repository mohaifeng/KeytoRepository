################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Dev/usart_driver.c 

OBJS += \
./User/Dev/usart_driver.o 

C_DEPS += \
./User/Dev/usart_driver.d 


# Each subdirectory must supply rules for building sources it contributes
User/Dev/%.o User/Dev/%.su User/Dev/%.cyclo: ../User/Dev/%.c User/Dev/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I../User/Rev -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-Dev

clean-User-2f-Dev:
	-$(RM) ./User/Dev/usart_driver.cyclo ./User/Dev/usart_driver.d ./User/Dev/usart_driver.o ./User/Dev/usart_driver.su

.PHONY: clean-User-2f-Dev

