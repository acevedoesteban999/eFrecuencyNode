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


bool get_int_param_value(const char *input, const char *key, int *value);

bool get_float_param_value(const char *input, const char *key, float *value);

void set_uri_handlers(httpd_uri_t *uri_handlers, size_t uris_size);

void set_login_uri_handler();

void start_webserver(uint16_t max_uri);
