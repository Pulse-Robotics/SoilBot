# Add main.c and supporting libs
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES nvs_flash esp_wifi spiffs json led_strip esp_adc esp_hw_support)

# Add support /dirs to flash mem
spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
