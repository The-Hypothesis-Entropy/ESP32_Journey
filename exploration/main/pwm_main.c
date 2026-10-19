/*
LEDC generates pwm on any gpio. Each output needs two things; 
-> timer: sets frequency and resolution
-> channel: attaches timer to a pin and holds the duty
multiple channels can share one timer (same frequency) or use different timers (different frequencies)
*/

#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define PWM_PIN GPIO_NUM_4

void app_main(void){
  //Configure timer: 1KHz, 10-bit resolution
ledc_timer_config_t timer_cfg = {
  .speed_mode = LEDC_LOW_SPEED_MODE,
  .duty_resolution = LEDC_TIMER_10_BIT,
  .timer_num = LEDC_TIMER_0,
  .freq_hz = 1000,
  .clk_cfg = LEDC_AUTO_CLK,
};
ledc_timer_config(&timer_cfg);

//Attach channel 0 to timer 0 on pwm_pin, 50% duty
ledc_channel_config_t chan_cfg = {
  .gpio_num = PWM_PIN,
  .speed_mode = LEDC_LOW_SPEED_MODE,
  .channel = LEDC_CHANNEL_0,
  .timer_sel = LEDC_TIMER_0,
  .duty = 512, //50% of 1023 (2^10-1)
};
ledc_channel_config(&chan_cfg);
//ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, 1023);
//ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
//ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 5000); //change frequency to 1KHz
//change  at runtime
while(1){
  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 5000); //change frequency to 1KHz
  //ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  vTaskDelay(pdMS_TO_TICKS(1000));

  ledc_set_freq(LEDC_LOW_SPEED_MODE, LEDC_TIMER_0, 100); //change frequency to 1KHz
  //ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
  vTaskDelay(pdMS_TO_TICKS(1000)); 
}
}