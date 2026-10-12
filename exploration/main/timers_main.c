#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "timer";

static void on_tick(void *arg)
{
    ESP_LOGI(TAG, "tick");
}

void app_main(void)
{
    esp_timer_handle_t h;

    const esp_timer_create_args_t args = {
        .callback = on_tick,
        .name = "my_timer",
    };

    esp_timer_create(&args, &h);
    esp_timer_start_periodic(h, 500000);   /* 500 ms, in microseconds */
}