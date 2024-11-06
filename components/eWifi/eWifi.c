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
esp_err_t  wifi_init_softap(void)
{
    ESP_ERROR_CHECK_WITHOUT_ABORT(init_nvs());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_init(&cfg));
    
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));


    wifi_config_t wifi_config = {
        .ap = {
            .ssid = ESP_WIFI_SSID,
            .ssid_len = strlen(ESP_WIFI_SSID),
            .password = ESP_WIFI_PASS,
            .max_connection = MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    if (strlen(ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    esp_err_t  ret = esp_wifi_start();
    if (ret == ESP_OK){
        esp_netif_ip_info_t ip_info;
        esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);

        char ip_addr[16];
        // Obtiene la información de IP
        inet_ntoa_r(ip_info.ip.addr, ip_addr, 16);
        ESP_LOGI(TAG_WIFI, "Set up softAP with IP: %s", ip_addr);
        ESP_LOGI(TAG_WIFI, "wifi_init_softap finished. SSID:'%s' password:'%s'",ESP_WIFI_SSID, ESP_WIFI_PASS);
    }

    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    return ret;
    
}

// Manejo de autenticación
bool isAuth(httpd_req_t *req)
{
    char session_token[TOKEN_LEN];
    httpd_req_get_hdr_value_str(req, "Cookie", session_token, TOKEN_LEN);
    return (strlen(session_token) && get_user_by_session_token(session_token));
}

// Manejador de la ruta /home (GET)
esp_err_t home_get_handler(httpd_req_t *req) {
    // Verifica autenticación
    if (isAuth(req)) {
        ESP_LOGI(TAG_WIFI, "Serve home");
        const uint32_t home_len = home_end - home_start;
        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, home_start, home_len);
    } else {
        // Redirige a /login si no está autenticado
        ESP_LOGE(TAG_WIFI, "User not authenticated, redirecting to /login");
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

esp_err_t home_post_handler(httpd_req_t *req) {
    // Verifica autenticación
    if (!isAuth(req)) {
        ESP_LOGE(TAG_WIFI, "User not authenticated, redirecting to /login");
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login");
        httpd_resp_send(req, NULL, 0);
        return ESP_OK;
    }

    ESP_LOGI(TAG_WIFI, "Serve Init");
    
    // Búfer para datos del formulario
    char buff[BUFF_LEN];
    int ret, remaining = req->content_len;

    // Verifica el tamaño del cuerpo de la solicitud
    if (remaining > sizeof(buff) - 1) { // Reservar espacio para el null terminator
        ESP_LOGE(TAG_WIFI, "Request body too large");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Request body too large");
        return ESP_FAIL;
    }

    // Leer el cuerpo de la solicitud
    ret = httpd_req_recv(req, buff, remaining);
    if (ret <= 0) { // Error o conexión cerrada
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req); // Timeout
        }
        return ESP_FAIL;
    }

    buff[ret] = '\0'; // Terminar la cadena
    ESP_LOGI(TAG_WIFI, "Received form data: %s", buff);

    // Respuesta al cliente
    const char *response = "OK";
    httpd_resp_send(req, response, strlen(response));

    return ESP_OK;
}


// Manejador de la ruta /login (GET)
esp_err_t login_get_handler(httpd_req_t *req)
{
    const uint32_t login_len = login_end - login_start;

    ESP_LOGI(TAG_WIFI, "Serve Login");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, login_start, login_len);

    return ESP_OK;
}

// Manejador de autenticación (POST)
esp_err_t login_post_handler(httpd_req_t *req) {
    char buff[BUFF_LEN];
    int ret = httpd_req_recv(req, buff, sizeof(buff) - 1);
    if (ret <= 0) {
        // Manejo de error
        return ESP_FAIL;
    }
    buff[ret] = '\0'; // Terminar la cadena

    // Extraer nombre de usuario y contraseña
    char *username = strtok(buff, "&");
    char *password = strtok(NULL, "&");
    
    username = username + strlen("username="); // Saltar "username="
    password = password + strlen("password="); // Saltar "password="
    
    if (authenticate_user(username, password)) {
        // Autenticación exitosa
        httpd_resp_set_hdr(req, "Set-Cookie", find_user_by_username(username)->session_token); // Establecer cookie
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/home");
        httpd_resp_send(req, NULL, 0);
    } else {
        // Manejar fallo de autenticación
        httpd_resp_send(req, "Authentication Failed", HTTPD_RESP_USE_STRLEN);
    }
    return ESP_OK;
}

// Manejador de cierre de sesión
esp_err_t logout_handler(httpd_req_t *req) {
    char session_token[TOKEN_LEN];
    httpd_req_get_hdr_value_str(req, "Cookie", session_token, TOKEN_LEN);
    if(logout_user(session_token)) {
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login");
        httpd_resp_send(req, NULL, 0);
    }
    return ESP_OK;
}

// Manejador de Error 404
esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" home directory
    httpd_resp_set_hdr(req, "Location", "/home");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);

    ESP_LOGW(TAG_WIFI, "Redirecting to home");
    return ESP_OK;
}

// Función para iniciar el servidor web
esp_err_t start_webserver(void) {

    esp_err_t ret = wifi_init_softap();
    if (ret == ESP_OK){
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        
        ESP_LOGI(TAG_WIFI, "Starting web server on port: '%d'", config.server_port);

        // Inicia el servidor HTTP
        ret = httpd_start(&WebServer, &config);
        if (ret == ESP_OK) {
            
            httpd_uri_t home_get_uri = {
                .uri = "/home",
                .method = HTTP_GET,
                .handler = home_get_handler,
                .user_ctx = NULL
            };
            httpd_uri_t home_post_uri = {
                .uri = "/home",
                .method = HTTP_POST,
                .handler = home_post_handler,
                .user_ctx = NULL
            };
            httpd_uri_t login_uri = {
                .uri = "/login",
                .method = HTTP_GET,
                .handler = login_get_handler,
                .user_ctx = NULL
            };
            httpd_uri_t login_post_uri = {
                .uri = "/login",
                .method = HTTP_POST,
                .handler = login_post_handler,
                .user_ctx = NULL
            };
            httpd_uri_t logout_uri = {
                .uri = "/logout",
                .method = HTTP_GET,
                .handler = logout_handler,
                .user_ctx = NULL
            };

            
            // Registra las URIs
            httpd_register_uri_handler(WebServer, &home_get_uri);
            httpd_register_uri_handler(WebServer, &home_post_uri);
            httpd_register_uri_handler(WebServer, &login_uri);
            httpd_register_uri_handler(WebServer, &login_post_uri);
            httpd_register_uri_handler(WebServer, &logout_uri);
            httpd_register_err_handler(WebServer, HTTPD_404_NOT_FOUND, http_404_error_handler);
            
            ret = init_users();
        }
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(ret);
    return ret;
}