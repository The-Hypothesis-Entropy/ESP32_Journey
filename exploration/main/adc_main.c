/*
CREATE ADC UNIT VIA HANDLE AND CONFIG
CONFIGURE ADC CHANNEL VIA HANDLE AND CONFIG
READ ADC VALUE
*/

#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define ADC_CHANNEL   ADC_CHANNEL_0   /* GPIO 0 on ESP32-C3 (coincidence. always verify) */

static const char *TAG = "adc";

void app_main(void)
{
    /* 1. Create the unit */
    adc_oneshot_unit_handle_t adc;
    adc_oneshot_unit_init_cfg_t unit_cfg = {0};
    adc_oneshot_new_unit(&unit_cfg, &adc);

    /* 2. Configure the channel */
    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,   /* 0 - 2500 mV */
    };
    adc_oneshot_config_channel(adc, ADC_CHANNEL, &chan_cfg);

    /* 3. Read */
    int raw;

    while (1) {
        adc_oneshot_read(adc, ADC_CHANNEL, &raw);

        /* raw is 0..4095, full scale is 2500 mV at DB_12 */
        int mv = (raw * 2500) / 4095;

        ESP_LOGI(TAG, "raw=%d  mv=%d", raw, mv);

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}