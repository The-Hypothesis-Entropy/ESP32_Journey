/*
CREATE A UART CONFIG, APPLY IT TO UART1
SET THE PINS FOR UART1
INSTALL THE DRIVER FOR UART1
USE write_bytes and read_bytes
*/

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/uart.h"

#define GPS_UART_PORT   UART_NUM_1
#define GPS_RX_PIN      5      /* connect GPS TX to this pin */
#define GPS_TX_PIN      4      /* connect GPS RX to this pin (optional) */
#define GPS_BAUD        38400   /* M10 default; use 115200 if configured */
#define BUF_SIZE        1024

static const char *TAG = "gps";

void app_main(){
  //Config:
  uart_config_t cfg = { 
    .baud_rate = GPS_BAUD,
    .data_bits = UART_DATA_8_BITS,
  };

  //Apply config to UART_NUM_1 (User's uart)
  uart_param_config(GPS_UART_PORT, &cfg);

  //Assign pins
  uart_set_pin(GPS_UART_PORT, GPS_TX_PIN, GPS_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  //Install driver
  uart_driver_install(GPS_UART_PORT, 256, 256, 0, NULL, 0);

  uint8_t buf[256];

  while (1) {
        int n = uart_read_bytes(GPS_UART_PORT, buf, sizeof(buf),
                                pdMS_TO_TICKS(100));
        if (n > 0) {
            buf[n] = '\0';
            ESP_LOGI(TAG, "%s", buf);
        }
        else {
            ESP_LOGI(TAG, "No data received");
        }
  }
}