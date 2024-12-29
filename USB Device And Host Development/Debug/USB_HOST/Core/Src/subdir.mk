################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USB_HOST/Core/Src/USB_Host.c \
../USB_HOST/Core/Src/USB_Host_Device_Manager.c \
../USB_HOST/Core/Src/USB_Host_Enumerate.c \
../USB_HOST/Core/Src/USB_Host_Hub.c \
../USB_HOST/Core/Src/USB_Host_Pipes.c \
../USB_HOST/Core/Src/USB_Host_Transfers.c 

C_DEPS += \
./USB_HOST/Core/Src/USB_Host.d \
./USB_HOST/Core/Src/USB_Host_Device_Manager.d \
./USB_HOST/Core/Src/USB_Host_Enumerate.d \
./USB_HOST/Core/Src/USB_Host_Hub.d \
./USB_HOST/Core/Src/USB_Host_Pipes.d \
./USB_HOST/Core/Src/USB_Host_Transfers.d 

OBJS += \
./USB_HOST/Core/Src/USB_Host.o \
./USB_HOST/Core/Src/USB_Host_Device_Manager.o \
./USB_HOST/Core/Src/USB_Host_Enumerate.o \
./USB_HOST/Core/Src/USB_Host_Hub.o \
./USB_HOST/Core/Src/USB_Host_Pipes.o \
./USB_HOST/Core/Src/USB_Host_Transfers.o 


# Each subdirectory must supply rules for building sources it contributes
USB_HOST/Core/Src/%.o USB_HOST/Core/Src/%.su USB_HOST/Core/Src/%.cyclo: ../USB_HOST/Core/Src/%.c USB_HOST/Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407ZETx -DSTM32 -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/CMSIS/Include" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/Drivers/Skipper Drivers/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_LL_CORE/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_HOST/Core/Inc" -I"C:/Users/jackh/git/STM32_Skipper_Development/USB Device And Host Development/USB_HOST/Config" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-USB_HOST-2f-Core-2f-Src

clean-USB_HOST-2f-Core-2f-Src:
	-$(RM) ./USB_HOST/Core/Src/USB_Host.cyclo ./USB_HOST/Core/Src/USB_Host.d ./USB_HOST/Core/Src/USB_Host.o ./USB_HOST/Core/Src/USB_Host.su ./USB_HOST/Core/Src/USB_Host_Device_Manager.cyclo ./USB_HOST/Core/Src/USB_Host_Device_Manager.d ./USB_HOST/Core/Src/USB_Host_Device_Manager.o ./USB_HOST/Core/Src/USB_Host_Device_Manager.su ./USB_HOST/Core/Src/USB_Host_Enumerate.cyclo ./USB_HOST/Core/Src/USB_Host_Enumerate.d ./USB_HOST/Core/Src/USB_Host_Enumerate.o ./USB_HOST/Core/Src/USB_Host_Enumerate.su ./USB_HOST/Core/Src/USB_Host_Hub.cyclo ./USB_HOST/Core/Src/USB_Host_Hub.d ./USB_HOST/Core/Src/USB_Host_Hub.o ./USB_HOST/Core/Src/USB_Host_Hub.su ./USB_HOST/Core/Src/USB_Host_Pipes.cyclo ./USB_HOST/Core/Src/USB_Host_Pipes.d ./USB_HOST/Core/Src/USB_Host_Pipes.o ./USB_HOST/Core/Src/USB_Host_Pipes.su ./USB_HOST/Core/Src/USB_Host_Transfers.cyclo ./USB_HOST/Core/Src/USB_Host_Transfers.d ./USB_HOST/Core/Src/USB_Host_Transfers.o ./USB_HOST/Core/Src/USB_Host_Transfers.su

.PHONY: clean-USB_HOST-2f-Core-2f-Src

