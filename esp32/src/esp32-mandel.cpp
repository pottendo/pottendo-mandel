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
    while(1) sleep(1);
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
    while(1) sleep(1);

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

#else
#error "no esp32 arch defined"
#endif
void loop(void)
{
    sleep(10);
}
