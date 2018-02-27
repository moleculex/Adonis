################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/_write.c \
../src/app.c \
../src/flash.c \
../src/kx122.c \
../src/main.c \
../src/power.c \
../src/rn2483.c 

OBJS += \
./src/_write.o \
./src/app.o \
./src/flash.o \
./src/kx122.o \
./src/main.o \
./src/power.o \
./src/rn2483.o 

C_DEPS += \
./src/_write.d \
./src/app.d \
./src/flash.d \
./src/kx122.d \
./src/main.d \
./src/power.d \
./src/rn2483.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DSTM32F30X -DUSE_STDPERIPH_DRIVER -DHSI_VALUE=8000000 -I"../inc" -I"../system/inc" -I"../system/inc/cmsis" -I"../system/inc/stm32f3-stdperiph" -I"/Users/peterbruce/Documents/workspace/Adonis/inc/FreeRTOS" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


