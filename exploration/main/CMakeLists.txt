#set(PROJECT "exploration_main")
#set(PROJECT "gpio_main")
#set(PROJECT "uart_main")
#set(PROJECT "timers_main")
#set(PROJECT "i2c_main")
#set(PROJECT "spi_main")
#set(PROJECT "m10_main")
#set(PROJECT "adc_main")
#set(PROJECT "pwm_main")
#set(PROJECT "blink_example_main")
#set(PROJECT "wifi_main")
set(PROJECT "server_main")
#set(PROJECT "pwm_main")
#set(PROJECT "pwm_main")

idf_component_register(SRCS "server_main.c" "wifi_main.c" "pwm_main.c" "${PROJECT}.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_uart esp_driver_gpio esp_timer esp_driver_i2c esp_adc esp_driver_ledc esp_wifi esp_event esp_netif nvs_flash esp_http_server)
