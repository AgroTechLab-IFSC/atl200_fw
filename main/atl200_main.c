/**
 * @file atl200_main.c
 * @brief Main file for the ATL200 firmware.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 */
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include "sdkconfig.h"

/* Set the log level for this file */
static esp_log_level_t log_level = ESP_LOG_INFO;

/* Constants */
static const char *TAG = "main";

/**
 * @brief Main application entry point.
 * @details This function initializes the LED strip and starts the main application logic.
 *          It is called by the ESP-IDF framework after the system has been initialized.
 */
void app_main(void) {

    esp_log_level_set(TAG, ESP_LOG_INFO);
    ESP_LOGI(TAG, "Starting ATL-200 firmware...");
    
    /* Configure the peripheral according to the LED type */
    // configure_led();
}