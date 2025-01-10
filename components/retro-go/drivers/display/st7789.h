#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "rg_system.h"
#include "rg_display.h"
#include "driver/spi_master.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

// Macros for sending commands and data via SPI
#define ST7789_CMD(cmd, data...)                        \
    do {                                                \
        const uint8_t c = cmd, x[] = {data};            \
        spi_queue_transaction(&c, 1, 0);                \
        if (sizeof(x))                                  \
            spi_queue_transaction(&x, sizeof(x), 1);    \
    } while (0)

static void st7789_set_backlight(float percent)
{
#if defined(RG_GPIO_LCD_BCKL)
    float level = RG_MIN(RG_MAX(percent / 100.f, 0), 1.f);
    ledc_set_fade_time_and_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0,
                                 (uint32_t)(0x1FFF * level), 50, 0);
#else
    // Optionally handle other ways to set background light
#endif
}

static void st7789_init(void)
{
    gpio_set_direction(RG_GPIO_LCD_DC, GPIO_MODE_OUTPUT);
    gpio_set_level(RG_GPIO_LCD_DC, 1);

#if defined(RG_GPIO_LCD_RST)
    gpio_set_direction(RG_GPIO_LCD_RST, GPIO_MODE_OUTPUT);
    gpio_set_level(RG_GPIO_LCD_RST, 0);
    rg_usleep(100 * 1000);
    gpio_set_level(RG_GPIO_LCD_RST, 1);
    rg_usleep(10 * 1000);
#endif

    // Typical ST7789 initialization sequence can vary by panel:
    // Software reset
    ST7789_CMD(0x01);
    rg_usleep(120000);

    // Sleep Out
    ST7789_CMD(0x11);
    rg_usleep(120000);

    // Interface Pixel Format (16bit)
    ST7789_CMD(0x3A, 0x55);

    // Memory Access Control
    // Choose rotation and BGR or RGB
    // 0xC0 example = row/column exchange + BGR
    ST7789_CMD(0x36, 0xC0);

    // Your custom sequence here, e.g. Frame Rate, Porch settings, etc.

#ifdef RG_SCREEN_INIT
    RG_SCREEN_INIT(); // Additional user-defined commands
#endif

    // Display ON
    ST7789_CMD(0x29);
    rg_usleep(10000);

    // Clear screen or fill background
    rg_display_clear(C_BLACK);

    // Backlight on
    st7789_set_backlight(100.0f);
}

static void st7789_deinit(void)
{
    // Optionally perform panel-specific shutdown
    st7789_set_backlight(0);
    // For example:
    // ST7789_CMD(0x28); // display off
    // ST7789_CMD(0x10); // enter sleep mode
}

// Main driver struct
const rg_display_driver_t rg_display_driver_st7789 = {
    .name = "st7789",
    // If needed, assign function pointers for custom ops
    // e.g., .set_region, .frame_send, etc.
};
// this wrriten by ai o1 model and not tested yet.