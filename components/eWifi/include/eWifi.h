#pragma once
#include "esp_http_server.h"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "eAuth.h"
#include "eStore.h"

// Definiciones de WiFi
#define ESP_WIFI_SSID "ESP32-SIMULATOR"
#define ESP_WIFI_PASS "ESP32_PASSW"
#define MAX_STA_CONN 4
#define BUFF_LEN 100
#define MAX_URI 10

extern const char *TAG_WIFI;
extern httpd_handle_t WebServer;
// Declaraciones de variables HTML
extern const char login_start[] asm("_binary_login_html_start");
extern const char login_end[] asm("_binary_login_html_end");
extern const char home_start[] asm("_binary_home_html_start");
extern const char home_end[] asm("_binary_home_html_end");

// Prototipos de funciones
void wifi_init_softap(void);
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
void start_webserver(httpd_uri_t *uri_handlers, size_t uri_size);
esp_err_t stop_wifi_handle(void);
