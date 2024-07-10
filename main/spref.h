#ifndef SPREF_H
#define SPREF_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "mbedtls/sha256.h"
#include "utils.h"
#include "definitions.h"


//void calculate_spref(uint8_t *s_pref, nvs_handle_t nvs_handle);
void measure_puf_response(uint8_t *puf_response);

void sample_spref(nvs_handle_t nvs_handle);
void get_spref(uint8_t *s_pref, nvs_handle_t nvs_handle);
void get_puf_responses(nvs_handle_t nvs_handle, uint8_t **puf_responses);


#endif