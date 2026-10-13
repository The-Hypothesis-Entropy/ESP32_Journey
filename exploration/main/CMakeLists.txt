#set(PROJECT "exploration_main")
#set(PROJECT "gpio_main")
#set(PROJECT "uart_main")
#set(PROJECT "timers_main")
#set(PROJECT "i2c_main")
set(PROJECT "spi_main")
#set(PROJECT "exploration_main")
#set(PROJECT "blink_example_main")

idf_component_register(SRCS "${PROJECT}.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_uart esp_driver_gpio esp_timer esp_driver_i2c esp_driver_gpio)
