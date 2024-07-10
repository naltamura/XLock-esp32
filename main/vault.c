#include "vault.h"
/* void draw_vault_indexes(uint8_t *indexes, size_t index_count, size_t max_value) {
    // Draw index_count unique indexes between 0 and max_value - 1
    for (size_t i = 0; i < index_count; i++) {
        uint8_t new_index;
        int unique;
        do {
            unique = 1;
            new_index = esp_random() % max_value;
            for (size_t j = 0; j < i; j++) {
                if (indexes[j] == new_index) {
                    unique = 0;
                    break;
                }
            }
        } while (!unique);
        indexes[i] = new_index;
    }
} */

void draw_unique_indexes(int *indexes, size_t index_count, size_t max_value) {
    // Draw index_count unique indexes between 0 and max_value - 1
    for (size_t i = 0; i < index_count; i++) {
        int new_index;
        int unique;
        do {
            unique = 1;
            new_index = esp_random() % max_value;
            for (size_t j = 0; j < i; j++) {
                if (indexes[j] == new_index) {
                    unique = 0;
                    break;
                }
            }
        } while (!unique);
        indexes[i] = new_index;
    }
}


vault_t* init_vault(uint8_t *s_pref, size_t s_pref_size, size_t b_size, size_t L, size_t C) {
    vault_t *vault = (vault_t*)malloc(sizeof(vault_t));
    vault->bit_lockers = (bit_locker_t*)malloc(b_size * sizeof(bit_locker_t));

    for (size_t b = 0; b < b_size; b++) {
        vault->bit_lockers[b].locks = (lock_t*)malloc(L * sizeof(lock_t));
        for (size_t l = 0; l < L; l++) {
            draw_unique_indexes(vault->bit_lockers[b].locks[l].indexes, C, s_pref_size);
            uint8_t xor_result = 0;
            for (size_t i = 0; i < C; i++) {
                xor_result ^= s_pref[vault->bit_lockers[b].locks[l].indexes[i]];
            }
            vault->bit_lockers[b].locks[l].lock = xor_result;
            
            
            #ifdef LOG_GEN
            // Print lock and indexes
            printf("bit_locker[%zu].locks[%zu]: lock = %02x, indexes = ", b, l, vault->bit_lockers[b].locks[l].lock);
            for (size_t i = 0; i < C; i++) {
                printf("%02x ", vault->bit_lockers[b].locks[l].indexes[i]);
            }
            printf("\n");
            #endif
        }
    }
    return vault;
}

#define LOG_STORE
void store_vault(vault_t *vault, nvs_handle_t nvs_handle, size_t b_size, size_t L, size_t C) {
    esp_err_t err;
    for (size_t b = 0; b < b_size; b++) {
        for (size_t l = 0; l < L; l++) {
            char lock_key[32], index_key[32];
            sprintf(lock_key, "lock_%d_%d", (int)b, (int)l);
            sprintf(index_key, "index_%d_%d", (int)b, (int)l);
            err = nvs_set_u8(nvs_handle, lock_key, vault->bit_lockers[b].locks[l].lock);
            if (err != ESP_OK) {
                printf("Error (%s) storing lock in NVS!\n", esp_err_to_name(err));
            }

            err = nvs_set_blob(nvs_handle, index_key, vault->bit_lockers[b].locks[l].indexes, C * sizeof(int));
            if (err != ESP_OK) {
                printf("Error (%s) storing indexes in NVS!\n", esp_err_to_name(err));
            }

            // #ifdef LOG_STORE
            // // Print stored lock and indexes
            // printf("Stored lock: bit_locker[%zu].locks[%zu]: lock = %02x\n", b, l, vault->bit_lockers[b].locks[l].lock);
            // printf("Stored indexes: bit_locker[%zu].locks[%zu]: indexes = ", b, l);
            // for (size_t i = 0; i < C; i++) {
            //     printf("%d ", vault->bit_lockers[b].locks[l].indexes[i]);
            // }
            // printf("\n");
            // #endif
        }
    }
    nvs_commit(nvs_handle);
}
void print_vault(vault_t *vault, size_t b_size, size_t L, size_t C) {
    for (size_t b = 0; b < b_size; b++) {
        for (size_t l = 0; l < L; l++) {
            printf("Bit %d, Lock %d: Lock: %02x, Indexes: ", (int)b, (int)l, vault->bit_lockers[b].locks[l].lock);
            for (size_t i = 0; i < C; i++) {
                printf("%d", vault->bit_lockers[b].locks[l].indexes[i]);
                if (i < C - 1) {
                    printf(", ");
                }
            }
            printf("\n");
        }
    }
}


vault_t* get_vault(nvs_handle_t nvs_handle, size_t b_size, size_t L, size_t C) {
    vault_t *vault = (vault_t*)malloc(sizeof(vault_t));
    vault->bit_lockers = (bit_locker_t*)malloc(b_size * sizeof(bit_locker_t));

    for (size_t b = 0; b < b_size; b++) {
        vault->bit_lockers[b].locks = (lock_t*)malloc(L * sizeof(lock_t));
        for (size_t l = 0; l < L; l++) {
            char lock_key[32], index_key[32];
            sprintf(lock_key, "lock_%d_%d", (int)b, (int)l);
            sprintf(index_key, "index_%d_%d", (int)b, (int)l);
            esp_err_t err = nvs_get_u8(nvs_handle, lock_key, &vault->bit_lockers[b].locks[l].lock);
            if (err != ESP_OK) {
                printf("Error (%s) retrieving lock from NVS!\n", esp_err_to_name(err));
            }
            size_t required_size = C * sizeof(int);
            err = nvs_get_blob(nvs_handle, index_key, vault->bit_lockers[b].locks[l].indexes, &required_size);
            if (err != ESP_OK) {
                printf("Error (%s) retrieving indexes from NVS!\n", esp_err_to_name(err));
            }

            #ifdef LOG_GEN
            // Print retrieved lock and indexes
            printf("Retrieved lock: bit_locker[%zu].locks[%zu]: lock = %02x\n", b, l, vault->bit_lockers[b].locks[l].lock);
            printf("Retrieved indexes: bit_locker[%zu].locks[%zu]: indexes = ", b, l);
            for (size_t i = 0; i < C; i++) {
                printf("%d ", vault->bit_lockers[b].locks[l].indexes[i]);
            }
            printf("\n");
            #endif
        }
    }
    return vault;
}


void free_vault(vault_t *vault, size_t b_size) {
    for (size_t b = 0; b < b_size; b++) {
        free(vault->bit_lockers[b].locks);
    }
    free(vault->bit_lockers);
    free(vault);
}