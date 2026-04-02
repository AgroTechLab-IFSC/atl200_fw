/**
 * @file atl_button.c
 * @brief Source file for button management.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @copyright CC-BY-SA-4.0 license
 */
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include "atl_led.h"

/* Constants */
static const char *TAG = "atl_button";           //!< Module identification.

/* Global variables */
static QueueHandle_t button_evt_queue;          //!< Button event queue.
static TaskHandle_t atl_button_handle = NULL;   //!< Button task handle.
bool atl_button_pressed = false;                //!< Button pressed.

/**
 * @brief Button event handler
 * @param [in] args - Pointer to task arguments
*/
static void IRAM_ATTR button_isr_handler(void *args) {
    uint32_t button_pin = (uint32_t)args;
    xQueueSendFromISR(button_evt_queue, &button_pin, NULL);  
}

/**
 * @brief Button task
 * @param [in] args - Pointer to task arguments 
*/
static void atl_button_task(void *args) {
    uint32_t gpio_pin;

    ESP_LOGI(TAG, "Button task created");

    /* Task looping */
    while (true) {

        /* Check for button event */
        if (xQueueReceive(button_evt_queue, &gpio_pin, portMAX_DELAY)) {
            if (gpio_get_level(CONFIG_ATL_BUTTON_BUILTIN_PIN) == 0) {
                atl_button_pressed = true; 
            } else {
                atl_button_pressed = false;
            }             
        }        
    }    
}

/**
 * @brief Initialize button task.
 * @return esp_err_t
*/
esp_err_t atl_button_init(void) {
    esp_err_t err = ESP_OK;

    /* Configure button event */
    ESP_LOGI(TAG, "Starting button");
    err = gpio_set_direction(CONFIG_ATL_BUTTON_BUILTIN_PIN, GPIO_MODE_INPUT);
    err = gpio_pulldown_en(CONFIG_ATL_BUTTON_BUILTIN_PIN);
    err = gpio_pullup_dis(CONFIG_ATL_BUTTON_BUILTIN_PIN);
    err = gpio_set_intr_type(CONFIG_ATL_BUTTON_BUILTIN_PIN, GPIO_INTR_ANYEDGE);
    button_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    if (button_evt_queue == NULL) {
        ESP_LOGE(TAG, "Fail creating button event queue");
        goto error_proc;
    }

    /* Create BUTTON task at selected CPU */
    if (xTaskCreatePinnedToCore(atl_button_task, "atl_button", 2048, NULL, 10, &atl_button_handle, 0) != pdPASS) {
        ESP_LOGE(TAG, "Fail creating atl_button_task");
        err = ESP_ERR_NO_MEM;
        goto error_proc;   
    }

    /* Install interruption handler at button event */
    err = gpio_install_isr_service(0);
    err = gpio_isr_handler_add(CONFIG_ATL_BUTTON_BUILTIN_PIN, button_isr_handler, (void*)CONFIG_ATL_BUTTON_BUILTIN_PIN);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail installing button interruption handler");
        if (atl_button_handle != NULL) {
            vTaskDelete(atl_button_handle);
        }
        goto error_proc;
    }

    return err;

/* Error procedure */
error_proc:
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));
    atl_led_blink_error();  
    return err;
}