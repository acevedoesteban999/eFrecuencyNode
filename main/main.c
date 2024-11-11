#include "eWifi.h"
#include "uri_handlers.c"
#include "eGenerator.h"

void app_main(void)
{
    init_Generators();
                                                    
    size_t size = get_uri_handlers(NULL);       //Get size
    httpd_uri_t uris[size];   
    get_uri_handlers(uris);                     //Get URIS

    start_webserver(size + 3);
    
    set_login_uri_handler(login_asm_start,login_asm_end);

    set_uri_handlers(uris,size);
}