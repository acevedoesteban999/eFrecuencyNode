idf_component_register(
    SRCS 
        "eWifi.c"

    INCLUDE_DIRS 
        "include"

    REQUIRES  
        esp_http_server
        esp_wifi
        eStore
        eAuth
)
