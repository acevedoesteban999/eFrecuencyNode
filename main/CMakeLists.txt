idf_component_register(
    SRCS 
        "main.c"
        "uri_handlers.c"

    INCLUDE_DIRS 
        "."
    
    EMBED_FILES 
        "src/home.html" 
        "src/generator.html" 
        "src/login.html"

        "src/css/login_styles.css" 
        "src/css/main_styles.css" 
        "src/css/generator_styles.css" 
        
        "src/js/generator_index.js" 
    
    REQUIRES  
        eGenerator
        eWeb
)