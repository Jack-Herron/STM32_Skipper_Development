################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USB_CDC_DEVICE/Core/Src/USB_CDC_Device.c 

C_DEPS += \
./USB_CDC_DEVICE/Core/Src/USB_CDC_Device.d 

OBJS += \
./USB_CDC_DEVICE/Core/Src/USB_CDC_Device.o 


# Each subdirectory must supply rules for building sources it contributes
USB_CDC_DEVICE/Core/Src/%.o USB_CDC_DEVICE/Core/Src/%.su USB_CDC_DEVICE/Core/Src/%.cyclo: ../USB_CDC_DEVICE/Core/Src/%.c USB_CDC_DEVICE/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407ZETx -DSTM32 -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_CDC_DEVICE/Core/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Include" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/Skipper Drivers/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_LL_CORE/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_HOST/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-USB_CDC_DEVICE-2f-Core-2f-Src

clean-USB_CDC_DEVICE-2f-Core-2f-Src:
	-$(RM) ./USB_CDC_DEVICE/Core/Src/USB_CDC_Device.cyclo ./USB_CDC_DEVICE/Core/Src/USB_CDC_Device.d ./USB_CDC_DEVICE/Core/Src/USB_CDC_Device.o ./USB_CDC_DEVICE/Core/Src/USB_CDC_Device.su

.PHONY: clean-USB_CDC_DEVICE-2f-Core-2f-Src

