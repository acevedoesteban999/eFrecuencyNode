#include "eWifi.h"

const char *TAG_WIFI = "WIFI";
httpd_handle_t WebServer = NULL; 
char* _login_asm_start = NULL;
char* _login_asm_end = NULL;
char redirect_404[MAX_404_BUFFER_SIZE];

void wifi_event_handler(void *arg, esp_event_base_t event_base,int32_t event_id, void *event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI(TAG_WIFI, "station " MACSTR " join, AID=%d", MAC2STR(event->mac), event->aid);
    } 
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI(TAG_WIFI, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    init_nvs();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(
        esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &wifi_event_handler,
            NULL,
            NULL
        )
    );

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

bool get_int_param_value(const char *input, const char *key, int *value) {
    char pattern[20];
    snprintf(pattern, sizeof(pattern), "%s=", key);

    char *pos = strstr(input, pattern);
    if (pos) {
        pos += strlen(pattern);
        if (sscanf(pos, "%d", value) > 0) {
            return true;
        }
    }
    return false;
}

bool get_float_param_value(const char *input, const char *key, float *value){
    char pattern[20];
    snprintf(pattern, sizeof(pattern), "%s=", key);

    char *pos = strstr(input, pattern);
    if (pos) {
        pos += strlen(pattern);
        if (sscanf(pos, "%f", value) > 0) {
            return true;
        }
    }
    return false;
}

// Login(GET)
esp_err_t login_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, _login_asm_start, _login_asm_end - _login_asm_start);
    return ESP_OK;
}

// Login (POST)
esp_err_t login_post_handler(httpd_req_t *req) {
    char buff[BUFF_LEN];
    int ret = httpd_req_recv(req, buff, sizeof(buff) - 1);
    if (ret <= 0) {
        // Manejo de error
        return ESP_FAIL;
    }
    buff[ret] = '\0'; 

    // Extraer nombre de usuario y contraseña
    char *username = strtok(buff, "&");
    char *password = strtok(NULL, "&");
    
    username = username + strlen("username="); // Saltar "username="
    password = password + strlen("password="); // Saltar "password="
    
    if (authenticate_user(username, password)) {
        httpd_resp_set_hdr(req, "Set-Cookie", find_user_by_username(username)->session_token); // Establecer cookie
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/home");
        httpd_resp_send(req, NULL, 0);
    } else {
        httpd_resp_send(req, "Authentication Failed", HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

// Loguut (get)
esp_err_t logout_handler(httpd_req_t *req) {
    char session_token[TOKEN_LEN];
    httpd_req_get_hdr_value_str(req, "Cookie", session_token, TOKEN_LEN);
    if(logout_user(session_token)) {
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }
    return ESP_OK;
}

// Error 404
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    httpd_resp_set_status(req, "302 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", redirect_404);
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGW(TAG_WIFI, "Redirecting to home");
    return ESP_OK;
}

//Insert Handlers into WebServer
void set_custom_uri_handlers(httpd_uri_t*uri_handlers,size_t uris_size){

    for(unsigned i =0; i < uris_size; i++)
        httpd_register_uri_handler(WebServer, &uri_handlers[i]);
}

// Required char* to login start and end EMBED_FILE
void set_main_uri_handler(char*__login_asm_start,char*__login_asm_end,char*__redirect_404){
    _login_asm_start = __login_asm_start;
    _login_asm_end = __login_asm_end;
    if (strlen(__redirect_404) < MAX_404_BUFFER_SIZE)
        strcpy(redirect_404,__redirect_404);
    else
        strcpy(redirect_404,"/login.html");
    httpd_uri_t uri;
    uri.uri = "/login.html";
    uri.method = HTTP_GET;
    uri.handler = login_handler;
    uri.user_ctx = NULL;
    httpd_register_uri_handler(WebServer, &uri);
    
    uri.uri = "/login.html";
    uri.method = HTTP_POST;
    uri.handler = login_post_handler;
    uri.user_ctx = NULL;
    httpd_register_uri_handler(WebServer, &uri);
    
    uri.uri = "/logout";
    uri.method = HTTP_GET;
    uri.handler = logout_handler;
    uri.user_ctx = NULL;
    httpd_register_uri_handler(WebServer, &uri);

    
    httpd_register_err_handler(WebServer, HTTPD_404_NOT_FOUND, http_404_error_handler);
}

void start_webserver(uint16_t max_uri) {

    wifi_init_softap();
    
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = max_uri;
    
    ESP_ERROR_CHECK(httpd_start(&WebServer, &config));
    
    //eAuth
    init_users();
}
