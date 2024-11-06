idf_component_register(
    SRCS 
        "eWifi.c"
        "uri_handlers.c"

    INCLUDE_DIRS 
        "include"

    EMBED_FILES 
        "src/home.html" 
        "src/login.html" 

    REQUIRES  
        esp_http_server
        esp_wifi
        eStore
        eAuth
)
