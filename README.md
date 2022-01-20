# st7789
一个便于移植的 st7789 显示驱动，该驱动只有一个 `st7789.c` 源文件和一个 `st7789.h` 头文件。

本 st7789 驱动组件目前只支持 4 线 SPI 接口。

st7789 需要操纵一些硬件设备，如 SPI 和 GPIO，为了确保移植的便捷性，一些硬件相关的函数抽象出来由用户实现：

```C
/* Implemented by user */
extern void st7789_spi_init(void);
extern void st7789_spi_write(uint8_t *buffer, uint16_t length);
extern void st7789_pin_init(void);
extern void st7789_pin_put(st7789_pin_t pin, uint8_t value);
extern void st7789_delay_ms(uint32_t ms);
```

st7789_pin_init 函数用于初始化相应的引脚，设置引脚复用功能等，至少需要包括 SPI-SCK，SPI-MOSI 的初始化，以及 Reset，D/C 引脚的 GPIO 功能初始化。

st7789_pin_put 函数用于设置指定 GPIO 的引脚状态，0 代表低电平，1 代表高电平。目前支持以下几种引脚：

- ST7789_PIN_DC：Data / Command 选择引脚
- ST7789_PIN_CS：SPI 片选引脚
- ST7789_PIN_RST：复位引脚
- ST7789_PIN_BL：背光引脚

当然，在 MCU 与 st7789 的实际连接中，上述引脚可能不会全部拥有。例如 MCU 端可能不存在片选引脚和背光引脚，在硬件设计时将 st7789 的片选引脚直接接地，则该 SPI 设备一直被选中，还可以将 st7789 的背光引脚直接接电源，则默认开启背光，不需要程序控制。
当存在没有的引脚时，在 st7789_pin_put 函数里忽略相关引脚的调用即可。

st7789_spi_init 函数用于初始化相应的 SPI 硬件设备，包括配置波特率，数据大小，时钟极性，时钟相位，MSB 等。

st7789_spi_write 函数用于向初始化过的 SPI 硬件写入数据。

st7789_delay_ms 函数用于以毫秒为单位进行延迟。

examples 目录下有基于 rp2040 平台的例程，可供参考。