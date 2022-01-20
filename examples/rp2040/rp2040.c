#include "st7789.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#include <stdio.h>

#define GAMEKIT_RP2040_LCD_RESET 0
#define GAMEKIT_RP2040_LCD_DC    1
#define GAMEKIT_RP2040_LCD_SCK   2
#define GAMEKIT_RP2040_LCD_SDA   3

void st7789_spi_init(void)
{
    spi_init(spi0, 4 * 1000 * 1000);
    spi_set_format(spi0, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
}

void st7789_spi_write(uint8_t *buffer, uint16_t length)
{
    spi_write_blocking(spi0, buffer, length);
}

void st7789_pin_init(void)
{
    gpio_init(GAMEKIT_RP2040_LCD_DC);
    gpio_set_dir(GAMEKIT_RP2040_LCD_DC, true);

    gpio_init(GAMEKIT_RP2040_LCD_RESET);
    gpio_set_dir(GAMEKIT_RP2040_LCD_RESET, true);

    gpio_set_function(GAMEKIT_RP2040_LCD_SCK, GPIO_FUNC_SPI);
    gpio_set_function(GAMEKIT_RP2040_LCD_SDA, GPIO_FUNC_SPI);
}

void st7789_pin_put(st7789_pin_t pin, uint8_t value)
{
    switch (pin) {
    case ST7789_PIN_DC: 
        gpio_put(GAMEKIT_RP2040_LCD_DC, value);
        break;
    case ST7789_PIN_RST: 
        gpio_put(GAMEKIT_RP2040_LCD_RESET, value);
        break;
    default:
        break;
    }
}

void st7789_delay_ms(uint32_t ms)
{
    sleep_ms(ms);
}

/* Color definitions */
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

uint16_t color_array[] = {
    BLACK, BLUE, RED, GREEN, CYAN, MAGENTA, YELLOW, WHITE,
};
uint16_t color_array_len = (sizeof(color_array) / sizeof(color_array[0]));

int main(void)
{
    st7789_init(240, 240);

    uint16_t i = 0;
    while (1) {
        st7789_fill(color_array[i]);
        i = (i+1) % color_array_len;
        sleep_ms(1000);
    }
}
