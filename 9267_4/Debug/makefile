################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include synergy/ssp/src/driver/r_sci_uart/subdir.mk
-include synergy/ssp/src/driver/r_sci_spi/subdir.mk
-include synergy/ssp/src/driver/r_sci_common/subdir.mk
-include synergy/ssp/src/driver/r_ioport/subdir.mk
-include synergy/ssp/src/driver/r_gpt/subdir.mk
-include synergy/ssp/src/driver/r_flash_lp/hw/target/flash_lp_core/subdir.mk
-include synergy/ssp/src/driver/r_flash_lp/hw/target/subdir.mk
-include synergy/ssp/src/driver/r_flash_lp/subdir.mk
-include synergy/ssp/src/driver/r_elc/subdir.mk
-include synergy/ssp/src/driver/r_dtc/subdir.mk
-include synergy/ssp/src/driver/r_cgc/hw/target/subdir.mk
-include synergy/ssp/src/driver/r_cgc/hw/common/subdir.mk
-include synergy/ssp/src/driver/r_cgc/hw/subdir.mk
-include synergy/ssp/src/driver/r_cgc/subdir.mk
-include synergy/ssp/src/driver/r_adc/hw/target/s7g2/subdir.mk
-include synergy/ssp/src/driver/r_adc/hw/target/s3a7/subdir.mk
-include synergy/ssp/src/driver/r_adc/hw/target/s124/subdir.mk
-include synergy/ssp/src/driver/r_adc/hw/subdir.mk
-include synergy/ssp/src/driver/r_adc/subdir.mk
-include synergy/ssp/src/bsp/mcu/s124/subdir.mk
-include synergy/ssp/src/bsp/mcu/all/subdir.mk
-include synergy/ssp/src/bsp/cmsis/Device/RENESAS/S7G2/Source/subdir.mk
-include synergy/ssp/src/bsp/cmsis/Device/RENESAS/S3A7/Source/subdir.mk
-include synergy/ssp/src/bsp/cmsis/Device/RENESAS/S128/Source/subdir.mk
-include synergy/ssp/src/bsp/cmsis/Device/RENESAS/S124/Source/subdir.mk
-include synergy/ssp/src/bsp/board/s124_user/subdir.mk
-include src/synergy_gen/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(SRC_DEPS)),)
-include $(SRC_DEPS)
endif
ifneq ($(strip $(PP_DEPS)),)
-include $(PP_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(P_DEPS)),)
-include $(P_DEPS)
endif
ifneq ($(strip $(INC_DEPS)),)
-include $(INC_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
9267_4.hex \

SECONDARY_SIZE += \
9267_4.siz \


# All Target
all: 9267_4.elf secondary-outputs

# Tool invocations
9267_4.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM C++ Linker'
	$(file > 9267_4.elf.in,-mcpu=cortex-m0plus -mthumb -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g3 -T C:\\vc\\emcore\\LEML\\48pin\\release\\2--3-2017\\9267_4\\script\\S124.ld -Xlinker --gc-sections -L "C:\\vc\\emcore\\LEML\\48pin\\release\\2--3-2017\\9267_4\\synergy\\ssp\\src\\bsp\\cmsis\\DSP_Lib\\cm0plus_gcc" -L "C:\\vc\\emcore\\LEML\\48pin\\release\\2--3-2017\\9267_4\\synergy\\ssp\\src\\framework\\el\\tx\\cm0plus_gcc" -Wl,-Map,"9267_4.map" --specs=rdimon.specs -o "9267_4.elf" -Wl,--start-group $(OBJS) $(USER_OBJS) $(LIBS) -Wl,--end-group)
	arm-none-eabi-g++ @"9267_4.elf.in"
	@echo 'Finished building target: $@'
	@echo ' '

9267_4.hex: 9267_4.elf
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	arm-none-eabi-objcopy -O ihex "9267_4.elf"  "9267_4.hex"
	@echo 'Finished building: $@'
	@echo ' '

9267_4.siz: 9267_4.elf
	@echo 'Invoking: Cross ARM GNU Print Size'
	arm-none-eabi-size --format=berkeley "9267_4.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
ifneq ($(strip $(C_UPPER_DEPS)),)
	-$(file > clean_C_UPPER_DEPS.tmp, $(C_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_C_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_C_UPPER_DEPS.tmp
endif
ifneq ($(strip $(SECONDARY_SIZE)),)
	-$(file > clean_SECONDARY_SIZE.tmp, $(SECONDARY_SIZE))
	-@ xargs -s 6000 -t -a clean_SECONDARY_SIZE.tmp $(RM)
	-@$(RM) clean_SECONDARY_SIZE.tmp
endif
ifneq ($(strip $(P_DEPS)),)
	-$(file > clean_P_DEPS.tmp, $(P_DEPS))
	-@ xargs -s 6000 -t -a clean_P_DEPS.tmp $(RM)
	-@$(RM) clean_P_DEPS.tmp
endif
ifneq ($(strip $(INC_DEPS)),)
	-$(file > clean_INC_DEPS.tmp, $(INC_DEPS))
	-@ xargs -s 6000 -t -a clean_INC_DEPS.tmp $(RM)
	-@$(RM) clean_INC_DEPS.tmp
endif
ifneq ($(strip $(C_DEPS)),)
	-$(file > clean_C_DEPS.tmp, $(C_DEPS))
	-@ xargs -s 6000 -t -a clean_C_DEPS.tmp $(RM)
	-@$(RM) clean_C_DEPS.tmp
endif
ifneq ($(strip $(SRC_DEPS)),)
	-$(file > clean_SRC_DEPS.tmp, $(SRC_DEPS))
	-@ xargs -s 6000 -t -a clean_SRC_DEPS.tmp $(RM)
	-@$(RM) clean_SRC_DEPS.tmp
endif
ifneq ($(strip $(PP_DEPS)),)
	-$(file > clean_PP_DEPS.tmp, $(PP_DEPS))
	-@ xargs -s 6000 -t -a clean_PP_DEPS.tmp $(RM)
	-@$(RM) clean_PP_DEPS.tmp
endif
ifneq ($(strip $(CC_DEPS)),)
	-$(file > clean_CC_DEPS.tmp, $(CC_DEPS))
	-@ xargs -s 6000 -t -a clean_CC_DEPS.tmp $(RM)
	-@$(RM) clean_CC_DEPS.tmp
endif
ifneq ($(strip $(C++_DEPS)),)
	-$(file > clean_C++_DEPS.tmp, $(C++_DEPS))
	-@ xargs -s 6000 -t -a clean_C++_DEPS.tmp $(RM)
	-@$(RM) clean_C++_DEPS.tmp
endif
ifneq ($(strip $(OBJS)),)
	-$(file > clean_OBJS.tmp, $(OBJS))
	-@ xargs -s 6000 -t -a clean_OBJS.tmp $(RM)
	-@$(RM) clean_OBJS.tmp
endif
ifneq ($(strip $(CXX_DEPS)),)
	-$(file > clean_CXX_DEPS.tmp, $(CXX_DEPS))
	-@ xargs -s 6000 -t -a clean_CXX_DEPS.tmp $(RM)
	-@$(RM) clean_CXX_DEPS.tmp
endif
ifneq ($(strip $(SECONDARY_FLASH)),)
	-$(file > clean_SECONDARY_FLASH.tmp, $(SECONDARY_FLASH))
	-@ xargs -s 6000 -t -a clean_SECONDARY_FLASH.tmp $(RM)
	-@$(RM) clean_SECONDARY_FLASH.tmp
endif
ifneq ($(strip $(ASM_DEPS)),)
	-$(file > clean_ASM_DEPS.tmp, $(ASM_DEPS))
	-@ xargs -s 6000 -t -a clean_ASM_DEPS.tmp $(RM)
	-@$(RM) clean_ASM_DEPS.tmp
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
	-$(file > clean_S_UPPER_DEPS.tmp, $(S_UPPER_DEPS))
	-@ xargs -s 6000 -t -a clean_S_UPPER_DEPS.tmp $(RM)
	-@$(RM) clean_S_UPPER_DEPS.tmp
endif
ifneq ($(strip $(CPP_DEPS)),)
	-$(file > clean_CPP_DEPS.tmp, $(CPP_DEPS))
	-@ xargs -s 6000 -t -a clean_CPP_DEPS.tmp $(RM)
	-@$(RM) clean_CPP_DEPS.tmp
endif
	-$(RM)  9267_4.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
