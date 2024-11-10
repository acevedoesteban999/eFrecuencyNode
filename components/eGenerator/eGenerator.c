#include "eGenerator.h"



generator_struct GENERATORS[MAX_GENERATORS];



void write_generator_in_nvs(const char* name, generator_struct generator){
    write_struct_in_nvs(name,&generator);
}

generator_struct read_generator_in_nvs(const char* name) {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    size_t required_size = sizeof(generator_struct); 
    generator_struct generator;
    
    err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return DEFAULT_GENERATOR;
    }

    err = nvs_get_blob(nvs_handle, name, &generator, &required_size);
    nvs_close(nvs_handle);

    return generator;
}

void init_Generators(){
    char g_name[5];
    for(unsigned i=0;i<MAX_GENERATORS;i++){
        snprintf(g_name, sizeof(g_name),"g%u",i);
        GENERATORS[i] = read_generator_in_nvs(g_name);
    }
}

generator_struct get_generator(size_t generator){
    if (generator >= MAX_GENERATORS)
        return GENERATORS[0];
    return GENERATORS[generator];
}