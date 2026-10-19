#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define WIFI_SSID     "Entropy"
#define WIFI_PASSWORD "01workWork"

static const char *TAG = "wifi";

static void event_handler(void *arg, esp_event_base_t base,
                          int32_t id, void *data)
{
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(TAG, "disconnected, retrying...");
        esp_wifi_connect();
    }
    else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *e = (ip_event_got_ip_t *)data;
        ESP_LOGI(TAG, "got IP: " IPSTR, IP2STR(&e->ip_info.ip));
    }
}

static void wifi_sta_init(void){
    nvs_flash_init(); /* 1. NVS — required for Wi-Fi calibration data */
    esp_netif_init(); /* 2. TCP/IP stack */
    esp_event_loop_create_default(); /* 3. Default event loop */
    //esp_netif_create_default_wifi_sta(); /* 4. Attach Wi-Fi to LwIP as a station */
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT(); /* 5. Wi-Fi driver init */
    esp_wifi_init(&init_cfg);
}

static void wifi_ap_init(void){
    nvs_flash_init(); /* 1. NVS — required for Wi-Fi calibration data */
    esp_netif_init(); /* 2. TCP/IP stack */
    esp_event_loop_create_default(); /* 3. Default event loop */
    esp_netif_create_default_wifi_ap(); 
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT(); /* 5. Wi-Fi driver init */
    esp_wifi_init(&init_cfg);
}

static void wifi_apsta_init(void)
{
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    /* Netifs are created in the create function below */
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&init_cfg);
}

void wifi_create_station(const char *hostname, uint8_t *id, uint8_t *pswd)
{
    wifi_sta_init();   /* NVS + netif_init + event loop + esp_wifi_init */

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    esp_netif_set_hostname(sta_netif, hostname);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                        &event_handler, NULL, NULL);

    esp_wifi_set_mode(WIFI_MODE_STA); 

    wifi_config_t wifi_cfg = {0};
    snprintf((char *)wifi_cfg.sta.ssid, sizeof(wifi_cfg.sta.ssid),
             "%s", (char *)id);
    snprintf((char *)wifi_cfg.sta.password, sizeof(wifi_cfg.sta.password),
             "%s", (char *)pswd);

    esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg);
    esp_wifi_start();
}

void wifi_create_ap(uint8_t *id, uint8_t *pswd)
{
    wifi_ap_init();
    esp_wifi_set_mode(WIFI_MODE_AP);

    wifi_config_t ap_cfg = {0};   /* zero everything first */
    snprintf((char *)ap_cfg.ap.ssid,     sizeof(ap_cfg.ap.ssid),
             "%s", (char *)id);
    snprintf((char *)ap_cfg.ap.password, sizeof(ap_cfg.ap.password),
             "%s", (char *)pswd);
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;

    esp_wifi_set_config(WIFI_IF_AP, &ap_cfg);
    esp_wifi_start();
}

void wifi_scan(void){
    nvs_flash_init();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();

    vTaskDelay(pdMS_TO_TICKS(200));    /* let the driver finish starting */

    uint16_t count = 20;                /* INPUT: max records */
    wifi_ap_record_t records[20];

    esp_wifi_scan_start(NULL, true);
    esp_wifi_scan_get_ap_records(&count, records);   /* OUTPUT: actual count */

    for (int i = 0; i < count; i++) {
        ESP_LOGI("scan", "%s  %d dBm  ch%d",
                 records[i].ssid, records[i].rssi, records[i].primary);
    }
}

void wifi_station_getip(void){
  esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  esp_netif_ip_info_t ip_info;
  esp_netif_get_ip_info(netif, &ip_info);

  ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info.ip));
  ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&ip_info.netmask));
  ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&ip_info.gw));
}

void wifi_ap_getip(void){
  esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_AP_DEF");
  esp_netif_ip_info_t ip_info;
  esp_netif_get_ip_info(netif, &ip_info);

  ESP_LOGI(TAG, "IP: " IPSTR, IP2STR(&ip_info.ip));
  ESP_LOGI(TAG, "Netmask: " IPSTR, IP2STR(&ip_info.netmask));
  ESP_LOGI(TAG, "Gateway: " IPSTR, IP2STR(&ip_info.gw));
}

void wifi_create_apsta(const char *hostname,
                       uint8_t *sta_ssid, uint8_t *sta_pswd,
                       uint8_t *ap_ssid,  uint8_t *ap_pswd)
{
    wifi_apsta_init();

    /* Two netifs — one for the station side, one for the AP side */
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    esp_netif_t *ap_netif  = esp_netif_create_default_wifi_ap();
    (void)ap_netif;   /* unused, but must exist */

    esp_netif_set_hostname(sta_netif, hostname);

    /* Register event handlers */
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &event_handler, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                        &event_handler, NULL, NULL);

    /* Set mode to AP+STA */
    esp_wifi_set_mode(WIFI_MODE_APSTA);

    /* Station config (what router to connect to) */
    wifi_config_t sta_cfg = {0};
    snprintf((char *)sta_cfg.sta.ssid,     sizeof(sta_cfg.sta.ssid),
             "%s", (char *)sta_ssid);
    snprintf((char *)sta_cfg.sta.password, sizeof(sta_cfg.sta.password),
             "%s", (char *)sta_pswd);
    esp_wifi_set_config(WIFI_IF_STA, &sta_cfg);

    /* AP config (what network to broadcast) */
    wifi_config_t ap_cfg = {0};
    snprintf((char *)ap_cfg.ap.ssid,     sizeof(ap_cfg.ap.ssid),
             "%s", (char *)ap_ssid);
    snprintf((char *)ap_cfg.ap.password, sizeof(ap_cfg.ap.password),
             "%s", (char *)ap_pswd);
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    esp_wifi_set_config(WIFI_IF_AP, &ap_cfg);

    /* Start */
    esp_wifi_start();
}

void app_main(void)
{
    //app_main_sta();
    //app_main_ap();
    //scan_wifi();
    wifi_create_apsta("Hypothesis", (uint8_t *)"Entropy", (uint8_t *)"01workWork",
                      (uint8_t *)"hotspot", (uint8_t *)"hahahehe");
    while(1){
      wifi_station_getip();
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
