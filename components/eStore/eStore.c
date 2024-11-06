#include "eStore.h"

const char * TAG_STORE = "STORE";

// Inicializa el almacenamiento no volátil (NVS)
esp_err_t init_nvs() {
    esp_err_t ret = nvs_flash_init();
    
    // Manejo de particiones NVS llenas o versiones nuevas
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG_STORE,"Reboot Flash");
        ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());   // Borra la partición NVS
        ret = nvs_flash_init();                             // Inicializa nuevamente
    }
    return ret;
}

// Función auxiliar para abrir el manejador NVS
esp_err_t open_nvs_handle(nvs_handle_t *nvs_handle) {
    return nvs_open("storage", NVS_READWRITE, nvs_handle);
}

// Función auxiliar para cerrar el manejador NVS
void close_nvs_handle(nvs_handle_t nvs_handle) {
    nvs_close(nvs_handle);
}

// Guarda un valor booleano en NVS
void write_bool_in_nvs(const char* name, bool value) {
    nvs_handle_t nvs_handle;
    
    esp_err_t err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_u8(nvs_handle, name, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at Save: %s", esp_err_to_name(err));
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error in Commit: %s", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG_STORE, "Write %s => %u\n", name, value);
    }
    
    close_nvs_handle(nvs_handle);
}

// Guarda un valor entero de 8 bits en NVS
void write_int8_in_nvs(const char* name, int8_t value) {
    nvs_handle_t nvs_handle;
    
    esp_err_t err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_i8(nvs_handle, name, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at Save: %s", esp_err_to_name(err));
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG_STORE, "Error in Commit: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG_STORE, "Write: %s => %d\n", name, value);
        }
    }
    
    close_nvs_handle(nvs_handle);
}

// Guarda una cadena de caracteres en NVS
void write_string_in_nvs(const char* name, const char* value) {
    nvs_handle_t nvs_handle;

    esp_err_t err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_str(nvs_handle, name, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at Save: %s", esp_err_to_name(err));
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG_STORE, "Error in Commit: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG_STORE, "Write: %s => %s\n", name, value);
        }
    }

    close_nvs_handle(nvs_handle);
}


void read_string_from_nvs(const char* name ,char* buffer, size_t buffer_size) {
    nvs_handle_t nvs_handle;
    esp_err_t err;

    // Abre el handle de NVS
    err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error al abrir NVS: %s", esp_err_to_name(err));
        return;
    }
    err = nvs_get_str(nvs_handle, name, buffer, &buffer_size);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG_STORE, "Key dont found: %s", name);
    } else {
        ESP_LOGE(TAG_STORE, "Error: %s", esp_err_to_name(err));
    }

    // Cierra el handle de NVS
    close_nvs_handle(nvs_handle);
}

// Lee un valor entero de 8 bits desde NVS
int8_t read_i8_from_nvs(const char* name) {
    nvs_handle_t nvs_handle;
    esp_err_t err = open_nvs_handle(&nvs_handle);
    int8_t value = 0;  // Variable para almacenar el valor leído
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return 0;  // Retorna 0 si hay un error al abrir
    }

    // Lee el valor almacenado bajo la clave
    err = nvs_get_i8(nvs_handle, name, &value);
    close_nvs_handle(nvs_handle);

    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG_STORE, "Read %s => %i\n", name, value);
            return value;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(TAG_STORE, "Value %s not Found\n",name);
            break;
        default:
            ESP_LOGE(TAG_STORE, "Error at read value: %s", esp_err_to_name(err));
    }
    
    return 0;  // Retorna 0 si hay un error al leer el valor
}


// Guarda un valor entero de 64 bits en NVS
void write_uint64_in_nvs(const char* name, uint64_t value) {
    nvs_handle_t nvs_handle;
    
    esp_err_t err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return;
    }

    err = nvs_set_u64(nvs_handle, name, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at Save: %s", esp_err_to_name(err));
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG_STORE, "Error in Commit: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG_STORE, "Write: %s => %llu\n", name, value);
        }
    }

    close_nvs_handle(nvs_handle);
}

// Lee un valor entero de 64 bits desde NVS
uint64_t read_uint64_from_nvs(const char* name) {
    nvs_handle_t nvs_handle;
    esp_err_t err = open_nvs_handle(&nvs_handle);
    uint64_t value = 0; 
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return 0;  // Retorna 0 si hay un error al abrir
    }

    // Lee el valor almacenado bajo la clave
    err = nvs_get_u64(nvs_handle, name, &value);
    close_nvs_handle(nvs_handle);

    switch (err) {
        case ESP_OK:
            ESP_LOGI(TAG_STORE, "Read %s => %llu\n", name, value);
            return value;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(TAG_STORE, "Value %s not Found\n",name);
            break;
        default:
            ESP_LOGE(TAG_STORE, "Error at read value: %s", esp_err_to_name(err));
    }
    
    return 0;  // Retorna 0 si hay un error al leer el valor
}

// Guarda un valor float en NVS
void write_float_in_nvs(const char* name, float value) {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    
    // Abrir el handle de NVS
    err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return;
    }

    // Convertir el float a uint32_t
    uint32_t value_as_int;
    memcpy(&value_as_int, &value, sizeof(value_as_int));

    // Almacenar el valor como uint32_t en NVS
    err = nvs_set_u32(nvs_handle, name, value_as_int);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at Save: %s", esp_err_to_name(err));
    } else {
        err = nvs_commit(nvs_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG_STORE, "Error in Commit: %s", esp_err_to_name(err));
        } else {
            ESP_LOGI(TAG_STORE, "Write: %s => %f\n", name, value);
        }
    }

    // Cerrar el handle de NVS
    close_nvs_handle(nvs_handle);
}

// Lee un valor float desde NVS
float read_float_from_nvs(const char* name) {
    nvs_handle_t nvs_handle;
    esp_err_t err;
    
    // Abrir el handle de NVS
    err = open_nvs_handle(&nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at open NVS: %s", esp_err_to_name(err));
        return 0.0; // Valor por defecto en caso de error
    }

    uint32_t value_as_int = 0;
    // Leer el valor como uint32_t desde NVS
    err = nvs_get_u32(nvs_handle, name, &value_as_int);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGW(TAG_STORE, "Value not found: %s", name);
        close_nvs_handle(nvs_handle);
        return 0.0;  // Valor por defecto si no se encuentra el valor
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG_STORE, "Error at Read: %s", esp_err_to_name(err));
        close_nvs_handle(nvs_handle);
        return 0.0;  // Valor por defecto en caso de otro error
    }

    // Convertir el uint32_t de vuelta a float
    float value;
    memcpy(&value, &value_as_int, sizeof(value));

    // Cerrar el handle de NVS
    close_nvs_handle(nvs_handle);

    ESP_LOGI(TAG_STORE, "Read: %s => %f\n", name, value);
    
    return value;
}

