#pragma once

#include "esp_log.h"
#include "eStore.h"
#include <string.h>

#define MAX_GENERATORS 3

#define DEFAULT_GENERATOR (generator_struct) { \
    .mode = -1, \
}


typedef struct
{
    size_t mode;
    size_t OUT_GPIO;
    size_t analog_GPIO;
    float frecuency;
    float min_frecuency;
    float max_frecuency;
    float drift;
}generator_struct;

void write_generator_in_nvs(const char *name, generator_struct *generator);

generator_struct read_generator_in_nvs(const char *name);

void init_Generators();

generator_struct get_generator(size_t generator);

generator_struct *get_generator_ptr(size_t generator);
