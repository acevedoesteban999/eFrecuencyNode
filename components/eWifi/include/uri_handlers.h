#include "eWifi.h"
#include "eGenerator.h"

esp_err_t home_get_handler(httpd_req_t *req);

esp_err_t home_post_handler(httpd_req_t *req);

esp_err_t login_get_handler(httpd_req_t *req);

esp_err_t login_post_handler(httpd_req_t *req);

esp_err_t logout_handler(httpd_req_t *req);

void add_uri(httpd_uri_t *uris, httpd_uri_t uri, size_t *count, bool has_urli);

size_t get_uri_handlers(httpd_uri_t*uris);