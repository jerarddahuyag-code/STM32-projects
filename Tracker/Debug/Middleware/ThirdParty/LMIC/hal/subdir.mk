################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/ThirdParty/LMIC/hal/hal.c 

OBJS += \
./Middleware/ThirdParty/LMIC/hal/hal.o 

C_DEPS += \
./Middleware/ThirdParty/LMIC/hal/hal.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/ThirdParty/LMIC/hal/%.o Middleware/ThirdParty/LMIC/hal/%.su Middleware/ThirdParty/LMIC/hal/%.cyclo: ../Middleware/ThirdParty/LMIC/hal/%.c Middleware/ThirdParty/LMIC/hal/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Middleware/ThirdParty/LMIC -I../Middleware/ThirdParty/LMIC/se -I../Middleware/ThirdParty/LMIC/lmic -I../Middleware/ThirdParty/LMIC/hal -I../Middleware/ThirdParty/LMIC/aes -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32/Projects/Tracker/Middleware" -I"/Tracker/Middleware/ThirdParty/LMIC/se/drivers/default" -I"D:/STM32/Projects/Tracker/Middleware/ThirdParty/LMIC/aes/ideetron" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-hal

clean-Middleware-2f-ThirdParty-2f-LMIC-2f-hal:
	-$(RM) ./Middleware/ThirdParty/LMIC/hal/hal.cyclo ./Middleware/ThirdParty/LMIC/hal/hal.d ./Middleware/ThirdParty/LMIC/hal/hal.o ./Middleware/ThirdParty/LMIC/hal/hal.su

.PHONY: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-hal

