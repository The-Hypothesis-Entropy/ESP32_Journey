/*
INITIALIZE THE I2C BUS
ADD A DEVICE TO THE BUS
USE TRANSMIT AND RECEIVE FUNCTIONS TO SEND AND RECEIVE DATA.
*/

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define SDA_PIN  GPIO_NUM_4
#define SCL_PIN  GPIO_NUM_5

//static const char *TAG = "i2c";

static const char *TAG = "mpu";

void app_main_base(void)
{
    /* --- 1. Create the bus (the physical I2C peripheral) --- */
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port             = I2C_NUM_0,
        .sda_io_num           = SDA_PIN,
        .scl_io_num           = SCL_PIN,
        .clk_source           = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt    = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus;
    i2c_new_master_bus(&bus_cfg, &bus);

    /* --- 2. Attach a device (a slave on the bus) --- */
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = 0x68,          /* MPU6050, 7-bit unshifted */
        .scl_speed_hz    = 100000,
    };

    i2c_master_dev_handle_t dev;
    i2c_master_bus_add_device(bus, &dev_cfg, &dev);

    /* --- 3. Read a register (WHO_AM_I at 0x75) --- */
    uint8_t reg   = 0x75;
    uint8_t value = 0;

    i2c_master_transmit_receive(dev, &reg, 1, &value, 1, 100);

    ESP_LOGI(TAG, "WHO_AM_I = 0x%02X", value);
}

//------------------------------------------------------------------------------


void app_main(void)
{
    /* --- Bus --- */
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = SDA_PIN,
        .scl_io_num = SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    i2c_master_bus_handle_t bus;
    i2c_new_master_bus(&bus_cfg, &bus);

    /* --- Device --- */
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = 0x68,
        .scl_speed_hz    = 100000,
    };
    i2c_master_dev_handle_t dev;
    i2c_master_bus_add_device(bus, &dev_cfg, &dev);

    /* --- Wake the sensor (write 0x00 to PWR_MGMT_1 at 0x6B) --- */
    uint8_t wake[2] = { 0x6B, 0x00 };
    i2c_master_transmit(dev, wake, 2, 100);

    /* --- Read 14 bytes starting at 0x3B --- */
    uint8_t reg = 0x3B;
    uint8_t raw[14];

    while (1) {
        i2c_master_transmit_receive(dev, &reg, 1, raw, 14, 100);

        /* Combine high/low bytes into signed 16-bit values */
        int16_t ax = (int16_t)((raw[0]  << 8) | raw[1]);
        int16_t ay = (int16_t)((raw[2]  << 8) | raw[3]);
        int16_t az = (int16_t)((raw[4]  << 8) | raw[5]);
        int16_t gx = (int16_t)((raw[8]  << 8) | raw[9]);
        int16_t gy = (int16_t)((raw[10] << 8) | raw[11]);
        int16_t gz = (int16_t)((raw[12] << 8) | raw[13]);

        ESP_LOGI(TAG, "ax=%d ay=%d az=%d | gx=%d gy=%d gz=%d",
                 ax, ay, az, gx, gy, gz);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
