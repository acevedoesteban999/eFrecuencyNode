
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
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_set_hdr(req, "Cache-Control", "public, max-age=86400");
    httpd_resp_send(req, generator_index_asm_start, generator_index_asm_end-generator_index_asm_start);
    return ESP_OK;
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

esp_err_t set_generator_post_handler(httpd_req_t *req) {
    // Verifica autenticación
    if (isAuth(req)) {
        char json_response[100];
        int ret, remaining = req->content_len;
        char buff[remaining + 1];

        
        ret = httpd_req_recv(req, buff, remaining);
        if (ret <= 0) { 
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req); 
            }
            return ESP_FAIL;
        }

        buff[ret] = '\0'; 
        
        int mode,generator_index;
        if(!get_int_param_value(buff,"generator",&generator_index))
            return httpd_resp_send_custom_err(req,"422","Miss Generator Value");
        if(!get_int_param_value(buff,"mode",&mode))
            return httpd_resp_send_custom_err(req,"422","Miss Mode Value");
        
        generator_struct*generator = get_generator_ptr(generator_index);
        
        if(mode == 0){
            float frecuency;
            int out_gpio;
            if(!get_int_param_value(buff,"out_gpio",&out_gpio))
                return httpd_resp_send_custom_err(req,"422","Miss a Value");
            
            if(!get_float_param_value(buff,"frecuency",&frecuency))
                return httpd_resp_send_custom_err(req,"422","Miss a Value");
            
            generator->mode = mode;
            generator->OUT_GPIO = out_gpio;
            generator->frecuency = frecuency;
            write_generator_in_nvs("g0",generator);
        }
        else
            generator->mode = -1;

        snprintf(json_response, sizeof(json_response),"{\"mode\":%d}",generator->mode);   

        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json_response, strlen(json_response));
    } else {

        ESP_LOGE(TAG_WIFI, "User not authenticated, redirecting to /login");
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }

    return ESP_OK;
}


esp_err_t get_generator_data_post_handler(httpd_req_t *req) {
    // Verifica autenticación
    if (isAuth(req)) {
        int ret, remaining = req->content_len;
        char buff[remaining + 1];

        
        ret = httpd_req_recv(req, buff, remaining);
        if (ret <= 0) { 
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                httpd_resp_send_408(req); 
            }
            return ESP_FAIL;
        }

        buff[ret] = '\0'; 
        
        int gener;
        if(!get_int_param_value(buff,"generator",&gener))
            return httpd_resp_send_custom_err(req,"422","Miss Generator Value");
        
        char json_response[100];
        generator_struct generator = get_generator(gener);
        
        switch (generator.mode)
        {
        case 0:
            snprintf(json_response, sizeof(json_response),
                "{\"mode\":%d,\"out_gpio\":%i,\"frecuency\":%f}",
                generator.mode,
                generator.OUT_GPIO,
                generator.frecuency
            );   
            break;
        
        case 1:
            snprintf(json_response, sizeof(json_response),
                "{\"mode\":%d,\"out_gpio\":%i,\"min_frecuency\":%f,\"max_frecuency\":%f,\"drift\":%f}",
                generator.mode,
                generator.OUT_GPIO,
                generator.min_frecuency,
                generator.max_frecuency,
                generator.drift
            );   
                
            break;

        case 2:
            snprintf(json_response, sizeof(json_response),
                    "{\"mode\":%d,\"out_gpio\":%i,\"analog_gpio\":%i}",
                    generator.mode,
                    generator.OUT_GPIO,
                    generator.analog_GPIO
                ); 
            break;
        
        default: // case: -1 
            snprintf(json_response, sizeof(json_response),
                "{\"mode\":%d}",
                generator.mode
            );   
        
            break;
        }
        httpd_resp_set_type(req, "application/json");
        httpd_resp_send(req, json_response, strlen(json_response));
    } else {
        // Redirige a /login si no está autenticado
        ESP_LOGE(TAG_WIFI, "User not authenticated, redirecting to /login");
        httpd_resp_set_status(req, "302 Found");
        httpd_resp_set_hdr(req, "Location", "/login.html");
        httpd_resp_send(req, NULL, 0);
    }

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
    buff[ret] = '\0'; 

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

void add_uri(httpd_uri_t*uris,httpd_uri_t uri,size_t*count,bool has_urli){
    if(has_urli)
        uris[*count] = uri;
    *count += 1;
}

size_t get_uri_handlers(httpd_uri_t*uris){
    bool has_urli = (uris != NULL);
    httpd_uri_t uri;
    size_t count = 0;

    if(has_urli){ 
        uri.uri = "/home.html";
        uri.method = HTTP_GET;
        uri.handler = home_get_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);

    if(has_urli){ 
        uri.uri = "/generator.html";
        uri.method = HTTP_GET;
        uri.handler = generator_get_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);
    
    if(has_urli){ 
        uri.uri = "/login.html";
        uri.method = HTTP_GET;
        uri.handler = login_get_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);
   
    if(has_urli){ 
        uri.uri = "/login.html";
        uri.method = HTTP_POST;
        uri.handler = login_post_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);
    
    if(has_urli){ 
        uri.uri = "/logout";
        uri.method = HTTP_GET;
        uri.handler = logout_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);

    if(has_urli){ 
        uri.uri = "/css/login_styles.css";
        uri.method = HTTP_GET;
        uri.handler = login_styles_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);
    
    if(has_urli){ 
        uri.uri = "/css/main_styles.css";
        uri.method = HTTP_GET;
        uri.handler = main_styles_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);

    if(has_urli){ 
        uri.uri = "/css/generator_styles.css";
        uri.method = HTTP_GET;
        uri.handler = generator_styles_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);

    if(has_urli){
        uri.uri = "/js/generator_index.js";
        uri.method = HTTP_GET;
        uri.handler = generator_index_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);

    if(has_urli){
        uri.uri = "/get_generator_data";
        uri.method = HTTP_POST;
        uri.handler = get_generator_data_post_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);

    if(has_urli){
        uri.uri = "/set_generator";
        uri.method = HTTP_POST;
        uri.handler = set_generator_post_handler;
        uri.user_ctx = NULL;
    }
    add_uri(uris,uri,&count,has_urli);



    return count;
    
}