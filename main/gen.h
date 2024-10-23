#ifndef GEN_H
#define GEN_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "mbedtls/sha256.h"
#include "utils.h"
#include "esp_log.h"
//#include "puf_gen.h"
#include "definitions.h"
#include "spref.h"
#include "vault.h"

uint8_t get_majority_value(uint8_t *values, int length);
void gen_procedure(uint8_t *S_read, vault_t *V, size_t B_size, size_t L, size_t C, uint8_t *K, uint8_t *n, uint8_t *R, size_t key_size, uint8_t *T);
void calculate_k_pre(uint8_t* k_pre, uint8_t* s_read, vault_t* vault, uint8_t* indexes, size_t key_size, size_t num_locks, size_t xor_size);
void store_gen_values(nvs_handle_t nvs_handle, uint8_t *R, uint8_t *T, uint8_t n, size_t key_size);
void retrieve_gen_values(nvs_handle_t nvs_handle, uint8_t *R, uint8_t *T, uint8_t *n, size_t key_size);

#endif