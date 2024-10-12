#include "credentials.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

static const char *TAG = "http_server";

esp_err_t alarm_get_handler(httpd_req_t *req) {
  const char *resp_str = "alarm on";
  httpd_resp_send(req, resp_str, strlen(resp_str));
  return ESP_OK;
}

httpd_handle_t start_server(void) {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  if (httpd_start(&server, &config) == ESP_OK) {
    httpd_uri_t alarm_uri = {.uri = "/alarm",
                             .method = HTTP_GET,
                             .handler = alarm_get_handler,
                             .user_ctx = NULL};
    httpd_register_uri_handler(server, &alarm_uri);
  }
  return server;
}

void wifi_init(const char *ssid, const char *password) {
  esp_err_t ret;

  // Initialize NVS
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_INVALID_STATE) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // Initialize Wi-Fi
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  // Set Wi-Fi mode to station
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

  // Configure Wi-Fi connection
  wifi_config_t wifi_config = {
      .sta = {.ssid = SSID, .password = PSK},
  };
  strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid) - 1);
  strncpy((char *)wifi_config.sta.password, password,
          sizeof(wifi_config.sta.password) - 1);

  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  // Connect to the Wi-Fi
  ESP_ERROR_CHECK(esp_wifi_connect());

  ESP_LOGI(TAG, "Connecting to WiFi...");
}

void app_main(void) {
  // Replace with your Wi-Fi credentials
  const char *ssid = "your_SSID";
  const char *password = "your_PASSWORD";

  wifi_init(ssid, password);

  // Wait for Wi-Fi connection
  while (true) {
    wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
      if (ap_info.primary != 0) {
        ESP_LOGI(TAG, "Connected to WiFi");
        break;
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }

  start_server();
  ESP_LOGI(TAG, "Server started");
}
