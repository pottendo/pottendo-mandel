#include <Arduino.h>
#include "mandel-arch.h"
int main(void);

void esp32_showstat(void)
{
#if 0    
    printf("Chip model: %s, %dMHz, %d cores\n", ESP.getChipModel(), ESP.getCpuFreqMHz(), ESP.getChipCores());
    printf("Free heap: %d/%d %d max block\n", ESP.getFreeHeap(), ESP.getHeapSize(),ESP.getMaxAllocHeap());
    printf("Free PSRAM: %d/%d %d max block\n",ESP.getFreePsram(), ESP.getPsramSize(),ESP.getMaxAllocPsram());
#else    
    Serial.print("Model: ");
    Serial.print(ESP.getChipModel());
    Serial.print(", Freq: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.print(", Cores: ");
    Serial.print(ESP.getChipCores());
    Serial.print(", Heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.print("/");
    Serial.print(ESP.getHeapSize());
    Serial.print(", PSRam: ");
    Serial.print(ESP.getFreePsram());
    Serial.print("/");
    Serial.println(ESP.getPsramSize());
#endif
}

#ifdef ARDUINO_INKPLATECOLOR
#include "Inkplate.h"
Inkplate display;

void esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    display.drawPixel(x, y, c);
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    display.display();
    //while(1) sleep(1);
}

void setup(void)
{
    Serial.begin(115200);
    display.begin();
    esp32_showstat();
    img_w = display.width();
    img_h = display.height();
    iter = 64;
    main();
}
#elif defined(ULCD)
#include <Wire.h>
#include <OneBitDisplay.h>
extern TwoWire *pWire;
#define SDA_PIN 41
#define SCL_PIN 40
ONE_BIT_DISPLAY obd;

void esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    obd.drawPixel(x, y, c);
    obd.display();
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    obd.display();
    //while(1) sleep(1);
}

void setup(void)
{
    Serial.begin(115200);
    esp32_showstat();

    obd.setI2CPins(SDA_PIN, SCL_PIN);
    obd.I2Cbegin(OLED_72x40);
    obd.allocBuffer();
    //obd.setContrast(50);
    obd.fillScreen(0);
    obd.setFont(FONT_8x8);
    obd.println("Starting...");
    obd.display();
    img_w = obd.width();
    img_h = obd.height();
    iter = 64;
    /*
        for (int x = 0; x < img_w; x++)
            for (int y = 0; y < img_h; y++)
            {
                obd.drawPixel(x, y, 1);
                obd.display();
                delay(100);
            }
            */
    main();
}
#elif defined(WAVESHARE)
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include <stdlib.h>

static UBYTE *BlackImage = nullptr;

void setup(void)
{
    Serial.begin(115200);
    esp32_showstat();

    iter = 64;
    img_w = EPD_7IN5_V2_WIDTH / 2;
    img_h = EPD_7IN5_V2_HEIGHT / 2;

    DEV_Module_Init();

    printf("e-Paper Init and Clear %dx%d...\r\n", EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT);
    EPD_7IN5_V2_Init();
    EPD_7IN5_V2_Clear();
    DEV_Delay_ms(500);

    // Create a new image cache
    /* you have to edit the startup_stm32fxxx.s file and set a big enough heap size */
    UWORD Imagesize = ((EPD_7IN5_V2_WIDTH % 8 == 0) ? (EPD_7IN5_V2_WIDTH / 8) : (EPD_7IN5_V2_WIDTH / 8 + 1)) * EPD_7IN5_V2_HEIGHT;
    if ((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL)
    {
        printf("Failed to apply for black memory...\r\n");
        while (1)
            ;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_7IN5_V2_WIDTH, EPD_7IN5_V2_HEIGHT, 0, BLACK);

    EPD_7IN5_V2_Init_Fast();
    Paint_SetScale(4);
    printf("SelectImage:BlackImage\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_DrawLine(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawLine(70, 70, 20, 120, BLACK, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    Paint_DrawRectangle(20, 70, 70, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(80, 70, 130, 120, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawCircle(45, 95, 20, BLACK, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    Paint_DrawCircle(105, 95, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    Paint_DrawLine(85, 95, 125, 95, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    Paint_DrawLine(105, 75, 105, 115, BLACK, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);

    printf("EPD_Display\r\n");
    EPD_7IN5_V2_Display(BlackImage);
    main();
}

void esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    int col;
    col = (c == 0) ? WHITE : BLACK;
    Paint_DrawPoint(x, y, c, DOT_PIXEL_2X2, DOT_STYLE_DFT);
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    EPD_7IN5_V2_Display(BlackImage);
    // while(1) sleep(1);
}

#else
#error "no esp32 arch defined"
#endif
void loop(void)
{
    sleep(10);
}
