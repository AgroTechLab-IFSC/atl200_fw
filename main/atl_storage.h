/**
 * @file atl_storage.h
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @brief Header file for NVS (Non-Volatile Storage) management.
 */
#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize NVS (Non-Volatile Storage).
 * @details Initialize NVS system. If errors ESP_ERR_NVS_NO_FREE_PAGES or ESP_ERR_NVS_NEW_VERSION_FOUND occurs, erase flash and retry initialization.
 * @return esp_err_t - If ERR_OK success. 
 */
esp_err_t atl_storage_init(void);

/**
 * @brief Erase NVS (Non-Volatile Storage).
 * @details Erase NVS.
 * @return esp_err_t - If ERR_OK success. 
 */
esp_err_t atl_storage_erase_nvs(void);

#ifdef __cplusplus
}
#endif