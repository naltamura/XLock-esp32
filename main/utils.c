#include "utils.h"
#include "definitions.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>

esp_err_t initialize_nvs_and_memory(uint8_t **s_pref, uint8_t **s_read, nvs_handle_t nvs_handle) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    *s_pref = (uint8_t *)malloc(PUF_RESPONSE_BYTES);
    if (*s_pref == NULL) {
        printf("Failed to allocate memory for S_pref\n");
        return ESP_ERR_NO_MEM;
    }
    *s_read = (uint8_t *)malloc(PUF_RESPONSE_BYTES);
    if (*s_read == NULL) {
        printf("Failed to allocate memory for S_read\n");
        free(*s_pref);
        return ESP_ERR_NO_MEM;
    }

    // ret = nvs_open("storage", NVS_READWRITE, nvs_handle);
    // if (ret != ESP_OK) {
    //     printf("Error (%s) opening NVS handle!\n", esp_err_to_name(ret));
    //     free(*s_pref);
    //     free(*s_read);
    //     return ret;
    // }

    return ESP_OK;
}





float calculate_bit_entropy(uint8_t *data, int length) {
    int count[2] = {0};  // We only have two possible values for bits: 0 and 1

    for (int i = 0; i < length; i++) {
        for (int bit = 0; bit < 8; bit++) {
            int bit_value = (data[i] >> bit) & 1;
            count[bit_value]++;
        }
    }

    int total_bits = length * 8;
    float p0 = (float)count[0] / total_bits;
    float p1 = (float)count[1] / total_bits;

    float entropy = 0.0;
    if (p0 > 0) {
        entropy -= p0 * log2(p0);
    }
    if (p1 > 0) {
        entropy -= p1 * log2(p1);
    }

    return entropy;
}

void evaluate_puf_response(uint8_t *puf_response) {
    int count_ones = 0;
    for (int i = 0; i < PUF_RESPONSE_BYTES; i++) {
        count_ones += __builtin_popcount(puf_response[i]);
    }
    float proportion_ones = (float)count_ones / PUF_RESPONSE_BITS;
    printf("Proportion of Ones: %.2f\n", proportion_ones);

    float entropy_per_bit = calculate_bit_entropy(puf_response, PUF_RESPONSE_BYTES);
    printf("Entropy per Bit: %.2f bits/bit\n", entropy_per_bit);
    float total_entropy = entropy_per_bit * PUF_RESPONSE_BITS;
    printf("Total Entropy: %.2f bits (out of %d bits)\n", total_entropy, PUF_RESPONSE_BITS);

    float entropy_percentage = (total_entropy / PUF_RESPONSE_BITS) * 100;
    printf("Entropy Percentage: %.2f%%\n", entropy_percentage);
}