################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../QA_Drivers/QAD_Devices/QAD_FT6206.cpp 

OBJS += \
./QA_Drivers/QAD_Devices/QAD_FT6206.o 

CPP_DEPS += \
./QA_Drivers/QAD_Devices/QAD_FT6206.d 


# Each subdirectory must supply rules for building sources it contributes
QA_Drivers/QAD_Devices/%.o: ../QA_Drivers/QAD_Devices/%.cpp QA_Drivers/QAD_Devices/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F769xx -c -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Drivers/Componenets/Common -I../Drivers/Components/otm8009a -I../Core -I../QA_Drivers -I../QA_Drivers/QAD_PeripheralManagers -I../QA_Drivers/QAD_Devices -I../QA_Tools -I../QA_Systems -I../QA_Systems/QAS_Serial -I../QA_Systems/QAS_LCD -I../QA_Systems/QAS_LCD/QAS_LCD_Fonts -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-QA_Drivers-2f-QAD_Devices

clean-QA_Drivers-2f-QAD_Devices:
	-$(RM) ./QA_Drivers/QAD_Devices/QAD_FT6206.d ./QA_Drivers/QAD_Devices/QAD_FT6206.o

.PHONY: clean-QA_Drivers-2f-QAD_Devices

