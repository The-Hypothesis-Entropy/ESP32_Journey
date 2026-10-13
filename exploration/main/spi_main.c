#include "driver/spi_master.h"
#include "esp_log.h"
#include "driver/gpio.h"

#define MOSI_PIN  GPIO_NUM_4
#define MISO_PIN  GPIO_NUM_5
#define SCLK_PIN  GPIO_NUM_6
#define CS_PIN    GPIO_NUM_7

static const char *TAG = "spi";

void app_main(void)
{
    /* --- 1. Initialize the bus (the SPI peripheral) --- */
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = MOSI_PIN,
        .miso_io_num = MISO_PIN,
        .sclk_io_num = SCLK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };
    spi_bus_initialize(SPI2_HOST, &bus_cfg, SPI_DMA_CH_AUTO);

    /* --- 2. Attach a device (a slave on the bus) --- */
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1 * 1000 * 1000,   /* 1 MHz */
        .mode           = 0,                  /* CPOL=0, CPHA=0 */
        .spics_io_num   = CS_PIN,             /* driver toggles CS automatically */
        .queue_size     = 1,
    };
    spi_device_handle_t dev;
    spi_bus_add_device(SPI2_HOST, &dev_cfg, &dev);

    /* --- 3. Transmit / receive --- */
    uint8_t tx[4] = { 0x01, 0x02, 0x03, 0x04 };
    uint8_t rx[4] = { 0 };

    spi_transaction_t txn = {
        .length    = 4 * 8,       /* length in BITS, not bytes */
        .tx_buffer = tx,
        .rx_buffer = rx,
    };
    /*
    ONE TRANSACTION: FOR LENGTH BITS, TRANSMIT FROM TX, RECEIVE TO RX (BOTH POINTERS. LOOK CAREFULLY!)
    */
    spi_device_transmit(dev, &txn); 

    ESP_LOGI(TAG, "rx = %02X %02X %02X %02X", rx[0], rx[1], rx[2], rx[3]);
}