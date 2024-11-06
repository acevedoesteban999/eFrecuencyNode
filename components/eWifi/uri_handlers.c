
#include "uri_handlers.h"

// Manejador de la ruta /home (GET)
esp_err_t home_get_handler(httpd_req_t *req) {
    // Verifica autenticación
    if (isAuth(req)) {
        ESP_LOGI(TAG_WIFI, "Serve home");
        const uint32_t home_len = home_end - home_start;
        printf(home_start);
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

void add_uri(httpd_uri_t*uris,httpd_uri_t uri,size_t*count){
    if (*count >= MAX_URI)
        return;
    uris[*count] = uri;
    *count += 1;
}

size_t get_uri_handlers(httpd_uri_t*uris){
    
    httpd_uri_t uri;
    size_t count = 0;
    
    uri.uri = "/home";
    uri.method = HTTP_GET;
    uri.handler = home_get_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);

    uri.uri = "/home";
    uri.method = HTTP_POST;
    uri.handler = home_post_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
    
    uri.uri = "/login";
    uri.method = HTTP_GET;
    uri.handler = login_get_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
   
    uri.uri = "/login";
    uri.method = HTTP_POST;
    uri.handler = login_post_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
    
    uri.uri = "/logout";
    uri.method = HTTP_GET;
    uri.handler = logout_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
    return count;
    
}