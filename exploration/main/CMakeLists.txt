#set(PROJECT "exploration_main")
#set(PROJECT "gpio_main")
set(PROJECT "uart_main")
#set(PROJECT "exploration_main")
#set(PROJECT "exploration_main")
#set(PROJECT "exploration_main")
#set(PROJECT "exploration_main")
#set(PROJECT "blink_example_main")

idf_component_register(SRCS "${PROJECT}.c"
                       INCLUDE_DIRS "."
                       REQUIRES esp_driver_uart esp_driver_gpio)
