
#include "uri_handlers.h"

esp_err_t login_styles_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/css");
    httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=86400");
    httpd_resp_send(req, login_styles_asm_start, login_styles_asm_end - login_styles_asm_start);
    return ESP_OK;
}

esp_err_t main_styles_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/css");
    httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=86400");
    httpd_resp_send(req, main_styles_asm_start, main_styles_asm_end - main_styles_asm_start);
    return ESP_OK;
}

esp_err_t generator_styles_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/css");
    httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=86400");
    httpd_resp_send(req, generator_styles_asm_start, generator_styles_asm_end - generator_styles_asm_start);
    return ESP_OK;
}



esp_err_t generator_index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/js");
    httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=86400");
    httpd_resp_send(req, generator_index_asm_start, generator_index_asm_end-generator_index_asm_start);
    return ESP_OK;
}

esp_err_t home_get_handler(httpd_req_t *req) {
    // Verifica autenticación
    if (isAuth(req)) {
        ESP_LOGI(TAG_WIFI, "Serve home");
        char* buffer = calloc((home_asm_end - home_asm_start)+1,sizeof(char));
        
        if (buffer == NULL){
            ESP_LOGE("ERROR","No Memory for calloc");
            return ESP_FAIL;
        }
        
        strlcpy(buffer,home_asm_start,home_asm_end - home_asm_start);
        
        snprintf(buffer, strlen(buffer), buffer,
            "TEST1", 
            "TEST2"
        );

        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, buffer, strlen(buffer));
        
        free(buffer);
    } else {
        // Redirige a /login si no está autenticado
        ESP_LOGE(TAG_WIFI, "User not authenticated, redirecting to /login");
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

esp_err_t generator_get_handler(httpd_req_t *req) {
    // Verifica autenticación
    if (isAuth(req)) {
        ESP_LOGI(TAG_WIFI, "Serve Generator");
        // const uint32_t home_len = home_end - home_start;
        char* buffer = calloc((generator_asm_end - generator_asm_start)+1,sizeof(char));
        
        if (buffer == NULL){
            ESP_LOGE("ERROR","No Memory for calloc");
            return ESP_FAIL;
        }
        strlcpy(buffer,generator_asm_start,generator_asm_end - generator_asm_start);
        snprintf(buffer, strlen(buffer), buffer,
            "TEST1", 
            "TEST2"
        );

        httpd_resp_set_type(req, "text/html");
        httpd_resp_send(req, buffer, strlen(buffer));
        
        free(buffer);
    } else {
        // Redirige a /login si no está autenticado
        ESP_LOGE(TAG_WIFI, "User not authenticated, redirecting to /login");
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}

// Manejador de la ruta /login (GET)
esp_err_t login_get_handler(httpd_req_t *req)
{

    ESP_LOGI(TAG_WIFI, "Serve Login");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, login_asm_start, login_asm_end - login_asm_start);

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
        httpd_resp_set_hdr(req, "Location", "/login.html");
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
    
    uri.uri = "/home.html";
    uri.method = HTTP_GET;
    uri.handler = home_get_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);

    uri.uri = "/generator.html";
    uri.method = HTTP_GET;
    uri.handler = generator_get_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
    
    uri.uri = "/login.html";
    uri.method = HTTP_GET;
    uri.handler = login_get_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
   
    uri.uri = "/login.html";
    uri.method = HTTP_POST;
    uri.handler = login_post_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
    
    uri.uri = "/logout";
    uri.method = HTTP_GET;
    uri.handler = logout_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);

    uri.uri = "/css/login_styles.css";
    uri.method = HTTP_GET;
    uri.handler = login_styles_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);
    
    uri.uri = "/css/main_styles.css";
    uri.method = HTTP_GET;
    uri.handler = main_styles_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);

    uri.uri = "/css/generator_styles.css";
    uri.method = HTTP_GET;
    uri.handler = generator_styles_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);

    uri.uri = "/js/generator_index.js";
    uri.method = HTTP_GET;
    uri.handler = generator_index_handler;
    uri.user_ctx = NULL;
    add_uri(uris,uri,&count);



    return count;
    
}