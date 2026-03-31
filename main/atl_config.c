/**
 * @file atl_config.c
 * @author Robson Costa (robson.costa@ifs.edu.br)
 * @brief Source file for configuration management.
 */
#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <esp_err.h>
#include <esp_log.h>
#include <esp_mac.h>
#include <nvs.h>
#include "sdkconfig.h"
#include "atl_config.h"

/* Constants */
static const char *TAG = "atl_config";                   //!< Module identification.
static const uint32_t atl_config_mutex_timeout = 5000;   //!< Configuration mutex timeout

/* Global variables */
static SemaphoreHandle_t atl_config_mutex;       //!< Configuration semaphore.
static atl_config_t atl_config;                   //!< Configuration structure.

/**
 * @brief Create configuration file with default values (based on MenuConfig).
 * @details Create configuration file with default values (based on MenuConfig). 
 */
static void atl_config_create_default(void) {    
    char ssid[32];
    unsigned char mac[6] = {0};

    /** Creates default LED configuration **/
    atl_config.led.behaviour = (atl_led_behaviour_e)ATL_LED_ENABLED_FULL;

    /** Creates default WiFi configuration **/
    // atl_config.wifi.mode = CONFIG_ATL_WIFI_AP_MODE;
    // esp_efuse_mac_get_default(mac);
    // sprintf(ssid, "%s%02x%02x%02x", CONFIG_ATL_WIFI_AP_PREFIX, mac[3], mac[4], mac[5]+1);
    // strncpy((char*)&atl_config.wifi.ap_ssid, ssid, sizeof(atl_config.wifi.ap_ssid));
    // snprintf((char *)atl_config.wifi.ap_pass, sizeof(atl_config.wifi.ap_pass), CONFIG_ATL_WIFI_AP_PASS);
    // atl_config.wifi.ap_channel = CONFIG_ATL_WIFI_AP_CHANNEL;
    // atl_config.wifi.ap_max_conn = CONFIG_ATL_WIFI_AP_MAX_CONN;
    // strncpy((char*)&atl_config.wifi.sta_ssid, ssid, sizeof(atl_config.wifi.ap_ssid));
    // snprintf((char *)atl_config.wifi.sta_pass, sizeof(atl_config.wifi.sta_pass), CONFIG_ATL_WIFI_AP_PASS);
    // atl_config.wifi.sta_max_conn_retry = 5;

    /** Creates default Webserver configuration **/
    // atl_config.webserver.mode = (atl_webserver_mode_e)CONFIG_ATL_WEBSERVER_MODE;
    // snprintf((char *)atl_config.webserver.admin_user, sizeof(atl_config.webserver.admin_user), CONFIG_ATL_WEBSERVER_ADMIN_USER);
    // snprintf((char *)atl_config.webserver.admin_pass, sizeof(atl_config.webserver.admin_pass), CONFIG_ATL_WEBSERVER_ADMIN_PASS);
}

/**
 * @brief Initialize configuration from NVS.
 * @details If not possible load configuration file, create a new with default values.
 * @return esp_err_t - If ERR_OK success, otherwise fail.
 */
esp_err_t atl_config_init(void) {
    esp_err_t err = ESP_OK;
    nvs_handle_t nvs_handler;

    ESP_LOGI(TAG, "Starting configuration management");

    /* Creates configuration semaphore (mutex) */
    atl_config_mutex = xSemaphoreCreateMutex();
    if (atl_config_mutex == NULL) {
        ESP_LOGE(TAG, "Error creating configuration semaphore!");
        err = ESP_FAIL;
        goto error_proc;
    }

    /* Open NVS system */
    ESP_LOGI(TAG, "Mounting NVS storage");
    err = nvs_open("nvs", NVS_READWRITE, &nvs_handler);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail mounting NVS storage");
        goto error_proc;
    }

    /* Read the memory size required to configuration file */
    ESP_LOGI(TAG, "Loading configuration file");
    size_t file_size = sizeof(atl_config_t);
    err = nvs_get_blob(nvs_handler, "atl_cfg", &atl_config, &file_size);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "Fail loading configuration file!");
        goto error_proc;
    } else if (err != ESP_OK) {
        ESP_LOGW(TAG, "File not found! Creating new file with default values!");
        atl_config_create_default();

        /* Creates configuration file */
        err = nvs_set_blob(nvs_handler, "atl_cfg", &atl_config, sizeof(atl_config_t));
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail creating new configuration file!");
            goto error_proc;
        }

        /* Write configuration file in NVS */
        err = nvs_commit(nvs_handler);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail writing new configuration file!");
            goto error_proc;
        } 
    }

    /* Close NVS */
    ESP_LOGI(TAG, "Unmounting NVS storage");
    nvs_close(nvs_handler);
    return ESP_OK;

/* Error procedure */
error_proc:
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));
    nvs_close(nvs_handler);
    atl_led_blink_error();
    return err;
}

/**
 * @brief Commit configuration at NVS.
 * @return esp_err_t - If ERR_OK success.
 */
static esp_err_t atl_config_commit_nvs(void) {
    esp_err_t err = ESP_OK;
    nvs_handle_t nvs_handler;
    ESP_LOGD(TAG, "Commiting configuration at NVS");    
    
    /* Open NVS system */
    err = nvs_open("nvs", NVS_READWRITE, &nvs_handler);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail mounting NVS storage");
        goto error_proc;
    }

    /* Creates configuration file */
    err = nvs_set_blob(nvs_handler, "atl_cfg", &atl_config, sizeof(atl_config_t));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail creating new configuration file!");
        goto error_proc;
    }

    /* Write configuration file in NVS */
    err = nvs_commit(nvs_handler);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail writing new configuration file!");
        goto error_proc;
    }

    /* Close NVS */
    ESP_LOGD(TAG, "Unmounting NVS storage");
    nvs_close(nvs_handler);
    return ESP_OK;

/* Error procedure */
error_proc:
    ESP_LOGE(TAG, "Error: %s", esp_err_to_name(err));
    nvs_close(nvs_handler);
    return err;
}


/**
 * @brief Get all configuration.
 * @param[out] config_ptr pointer to write configuration.
 */
void atl_config_get(atl_config_t *config_ptr) {

    /* Take semaphore */
    if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
        ESP_LOGW(TAG, "Timeout taking mutex");
    }
    
    /* Get configuration */
    *config_ptr = atl_config;

    /* Give semaphore */
    if (!xSemaphoreGive(atl_config_mutex)) {
        ESP_LOGW(TAG, "Fail giving mutex");
    }
}


/**
 * @brief Set all configuration.
 * @param[in] config_ptr pointer with new configuration.
 */
void atl_config_set(atl_config_t *config_ptr) {
    /* Take semaphore */
    if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
        ESP_LOGW(TAG, "Timeout taking mutex");
    }
    
    /* Set configuration */
    atl_config = *config_ptr;

    /* Commit configuration to NVS */
    if (atl_config_commit_nvs() != ESP_OK) {
        ESP_LOGE(TAG, "Fail to commit configuration to NVS!");
        atl_led_blink_error();
    }

    /* Give semaphore */
    if (!xSemaphoreGive(atl_config_mutex)) {
        ESP_LOGW(TAG, "Fail giving mutex");
    }
}


/**
 * @brief Get LED configuration.
 * @param[out] led_config_ptr pointer to write configuration.
 */
void atl_config_led_get(atl_led_config_t *led_config_ptr) {

    /* Take semaphore */
    if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
        ESP_LOGW(TAG, "Timeout taking mutex");
    }
    
    /* Get LED configuration */
    *led_config_ptr = atl_config.led;

    /* Give semaphore */
    if (!xSemaphoreGive(atl_config_mutex)) {
        ESP_LOGW(TAG, "Fail giving mutex");
    }
}


/**
 * @brief Set LED configuration.
 * @param[in] led_config_ptr pointer with new configuration.
 */
void atl_config_led_set(atl_led_config_t *led_config_ptr) {

    /* Take semaphore */
    if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
        ESP_LOGW(TAG, "Timeout taking mutex");
    }

    /* Set LED configuration */
    atl_config.led = *led_config_ptr;

    /* Commit configuration to NVS */
    if (atl_config_commit_nvs() != ESP_OK) {
        ESP_LOGE(TAG, "Fail to commit configuration to NVS!");
        atl_led_blink_error();
    }

    /* Give semaphore */
    if (!xSemaphoreGive(atl_config_mutex)) {
        ESP_LOGW(TAG, "Fail giving mutex");
    }
}

// /**
//  * @brief Get WiFi configuration.
//  * @param[out] wifi_config_ptr pointer to write configuration.
//  */
// void atl_config_wifi_get(atl_wifi_config_t *wifi_config_ptr) {

//     /* Take semaphore */
//     if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
//         ESP_LOGW(TAG, "Timeout taking mutex");
//     }

//     /* Get WiFi configuration */
//     *wifi_config_ptr = atl_config.wifi;

//     /* Give semaphore */
//     if (!xSemaphoreGive(atl_config_mutex)) {
//         ESP_LOGW(TAG, "Fail giving mutex");
//     }
// }


// /**
//  * @brief Set WiFi configuration.
//  * @param[in] wifi_config_ptr pointer with new configuration.
//  */
// void atl_config_wifi_set(atl_wifi_config_t *wifi_config_ptr) {

//     /* Take semaphore */
//     if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
//         ESP_LOGW(TAG, "Timeout taking mutex");
//     }

//     /* Set WiFi configuration */
//     atl_config.wifi = *wifi_config_ptr;

//     /* Commit configuration to NVS */
//     if (atl_config_commit_nvs() != ESP_OK) {
//         ESP_LOGE(TAG, "Fail to commit configuration to NVS!");
//         atl_led_blink_error();
//     }

//     /* Give semaphore */
//     if (!xSemaphoreGive(atl_config_mutex)) {
//         ESP_LOGW(TAG, "Fail giving mutex");
//     }
// }

// /**
//  * @brief Get Webserver configuration.
//  * @param[out] webserver_config_ptr pointer to write configuration.
//  */
// void atl_config_webserver_get(atl_webserver_config_t *webserver_config_ptr) {

//     /* Take semaphore */
//     if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
//         ESP_LOGW(TAG, "Timeout taking mutex");
//     }

//     /* Get Webserver configuration */
//     *webserver_config_ptr = atl_config.webserver;

//     /* Give semaphore */
//     if (!xSemaphoreGive(atl_config_mutex)) {
//         ESP_LOGW(TAG, "Fail giving mutex");
//     }
// }


// /**
//  * @brief Set Webserver configuration.
//  * @param[in] webserver_config_ptr pointer with new configuration.
//  */
// void atl_config_webserver_set(atl_webserver_config_t *webserver_config_ptr) {

//     /* Take semaphore */
//     if (!xSemaphoreTake(atl_config_mutex, pdMS_TO_TICKS(atl_config_mutex_timeout))) {
//         ESP_LOGW(TAG, "Timeout taking mutex");
//     }

//     /* Set Webserver configuration */
//     atl_config.webserver = *webserver_config_ptr;

//     /* Commit configuration to NVS */
//     if (atl_config_commit_nvs() != ESP_OK) {
//         ESP_LOGE(TAG, "Fail to commit configuration to NVS!");
//         atl_led_blink_error();
//     }

//     /* Give semaphore */
//     if (!xSemaphoreGive(atl_config_mutex)) {
//         ESP_LOGW(TAG, "Fail giving mutex");
//     }
// }