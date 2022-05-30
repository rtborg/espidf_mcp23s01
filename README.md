# MCP23S08 SPI Expander driver for espidf

Original version: [Alex Fabre](https://github.com/AlexFabre/MCP23x08)

## Driver Description
The driver is intended to work with any board which supports SPI. The user is required to provide implementation of the function `int (*spi_transfer)(uint8_t *tx_buffer, uint8_t tx_len, uint8_t *rx_buffer, uint8_t *rx_len)`. Initialization sequence is descibed in the driver header file.

## espidf example application
File `main.c` shows a minimal application to read and write data to the GPIO expander:
* SPI initialization 
* `spi_transfer` function implementation
* MCP23S08 initialization
* FreeRTOS task which reads GPIO port and toggles its value  

--- 
