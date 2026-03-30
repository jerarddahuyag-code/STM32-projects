################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/ThirdParty/LMIC/lmic/lmic.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_as923.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_au915.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_channelshuffle.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_compliance.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_eu868.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_eu_like.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_in866.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_kr920.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_us915.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_us_like.c \
../Middleware/ThirdParty/LMIC/lmic/lmic_util.c \
../Middleware/ThirdParty/LMIC/lmic/oslmic.c \
../Middleware/ThirdParty/LMIC/lmic/radio_sx126x.c \
../Middleware/ThirdParty/LMIC/lmic/radio_sx127x.c 

OBJS += \
./Middleware/ThirdParty/LMIC/lmic/lmic.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_as923.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_au915.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_channelshuffle.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_compliance.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_eu868.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_eu_like.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_in866.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_kr920.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_us915.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_us_like.o \
./Middleware/ThirdParty/LMIC/lmic/lmic_util.o \
./Middleware/ThirdParty/LMIC/lmic/oslmic.o \
./Middleware/ThirdParty/LMIC/lmic/radio_sx126x.o \
./Middleware/ThirdParty/LMIC/lmic/radio_sx127x.o 

C_DEPS += \
./Middleware/ThirdParty/LMIC/lmic/lmic.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_as923.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_au915.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_channelshuffle.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_compliance.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_eu868.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_eu_like.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_in866.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_kr920.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_us915.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_us_like.d \
./Middleware/ThirdParty/LMIC/lmic/lmic_util.d \
./Middleware/ThirdParty/LMIC/lmic/oslmic.d \
./Middleware/ThirdParty/LMIC/lmic/radio_sx126x.d \
./Middleware/ThirdParty/LMIC/lmic/radio_sx127x.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/ThirdParty/LMIC/lmic/%.o Middleware/ThirdParty/LMIC/lmic/%.su Middleware/ThirdParty/LMIC/lmic/%.cyclo: ../Middleware/ThirdParty/LMIC/lmic/%.c Middleware/ThirdParty/LMIC/lmic/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Middleware/ThirdParty/LMIC -I../Middleware/ThirdParty/LMIC/se -I../Middleware/ThirdParty/LMIC/lmic -I../Middleware/ThirdParty/LMIC/hal -I../Middleware/ThirdParty/LMIC/aes -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32/Projects/Tracker/Middleware" -I"/Tracker/Middleware/ThirdParty/LMIC/se/drivers/default" -I"D:/STM32/Projects/Tracker/Middleware/ThirdParty/LMIC/aes/ideetron" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-lmic

clean-Middleware-2f-ThirdParty-2f-LMIC-2f-lmic:
	-$(RM) ./Middleware/ThirdParty/LMIC/lmic/lmic.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic.d ./Middleware/ThirdParty/LMIC/lmic/lmic.o ./Middleware/ThirdParty/LMIC/lmic/lmic.su ./Middleware/ThirdParty/LMIC/lmic/lmic_as923.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_as923.d ./Middleware/ThirdParty/LMIC/lmic/lmic_as923.o ./Middleware/ThirdParty/LMIC/lmic/lmic_as923.su ./Middleware/ThirdParty/LMIC/lmic/lmic_au915.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_au915.d ./Middleware/ThirdParty/LMIC/lmic/lmic_au915.o ./Middleware/ThirdParty/LMIC/lmic/lmic_au915.su ./Middleware/ThirdParty/LMIC/lmic/lmic_channelshuffle.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_channelshuffle.d ./Middleware/ThirdParty/LMIC/lmic/lmic_channelshuffle.o ./Middleware/ThirdParty/LMIC/lmic/lmic_channelshuffle.su ./Middleware/ThirdParty/LMIC/lmic/lmic_compliance.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_compliance.d ./Middleware/ThirdParty/LMIC/lmic/lmic_compliance.o ./Middleware/ThirdParty/LMIC/lmic/lmic_compliance.su ./Middleware/ThirdParty/LMIC/lmic/lmic_eu868.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_eu868.d ./Middleware/ThirdParty/LMIC/lmic/lmic_eu868.o ./Middleware/ThirdParty/LMIC/lmic/lmic_eu868.su ./Middleware/ThirdParty/LMIC/lmic/lmic_eu_like.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_eu_like.d ./Middleware/ThirdParty/LMIC/lmic/lmic_eu_like.o ./Middleware/ThirdParty/LMIC/lmic/lmic_eu_like.su ./Middleware/ThirdParty/LMIC/lmic/lmic_in866.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_in866.d ./Middleware/ThirdParty/LMIC/lmic/lmic_in866.o ./Middleware/ThirdParty/LMIC/lmic/lmic_in866.su ./Middleware/ThirdParty/LMIC/lmic/lmic_kr920.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_kr920.d ./Middleware/ThirdParty/LMIC/lmic/lmic_kr920.o ./Middleware/ThirdParty/LMIC/lmic/lmic_kr920.su ./Middleware/ThirdParty/LMIC/lmic/lmic_us915.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_us915.d ./Middleware/ThirdParty/LMIC/lmic/lmic_us915.o ./Middleware/ThirdParty/LMIC/lmic/lmic_us915.su ./Middleware/ThirdParty/LMIC/lmic/lmic_us_like.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_us_like.d ./Middleware/ThirdParty/LMIC/lmic/lmic_us_like.o ./Middleware/ThirdParty/LMIC/lmic/lmic_us_like.su ./Middleware/ThirdParty/LMIC/lmic/lmic_util.cyclo ./Middleware/ThirdParty/LMIC/lmic/lmic_util.d ./Middleware/ThirdParty/LMIC/lmic/lmic_util.o ./Middleware/ThirdParty/LMIC/lmic/lmic_util.su ./Middleware/ThirdParty/LMIC/lmic/oslmic.cyclo ./Middleware/ThirdParty/LMIC/lmic/oslmic.d ./Middleware/ThirdParty/LMIC/lmic/oslmic.o ./Middleware/ThirdParty/LMIC/lmic/oslmic.su ./Middleware/ThirdParty/LMIC/lmic/radio_sx126x.cyclo ./Middleware/ThirdParty/LMIC/lmic/radio_sx126x.d ./Middleware/ThirdParty/LMIC/lmic/radio_sx126x.o ./Middleware/ThirdParty/LMIC/lmic/radio_sx126x.su ./Middleware/ThirdParty/LMIC/lmic/radio_sx127x.cyclo ./Middleware/ThirdParty/LMIC/lmic/radio_sx127x.d ./Middleware/ThirdParty/LMIC/lmic/radio_sx127x.o ./Middleware/ThirdParty/LMIC/lmic/radio_sx127x.su

.PHONY: clean-Middleware-2f-ThirdParty-2f-LMIC-2f-lmic

