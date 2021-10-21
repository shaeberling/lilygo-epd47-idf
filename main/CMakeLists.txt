idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS ""
		       REQUIRES "main"
                    "nvs_flash"
                    "esp_adc_cal"
                    "lilygo-epd47")
