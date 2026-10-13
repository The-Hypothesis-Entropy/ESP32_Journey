#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "timer";

static void on_tick(void *arg)
{
    ESP_LOGI(TAG, "tick");
}

void app_main(void)
{
    esp_timer_handle_t h; //timer handle

    const esp_timer_create_args_t args = { // create args (task structure is default over isr)
        .callback = on_tick,
        .name = "my_timer",
    };

    esp_timer_create(&args, &h); //create timer
    esp_timer_start_periodic(h, 500000);   /*start timer: 500 ms, in microseconds */
}