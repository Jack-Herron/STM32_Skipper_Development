################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USB_LL_CORE/Src/USB_LL_Device.c \
../USB_LL_CORE/Src/USB_LL_Hardware.c \
../USB_LL_CORE/Src/USB_LL_Host.c \
../USB_LL_CORE/Src/USB_LL_Interrupts.c \
../USB_LL_CORE/Src/USB_LL_Interrupts_Device.c \
../USB_LL_CORE/Src/USB_LL_Interrupts_Host.c 

C_DEPS += \
./USB_LL_CORE/Src/USB_LL_Device.d \
./USB_LL_CORE/Src/USB_LL_Hardware.d \
./USB_LL_CORE/Src/USB_LL_Host.d \
./USB_LL_CORE/Src/USB_LL_Interrupts.d \
./USB_LL_CORE/Src/USB_LL_Interrupts_Device.d \
./USB_LL_CORE/Src/USB_LL_Interrupts_Host.d 

OBJS += \
./USB_LL_CORE/Src/USB_LL_Device.o \
./USB_LL_CORE/Src/USB_LL_Hardware.o \
./USB_LL_CORE/Src/USB_LL_Host.o \
./USB_LL_CORE/Src/USB_LL_Interrupts.o \
./USB_LL_CORE/Src/USB_LL_Interrupts_Device.o \
./USB_LL_CORE/Src/USB_LL_Interrupts_Host.o 


# Each subdirectory must supply rules for building sources it contributes
USB_LL_CORE/Src/%.o USB_LL_CORE/Src/%.su USB_LL_CORE/Src/%.cyclo: ../USB_LL_CORE/Src/%.c USB_LL_CORE/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407ZETx -DSTM32 -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Include" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/Skipper Drivers/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_LL_CORE/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_HOST/Core/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_CDC_DEVICE/Core/Inc" -I"${workspace_loc:/USB Device And Host Development/USART/USART_APP/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-USB_LL_CORE-2f-Src

clean-USB_LL_CORE-2f-Src:
	-$(RM) ./USB_LL_CORE/Src/USB_LL_Device.cyclo ./USB_LL_CORE/Src/USB_LL_Device.d ./USB_LL_CORE/Src/USB_LL_Device.o ./USB_LL_CORE/Src/USB_LL_Device.su ./USB_LL_CORE/Src/USB_LL_Hardware.cyclo ./USB_LL_CORE/Src/USB_LL_Hardware.d ./USB_LL_CORE/Src/USB_LL_Hardware.o ./USB_LL_CORE/Src/USB_LL_Hardware.su ./USB_LL_CORE/Src/USB_LL_Host.cyclo ./USB_LL_CORE/Src/USB_LL_Host.d ./USB_LL_CORE/Src/USB_LL_Host.o ./USB_LL_CORE/Src/USB_LL_Host.su ./USB_LL_CORE/Src/USB_LL_Interrupts.cyclo ./USB_LL_CORE/Src/USB_LL_Interrupts.d ./USB_LL_CORE/Src/USB_LL_Interrupts.o ./USB_LL_CORE/Src/USB_LL_Interrupts.su ./USB_LL_CORE/Src/USB_LL_Interrupts_Device.cyclo ./USB_LL_CORE/Src/USB_LL_Interrupts_Device.d ./USB_LL_CORE/Src/USB_LL_Interrupts_Device.o ./USB_LL_CORE/Src/USB_LL_Interrupts_Device.su ./USB_LL_CORE/Src/USB_LL_Interrupts_Host.cyclo ./USB_LL_CORE/Src/USB_LL_Interrupts_Host.d ./USB_LL_CORE/Src/USB_LL_Interrupts_Host.o ./USB_LL_CORE/Src/USB_LL_Interrupts_Host.su

.PHONY: clean-USB_LL_CORE-2f-Src

