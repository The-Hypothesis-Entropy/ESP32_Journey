#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_http_server.h"

/*
static const char *TAG = "WEB_SERVER";

// Handler for the root URL "/"
static esp_err_t root_get_handler(httpd_req_t *req) {
    const char* resp_str = "<h1>ESP32 Web Server is Live!</h1>";
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// URI configuration for the root path
static const httpd_uri_t root_uri = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

// Function to start the web server
httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Increase stack size for the server's internal task
    config.stack_size = 8192;

    // Start the HTTP server
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Server started successfully.");
        // Register the URI handler
        httpd_register_uri_handler(server, &root_uri);
        return server;
    }
    ESP_LOGE(TAG, "Failed to start server");
    return NULL;
}

// Wi-Fi initialization for Access Point mode
static void wifi_init_softap(void) {
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "ESP32_Server",
            .password = "password123",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA2_PSK
        },
    };
    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    esp_wifi_start();
    ESP_LOGI(TAG, "SoftAP started. SSID: ESP32_Server, Password: password123");
}

void app_main_basic(void) {
    // 1. Setup Wi-Fi in AP mode
    wifi_init_softap();

    // 2. Start the web server
    start_webserver();

    // 3. Main loop does nothing
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

*/

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define AP_SSID    "ESP32_LED"
#define AP_PSWD    "password123"
#define LED_PIN    GPIO_NUM_8

static const char *TAG = "server";
static bool led_state = false;

/* ---------- HTTP handlers (identical to station version) ---------- */

static esp_err_t root_handler(httpd_req_t *req)
{
    const char *html =
        "<!DOCTYPE html><html><body>"
        "<h1>ESP32 LED Control</h1>"
        "<p>LED is currently: <b>%s</b></p>"
        "<form action=\"/toggle\" method=\"GET\">"
        "<button type=\"submit\">Toggle LED</button>"
        "</form>"
        "</body></html>";

    char buf[512];
    snprintf(buf, sizeof(buf), html, led_state ? "ON" : "OFF");
    httpd_resp_send(req, buf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t toggle_handler(httpd_req_t *req)
{
    led_state = !led_state;
    gpio_set_level(LED_PIN, led_state ? 1 : 0);

    httpd_resp_set_status(req, "303 See Other");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

static const httpd_uri_t root_uri = {
    .uri     = "/",
    .method  = HTTP_GET,
    .handler = root_handler,
};

static const httpd_uri_t toggle_uri = {
    .uri     = "/toggle",
    .method  = HTTP_GET,
    .handler = toggle_handler,
};

/* ---------- Server start ---------- */

static void start_server(void)
{
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    httpd_start(&server, &cfg);
    httpd_register_uri_handler(server, &root_uri);
    httpd_register_uri_handler(server, &toggle_uri);

    ESP_LOGI(TAG, "Server started. Open http://192.168.4.1");
}

/* ---------- Wi-Fi in AP mode ---------- */

static void wifi_init_ap(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    wifi_config_t ap_cfg = {0};
    snprintf((char *)ap_cfg.ap.ssid,     sizeof(ap_cfg.ap.ssid),
             "%s", AP_SSID);
    snprintf((char *)ap_cfg.ap.password, sizeof(ap_cfg.ap.password),
             "%s", AP_PSWD);
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_set_mode(WIFI_MODE_AP);
    esp_wifi_set_config(WIFI_IF_AP, &ap_cfg);
    esp_wifi_start();
}

/* ---------- Main ---------- */

void app_main(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    wifi_init_ap();

    /* In AP mode the IP is fixed (192.168.4.1). No event needed. */
    start_server();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}