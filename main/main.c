#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "esp_sleep.h"
#include "led_strip.h"

static const char *TAG = "SoilBot";
#define LED_PIN 2  // Onboard NeoPixel GPIO

// Global params loaded from JSON
char sensor_name[32] = "Default";
char sensor_msg[64] = "No Msg";
int sleep_seconds = 30;
int r = 0, g = 255, b = 0; // Default Green
int pulse_ms = 500;        // Default 0.5s

// Neopixel LED pulse
void neopixel_pulse(int red, int green, int blue, int duration) {
    led_strip_handle_t led_strip;
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN,
        .max_leds = 1,
    };
    led_strip_rmt_config_t rmt_config = { .resolution_hz = 10 * 1000 * 1000 };

    if (led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip) == ESP_OK) {
        led_strip_set_pixel(led_strip, 0, red, green, blue);
        led_strip_refresh(led_strip);
        
        // Pass the duration from JSON here
        vTaskDelay(pdMS_TO_TICKS(duration));

        led_strip_clear(led_strip);
        led_strip_refresh(led_strip);
        led_strip_del(led_strip);
    }
}

// Configuration loader
void load_config() {
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    esp_vfs_spiffs_register(&conf);

    FILE* f = fopen("/spiffs/config.json", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open config.json");
        return;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *data = malloc(fsize + 1);
    fread(data, 1, fsize, f);
    fclose(f);
    data[fsize] = 0;

    cJSON *json = cJSON_Parse(data);
    if (json) {
        // First, grab the "settings" wrapper object
        cJSON *settings = cJSON_GetObjectItem(json, "settings");
        
        if (settings) {
            // Now pull items from the settings object instead of the root
            cJSON *name = cJSON_GetObjectItem(settings, "sensor_name");
            cJSON *slp  = cJSON_GetObjectItem(settings, "sleep_seconds");
            cJSON *msg  = cJSON_GetObjectItem(settings, "msg");
            cJSON *red  = cJSON_GetObjectItem(settings, "led_r");
            cJSON *grn  = cJSON_GetObjectItem(settings, "led_g");
            cJSON *blu  = cJSON_GetObjectItem(settings, "led_b");
            cJSON *ont  = cJSON_GetObjectItem(settings, "led_on_ms");
            
            if (name && cJSON_IsString(name)) strncpy(sensor_name, name->valuestring, sizeof(sensor_name));
            if (slp && cJSON_IsNumber(slp))   sleep_seconds = slp->valueint;
            if (msg && cJSON_IsString(msg))   strncpy(sensor_msg, msg->valuestring, sizeof(sensor_msg));
            if (red && cJSON_IsNumber(red))   r = red->valueint;
            if (grn && cJSON_IsNumber(grn))   g = grn->valueint;
            if (blu && cJSON_IsNumber(blu))   b = blu->valueint;
            if (ont && cJSON_IsNumber(ont))   pulse_ms = ont->valueint;
        }
        
        cJSON_Delete(json);
    }
    free(data);
    esp_vfs_spiffs_unregister("storage");
}




void app_main(void) {
    // Task Delay - FOR SERIAL DEBUG
    vTaskDelay(pdMS_TO_TICKS(2000)); 
    // Init Storage and Config
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        nvs_flash_erase();
        nvs_flash_init();
    }
    load_config();


    // LED heartbeat
    neopixel_pulse(r, g, b, pulse_ms);

    // Status Report
    printf("\n--- WAKEUP REPORT ---\n");
    printf("Name: %s\n", sensor_name);
    printf("Msg:  %s\n", sensor_msg);
    printf("Sleep: %d seconds\n", sleep_seconds);
    printf("---------------------\n\n");

    // Sleep mode
    esp_sleep_enable_timer_wakeup(sleep_seconds * 1000000ULL);
    esp_deep_sleep_start();
}
