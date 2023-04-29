AS   = arm-none-eabi-gcc -c -x assembler-with-cpp
CC   = arm-none-eabi-gcc -c
LD   = arm-none-eabi-g++
HX   = arm-none-eabi-objcopy -O ihex
BN   = arm-none-eabi-objcopy -O binary
SZ   = arm-none-eabi-size

TGT  = relay-controller
DIR  = ./build
LDS  = ./stm32f0.ld

DEF  = USE_STDPERIPH_DRIVER
DEF += STM32F030

SRC  = ./spd
SRC += ./src
SRC += ./system

INC  = ./cmsis
INC += ./spd
INC += ./src
INC += ./system

MCU  = -ffunction-sections -fdata-sections
MCU += -fsigned-char -ffreestanding -fmessage-length=0
MCU += -mcpu=cortex-m0 -mthumb

CCF  = $(addprefix -D, $(DEF)) $(addprefix -I, $(INC))
CCF += -std=c99 -Wa,-adhlns="$@.lst"
CCF += -MMD -MP -MF"$(patsubst %.o,%.d,$@)" -MT"$(patsubst %.o,%.d,$@)"
CCF += $(MCU)

ASF  = -Wa,-adhlns="$@.lst"
ASF += -MMD -MP -MF"$(patsubst %.o,%.d,$@)" -MT"$(patsubst %.o,%.d,$@)"
ASF += $(MCU)

LDF  = -T$(LDS)
LDF += -nostartfiles -Xlinker --gc-sections -u _printf_float
LDF += --specs=nosys.specs
LDF += $(MCU)

OBJ  = $(addprefix $(DIR)/, $(patsubst %.c, %.o, $(wildcard  $(addsuffix /*.c, $(SRC)))))
OBJ += $(addprefix $(DIR)/, $(patsubst %.S, %.o, $(wildcard  $(addsuffix /*.S, $(SRC)))))

OCD  = -d0
OCD += -f ./openocd.cfg
OCD += -c 'init' -c 'reset init' -c 'halt'
OCD += -c 'flash write_image erase $(DIR)/$(TGT).elf'
OCD += -c 'reset run' -c 'shutdown'

all: dirs $(DIR)/$(TGT).hex $(DIR)/$(TGT).bin size

dirs:
	@mkdir -p $(addprefix $(DIR)/, $(wildcard  $(SRC)))

flash:
	@printf "FLASH:\n"
	@sudo openocd $(OCD)
	
clean:
	@echo '----------------------------------------------------------------'
	@echo 'Cleaning all trash...'
	@echo '----------------------------------------------------------------'
	@rm -rf $(DIR)

$(DIR)/$(TGT).hex: $(DIR)/$(TGT).elf
	@$(HX) $(DIR)/$(TGT).elf $(DIR)/$(TGT).hex

$(DIR)/$(TGT).bin: $(DIR)/$(TGT).elf
	@$(BN) $(DIR)/$(TGT).elf $(DIR)/$(TGT).bin

size:
	@echo '----------------------------------------------------------------'
	@$(SZ) $(DIR)/$(TGT).elf
	@echo '----------------------------------------------------------------'
	
$(DIR)/$(TGT).elf: $(OBJ)
	@printf '(LD) $@\n'
	@$(LD) $(LDF) -o $@ $^

$(DIR)/%.o: %.c
	@printf '(CC) $<\n'
	@$(CC) $(CCF) -o $@ $<

$(DIR)/%.o: %.S
	@printf '(AS) $<\n'
	@$(AS) $(ASF) -o $@ $<
