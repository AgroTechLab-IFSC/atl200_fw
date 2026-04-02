/**
 * @file atl_led.c
 * @brief Source file for LED management.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @copyright CC-BY-SA-4.0 license
 */
#include <stdio.h>
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <esp_log.h>
#include <led_strip.h>
#include "atl_led.h"
#include "atl_storage.h"

/* Constants */
static const char *TAG = "atl_led";                    //!< Module identification.
static const uint16_t atl_led_mutex_timeout = 5000;    //!< LED mutex default timeout.
static const char *atl_led_behaviour_str[] = {         //!< LED behaviour string.
    "ATL_LED_DISABLED",
    "ATL_LED_ENABLED_FAILS",
    "ATL_LED_ENABLED_COMM_FAILS",
    "ATL_LED_ENABLED_FULL",
};

/* Global variables */
static SemaphoreHandle_t atl_led_mutex;                        //!< LED mutex.
static bool atl_led_state = false;                             //!< LED enabled.
static led_strip_handle_t atl_led_strip;                       //!< LED handle.
static atl_led_rgb_color_t atl_led_rgb_color = {0, 0, 0};      //!< LED color.
static TaskHandle_t atl_led_handle = NULL;                     //!< LED task handle.
static uint8_t atl_button_count = 0;                           //!< Button pressed count.
extern bool atl_button_pressed;                                //!< Button pressed.

/**
 * @brief LED builtin task
 * @param [in] args - Pointer to task arguments 
*/
static void atl_led_task(void *args) {

    ESP_LOGI(TAG, "LED task created");

    /* Set run color */
    atl_led_set_color(0, 0, 255);

    /* Task looping */
    while (true) {

        /* Check if button was pressed */
        while (atl_button_pressed) {
            atl_button_count++;
            
            /* Check if is factory reset */
            if (atl_button_count == 10) {
                ESP_LOGW(TAG, ">>> Executing factory reset <<<");
                atl_led_blink_reboot();
                atl_storage_erase_nvs();
                esp_restart();
            }

            /* Toggle period */
            atl_led_blink(2, 100, 255, 69, 0);
            vTaskDelay(pdMS_TO_TICKS(250));

        }
        
        /* Toggle led builtin */
        atl_led_toggle();

        /* Toggle period */
        vTaskDelay(pdMS_TO_TICKS(CONFIG_ATL_LED_PERIOD));
    }    
}

/**
 * @brief Get the led behaviour string object.
 * @param behaviour - LED behaviour enum value
 * @return LED behaviour string
 */
const char* atl_led_get_behaviour_str(atl_led_behaviour_e behaviour) {
    return atl_led_behaviour_str[behaviour];
}

/**
 * @brief Get the led behaviour string object.
 * @param behaviour_str - LED behaviour string
 * @return LED behaviour enum
 */
atl_led_behaviour_e atl_led_get_behaviour(char *behaviour_str) {
    uint8_t i = 0;
    uint8_t count = sizeof(atl_led_behaviour_str) / sizeof(atl_led_behaviour_str[0]);
    for (i = 0; i < count; i++) {
        if (strcmp(behaviour_str, atl_led_behaviour_str[i]) == 0) {
            return i;
        }
    }
    return ATL_LED_BEHAVIOUR_INVALID;
}

/**
 * @brief Initialize led task.
 * @return esp_err_t
*/
esp_err_t atl_led_init(void) {
    esp_err_t err = ESP_OK;

    /* Creating LED mutex */
    atl_led_mutex = xSemaphoreCreateMutex();
    if (atl_led_mutex == NULL) {
        ESP_LOGE(TAG, "Could not create LED mutex");
        goto error_proc;
    }

    /* LED strip general initialization, according to your led board design */
    led_strip_config_t strip_config = {
        .strip_gpio_num = CONFIG_ATL_LED_GPIO,                         //!< The GPIO that connected to the LED strip's data line.
        .max_leds = 1,                                                  //!< The number of LEDs in the strip.
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,    //!< Pixel format of your LED strip.
        .led_model = LED_MODEL_WS2812,                                  //!< LED strip model.
        .flags.invert_out = false,                                      //!< whether to invert the output signal.
    };

    /* LED strip backend configuration: RMT */
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        //!< different clock source can lead to different power consumption.
        .resolution_hz = LED_STRIP_RMT_RES_HZ, //!< RMT counter clock frequency.
        .mem_block_symbols = 64,               //!< Number of symbols that can be stored in one RMT memory block. Set this according to the number of LEDs in your strip and the timing parameters of your LED strip model.
        .flags.with_dma = false,               //!< DMA feature is available on ESP target like ESP32-S3.
    };         

    /* Initialize LED builtin */
    ESP_LOGI(TAG, "Created strip object with RMT backend");
    err = led_strip_new_rmt_device(&strip_config, &rmt_config, &atl_led_strip);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail creating RMT device at LED");
        goto error_proc;
    }

    /* Power off led strip */
    atl_led_set_enabled(false);

    /* Set LED to blue (initialization color) */
    atl_led_set_color(0, 0, 255);
    
    /* Power on led strip */
    atl_led_set_enabled(true);

    /* Create LED RGB task at selected CPU */
    if (xTaskCreatePinnedToCore(atl_led_task, "atl_led", 2048, NULL, 10, &atl_led_handle, 1) != pdPASS) {
        ESP_LOGE(TAG, "Fail creating LED task");
        goto error_proc;   
    }

    return err;

/* Error procedure */
error_proc:
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));   
    return err;  
}

/**
 * @brief Reset led task.
 * @return esp_err_t
*/
esp_err_t atl_led_reset(void) {
    esp_err_t err = ESP_OK;

    /* Delete task */
    if (atl_led_handle != NULL) {
        vTaskDelete(atl_led_handle);
        atl_led_handle = NULL;
    }

    /* Delete mutex */
    if (atl_led_mutex != NULL) {
        vSemaphoreDelete(atl_led_mutex);
        atl_led_mutex = NULL;
    }

    atl_led_state = false;
    atl_button_count = 0;

    /* Initialize LED RGB */
    err = atl_led_init();

    return err;
}

/**
 * @brief Toggle led.
 * @return esp_err_t
*/
esp_err_t atl_led_toggle(void) {
    esp_err_t err = ESP_OK;

    /* Take semaphore */
    if (!xSemaphoreTake(atl_led_mutex, pdMS_TO_TICKS(atl_led_mutex_timeout))) {
        ESP_LOGE(TAG, "Timeout taking LED mutex");
        return ESP_ERR_TIMEOUT;
    }
    
    /* Update led state variable */
    atl_led_state = !atl_led_state;

    /* Change LED strip state */
    if (atl_led_state == true) {
        err = led_strip_set_pixel(atl_led_strip, 0, atl_led_rgb_color.red, atl_led_rgb_color.green, atl_led_rgb_color.blue);
        err = led_strip_refresh(atl_led_strip);
    } else {
        err = led_strip_clear(atl_led_strip);
        err = led_strip_refresh(atl_led_strip);
    }

    /* Give semaphore */
    if (!xSemaphoreGive(atl_led_mutex)) {
        ESP_LOGE(TAG, "Fail giving LED mutex");
        return ESP_FAIL;
    }

    return err;
}

/**
 * @brief Blink led.
 * @param [in] times blink times
 * @param [in] interval interval between blinks
 * @param [in] red red value (0..255)
 * @param [in] green green value (0..255)
 * @param [in] blue blue value (0..255)
*/
void atl_led_blink(uint8_t times, uint16_t interval, uint8_t red, uint8_t green, uint8_t blue) {
    /* Take semaphore */
    if (!xSemaphoreTake(atl_led_mutex, pdMS_TO_TICKS(atl_led_mutex_timeout))) {
        ESP_LOGE(TAG, "Timeout taking LED mutex");
        return;
    }
 
    /* Set all LED off to clear all pixels */
    led_strip_clear(atl_led_strip);
    led_strip_refresh(atl_led_strip);

    /* Blink looping */    
    for (uint8_t i = 0; i < times; i++) {
        
        /* Set the LED on */
        led_strip_set_pixel(atl_led_strip, 0, red, green, blue);
        led_strip_refresh(atl_led_strip);

        /* Wait ON interval */
        vTaskDelay(pdMS_TO_TICKS(200));

        /* Set all LED off */
        led_strip_clear(atl_led_strip);
        led_strip_refresh(atl_led_strip);

        /* Wait OFF interval */
        vTaskDelay(pdMS_TO_TICKS(interval));
    }

    /* Give semaphore */
    if (!xSemaphoreGive(atl_led_mutex)) {
        ESP_LOGE(TAG, "Fail giving LED mutex");
    }

}

/**
 * @brief Set led builtin color.
 * @param [in] red red value (0..255)
 * @param [in] green green value (0..255)
 * @param [in] blue blue value (0..255)
*/
void atl_led_set_color(uint8_t red, uint8_t green, uint8_t blue) {
    /* Take semaphore */
    if (!xSemaphoreTake(atl_led_mutex, pdMS_TO_TICKS(atl_led_mutex_timeout))) {
        ESP_LOGE(TAG, "Timeout taking LED mutex");
        return;
    }

    /* Update LED color */
    atl_led_rgb_color.red = red;
    atl_led_rgb_color.green = green;
    atl_led_rgb_color.blue = blue;

    /* Give semaphore */
    if (!xSemaphoreGive(atl_led_mutex)) {
        ESP_LOGW(TAG, "Fail giving LED mutex");
    }

}

/**
 * @brief Enabled/disabled led.
 * @param [in] status enabled or disabled led
*/
void atl_led_set_enabled(bool status) {
    
    /* Take semaphore */
    if (!xSemaphoreTake(atl_led_mutex, pdMS_TO_TICKS(atl_led_mutex_timeout))) {
        ESP_LOGE(TAG, "Timeout taking LED mutex");
        return;
    }

    /* Update led state */
    atl_led_state = status;

    /* Update LED strip */
    if (status == false) {
        led_strip_clear(atl_led_strip);
        led_strip_refresh(atl_led_strip);
    } else {
        led_strip_set_pixel(atl_led_strip, 0, atl_led_rgb_color.red, atl_led_rgb_color.green, atl_led_rgb_color.blue);
        led_strip_refresh(atl_led_strip);
    }

    /* Give semaphore */
    if (!xSemaphoreGive(atl_led_mutex)) {
        ESP_LOGE(TAG, "Fail giving LED mutex");
    }
}

/**
 * @brief Blink led rgb error
*/
void atl_led_blink_error() {
    atl_led_blink(5, 100, 255, 0, 0);
}

/**
 * @brief Blink led reboot
*/
void atl_led_blink_reboot() {
    atl_led_blink(3, 300, 255, 165, 0);
}