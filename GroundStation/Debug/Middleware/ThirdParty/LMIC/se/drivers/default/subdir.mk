################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/ThirdParty/LMIC/se/drivers/default/lmic_se_default.c 

OBJS += \
./Middleware/ThirdParty/LMIC/se/drivers/default/lmic_se_default.o 

C_DEPS += \
./Middleware/ThirdParty/LMIC/se/drivers/default/lmic_se_default.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/ThirdParty/LMIC/se/drivers/default/%.o Middleware/ThirdParty/LMIC/se/drivers/default/%.su Middleware/ThirdParty/LMIC/se/drivers/default/%.cyclo: ../Middleware/ThirdParty/LMIC/se/drivers/default/%.c Middleware/ThirdParty/LMIC/se/drivers/default/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/hal" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/lmic" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/aes" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/se" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-se-2f-drivers-2f-default

clean-Middleware-2f-ThirdParty-2f-LMIC-2f-se-2f-drivers-2f-default:
	-$(RM) ./Middleware/ThirdParty/LMIC/se/drivers/default/lmic_se_default.cyclo ./Middleware/ThirdParty/LMIC/se/drivers/default/lmic_se_default.d ./Middleware/ThirdParty/LMIC/se/drivers/default/lmic_se_default.o ./Middleware/ThirdParty/LMIC/se/drivers/default/lmic_se_default.su

.PHONY: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-se-2f-drivers-2f-default

