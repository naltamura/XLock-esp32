#ifndef VAULT_H
#define VAULT_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "utils.h"
//#include "puf_gen.h"
#include "mbedtls/sha256.h"
#include "definitions.h"
#include "spref.h"
#include "mbedtls/sha256.h"
#include "esp_random.h"




typedef struct {
    uint8_t lock;  // XOR result for each bit in B
    int indexes[BITS_PER_XOR];  // Indexes for XOR
} lock_t;

typedef struct {
    lock_t *locks;  // Locks for each bit in B
} bit_locker_t;

typedef struct {
    bit_locker_t *bit_lockers;  // Array of bit_lockers
} vault_t;


void draw_unique_indexes(int *indexes, size_t index_count, size_t max_value);
vault_t* init_vault(uint8_t *s_pref, size_t s_pref_size, size_t b_size, size_t L, size_t C);
void store_vault(vault_t *vault, nvs_handle_t nvs_handle, size_t b_size, size_t L, size_t C);
void print_vault(vault_t *vault, size_t b_size, size_t L, size_t C);
vault_t* get_vault(nvs_handle_t nvs_handle, size_t b_size, size_t L, size_t C);
void free_vault(vault_t *vault, size_t b_size);

#endif