#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs.h>
#include <nvs_flash.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/timers.h>
#include <string.h>

#include "mcp23s08.h"

static const int MCP_SPI_HOST = 2;
static const int CLOCK_MHZ = 10;
static const int MISO = 19;
static const int MOSI = 23;
static const int SCLK = 18;
static const int MCP_CS = 32; 

static const char *TAG = "MCP23S01 Demo";

static spi_device_handle_t mcp_spi_handle;      
static MCP23S08_config_t mcp_config;

void mcp_toggle_task(void *params)
{
    MCP23S08_Port_t expander;

    while (1)
    {
        expander.GPIO = MCP23S08_port_read(&mcp_config);
        ESP_LOGI(TAG, "Port value: 0x%02X", expander.GPIO);
        // MCP23S08_port_write(&mcp_config, ~expander.GPIO);     
        vTaskDelay(2000 / portTICK_PERIOD_MS);        
    }
}

esp_err_t spi_initialize(spi_host_device_t spi_host, int clock_mhz, int miso, int mosi, int sclk, int cs)
{
    esp_err_t ret;

    spi_bus_config_t bus_cfg = {
        .miso_io_num = miso,
        .mosi_io_num = mosi,
        .sclk_io_num = sclk,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 32,
    };

    ret = spi_bus_initialize(spi_host, &bus_cfg, SPI_DMA_CH_AUTO);

    switch (ret)
    {
    case ESP_ERR_INVALID_STATE:
        ESP_LOGE(TAG, "SPI driver already initialized.");
        break;
    case ESP_OK:
        ESP_LOGI(TAG, "SPI driver initialized");
        break;
    default:
        ESP_LOGE(TAG, "Failed to initialize bus.");
        ESP_ERROR_CHECK(ret); /* Bomb if error is not any of the above */
    }

    spi_device_interface_config_t devcfg = {
        .command_bits = 0,
        .address_bits = 0,
        .dummy_bits = 0,
        .mode = 0,
        .spics_io_num = cs,
        .clock_speed_hz = 1000 * 1000 * clock_mhz,
        .flags = 0,
        .queue_size = 1,
    };

    ESP_ERROR_CHECK(spi_bus_add_device(spi_host, &devcfg, &mcp_spi_handle));
    gpio_set_pull_mode(cs, GPIO_PULLUP_ONLY);   /* Use only when board does not have a pull-up on the CS line */

    ESP_LOGI(TAG, "SPI bus initialized!");

    return 0;
}

/* Abstract interface defined in mcp23s08.h in definition of MCP23S08_config_t */
int spi_transfer(uint8_t *tx_buffer, uint8_t tx_len, uint8_t *rx_buffer, uint8_t *rx_len)
{
    esp_err_t ret = ESP_OK;

    spi_transaction_t trans = {
        .length = 8 * tx_len,
        .tx_buffer = tx_buffer,
        .rx_buffer = rx_buffer,
    };

    if (spi_device_polling_transmit(mcp_spi_handle, &trans) != ESP_OK)
    {
        ESP_LOGE(TAG, "%s(%d): spi transmit failed", __FUNCTION__, __LINE__);
        ret = ESP_FAIL;
    }

    return ret;
}

void app_main(void)
{
    /* Initialize NVS */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    /* Bomb on fail */
    ESP_ERROR_CHECK(ret);

    spi_initialize(MCP_SPI_HOST, CLOCK_MHZ, MISO, MOSI, SCLK, MCP_CS);

    mcp_config.address = ADDR_00;               /* Device HW address */
    mcp_config.registers.IOCON = 0x28;          /* SEQOP disabled, hardware address enabled */
    mcp_config.registers.IODIR = 0x0F;          /* Only upper 4 bits are outputs */
    mcp_config.spi_transfer = spi_transfer;     /* Provide the SPI r/w function; note that driver does not do any error-checking */

    if (MCP23S08_init(&mcp_config))
    {
        ESP_LOGI(TAG, "Initialized MCP23S08");        
        xTaskCreate(mcp_toggle_task, NULL, 1024 * 2, NULL, 1, NULL);
    }
    else
    {
        ESP_LOGE(TAG, "Failed initializing MCP23S08!");
    }
}