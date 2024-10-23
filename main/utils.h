#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "definitions.h"

esp_err_t initialize_nvs_and_memory(uint8_t **s_pref, uint8_t **s_read, nvs_handle_t nvs_handle);
void noise_reduction(uint8_t **puf_responses, uint8_t *s_pref);
float calculate_bit_entropy(uint8_t *data, int length);
void evaluate_puf_response(uint8_t *puf_response);

#endif