/*
Credits:
Driver is adapted from https://github.com/AlexFabre/MCP23x08
The overall concept is in line with the guidelines described
at https://embeddedartistry.com/blog/2019/08/05/practical-decoupling-techniques-applied-to-a-c-based-radio-driver/

HOW TO USE DRIVER

1. Initialize SPI
2. Implement the SPI xfer function: int (*spi_transfer)(uint8_t *tx_buffer, uint8_t tx_len, uint8_t *rx_buffer, uint8_t *rx_len)
3. Initialize the driver configuration structure:

    MCP23S08_config_t mcp_config = {
        .address = ADDR_00,         // Defined by device pins state
        .registers.IOCON = 0x28,    // Consult datasheet
        .registers.IODIR = 0xf0,    // Configure I/O port direction
        .spi_transfer = spi_transfer
    };

4. Initialize the driver and check the return value. The function makes sure
we're able to write and read to the registers:

    printf("MCP23S08 Init: %d\r\n", MCP23S08_init(&mcp_config));

    MCP23S08_Port_t mcp_expander;
    driver.GPIO = MCP23S08_readport(&config);
*/

#ifndef MCP23S08_H
#define MCP23S08_H

#include <stdint.h>

typedef union {
    uint8_t GPIO;
    struct {
        uint8_t GPIO0:1;
        uint8_t GPIO1:1;
        uint8_t GPIO2:1;
        uint8_t GPIO3:1;
        uint8_t GPIO4:1;
        uint8_t GPIO5:1;
        uint8_t GPIO6:1;
        uint8_t GPIO7:1;
    };
} MCP23S08_Port_t;

typedef enum {
    MCP23S08_IODIR,			/* IODIR: I/O DIRECTION REGISTER */
    MCP23S08_IPOL,			/* IPOL: INPUT POLARITY PORT REGISTER If a bit is set, the corresponding GPIO register bit will reflect the inverted value on the pin */
    MCP23S08_GPINTEN,		/* INTERRUPT-ON-CHANGE CONTROL REGISTER */
    MCP23S08_DEFVAL,		/* DEFAULT COMPARE REGISTER FOR INTERRUPT-ON-CHANGE */
    MCP23S08_INTCON,		/* INTERRUPT CONTROL REGISTER */
    MCP23S08_IOCON,			/* I/O EXPANDER CONFIGURATION REGISTER */
    MCP23S08_GPPU,			/* GPPU: GPIO PULL-UP RESISTOR REGISTER */
    MCP23S08_INTF,			/* READ ONLY : INTF: INTERRUPT FLAG REGISTER (ADDR 0x07) */
    MCP23S08_INTCAP,		/* READ ONLY : INTCAP: INTERRUPT CAPTURED VALUE FOR PORT REGISTER */
    MCP23S08_GPIO,			/* Port adress */
    MCP23S08_OLAT,			/* OUTPUT LATCH REGISTER */
} MCP23S08_register_t;

typedef enum {
    ADDR_00 = 0b00,
    ADDR_01 = 0b01,
    ADDR_10 = 0b10,
    ADDR_11 = 0b11
} MCP23S08_address_e;

typedef struct {
    uint8_t IODIR;
    uint8_t IPOL;
    uint8_t GPINTEN;
    uint8_t DEFVAL;
    uint8_t INTCON;
    uint8_t IOCON;
    uint8_t GPPU;
    uint8_t default_GPIO;
    uint8_t OLAT;
} MCP23S08_reg_config_t;

/*
* MCP23S08 Configuration structure
* MCP23S08_address_e address - device hardware address, as defined by state of pins A[0:1]
* MCP23S08_reg_config_t registers - device registers structure
* int (*spi_transfer)(uint8_t*, uint8_t, uint8_t*, uint8_t*) - pointer to function used
* for SPI transmit and receive. The function should return 0 on success.
*/
typedef struct {
    MCP23S08_address_e address;
    MCP23S08_reg_config_t registers;
    /**
     * @brief Interface for SPI read/write function
     * @param tx_buffer Array of bytes to be written to the SPI peripheral
     * @param tx_len Length of data to be transmitted
     * @param rx_buffer Array of bytes to hold the response. Ensure it is at least big enough as tx_buffer
     * @param rx_len Pointer to uint8_t, which receives the length of the received data
     * @return 0 on success
     */
    int (*spi_transfer)(uint8_t *tx_buffer, uint8_t tx_len, uint8_t *rx_buffer, uint8_t *rx_len);
} MCP23S08_config_t;

/**
 * @brief MCP23S08_init Initialization function. Shall be called before using the API.
 * @param config Pointer to MCP23S08 configuration structure
 * @return 1 on success, 0 on failure
 */
uint8_t MCP23S08_init(const MCP23S08_config_t *config);

/**
 * @brief MCP23S08_register_read Read a single register
 * @param config Pointer to MCP23S08 configuration structure
 * @param reg Register to read
 * @return Value of register. On failure, function returns 0x00, use with caution
 */
uint8_t MCP23S08_register_read(const MCP23S08_config_t *config, MCP23S08_register_t reg);

/**
 * @brief MCP23S08_register_write Write value to register
 * @param config Pointer to MCP23S08 configuration structure
 * @param reg Register to swrite
 * @param value Value to write into register
 * @return 1 on success, 0 on failure
 */
uint8_t MCP23S08_register_write(const MCP23S08_config_t *config, MCP23S08_register_t reg, uint8_t value);

/**
 * @brief MCP23S08_port_read Read value of the PORT register
 * @param config Pointer to MCP23S08 configuration structure
 * @return Value of PORT register
 */
uint8_t MCP23S08_port_read(const MCP23S08_config_t *config);

/**
 * @brief MCP23S08_port_write Write a value to the PORT register
 * @param config Pointer to MCP23S08 configuration structure
 * @param value Value to write to PORT register
 * @return
 */
uint8_t MCP23S08_port_write(const MCP23S08_config_t *config, uint8_t value);

#endif // MCP23S08_H
