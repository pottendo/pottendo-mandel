#include "mandel-arch.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include <stdbool.h>
extern "C" {
#include "pico-ili9341xpt2046/ili9341.h"
#include "pico-ili9341xpt2046/ili9341_framebuffer.h"
#include "pico-ili9341xpt2046/ili9341_draw.h"
#include "pico-ili9341xpt2046/xpt2046.h"
#include "pico-ili9341xpt2046/ugui.h"
}

ili9341_config_t ili9341_config = {
	.port = spi1,
	.pin_miso = 11,
	.pin_cs = 13,
	.pin_sck = 10,
	.pin_mosi = 12,
	.pin_reset = 15,
	.pin_dc = 14,
    .pin_led = 8
};

void pixel_set(UG_S16 x, UG_S16 y, UG_COLOR rgb)
{
    uint16_t B = ((rgb >> 16) & 0x0000FF) << 1;
    uint16_t G = ((rgb >> 8) & 0x0000FF) << 1;
    uint16_t R = (rgb & 0x0000FF) << 1;
    UG_COLOR RGB16 = RGBConv(R,G,B);
    draw_pixel(x,y,RGB16);
}
// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

// Perform initialisation
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on)
{
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

static UG_GUI gui;
CANVAS_TYPE *pico_init(void) 
{
    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);

    ili9341_init();
    //ts_spi_setup();

    fill_screen(0x0000);
    UG_Init(&gui, pixel_set, 240, 240);

    return NULL;
}

int pico_setpx(CANVAS_TYPE *canvas, int x, int y, int c)
{
    UG_DrawPixel(x, y, c);
    return 0;
}
