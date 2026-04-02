/**
 * @file atl200_main.c
 * @brief Main file for the ATL200 firmware.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @copyright CC-BY-SA-4.0 license
 */
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "sdkconfig.h"
#include "atl_led.h"
#include "atl_button.h"
#include "atl_storage.h"

/* Constants */
static const char *TAG = "atl_main";    //!< Module identification.

/**
 * @brief Main application entry point.
 * @details This function initializes the LED strip and starts the main application logic.
 *          It is called by the ESP-IDF framework after the system has been initialized.
 */
void app_main(void) {
    esp_err_t ret = ESP_OK;
    ESP_LOGI(TAG, "ATL-200 starting...");
    
    /* Configure LED builtin */
    ret = atl_led_init();

    /* Initialize button task */
    ret = atl_button_init();

    /* Initialize NVS */
    ret = atl_storage_init();

    /* Check if ATL-200 initialization was successful */
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start ATL-200! Error: %d", ret);
        return;
    } else {
        ESP_LOGI(TAG, "ATL-200 has been successfully started!");
    }
}