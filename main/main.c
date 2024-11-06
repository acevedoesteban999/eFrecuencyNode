#include "eWifi.h"
#include "uri_handlers.h"

void app_main(void)
{


    httpd_uri_t uris[MAX_URI];
    size_t size = get_uri_handlers(uris);
    start_webserver(uris,size);

}