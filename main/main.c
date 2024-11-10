#include "eWifi.h"
#include "uri_handlers.h"
#include "eGenerator.h"

void app_main(void)
{
    init_Generators();
    size_t size = get_uri_handlers(NULL);
    httpd_uri_t uris[size];   
    get_uri_handlers(uris);
    start_webserver(uris,size);
}