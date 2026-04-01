################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/ThirdParty/LMIC/aes/ideetron/AES-128_V10.c 

OBJS += \
./Middleware/ThirdParty/LMIC/aes/ideetron/AES-128_V10.o 

C_DEPS += \
./Middleware/ThirdParty/LMIC/aes/ideetron/AES-128_V10.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/ThirdParty/LMIC/aes/ideetron/%.o Middleware/ThirdParty/LMIC/aes/ideetron/%.su Middleware/ThirdParty/LMIC/aes/ideetron/%.cyclo: ../Middleware/ThirdParty/LMIC/aes/ideetron/%.c Middleware/ThirdParty/LMIC/aes/ideetron/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/hal" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/lmic" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/aes" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC/se" -I"D:/STM32/Projects/GroundStation/Middleware/ThirdParty/LMIC" -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-aes-2f-ideetron

clean-Middleware-2f-ThirdParty-2f-LMIC-2f-aes-2f-ideetron:
	-$(RM) ./Middleware/ThirdParty/LMIC/aes/ideetron/AES-128_V10.cyclo ./Middleware/ThirdParty/LMIC/aes/ideetron/AES-128_V10.d ./Middleware/ThirdParty/LMIC/aes/ideetron/AES-128_V10.o ./Middleware/ThirdParty/LMIC/aes/ideetron/AES-128_V10.su

.PHONY: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-aes-2f-ideetron

