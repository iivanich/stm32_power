# stm32_power
stm32f103C8 program for controlling a boost converter for solar powered boiler

This program depends on STM32Cube_FW_F1_V1.4.0 which is not in the repository, but can be downloaded from STM's site.

It can be flashed on stm32f103 MCUs, e.g. this one: http://wiki.stm32duino.com/index.php?title=Blue_Pill 
The device is easily obtainable, e.g. on aliexpress

The code implements the control for a boost converter based on the IR2110 chip, which steps up the 30V output of a 200W solar panel to 
about 80V input for an electrical 3KW boiler. It implements MPPT and demonstrates the usage of the following STM32 MCU peripherals:

1. TIM1 (dual channel complementary PWM with dead time)
2. ADC (3 channel, DMA transfer to memory)
3. USART (complete driver with fifo, IRQ based)
4. DAC (basic usage)
5. FLASH (basic usage)
