#include "mcp23s08.h"
#include <stddef.h>

#define MCP23S08_READ_CONTROLE_BYTE 	0x41u
#define MCP23S08_WRITE_CONTROLE_BYTE 	0x40u

#define MCP23S08_SPI_CONTROL_BYTE       0x00
#define MCP23S08_SPI_WRITE_BIT          0x00
#define MCP23S08_SPI_READ_BIT           0x01
#define MCP23S08_SPI_REGISTER_ADDRESS_BYTE   0x01
#define MCP23S08_SPI_VALUE_BYTE         0x02
#define MCP23S08_SPI_WRITE_REGISTER_BUFFER_LEN 0x03

/* PRIVATE API */

int _mcp23s08_write_register(const MCP23S08_config_t *config, MCP23S08_register_t reg, uint8_t value)
{
    uint8_t tx_buffer[3] = {0, 0, 0};
    uint8_t rx_len = 0x00;
    uint8_t *rx_buffer = NULL;

    /* Create SPI control byte; last bit set to 1*/
    tx_buffer[MCP23S08_SPI_CONTROL_BYTE] = MCP23S08_WRITE_CONTROLE_BYTE | (config->address << 1) | MCP23S08_SPI_WRITE_BIT;
    tx_buffer[MCP23S08_SPI_REGISTER_ADDRESS_BYTE] = reg;
    tx_buffer[MCP23S08_SPI_VALUE_BYTE] = value;

    return config->spi_transfer(tx_buffer, MCP23S08_SPI_WRITE_REGISTER_BUFFER_LEN, rx_buffer, &rx_len);
}

uint8_t _mcp23s08_read_register(const MCP23S08_config_t *config, MCP23S08_register_t reg)
{
    uint8_t tx_buffer[3] = {0, 0, 0,};
    uint8_t rx_buffer[3] = {0, 0, 0,};
    uint8_t rx_len = 0;

    /* Create SPI control byte; last bit set to 0*/
    tx_buffer[MCP23S08_SPI_CONTROL_BYTE] = MCP23S08_READ_CONTROLE_BYTE | (config->address << 1) | MCP23S08_SPI_READ_BIT;
    tx_buffer[MCP23S08_SPI_REGISTER_ADDRESS_BYTE] = reg;
    tx_buffer[MCP23S08_SPI_VALUE_BYTE] = 0x00;

    config->spi_transfer(tx_buffer, MCP23S08_SPI_WRITE_REGISTER_BUFFER_LEN, rx_buffer, &rx_len);

    return rx_buffer[MCP23S08_SPI_VALUE_BYTE];
}

/* PUBLIC API */

uint8_t MCP23S08_init(const MCP23S08_config_t *config)
{
    uint8_t iocon = 0;

    /* Write & read back IOCON first */
    _mcp23s08_write_register(config, MCP23S08_IOCON, config->registers.IOCON);
    _mcp23s08_write_register(config, MCP23S08_IODIR, config->registers.IODIR);
    _mcp23s08_write_register(config, MCP23S08_IPOL, config->registers.IPOL);
    _mcp23s08_write_register(config, MCP23S08_GPINTEN, config->registers.GPINTEN);
    _mcp23s08_write_register(config, MCP23S08_DEFVAL, config->registers.DEFVAL);
    _mcp23s08_write_register(config, MCP23S08_INTCON, config->registers.INTCON);
    _mcp23s08_write_register(config, MCP23S08_GPPU, config->registers.GPPU);
    _mcp23s08_write_register(config, MCP23S08_GPIO, config->registers.default_GPIO);
    _mcp23s08_write_register(config, MCP23S08_OLAT, config->registers.OLAT);

    iocon = _mcp23s08_read_register(config, MCP23S08_IOCON);

    return (iocon == config->registers.IOCON);
}

uint8_t MCP23S08_register_read(const MCP23S08_config_t *config, MCP23S08_register_t reg)
{
    return _mcp23s08_read_register(config, reg);
}

uint8_t MCP23S08_register_write(const MCP23S08_config_t *config, MCP23S08_register_t reg, uint8_t value)
{
    _mcp23s08_write_register(config, reg, value);

    return (value == _mcp23s08_read_register(config, reg));
}

uint8_t MCP23S08_port_read(const MCP23S08_config_t *config)
{
    return _mcp23s08_read_register(config, MCP23S08_GPIO);
}

uint8_t MCP23S08_port_write(const MCP23S08_config_t *config, uint8_t value)
{
    _mcp23s08_write_register(config, MCP23S08_GPIO, value);

    return (value == _mcp23s08_read_register(config, MCP23S08_GPIO));
}
