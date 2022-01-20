#include "st7789.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/*  ST7789 Commands */
#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN    0x10
#define ST7789_SLPOUT   0x11
#define ST7789_PTLON    0x12
#define ST7789_NORON    0x13

#define ST7789_INVOFF   0x20
#define ST7789_INVON    0x21
#define ST7789_DISPOFF  0x28
#define ST7789_DISPON   0x29
#define ST7789_CASET    0x2A
#define ST7789_RASET    0x2B
#define ST7789_RAMWR    0x2C
#define ST7789_RAMRD    0x2E

#define ST7789_PTLAR    0x30
#define ST7789_VSCRDEF  0x33
#define ST7789_COLMOD   0x3A
#define ST7789_MADCTL   0x36
#define ST7789_VSCSAD   0x37

#define ST7789_MADCTL_MY    0x80
#define ST7789_MADCTL_MX    0x40
#define ST7789_MADCTL_MV    0x20
#define ST7789_MADCTL_ML    0x10
#define ST7789_MADCTL_BGR   0x08
#define ST7789_MADCTL_MH    0x04
#define ST7789_MADCTL_RGB   0x00

#define ST7789_RDID1    0xDA
#define ST7789_RDID2    0xDB
#define ST7789_RDID3    0xDC
#define ST7789_RDID4    0xDD

#define COLOR_MODE_65K      0x50
#define COLOR_MODE_262K     0x60
#define COLOR_MODE_12BIT    0x03
#define COLOR_MODE_16BIT    0x05
#define COLOR_MODE_18BIT    0x06
#define COLOR_MODE_16M      0x07

static uint16_t g_width;
static uint16_t g_height;

static inline void delay_ms(uint32_t ms)
{
    st7789_delay_ms(ms);
}

static void st7789_spi_write_byte(uint8_t byte)
{
    st7789_spi_write(&byte, 1);
}

static void st7789_write_command(uint8_t command, uint8_t *data, uint16_t size)
{
    st7789_pin_put(ST7789_PIN_CS, 0);

    st7789_pin_put(ST7789_PIN_DC, 0);
    st7789_spi_write_byte(command);

    if (data != NULL && size > 0) {
        st7789_pin_put(ST7789_PIN_DC, 1);
        st7789_spi_write(data, size);
    }

    st7789_pin_put(ST7789_PIN_CS, 1);
}

static void st7789_write_command_no_data(uint8_t command)
{
    st7789_write_command(command, NULL, 0);
}

static void st7789_write_data(uint8_t *data, uint16_t size)
{
    st7789_pin_put(ST7789_PIN_CS, 0);

    st7789_pin_put(ST7789_PIN_DC, 1);
    st7789_spi_write(data, size);

    st7789_pin_put(ST7789_PIN_CS, 1);
}

static void st7789_hard_reset(void)
{
    st7789_pin_put(ST7789_PIN_CS, 0);

    st7789_pin_put(ST7789_PIN_RST, 1);
    delay_ms(50);
    st7789_pin_put(ST7789_PIN_RST, 0);
    delay_ms(50);
    st7789_pin_put(ST7789_PIN_RST, 1);
    delay_ms(150);

    st7789_pin_put(ST7789_PIN_CS, 1);
}

static void st7789_soft_reset(void)
{
    st7789_write_command_no_data(ST7789_SWRESET);
    delay_ms(150);
}

static void st7789_sleep_mode(bool value)
{
    if (value) {
        st7789_write_command_no_data(ST7789_SLPIN);
    } else {
        st7789_write_command_no_data(ST7789_SLPOUT);
    }
}

static void st7789_set_color_mode(uint8_t mode)
{
    mode = mode & 0x77;
    st7789_write_command(ST7789_COLMOD, &mode, 1);
}

/**
 * @param rotation (int): 0-Portrait, 1-Landscape, 2-Inverted Portrait, 3-Inverted Landscape
*/
static void st7789_rotation(uint8_t rotation)
{
    assert(rotation == 0);  /* Only support 0 mode now. */

    rotation %= 4;
    uint8_t madctl = ST7789_MADCTL_RGB;
    st7789_write_command(ST7789_MADCTL, &madctl, 1);
}

static void st7789_inversion_mode(bool value)
{
    if (value) {
        st7789_write_command_no_data(ST7789_INVON);
    } else {
        st7789_write_command_no_data(ST7789_INVOFF);
    }
}

static void st7789_set_columns(uint16_t start, uint16_t end)
{
    uint8_t data[4];

    data[0] = (uint8_t)(start >> 8);
    data[1] = (uint8_t)(start & 0xff);
    data[2] = (uint8_t)(end >> 8);
    data[3] = (uint8_t)(end & 0xff);
    st7789_write_command(ST7789_CASET, data, 4);
}

static void st7789_set_rows(uint16_t start, uint16_t end)
{
    uint8_t data[4];

    data[0] = (uint8_t)(start >> 8);
    data[1] = (uint8_t)(start & 0xff);
    data[2] = (uint8_t)(end >> 8);
    data[3] = (uint8_t)(end & 0xff);
    st7789_write_command(ST7789_RASET, data, 4);
}

void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    st7789_set_columns(x0, x1);
    st7789_set_rows(y0, y1);
    st7789_write_command_no_data(ST7789_RAMWR);
}

void st7789_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    st7789_set_window(x, y, x+width-1, y+height-1);

    uint8_t data[2];
    data[0] = (uint8_t)(color >> 8);
    data[1] = (uint8_t)(color & 0xff);

    // TODO 优化
    for (uint32_t i = 0; i < (uint32_t)width * height; i++) {
        st7789_write_data(data, 2);
    }
}

void st7789_vline(uint16_t x, uint16_t y, uint16_t length, uint16_t color)
{
    st7789_fill_rect(x, y, 1, length, color);
}

void st7789_hline(uint16_t x, uint16_t y, uint16_t length, uint16_t color)
{
    st7789_fill_rect(x, y, length, 1, color);
}

/**
 * @brief Fill the entire FrameBuffer with the specified color.
 * 
 * @param  
 * @param color 565 encoded color
 */ 
void st7789_fill(uint16_t color)
{
    st7789_fill_rect(0, 0, g_width, g_height, color);
}

void st7789_init(uint16_t width, uint16_t height)
{
    /* Init pin. */
    st7789_pin_init();

    /* Init spi. */
    st7789_spi_init();

    g_width  = width;
    g_height = height;

    st7789_spi_write_byte(0xFF);

    st7789_hard_reset();
    st7789_soft_reset();
    st7789_sleep_mode(false);

    st7789_set_color_mode(COLOR_MODE_65K|COLOR_MODE_16BIT);
    delay_ms(50);
    st7789_rotation(0);
    st7789_inversion_mode(true);
    delay_ms(10);
    st7789_write_command_no_data(ST7789_NORON);
    delay_ms(10);

    st7789_pin_put(ST7789_PIN_BL, 1);

    st7789_fill(0);
    st7789_write_command_no_data(ST7789_DISPON);
    delay_ms(500);
}