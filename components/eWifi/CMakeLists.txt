idf_component_register(
    SRCS 
        "eWifi.c"
        "uri_handlers.c"

    INCLUDE_DIRS 
        "include"

    EMBED_FILES 
        "src/home.html" 
        "src/generator.html" 
        "src/login.html" 
        "src/styles.css" 
        "src/index.js" 

    REQUIRES  
        esp_http_server
        esp_wifi
        eStore
        eAuth
)
