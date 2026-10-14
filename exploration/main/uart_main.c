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

#define TX_PIN GPIO_NUM_3
#define RX_PIN GPIO_NUM_1

void app_main(void){
  //Config:
  uart_config_t cfg = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
  };

  //Apply config to UART_NUM_1 (User's uart)
  uart_param_config(UART_NUM_1, &cfg);

  //Assign pins
  uart_set_pin(UART_NUM_1, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

  //Install driver
  uart_driver_install(UART_NUM_1, 256, 256, 0, NULL, 0);



  //Send a string
  const char *msg = "Hello! This is ESP32 communicating through UART!!!\n";
  uart_write_bytes(UART_NUM_1, msg, strlen(msg)); //copy msg to UART buffer and then return while sending occurs in background. if buffer<msg size, will block 

  //Receive buffer
  uint8_t rx_buf[64];

  while(1){
    int n = uart_read_bytes(UART_NUM_1, rx_buf, sizeof(rx_buf), pdMS_TO_TICKS(100)); //copies n bytes from UART buffer to rx_buf.
    if (n > 0){
      ESP_LOGI("UART", "Received %d bytes: %.*s", n, n, rx_buf);
      uart_write_bytes(UART_NUM_1, rx_buf, n); // Echo back the received data
    }
  }
}