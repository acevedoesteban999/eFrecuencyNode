#pragma once

#include "esp_log.h"
#include "eStore.h"
#include <string.h>

#define MAX_GENERATORS 3

#define DEFAULT_GENERATOR (generator_struct) { \
    .active = false, \
    .mode = -1, \
}


typedef struct
{
    bool active;
    size_t OUT_GPIO;
    size_t IN_GPIO;
    size_t mode;
    float frecuency;
    float min_frecuency;
    float max_frecuency;
}generator_struct;

void write_generator_in_nvs(const char *name, generator_struct value);

generator_struct read_generator_in_nvs(const char *name);

void init_Generators();

generator_struct get_generator(size_t generator);
