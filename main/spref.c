#include "spref.h"

void measure_puf_response(uint8_t *puf_response) {
    if (PUF_RESPONSE_BYTES > RTC_FAST_MEM_SIZE) {
        printf("Error: PUF response size exceeds RTC Fast Memory bounds\n");
        return;
    }
    memcpy(puf_response, (uint8_t*)RTC_FAST_MEM_START, PUF_RESPONSE_BYTES);
}

void extract_S_pref(uint8_t **puf_responses, uint8_t *s_pref) {
    int count_ones[PUF_RESPONSE_BITS] = {0};

    // Count the occurrences of 1s for each bit position
    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        for (int j = 0; j < PUF_RESPONSE_BITS; j++) {
            int byte_index = j / 8;
            int bit_index = j % 8;
            if (puf_responses[i][byte_index] & (1 << bit_index)) {
                count_ones[j]++;
            }
        }
    }

    // Determine the most frequent bit for each position
    for (int j = 0; j < PUF_RESPONSE_BITS; j++) {
        int byte_index = j / 8;
        int bit_index = j % 8;
        if (count_ones[j] > NUM_MEASUREMENTS / 2) {
            s_pref[byte_index] |= (1 << bit_index);
        } else {
            s_pref[byte_index] &= ~(1 << bit_index);
        }
    }
}

// void calculate_spref(uint8_t *s_pref, nvs_handle_t nvs_handle) {
//     uint8_t **puf_responses = (uint8_t **)malloc(NUM_MEASUREMENTS * sizeof(uint8_t *));
//     if (puf_responses == NULL) {
//         printf("Failed to allocate memory for PUF responses\n");
//         return;
//     }
//     for (int i = 0; i < NUM_MEASUREMENTS; i++) {
//         puf_responses[i] = (uint8_t *)malloc(PUF_RESPONSE_BYTES);
//         if (puf_responses[i] == NULL) {
//             printf("Failed to allocate memory for PUF response %d\n", i);
//             return;
//         }
//     }

//     uint16_t measurement_index = 0;
//     esp_err_t ret = nvs_get_u16(nvs_handle, "meas_index", (uint16_t *)&measurement_index);
//     if (ret == ESP_ERR_NVS_NOT_FOUND) {
//         measurement_index = 0;
//     } else if (ret != ESP_OK) {
//         printf("Error (%s) reading from NVS! nvs_get_u16\n", esp_err_to_name(ret));
//         return;
//     }
    
//     if (measurement_index < NUM_MEASUREMENTS) {
//         measure_puf_response(puf_responses[measurement_index]);

//         char key[10];
//         snprintf(key, sizeof(key), "puf_%hu", measurement_index);
//         ret = nvs_set_blob(nvs_handle, key, puf_responses[measurement_index], PUF_RESPONSE_BYTES);
//         if (ret != ESP_OK) {
//             printf("Error (%s) saving to NVS!\n", esp_err_to_name(ret));
//         }
//         ret = nvs_commit(nvs_handle);
//         if (ret != ESP_OK) {
//             printf("Error (%s) committing to NVS!\n", esp_err_to_name(ret));
//         }

//         measurement_index++;
//         ret = nvs_set_u16(nvs_handle, "meas_index", measurement_index);
//         if (ret != ESP_OK) {
//             printf("Error (%s) saving to NVS!\n", esp_err_to_name(ret));
//         }
//         ret = nvs_commit(nvs_handle);
//         if (ret != ESP_OK) {
//             printf("Error (%s) committing to NVS!\n", esp_err_to_name(ret));
//         }

//         nvs_close(nvs_handle);
//         esp_restart();
//     } else {
//         for (int i = 0; i < NUM_MEASUREMENTS; i++) {
//             char key[10];
//             snprintf(key, sizeof(key), "puf_%d", i);
//             size_t required_size = PUF_RESPONSE_BYTES;
//             ret = nvs_get_blob(nvs_handle, key, puf_responses[i], &required_size);
//             if (ret != ESP_OK) {
//                 printf("Error (%s) reading from NVS! nvs_get_blob , index %d\n", esp_err_to_name(ret), i);
//                 return;
//             }
//         }
//         noise_reduction(puf_responses, s_pref);
        
//         for (int i = 0; i < NUM_MEASUREMENTS; i++) {
//             free(puf_responses[i]);
//         }
//         free(puf_responses);


//         #ifdef LOG_SPREF
//         // Print S_pref
//         printf("S_pref: ");
//         for (int i = 0; i < PUF_RESPONSE_BYTES; i++) {
//             printf("%02x", s_pref[i]);
//         }
//         printf("\n");
//         #endif
//     }
// }


void sample_spref(nvs_handle_t nvs_handle) {
    uint8_t **puf_responses = (uint8_t **)malloc(NUM_MEASUREMENTS * sizeof(uint8_t *));
    if (puf_responses == NULL) {
        printf("Failed to allocate memory for PUF responses\n");
        return;
    }
    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        puf_responses[i] = (uint8_t *)malloc(PUF_RESPONSE_BYTES);
        if (puf_responses[i] == NULL) {
            printf("Failed to allocate memory for PUF response %d\n", i);
            return;
        }
    }

    uint16_t measurement_index = 0;
    esp_err_t ret = nvs_get_u16(nvs_handle, "meas_index", &measurement_index);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        measurement_index = 0;
    } else if (ret != ESP_OK) {
        printf("Error (%s) reading from NVS! nvs_get_u16\n", esp_err_to_name(ret));
        return;
    }

    //ESP_LOGW("PUF INDEX ", "PUF Response %d: ", measurement_index);
    if (measurement_index < NUM_MEASUREMENTS) {
        measure_puf_response(puf_responses[measurement_index]);
        // for (int j = 0; j < PUF_RESPONSE_BYTES; j++) {
        //     printf("%02x", puf_responses[measurement_index][j]);
        // }
        // vTaskDelay(100);
        char key[10];
        snprintf(key, sizeof(key), "puf_%hu", measurement_index);
        ret = nvs_set_blob(nvs_handle, key, puf_responses[measurement_index], PUF_RESPONSE_BYTES);
        if (ret != ESP_OK) {
            printf("Error (%s) saving to NVS!\n", esp_err_to_name(ret));
        }
        ret = nvs_commit(nvs_handle);
        if (ret != ESP_OK) {
            printf("Error (%s) committing to NVS!\n", esp_err_to_name(ret));
        }

        measurement_index++;
        ret = nvs_set_u16(nvs_handle, "meas_index", measurement_index);
        if (ret != ESP_OK) {
            printf("Error (%s) saving to NVS!\n", esp_err_to_name(ret));
        }
        ret = nvs_commit(nvs_handle);
        if (ret != ESP_OK) {
            printf("Error (%s) committing to NVS!\n", esp_err_to_name(ret));
        }

        nvs_close(nvs_handle);
        esp_restart();
    } else {


        get_puf_responses(nvs_handle, puf_responses);

        for (size_t i = 0; i < NUM_MEASUREMENTS; i++)
        {
            ESP_LOG_BUFFER_HEX("PUF RESPONSE",puf_responses[i],PUF_RESPONSE_BYTES);

        }
        
        // All measurements are done, calculate S_pref and store it in NVS
        uint8_t s_pref[PUF_RESPONSE_BYTES];
        evaluate_puf_response(puf_responses);
        extract_S_pref(puf_responses, s_pref);

        ret = nvs_set_blob(nvs_handle, "s_pref", s_pref, PUF_RESPONSE_BYTES);
        if (ret != ESP_OK) {
            printf("Error (%s) saving S_pref to NVS!\n", esp_err_to_name(ret));
        }
        ret = nvs_commit(nvs_handle);
        if (ret != ESP_OK) {
            printf("Error (%s) committing S_pref to NVS!\n", esp_err_to_name(ret));
        }

#ifdef LOG_SPREF
        // Print S_pref
        printf("S_pref: ");
        for (int i = 0; i < PUF_RESPONSE_BYTES; i++) {
            printf("%02x", s_pref[i]);
        }
        printf("\n");
#endif
    }

    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        free(puf_responses[i]);
    }
    free(puf_responses);
}

void get_spref(uint8_t *s_pref, nvs_handle_t nvs_handle) {
    size_t required_size = PUF_RESPONSE_BYTES;
    esp_err_t ret = nvs_get_blob(nvs_handle, "s_pref", s_pref, &required_size);
    if (ret != ESP_OK) {
        printf("Error (%s) reading S_pref from NVS!\n", esp_err_to_name(ret));
    }

#ifdef LOG_SPREF
    // Print S_pref
    printf("S_pref: ");
    for (int i = 0; i < PUF_RESPONSE_BYTES; i++) {
        printf("%02x", s_pref[i]);
    }
    printf("\n");
#endif
}

void get_puf_responses(nvs_handle_t nvs_handle, uint8_t **puf_responses) {
    for (int i = 0; i < NUM_MEASUREMENTS; i++) {
        char key[10];
        snprintf(key, sizeof(key), "puf_%d", i);
        size_t required_size = PUF_RESPONSE_BYTES;
        esp_err_t ret = nvs_get_blob(nvs_handle, key, puf_responses[i], &required_size);
        if (ret != ESP_OK) {
            printf("Error (%s) reading PUF response from NVS, index %d\n", esp_err_to_name(ret), i);
        }
    }
}