################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_codeflash.c \
../synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_codeflash_extra.c \
../synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_dataflash.c \
../synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_flash_common.c 

C_DEPS += \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_codeflash.d \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_codeflash_extra.d \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_dataflash.d \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_flash_common.d 

OBJS += \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_codeflash.o \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_codeflash_extra.o \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_dataflash.o \
./synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/hw_flash_common.o 


# Each subdirectory must supply rules for building sources it contributes
synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/%.o: ../synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	C:\Renesas\e2_studio\eclipse\../Utilities/isdebuild arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -D_RENESAS_SYNERGY_ -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy_cfg\ssp_cfg\bsp" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy_cfg\ssp_cfg\driver" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy\ssp\inc" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy\ssp\inc\bsp" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy\ssp\inc\bsp\cmsis\Include" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy\ssp\inc\driver\api" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy\ssp\inc\driver\instances" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\src" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\src\synergy_gen" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy_cfg\ssp_cfg\framework\el" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy\ssp\inc\framework\el" -I"C:\vc\emcore\LEML\48pin\release\2--3-2017\9267_4\synergy\ssp\src\framework\el\tx" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


