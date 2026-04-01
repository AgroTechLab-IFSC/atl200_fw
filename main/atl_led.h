/**
 * @file atl_led.h
 * @brief Header file for LED management.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @copyright CC-BY-SA-4.0 license
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <stdbool.h>
#include <esp_err.h>

#define LED_STRIP_RMT_RES_HZ  (10 * 1000 * 1000)    //!< RMT counter clock frequency

/**
 * @typedef atl_led_behaviour_e
 * @brief   LED behaviour.
 */
typedef enum {
    ATL_LED_DISABLED,                   /**< LED disabled. */    
    ATL_LED_ENABLED_FAILS,              /**< LED enabled (fails). */
    ATL_LED_ENABLED_COMM_FAILS,         /**< LED enabled (communication fails). */
    ATL_LED_ENABLED_FULL,               /**< LED enabled (full). */
    ATL_LED_BEHAVIOUR_INVALID,          /**< Invalid LED behaviour. */
} atl_led_behaviour_e;

/**
 * @brief System configuration structure typedef.
 */
typedef struct {
    uint8_t red;        /**< LED red value. */
    uint8_t green;      /**< LED green value. */
    uint8_t blue;       /**< LED blue value. */
} atl_led_rgb_color_t;

/**
 * @brief LED configuration structure typedef.
 */
typedef struct {
    atl_led_behaviour_e    behaviour;    //!< LED behaviour.
} atl_led_config_t;

/**
 * @brief Get the led behaviour string object.
 * @param behaviour - LED behaviour enum value
 * @return LED behaviour string
 */
const char* atl_led_get_behaviour_str(atl_led_behaviour_e behaviour);

/**
 * @brief Get the led behaviour string object.
 * @param behaviour_str - LED behaviour string
 * @return LED behaviour enum
 */
atl_led_behaviour_e atl_led_get_behaviour(char *behaviour_str);

/**
 * @brief Initialize led task.
 * @return esp_err_t
*/
esp_err_t atl_led_init(void);

/**
 * @brief Reset led task.
 * @return esp_err_t
*/
esp_err_t atl_led_reset(void);

/**
 * @brief Toggle led.
 * @return esp_err_t
*/
esp_err_t atl_led_toggle(void);

/**
 * @brief Blink led.
 * @param [in] times blink times
 * @param [in] interval interval between blinks
 * @param [in] red red value (0..255)
 * @param [in] green green value (0..255)
 * @param [in] blue blue value (0..255)
*/
void atl_led_blink(uint8_t times, uint16_t interval, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Set led builtin color.
 * @param [in] red red value (0..255)
 * @param [in] green green value (0..255)
 * @param [in] blue blue value (0..255)
*/
void atl_led_set_color(uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Enabled/disabled led.
 * @param [in] status enabled or disabled led
*/
void atl_led_set_enabled(bool status);

/**
 * @brief Blink led error
*/
void atl_led_blink_error();

/**
 * @brief Blink led reboot
*/
void atl_led_blink_reboot();

#ifdef __cplusplus
}
#endif