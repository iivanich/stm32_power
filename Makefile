DEBUG = 1
PLATFORM = STM32F103C8
GCC_BIN = /home/ivanich/projects/stm32fx/gcc-arm-none-eabi-5_3/bin/
STCUBE = /home/ivanich/projects/stm32fx/STM32Cube_FW_F1_V1.4.0
STHAL = Drivers/STM32F1xx_HAL_Driver
STBSP = Drivers/BSP
STCMSIS = Drivers/CMSIS
STCMSISDEVICE = Drivers/CMSIS/Device/ST
STGCCSTARTUP = STM32F1xx/Source/Templates/gcc
PROJECT = ./bin/power

OBJECTS = src/complementary/main.o src/complementary/adc.o src/complementary/dac.o src/complementary/pwm.o src/complementary/handlers.o src/complementary/stm32f1xx_hal_msp.o src/complementary/stm32f1xx_it.o src/system/system_stm32f1xx.o src/fifo/fifo.o src/dbg/dbg.o src/usart/usart.o src/cmd_loop/cmd_loop.o src/tokenizer/tokenizer.o

ifeq ($(PLATFORM), STM32F103C8)
	LIB_OBJECT_STARTUP = $(STCUBE)/$(STCMSISDEVICE)/$(STGCCSTARTUP)/startup_stm32f103xb.o
endif


LIB_OBJECTS = $(LIB_OBJECT_STARTUP) $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_adc.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_adc_ex.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_can.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_cec.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_cortex.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_crc.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_dac.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_dac_ex.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_dma.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_eth.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_flash.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_gpio.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_hcd.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_i2c.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_irda.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_iwdg.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_msp_template.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_nand.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_nor.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_pccard.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_pcd.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_pcd_ex.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_pwr.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_rcc.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_rcc_ex.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_rtc.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_rtc_ex.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_sd.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_smartcard.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_spi.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_sram.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_tim.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_tim_ex.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_uart.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_usart.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_hal_wwdg.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_ll_sdmmc.o $(STCUBE)/$(STHAL)/Src/stm32f1xx_ll_usb.o $(STCUBE)/$(STBSP)/STM32F1xx_Nucleo/stm32f1xx_nucleo.o

SYS_OBJECTS =

INCLUDE_PATHS = -I. -I src/system -I src/tokenizer -I src/fifo -I src/cmd_loop -I src/usart -I src/dbg -I $(STCUBE)/$(STHAL)/Inc -I $(STCUBE)/$(STCMSISDEVICE)/STM32F1xx/Include -I $(STCUBE)/$(STCMSIS)/Include -I $(STCUBE)/$(STBSP)/STM32F1xx_Nucleo/
LIBRARY_PATHS =
LIBRARIES =

ifeq ($(PLATFORM), STM32F103C8)
	LINKER_SCRIPT = $(STCUBE)/$(STCMSISDEVICE)/$(STGCCSTARTUP)/linker/STM32F103C8Tx_FLASH.ld
endif

###############################################################################
AS      = $(GCC_BIN)arm-none-eabi-as
CC      = $(GCC_BIN)arm-none-eabi-gcc
CPP     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)arm-none-eabi-objdump
SIZE    = $(GCC_BIN)arm-none-eabi-size


CPU = -mcpu=cortex-m4 -mthumb
CC_FLAGS = $(CPU) -c -g3 -fno-common -fmessage-length=0 -Wall -Wextra -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -MMD -MP

ifeq ($(PLATFORM), STM32F103C8)
	CC_SYMBOLS = -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC -DTARGET_STM -DRECURSIVE_IRQ_ENDIS -DSTM32F103xB
endif

LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -u _printf_float -u _scanf_float -Wl,-Map=$(PROJECT).map,--cref
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys


ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNDEBUG -Os
endif

.PHONY: all clean lst size

all: $(PROJECT).bin $(PROJECT).hex size

clean:
	rm	-f	$(PROJECT).bin	$(PROJECT).elf	$(PROJECT).hex	$(PROJECT).map	$(PROJECT).lst	$(OBJECTS) $(LIB_OBJECTS)	$(DEPS)

.asm.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.s.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.S.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<

.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<

$(PROJECT).elf: $(OBJECTS) $(LIB_OBJECTS) $(SYS_OBJECTS)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)

$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -O ihex $< $@

$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

lst: $(PROJECT).lst

size: $(PROJECT).elf
	$(SIZE) $(PROJECT).elf

DEPS = $(OBJECTS:.o=.d)	$(SYS_OBJECTS:.o=.d) $(LIB_OBJECTS:.o=.d)
-include $(DEPS)
