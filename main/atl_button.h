/**
 * @file atl_button.h
 * @brief Header file for button management.
 * @author Robson Costa (robson.costa@ifsc.edu.br)
 * @copyright CC-BY-SA-4.0 license
 */
#pragma once

#include <esp_err.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize button task.
 * @return esp_err_t
*/
esp_err_t atl_button_init(void);

#ifdef __cplusplus
}
#endif