#include "gen.h"
//#define  DEBUG
// This function calculates the majority bit for each position in an array of bytes
uint8_t get_majority_value(uint8_t *values, int length) {
    int count_ones = 0;
    for (int i = 0; i < length; i++) {
        count_ones += values[i] & 1;

        #ifdef DEBUG
        // Print each value and the current count of ones
        printf("Value[%d] = %02x, Count of ones so far: %d\n", i, values[i], count_ones);
        #endif
    }
    
    uint8_t majority = (count_ones > length / 2) ? 1 : 0;

    #ifdef DEBUG
    // Print the final majority value
    printf("Final majority value: %d\n", majority);
    #endif

    return majority;
}
//#define LOG_GEN
void calculate_k_pre(uint8_t* k_pre, uint8_t* s_read, vault_t* vault, int* indexes, size_t key_size, size_t num_locks, size_t xor_size) {
    
     #ifdef LOG_GEN
    printf("calculate_k_pre inputs:\n");
    
    printf("\ns_read: ");
    for (size_t i = 0; i < PUF_RESPONSE_BYTES; i++) {
        printf("%02x ", s_read[i]);
    }
    printf("\nvault: \n");
    for (size_t i = 0; i < RANDOM_POOL_BITS; i++) {
        for (size_t j = 0; j < num_locks; j++) {
            printf("bit_locker[%zu].locks[%zu]: lock = %02x, indexes = ", i, j, vault->bit_lockers[i].locks[j].lock);
            for (size_t k = 0; k < xor_size; k++) {
                printf("%d ", vault->bit_lockers[i].locks[j].indexes[k]);
            }
            printf("\n");
        }
    }
    printf("indexes: ");
    for (size_t i = 0; i < key_size; i++) {
        printf("%d ", indexes[i]);
    }
    printf("\nkey_size: %zu\nnum_locks: %zu\nxor_size: %zu\n", key_size, num_locks, xor_size);
    #endif

    uint8_t l=0;
    for (size_t i = 0; i < key_size; i++)
    {   
        int r=indexes[i];
        uint8_t x[num_locks];
        memset(x, 0, num_locks);
        
        for (size_t j = 0; j < num_locks; j++)
        {
            uint8_t b_prime=0;
            uint8_t l = vault->bit_lockers[r].locks[j].lock;
            for (size_t k = 0; k < xor_size; k++)
            {
                int c=vault->bit_lockers[r].locks [j].indexes[k];
                //printf("bit_lockers[%d].locks[%d].indexes[%d] = %d\n", r, j, k, c);
                //ESP_LOGW("C","%d",c);
                b_prime ^= (s_read[c / 8] >> (c % 8)) & 1;
            }
            x[j] = b_prime ^ l;
        }
        k_pre[i] = get_majority_value(x, num_locks);
        
    }
    printf("k_pre: ");
    for (size_t i = 0; i < key_size; i++) {
        printf("%02x", k_pre[i]);
    }

}

void gen_procedure(uint8_t *S_read, vault_t *V, size_t B_size, size_t L, size_t C, uint8_t *K, uint8_t *n, int *R, size_t key_size, uint8_t *T) {
    // Generate nonce
    *n = esp_random() & 0xFF;  // Generate a single byte nonce

    // Draw unique key indexes
    draw_unique_indexes(R, key_size, B_size);
    uint8_t K_pre[key_size];

    calculate_k_pre(K_pre, S_read, V, R, key_size, L, C);


    printf("gen_procedure K_pre: ");
    for (size_t i = 0; i < key_size; i++) {
        printf("%02x", K_pre[i]);
    }
    printf("\n");

    // Hash K_pre and nonce to get K
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, K_pre, key_size);
    mbedtls_sha256_update(&ctx, n, 1);
    mbedtls_sha256_finish(&ctx, K);
    mbedtls_sha256_free(&ctx);

    // Hash K and R to get T
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, K, KEY_SIZE);
    mbedtls_sha256_update(&ctx, R, key_size);
    mbedtls_sha256_finish(&ctx, T);
    mbedtls_sha256_free(&ctx);
}

void store_gen_values(nvs_handle_t nvs_handle, int *R, uint8_t *T, uint8_t n, size_t key_size) {
    esp_err_t err;

    printf("Storing values in NVS:\n");
    printf("R: ");
    for (size_t i = 0; i < key_size; i++) {
        printf("%d ", R[i]);
    }
    printf("\n");

    printf("T: ");
    for (size_t i = 0; i < key_size; i++) {
        printf("%02x ", T[i]);
    }
    printf("\n");

    printf("n: %02x\n", n);

    err = nvs_set_blob(nvs_handle, "gen_R", R, key_size * sizeof(int));
    if (err != ESP_OK) {
        printf("Error (%s) storing R in NVS!\n", esp_err_to_name(err));
    }

    err = nvs_set_blob(nvs_handle, "gen_T", T, key_size);
    if (err != ESP_OK) {
        printf("Error (%s) storing T in NVS!\n", esp_err_to_name(err));
    }

    err = nvs_set_u8(nvs_handle, "gen_n", n);
    if (err != ESP_OK) {
        printf("Error (%s) storing nonce in NVS!\n", esp_err_to_name(err));
    }

    nvs_commit(nvs_handle);
}



void retrieve_gen_values(nvs_handle_t nvs_handle, int *R, uint8_t *T, uint8_t *n, size_t key_size) {
    esp_err_t err;
    size_t required_size;

    printf("Retrieving values from NVS:\n");

    required_size = key_size * sizeof(int);
    err = nvs_get_blob(nvs_handle, "gen_R", R, &required_size);
    if (err != ESP_OK) {
        printf("Error (%s) retrieving R from NVS!\n", esp_err_to_name(err));
    } else {
        printf("R: ");
        for (size_t i = 0; i < key_size; i++) {
            printf("%d ", R[i]);
        }
        printf("\n");
    }

    required_size = key_size;
    err = nvs_get_blob(nvs_handle, "gen_T", T, &required_size);
    if (err != ESP_OK) {
        printf("Error (%s) retrieving T from NVS!\n", esp_err_to_name(err));
    } else {
        printf("T: ");
        for (size_t i = 0; i < key_size; i++) {
            printf("%02x ", T[i]);
        }
        printf("\n");
    }

    err = nvs_get_u8(nvs_handle, "gen_n", n);
    if (err != ESP_OK) {
        printf("Error (%s) retrieving nonce from NVS!\n", esp_err_to_name(err));
    } else {
        printf("n: %02x\n", *n);
    }
}

