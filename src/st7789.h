#ifndef ST7789
#define ST7789

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum st7789_pin {
    ST7789_PIN_DC = 0,
    ST7789_PIN_CS,
    ST7789_PIN_RST,
    ST7789_PIN_BL,
} st7789_pin_t;

extern void st7789_init(uint16_t width, uint16_t height);
extern void st7789_fill(uint16_t color);
extern void st7789_fill_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
extern void st7789_vline(uint16_t x, uint16_t y, uint16_t length, uint16_t color);
extern void st7789_hline(uint16_t x, uint16_t y, uint16_t length, uint16_t color);

/* Implemented by user */
extern void st7789_spi_init(void);
extern void st7789_spi_write(uint8_t *buffer, uint16_t length);
extern void st7789_pin_init(void);
extern void st7789_pin_put(st7789_pin_t pin, uint8_t value);
extern void st7789_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* RP2040_MOUSE_ST7789_ */
