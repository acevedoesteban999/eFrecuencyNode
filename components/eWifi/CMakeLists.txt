idf_component_register(
    SRCS 
        "eWifi.c"
        "eAuth.c"

    INCLUDE_DIRS 
        "include"

    EMBED_FILES 
        "src/home.html" 
        "src/login.html" 

    REQUIRES  
        esp_http_server
        esp_wifi
        eStore
)
