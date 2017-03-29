################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/PROTOCOL.c \
../src/ProcessCmd.c \
../src/UART.c \
../src/adc.c \
../src/alarm.c \
../src/dac.c \
../src/flashHandle.c \
../src/hal_entry.c \
../src/sbs_cmds.c \
../src/tc_fc_table.c 

C_DEPS += \
./src/PROTOCOL.d \
./src/ProcessCmd.d \
./src/UART.d \
./src/adc.d \
./src/alarm.d \
./src/dac.d \
./src/flashHandle.d \
./src/hal_entry.d \
./src/sbs_cmds.d \
./src/tc_fc_table.d 

OBJS += \
./src/PROTOCOL.o \
./src/ProcessCmd.o \
./src/UART.o \
./src/adc.o \
./src/alarm.o \
./src/dac.o \
./src/flashHandle.o \
./src/hal_entry.o \
./src/sbs_cmds.o \
./src/tc_fc_table.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\e2_studio\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -D_RENESAS_SYNERGY_ -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy_cfg\ssp_cfg\bsp" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy_cfg\ssp_cfg\driver" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy\ssp\inc" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy\ssp\inc\bsp" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy\ssp\inc\driver\api" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy\ssp\inc\driver\instances" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\src" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\src\synergy_gen" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy_cfg\ssp_cfg\framework\el" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy\ssp\inc\framework\el" -I"C:\vc\emcore\LEML\48pin\12-2016\9267_4\synergy\ssp\src\framework\el\tx" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


