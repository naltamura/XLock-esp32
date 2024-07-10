#include "rep.h"

#define REP_LOG;
bool rep_procedure(uint8_t *S_read, vault_t *V, size_t B_size, size_t L, size_t C, uint8_t *n, int *R, size_t key_size, uint8_t *T, uint8_t *K) {
    // Generate K_pre

    #ifdef REP_LOG
    // Print input values
    printf("rep_procedure inputs:\n");
    printf("S_read: ");
    for (size_t i = 0; i < PUF_RESPONSE_BYTES; i++) {
        printf("%02x ", S_read[i]);
    }
    // printf("\nV (vault): \n");
    // for (size_t i = 0; i < B_size; i++) {
    //     for (size_t j = 0; j < L; j++) {
    //         printf("bit_locker[%zu].locks[%zu]: lock = %02x, indexes = ", i, j, V->bit_lockers[i].locks[j].lock);
    //         for (size_t k = 0; k < C; k++) {
    //             printf("%d ", V->bit_lockers[i].locks[j].indexes[k]);
    //         }
    //         printf("\n");
    //     }
    // }
    printf("nonce (n): %02x\n", *n);
    printf("R: ");
    for (size_t i = 0; i < key_size; i++) {
        printf("%d ", R[i]);
    }
    printf("\nT: ");
    for (size_t i = 0; i < K_SIZE; i++) {
        printf("%02x ", T[i]);
    }
    printf("\n");
    #endif

    uint8_t K_pre[key_size];

    calculate_k_pre(K_pre, S_read, V, (int*)R, key_size, L, C);

    printf("rep_procedure K_pre: ");
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
    
    printf("Generated K: ");
        for (int i = 0; i < KEY_SIZE; i++) {
            printf("%02x", K[i]);
        }
    printf("\n");

    // Hash K and R to get T'
    uint8_t T_prime[K_SIZE];
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, K, K_SIZE);
    mbedtls_sha256_update(&ctx, R, key_size);
    mbedtls_sha256_finish(&ctx, T_prime);
    mbedtls_sha256_free(&ctx);

    // Verify T
    return memcmp(T, T_prime, K_SIZE) == 0;
}