/*
RESET PIN, SET PIN DIRECTION
USE SET_LEVEL AND GET_LEVEL

YOU CAN ALSO USE PULLUP_EN, PULLDOWN_EN
INSTALL A ISR SERVICE(0); ONLY ONCE!
ATTACH A ISR HANDLER (qualify handler with IRAM_ATTR)
START ISR USING SET_INTR_TYPE
*/

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define LED_PIN GPIO_NUM_8

#define BTN_PIN GPIO_NUM_9

static volatile uint8_t press_count = 0;

void app_main_output(void *arg)
{
  /*
  You could make a config and pass it's pointer to gpio_config();
  Or you could simply reset the pin and then set it's direction
  */
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    for(;;) {
        gpio_set_level(LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        gpio_set_level(LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main_input(void *arg){
  gpio_reset_pin(BTN_PIN);
  gpio_set_direction(BTN_PIN, GPIO_MODE_INPUT);
  //gpio_pull_en(BTN_PIN);

  while(1){
    int level = gpio_get_level(BTN_PIN);
    if(level == 1){
      ESP_LOGI("GPIO", "INPUT IS HIGH");
    }
    else{
      ESP_LOGI("GPIO", "INPUT IS LOW");
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}


void IRAM_ATTR btn_isr(void *arg){
  press_count++;
  //ESP_LOGI("Interupt", "Button pressed %d times", press_count); DONT YOU DARE!
} 

void app_main(void){
  gpio_reset_pin(BTN_PIN);
  gpio_set_direction(BTN_PIN, GPIO_MODE_INPUT);
  gpio_pullup_en(BTN_PIN);

  gpio_install_isr_service(0); //install the default intr service, do this only once.
  gpio_isr_handler_add(BTN_PIN, btn_isr, NULL); //add handler AFTER ISR_SERVICE!
  gpio_set_intr_type(BTN_PIN, GPIO_INTR_NEGEDGE); //enables intr automatically
  
  int tracker = press_count;

  for(;;){
    if(tracker != press_count){
      ESP_LOGI("Interupt", "Button pressed %d times", press_count);
      tracker = press_count;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }

}


/*
void app_main_task(void){

  xTaskCreate(app_main_output, "app_main_output", 2048, NULL, 5, NULL);
  xTaskCreate(app_main_input, "app_main_input", 2048, NULL, 5, NULL);
}
*/