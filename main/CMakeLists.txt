idf_component_register(SRCS
                        "atl_config.c"
                        "atl_led.c"
                        "atl_main.c"
                        "atl_storage.c"
                       PRIV_REQUIRES
                        "nvs_flash"
                       INCLUDE_DIRS ".")
