#include <sdkconfig.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#include <driver/gpio.h>
#include <rom/gpio.h>

#include "include/sdcard.h"

static const char *TAG = "sdcard";

sdmmc_card_t* sdcardCard = NULL;

sdmmc_card_t* getCard() {
    return sdcardCard;
}

esp_err_t mount_sd(int pin_cmd, int pin_clk, int pin_d0, int pin_pwr, const char* mountpoint, bool format_if_failed, int max_files) {
    esp_err_t res;
    if (pin_pwr >= 0) {
        res = gpio_set_direction(pin_pwr, GPIO_MODE_OUTPUT);
        if (res != ESP_OK) return res;
        res = gpio_set_level(pin_pwr, true);
        if (res != ESP_OK) return res;
    }
    
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

    host.flags = SDMMC_HOST_FLAG_1BIT;
    slot_config.width = 1;
    
    gpio_pad_select_gpio(pin_cmd);
    gpio_set_direction(pin_cmd, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(pin_cmd, GPIO_PULLUP_ONLY);
    gpio_set_level(pin_cmd, 1);
    
    gpio_pad_select_gpio(pin_clk);
    gpio_set_direction(pin_clk, GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(pin_clk, GPIO_PULLUP_ONLY);
    gpio_set_level(pin_clk, 1);
    
    gpio_pad_select_gpio(pin_d0);
    gpio_set_direction(pin_d0,  GPIO_MODE_INPUT_OUTPUT_OD);
    gpio_set_pull_mode(pin_d0,  GPIO_PULLUP_ONLY);
    gpio_set_level(pin_d0,  1);
    
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = format_if_failed,
        .max_files              = max_files,
        .allocation_unit_size   = 0
    };
    
    res = esp_vfs_fat_sdmmc_mount(mountpoint, &host, &slot_config, &mount_config, &sdcardCard);
    
    if (res != ESP_OK) {
        if (res == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount the SD card filesystem.");
        } else if (res == ESP_ERR_NO_MEM) {
            ESP_LOGE(TAG, "Failed to initialize the SD card: not enough memory).");
        } else if (res == ESP_ERR_INVALID_RESPONSE) {
            ESP_LOGE(TAG, "Failed to initialize the SD card: invalid response).");
        } else if (res == ESP_ERR_INVALID_STATE) {
            ESP_LOGE(TAG, "Failed to initialize the SD card: invalid state).");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the SD card (%s). ", esp_err_to_name(res));
        }
        return res;
    }
    return res;
}
