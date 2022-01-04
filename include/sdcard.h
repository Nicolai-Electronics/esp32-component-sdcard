#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <esp_err.h>

esp_err_t mount_sd(int pin_cmd, int pin_clk, int pin_d0, int pin_pwr, const char* mountpoint, bool format_if_failed, int max_files);
