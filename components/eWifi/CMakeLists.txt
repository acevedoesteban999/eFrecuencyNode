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

        "src/css/login_styles.css" 
        "src/css/main_styles.css" 
        "src/css/generator_styles.css" 
        
        "src/js/generator_index.js" 
    REQUIRES  
        esp_http_server
        esp_wifi
        eStore
        eAuth
        eGenerator
)
