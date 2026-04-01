/**
 * @file atl_storage.c
 * @brief Source file for NVS (Non-Volatile Storage) management.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @copyright CC-BY-SA-4.0 license
 */
#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <nvs.h>
#include "atl_led.h"

/* Constants */
static const char *TAG = "atl_storage"; //!< Module identification.

/**
 * @brief Initialize NVS (Non-Volatile Storage).
 * @details Initialize NVS system. If errors ESP_ERR_NVS_NO_FREE_PAGES or ESP_ERR_NVS_NEW_VERSION_FOUND occurs, erase flash and retry initialization.
 * @return esp_err_t - If ERR_OK success. 
 */
esp_err_t atl_storage_init(void) {
    esp_err_t err = ESP_OK;
    ESP_LOGI(TAG, "Starting NVS (Non-Volatile Storage)");

    /* TODO: Remove, for development only */
    nvs_flash_erase();

    /* Start NVS */
    err = nvs_flash_init();
    if (err != ESP_OK && err != ESP_ERR_NVS_NO_FREE_PAGES && err != ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGE(TAG, "Fail initializing NVS");
        goto error_proc;
    }
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing and restarting NVS");
        err = nvs_flash_erase();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail erasing NVS");
            goto error_proc;
        }
        err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail initializing NVS");
            goto error_proc;
        }
    }
    return err;

/* Error procedure */
error_proc:
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));
    atl_led_blink_error();
    return err;
}

/**
 * @brief Erase NVS (Non-Volatile Storage).
 * @details Erase NVS.
 * @return esp_err_t - If ERR_OK success. 
 */
esp_err_t atl_storage_erase_nvs(void) {
    esp_err_t err = ESP_OK;
    err = nvs_flash_erase();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail erasing NVS");
        goto error_proc;
    }
    return err;

/* Error procedure */
error_proc:
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));
    atl_led_blink_error();
    return err;
}