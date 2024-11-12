idf_component_register(
    SRCS 
        "eWifi.c"
        "eAuth.c"

    INCLUDE_DIRS 
        "include"

    REQUIRES  
        esp_http_server
        esp_wifi
        eStore
)
