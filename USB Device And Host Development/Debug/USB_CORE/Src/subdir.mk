################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USB_CORE/Src/USB_LL_Device.c \
../USB_CORE/Src/USB_LL_Hardware.c \
../USB_CORE/Src/USB_LL_Host.c \
../USB_CORE/Src/USB_LL_Interrupts.c \
../USB_CORE/Src/USB_LL_Interrupts_Device.c \
../USB_CORE/Src/USB_LL_Interrupts_Host.c 

C_DEPS += \
./USB_CORE/Src/USB_LL_Device.d \
./USB_CORE/Src/USB_LL_Hardware.d \
./USB_CORE/Src/USB_LL_Host.d \
./USB_CORE/Src/USB_LL_Interrupts.d \
./USB_CORE/Src/USB_LL_Interrupts_Device.d \
./USB_CORE/Src/USB_LL_Interrupts_Host.d 

OBJS += \
./USB_CORE/Src/USB_LL_Device.o \
./USB_CORE/Src/USB_LL_Hardware.o \
./USB_CORE/Src/USB_LL_Host.o \
./USB_CORE/Src/USB_LL_Interrupts.o \
./USB_CORE/Src/USB_LL_Interrupts_Device.o \
./USB_CORE/Src/USB_LL_Interrupts_Host.o 


# Each subdirectory must supply rules for building sources it contributes
USB_CORE/Src/%.o USB_CORE/Src/%.su USB_CORE/Src/%.cyclo: ../USB_CORE/Src/%.c USB_CORE/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407ZETx -DSTM32 -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"C:/Users/Jack Herron/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Jack Herron/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Include" -I"C:/Users/Jack Herron/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/Skipper Drivers/Inc" -I"C:/Users/Jack Herron/git/STM32_Skipper_Development/USB Device And Host Development/USB_CORE/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-USB_CORE-2f-Src

clean-USB_CORE-2f-Src:
	-$(RM) ./USB_CORE/Src/USB_LL_Device.cyclo ./USB_CORE/Src/USB_LL_Device.d ./USB_CORE/Src/USB_LL_Device.o ./USB_CORE/Src/USB_LL_Device.su ./USB_CORE/Src/USB_LL_Hardware.cyclo ./USB_CORE/Src/USB_LL_Hardware.d ./USB_CORE/Src/USB_LL_Hardware.o ./USB_CORE/Src/USB_LL_Hardware.su ./USB_CORE/Src/USB_LL_Host.cyclo ./USB_CORE/Src/USB_LL_Host.d ./USB_CORE/Src/USB_LL_Host.o ./USB_CORE/Src/USB_LL_Host.su ./USB_CORE/Src/USB_LL_Interrupts.cyclo ./USB_CORE/Src/USB_LL_Interrupts.d ./USB_CORE/Src/USB_LL_Interrupts.o ./USB_CORE/Src/USB_LL_Interrupts.su ./USB_CORE/Src/USB_LL_Interrupts_Device.cyclo ./USB_CORE/Src/USB_LL_Interrupts_Device.d ./USB_CORE/Src/USB_LL_Interrupts_Device.o ./USB_CORE/Src/USB_LL_Interrupts_Device.su ./USB_CORE/Src/USB_LL_Interrupts_Host.cyclo ./USB_CORE/Src/USB_LL_Interrupts_Host.d ./USB_CORE/Src/USB_LL_Interrupts_Host.o ./USB_CORE/Src/USB_LL_Interrupts_Host.su

.PHONY: clean-USB_CORE-2f-Src

