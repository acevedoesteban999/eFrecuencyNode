#pragma once
#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "lwip/inet.h"
#include "eAuth.h"
#include "eStore.h"

// Definiciones de WiFi
#define ESP_WIFI_SSID "ESP32-SIMULATOR"
#define ESP_WIFI_PASS "ESP32_PASSW"
#define MAX_STA_CONN 4
#define BUFF_LEN 100

extern const char *TAG_WIFI;
extern httpd_handle_t WebServer;
// Declaraciones de variables HTML
extern const char login_start[] asm("_binary_login_html_start");
extern const char login_end[] asm("_binary_login_html_end");
extern const char home_start[] asm("_binary_home_html_start");
extern const char home_end[] asm("_binary_home_html_end");

// Prototipos de funciones
esp_err_t wifi_init_softap(void);
esp_err_t home_get_handler(httpd_req_t *req);
esp_err_t restart_post_handler(httpd_req_t *req);
esp_err_t init_post_handler(httpd_req_t *req);
esp_err_t login_get_handler(httpd_req_t *req);
esp_err_t login_post_handler(httpd_req_t *req);
esp_err_t logout_handler(httpd_req_t *req);
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
esp_err_t start_webserver(void);
esp_err_t stop_wifi_handle(void);