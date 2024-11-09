#include "eWifi.h"

const char *TAG_WIFI = "WIFI";
httpd_handle_t WebServer = NULL; // Manejador del servidor HTTP
// Manejador de eventos de WiFi
void wifi_event_handler(void *arg, esp_event_base_t event_base,int32_t event_id, void *event_data)
{
    // Maneja los eventos de conexión y desconexión de estaciones
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_WIFI, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
    } 
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG_WIFI, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

// Inicializa el modo SoftAP

void wifi_init_softap(void)
{
    init_nvs();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
            .pmf_cfg = {
                    .required = true,
            },
        },
    };
    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG_WIFI, "SSID: %s PASSW:%s",ESP_WIFI_SSID, ESP_WIFI_PASS);
}


// Manejador de Error 404
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" home directory
    httpd_resp_set_hdr(req, "Location", "/home.html");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGW(TAG_WIFI, "Redirecting to home");
    return ESP_OK;
}

// Función para iniciar el servidor web
void start_webserver(httpd_uri_t*uri_handlers,size_t uri_size) {

    wifi_init_softap();
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = uri_size;
    
    // Inicia el servidor HTTP
    ESP_ERROR_CHECK(httpd_start(&WebServer, &config));
    

    // Registra las URIs
    for(unsigned i =0; i < uri_size; i++)
        httpd_register_uri_handler(WebServer, &uri_handlers[i]);
   
    httpd_register_err_handler(WebServer, HTTPD_404_NOT_FOUND, http_404_error_handler);
    
    init_users();
}




