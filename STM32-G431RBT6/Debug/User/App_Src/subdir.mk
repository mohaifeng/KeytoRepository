################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/App_Src/app_usart.c 

OBJS += \
./User/App_Src/app_usart.o 

C_DEPS += \
./User/App_Src/app_usart.d 


# Each subdirectory must supply rules for building sources it contributes
User/App_Src/%.o User/App_Src/%.su User/App_Src/%.cyclo: ../User/App_Src/%.c User/App_Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -I"D:/GitHubRepository/STM32-G431RBT6/User/M_Inc" -I"D:/GitHubRepository/STM32-G431RBT6/User/App_Inc" -Os -ffunction-sections -fdata-sections -Wall -Werror -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-User-2f-App_Src

clean-User-2f-App_Src:
	-$(RM) ./User/App_Src/app_usart.cyclo ./User/App_Src/app_usart.d ./User/App_Src/app_usart.o ./User/App_Src/app_usart.su

.PHONY: clean-User-2f-App_Src

