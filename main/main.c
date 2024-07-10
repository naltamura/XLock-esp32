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
#include "vault.h"
#include "gen.h"
#include "rep.h"



void app_main(void) {
    uint8_t *s_pref = NULL;
    uint8_t *s_read = (uint8_t*)malloc(PUF_RESPONSE_BYTES);
    uint8_t *s_read_prime = (uint8_t*)malloc(PUF_RESPONSE_BYTES);
    uint8_t B[RANDOM_POOL_BITS]; // Random pool
    uint8_t K[KEY_SIZE];
    uint8_t n;
    int R[KEY_SIZE];
    uint8_t T[KEY_SIZE];
    nvs_handle_t nvs_handle;
    esp_err_t ret = 0;
    
    
#ifdef RESET_NVS   
    ret = nvs_flash_erase();
    if (ret != ESP_OK) {
        printf("Error (%s) erasing NVS flash!\n", esp_err_to_name(ret));
    } else {
        printf("NVS flash erased successfully!\n");
        
    }
#else
    
    ret = initialize_nvs_and_memory(&s_pref, &s_read, &nvs_handle);
    //ret = initialize_nvs_and_memory(&s_pref, &s_read_prime, &nvs_handle);
    
    if (ret != ESP_OK) {
        return;
    }

    #ifdef INIT_SPREF   

        sample_spref(nvs_handle);
        get_spref(s_pref,nvs_handle);
        #ifdef LOG_MAIN
        // Print S_pref
        printf("S_pref: ");
        for (int i = 0; i < PUF_RESPONSE_BYTES; i++) {
            printf("%02x", s_pref[i]);
        }
        printf("\n");
        #endif

        vault_t *vault = init_vault(s_pref, PUF_RESPONSE_BYTES, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR);

        // Store and print the vault
        store_vault(vault, nvs_handle, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR);
        print_vault(vault, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR);
        vTaskDelay(100);
        free_vault(vault, RANDOM_POOL_BITS);
        free(s_pref);
        free(s_read);
    #else
        #ifdef GEN_PROCEDURE 
        vault_t *retrieved_vault = get_vault(nvs_handle, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR);
        //print_vault(retrieved_vault, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR);
        vTaskDelay(100);

        // Measure PUF response to get S_read
        measure_puf_response(s_read);

        #ifdef LOG_MAIN
            // Print S_read
            printf("S_read: ");
            for (int i = 0; i < PUF_RESPONSE_BYTES; i++) {
            printf("%02x", s_read[i]);
            }
            printf("\n");
        #endif  
        
        gen_procedure(s_read, retrieved_vault, RANDOM_POOL_BITS, LOCKS_PER_BIT,BITS_PER_XOR, K, &n, R, KEY_SIZE, T);

        // Print generated K, n, R, T
        printf("Generated K: ");
        for (int i = 0; i < KEY_SIZE; i++) {
            printf("%02x", K[i]);
        }
        printf("\n");

        printf("Generated n: %02x\n", n);

        printf("Generated R: ");
        for (int i = 0; i < KEY_SIZE; i++) {
            printf("%d", R[i]);
        }
        printf("\n");

        printf("Generated T: ");
        for (int i = 0; i < KEY_SIZE; i++) {
            printf("%02x", T[i]);
        }
        printf("\n");
        store_gen_values(nvs_handle, R, T, n, K_SIZE);
        #endif

        #ifdef REP_PROCEDURE
            ret = initialize_nvs_and_memory(&s_pref, &s_read, &nvs_handle);
            vault_t *retrieved_vault = get_vault(nvs_handle, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR);
            //print_vault(retrieved_vault, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR);
            vTaskDelay(100);
            
            retrieve_gen_values(nvs_handle, R, T, &n, K_SIZE);

            measure_puf_response(s_read_prime);
            
            #ifdef LOG_MAIN
                // Print S_read
                printf("S_read_rep: ");
                for (int i = 0; i < PUF_RESPONSE_BYTES; i++) {
                printf("%02x", s_read_prime[i]);
                }
                printf("\n");
            #endif  
            
            
            // Execute the REP procedure
            bool is_valid = rep_procedure(s_read_prime, retrieved_vault, RANDOM_POOL_BITS, LOCKS_PER_BIT, BITS_PER_XOR, &n, R, K_SIZE, T, K);

            
            if (is_valid) {
                printf("The response is valid!\n");
            } else {
                printf("The response is invalid!\n");
            }
            vTaskDelay(500);
            
            free_vault(retrieved_vault, RANDOM_POOL_BITS);
            free(s_pref);
            free(s_read);
        
        #endif

    #endif

#endif

}
