#ifndef REP_H
#define REP_H

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
//#include "puf_gen.h"
#include "definitions.h"
#include "spref.h"
#include "vault.h"
#include "gen.h"

bool rep_procedure(uint8_t *S_read, vault_t *V, size_t B_size, size_t L, size_t C, uint8_t *n, int *R, size_t key_size, uint8_t *T, uint8_t *K);

#endif