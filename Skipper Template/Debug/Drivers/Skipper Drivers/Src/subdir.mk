################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/Skipper\ Drivers/Src/Skipper_Clock.c 

C_DEPS += \
./Drivers/Skipper\ Drivers/Src/Skipper_Clock.d 

OBJS += \
./Drivers/Skipper\ Drivers/Src/Skipper_Clock.o 


# Each subdirectory must supply rules for building sources it contributes
Drivers/Skipper\ Drivers/Src/Skipper_Clock.o: ../Drivers/Skipper\ Drivers/Src/Skipper_Clock.c Drivers/Skipper\ Drivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F407ZETx -DSTM32 -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"C:/Users/Jack Herron/git/STM32_Skipper_Development/Skipper Template/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Jack Herron/git/STM32_Skipper_Development/Skipper Template/Drivers/CMSIS/Include" -I"C:/Users/Jack Herron/git/STM32_Skipper_Development/Skipper Template/Drivers/Skipper Drivers/Inc" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -pedantic -Wmissing-include-dirs -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"Drivers/Skipper Drivers/Src/Skipper_Clock.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-Skipper-20-Drivers-2f-Src

clean-Drivers-2f-Skipper-20-Drivers-2f-Src:
	-$(RM) ./Drivers/Skipper\ Drivers/Src/Skipper_Clock.cyclo ./Drivers/Skipper\ Drivers/Src/Skipper_Clock.d ./Drivers/Skipper\ Drivers/Src/Skipper_Clock.o ./Drivers/Skipper\ Drivers/Src/Skipper_Clock.su

.PHONY: clean-Drivers-2f-Skipper-20-Drivers-2f-Src

