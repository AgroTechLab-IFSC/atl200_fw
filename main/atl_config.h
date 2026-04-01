/**
 * @file atl_config.h
 * @brief Header file for configuration management.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @copyright CC-BY-SA-4.0 license
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <esp_err.h>
#include "atl_led.h"
// #include "atl_wifi.h"
// #include "atl_webserver.h"

/**
 * @brief Configuration structure typedef.
 */
typedef struct {
    atl_led_config_t                led;        //!< LED configuration.
    // atl_wifi_config_t               wifi;       //!< WiFi configuration.
    // atl_webserver_config_t          webserver;  //!< Webserver configuration.
} atl_config_t;

/**
 * @brief Initialize configuration from NVS.
 * @details If not possible load configuration file, create a new with default values.
 * @return esp_err_t - If ERR_OK success, otherwise fail.
 */
esp_err_t atl_config_init(void);

/**
 * @brief Get all configuration.
 * @param[out] config_ptr pointer to write configuration.
 */
void atl_config_get(atl_config_t *config_ptr);

/**
 * @brief Set all configuration.
 * @param[in] config_ptr pointer with new configuration.
 */
void atl_config_set(atl_config_t *config_ptr);

/**
 * @brief Get LED configuration.
 * @param[out] led_config_ptr pointer to write configuration.
 */
void atl_config_led_get(atl_led_config_t *led_config_ptr);

/**
 * @brief Set LED configuration.
 * @param[in] led_config_ptr pointer with new configuration.
 */
void atl_config_led_set(atl_led_config_t *led_config_ptr);

// /**
//  * @brief Get WiFi configuration.
//  * @param[out] wifi_config_ptr pointer to write configuration.
//  */
// void atl_config_wifi_get(atl_wifi_config_t *wifi_config_ptr);

// /**
//  * @brief Set WiFi configuration.
//  * @param[in] wifi_config_ptr pointer with new configuration.
//  */
// void atl_config_wifi_set(atl_wifi_config_t *wifi_config_ptr);

// /**
//  * @brief Get Webserver configuration.
//  * @param[out] webserver_config_ptr pointer to write configuration.
//  */
// void atl_config_webserver_get(atl_webserver_config_t *webserver_config_ptr);

// /**
//  * @brief Set Webserver configuration.
//  * @param[in] webserver_config_ptr pointer with new configuration.
//  */
// void atl_config_webserver_set(atl_webserver_config_t *webserver_config_ptr);

#ifdef __cplusplus
}
#endif