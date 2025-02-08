#include <Arduino.h>
#include "mandel-arch.h"
#ifndef HELTEC
#include "esp_system.h"
#endif
int main(void);

void esp32_showstat(void)
{
#if 0
    printf("Chip model: %s, %dMHz, %d cores\n", ESP.getChipModel(), ESP.getCpuFreqMHz(), ESP.getChipCores());
    printf("Free heap: %d/%d %d max block\n", ESP.getFreeHeap(), ESP.getHeapSize(),ESP.getMaxAllocHeap());
    printf("Free PSRAM: %d/%d %d max block\n",ESP.getFreePsram(), ESP.getPsramSize(),ESP.getMaxAllocPsram());
#else    
#ifndef ESP8266
    Serial.print("Model: ");
    Serial.print(ESP.getChipModel());
    Serial.print(", Freq: ");
    Serial.print(ESP.getCpuFreqMHz());
    Serial.print("MHz, Cores: ");
    Serial.print(ESP.getChipCores());
    Serial.print(", Heap: ");
    Serial.print(ESP.getFreeHeap());
    Serial.print("/");
    Serial.print(ESP.getHeapSize());
    Serial.print(", PSRam: ");
    Serial.print(ESP.getFreePsram());
    Serial.print("/");
    Serial.println(ESP.getPsramSize());
#else
    printf("Chip ID: %d, Heap: %d\n", ESP.getChipId(), ESP.getFreeHeap());
#endif    
#endif
}

#ifdef ARDUINO_INKPLATECOLOR
#include "Inkplate.h"
Inkplate display;

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    display.drawPixel(x, y, c);
    if (display.readTouchpad(PAD1))
    {
        log_msg("%s: touch 1 -> clear display\n", __FUNCTION__);
        display.clearDisplay();
        display.display();
        return 1;
    }
    return 0;
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    display.display();
    esp32_showstat();
}

void setup(void)
{
    Serial.begin(115200);
    disableCore0WDT();
    disableCore1WDT();
    display.begin();
    esp32_showstat();
    img_w = display.width();
    img_h = display.height();
    iter = 512;
    main();
}
#elif defined(ULCD)
#include <Wire.h>
#include <OneBitDisplay.h>
extern TwoWire *pWire;
#define SDA_PIN 41
#define SCL_PIN 40
ONE_BIT_DISPLAY obd;

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    if (c & 1)
        obd.drawPixel(x, y, c);
    return 0;
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    obd.display();
    esp32_showstat();
    delay(3*1000);
    obd.fillScreen(0);
    obd.display();
}

void setup(void)
{
    Serial.begin(115200);
    disableCore0WDT();
    disableCore1WDT();
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
    iter = 241;
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
    disableCore0WDT();
    disableCore1WDT();
    esp32_showstat();

    iter = 500;
    img_w = EPD_7IN5_V2_WIDTH;
    img_h = EPD_7IN5_V2_HEIGHT;

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
    //Paint_SetScale(4);
    printf("SelectImage:BlackImage\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    main();
}

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    int col;
    col = (c & 1) ? WHITE : BLACK;
    Paint_DrawPoint(x, y, col, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    return 0;
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    EPD_7IN5_V2_Display(BlackImage);
    esp32_showstat();
    // while(1) sleep(1);
}

#elif defined(HELTEC)
#include "heltec.h"

void setup(void)
{
    Heltec.begin(true /*DisplayEnable Enable*/, true /*Serial Enable*/);
    Heltec.display->setContrast(255);
    img_w = DISPLAY_WIDTH / 2;
    img_h = DISPLAY_HEIGHT;
    main();
}

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    OLEDDISPLAY_COLOR col;
    col = (c & 1) ? WHITE : BLACK;
    Heltec.display->setColor(col);
    Heltec.display->drawHorizontalLine(x, y, (DISPLAY_WIDTH/2) - x);
    Heltec.display->drawLine((DISPLAY_WIDTH - 1) - x, y, (DISPLAY_WIDTH/2), y);
    Heltec.display->display();
    return 0;
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    esp32_showstat();
    delay(5 * 1000);
}

#elif defined(HELTECESP32) || defined(HELTECESP32_RCV)
#include <Wire.h>
#include <OneBitDisplay.h>
extern TwoWire *pWire;
#define SDA_PIN 4
#define SCL_PIN 15
#define RST_PIN 16
ONE_BIT_DISPLAY obd;
#include <heltec.h>
//#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6
#define BAND     433E6 //you can set band here directly,e.g. 868E6,915E6

void send_lora(char *m)
{
    // send packet
    LoRa.beginPacket();
    /*
     * LoRa.setTxPower(txPower,RFOUT_pin);
     * txPower -- 0 ~ 20
     * RFOUT_pin could be RF_PACONFIG_PASELECT_PABOOST or RF_PACONFIG_PASELECT_RFO
     *   - RF_PACONFIG_PASELECT_PABOOST -- LoRa single output via PABOOST, maximum output 20dBm
     *   - RF_PACONFIG_PASELECT_RFO     -- LoRa single output via RFO_HF / RFO_LF, maximum output 14dBm
     */
    LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
    LoRa.print(m);
    // LoRa.print(counter);
    LoRa.endPacket();
}

#ifdef HELTECESP32_RCV
String rssi = "RSSI --";
String packSize = "--";
String packet;
void cbk(int packetSize) {
        packet ="";
        char m[16 * 64], *t;
        int c = 0, x, y, d, i;

        packSize = String(packetSize,DEC);
        for (i = 0; i < packetSize; i++) { packet += m[c++] = (char) LoRa.read(); }
        rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
        m[c] = '\0';
        Serial.println(rssi + ", size = " + packSize + ", " + String(m));
        t = m;
        while (*t != '\0') {
                x = strtol(t, &t, 10);
                //Serial.print("x: "); Serial.print(x);
                t++;
                y = strtol(t, &t, 10);
                //Serial.print(",y: "); Serial.print(y);
                t++;
                d = strtol(t, &t, 10);
                //Serial.print(",d: "); Serial.println(d);
                t++;
                if (d != 0) {
                    obd.drawPixel(x, y, d);
                    obd.display();
                }
        }
        //LoRaData();
}
#endif

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    static char buf[16 * 128];
    static int idx;
    static int count = 0;

    if (c & 1)
        obd.drawPixel(x, y, c);

    snprintf(&buf[idx], 64, "%d/%d/%d/", x, y, (c & 1));
    idx += strlen(&buf[idx]);
    count++;
    if ((count % 16) == 0)
    {
        long s = millis();
        obd.display();
        send_lora(buf);
        count = idx = 0;
        //log_msg("%s: sent LoRa msg in %ld\n", __FUNCTION__, millis() - s);
    }

    return 0;
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    for (size_t i = 0; i < frecs.size(); i++)
    {
        auto it = &frecs[i];
        m->zoom(it->xl, it->yl, it->xh, it->yh);
        obd.display();
        esp32_showstat();
        delay(3*1000);
        obd.fillScreen(0);
        obd.display();
    }
    while(1)
        sleep(1);
}

void setup(void)
{
    Serial.begin(115200);
    disableCore0WDT();
    disableCore1WDT();

    Heltec.begin(false /*Heltec.displayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, false /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);

    esp32_showstat();

    obd.setI2CPins(SDA_PIN, SCL_PIN, RST_PIN);
    obd.I2Cbegin(OLED_128x64);
    obd.allocBuffer();
    //obd.setContrast(50);
    obd.fillScreen(0);
    obd.setFont(FONT_8x8);
    obd.println("Starting...");
    obd.display();
    img_w = obd.width();
    img_h = obd.height();
    iter = 241;
#ifdef HELTECESP32_RCV    
    LoRa.receive();
    while (1) {
        int packetSize = LoRa.parsePacket();
        if (packetSize) { cbk(packetSize);  }
            delay(10);
    }
#else
    main();
#endif    
}

#elif defined(LEDMATRIX)

#define MATRIX_WIDTH 64
#define MATRIX_HEIGHT 64
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
MatrixPanel_I2S_DMA dma_display;

#define R1_PIN 2
#define G1_PIN 15
#define B1_PIN 4
#define R2_PIN 32
#define G2_PIN 27
#define B2_PIN 23
#define A_PIN 5
#define B_PIN 18
#define C_PIN 19
#define D_PIN 21
#define E_PIN 33 // Change to a valid pin if using a 64 pixel row panel.
#define LAT_PIN 26
#define OE_PIN 25
#define CLK_PIN 22

class rgb_24
{
public:
    uint8_t r;
    uint8_t g;
    uint8_t b;

    rgb_24() { r = g = b = 0; }
    rgb_24(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
    ~rgb_24() = default;
};
static rgb_24 ct[256];

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    dma_display.drawPixelRGB888(x, y, ct[c].r, ct[c].g, ct[c].b);
    return 0;
}

void setup(void)
{
    Serial.begin(115200);
    disableCore0WDT();
    disableCore1WDT();
    delay(500);
    iter = 500;
    img_w = MATRIX_WIDTH;
    img_h = MATRIX_HEIGHT;
    for (int i = 0; i < 80; i++)
    {
        ct[1 + i] = rgb_24(15 + i * 3, 0, 0);
        ct[81 + i] = rgb_24(0, 15 + i * 3, 0);
        ct[161 + i] = rgb_24(0, 0, 15 + i * 3);
    }
    dma_display.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN);
    dma_display.flipDMABuffer();
    main();
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    for (size_t i = 0; i < frecs.size(); i++)
    {
        auto it = &frecs[i];
        m->zoom(it->xl, it->yl, it->xh, it->yh);
        dma_display.flush();
        dma_display.flipDMABuffer();
        esp32_showstat();
        delay(5 * 1000);
    }
}
#elif TTGO
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>
#define BLACK 0x0000

TFT_eSPI tft = TFT_eSPI(); // Invoke library, pins defined in User_Setup.h

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    tft.drawPixel(x, y, c);
    return 0;
}

void setup(void)
{
    Serial.begin(115200);
    disableCore0WDT();
    disableCore1WDT();
    delay(1000);
    esp32_showstat();
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(BLACK);
    iter = 241;
    img_h = TFT_WIDTH;
    img_w = TFT_HEIGHT;
    Serial.println("bla");
    Serial.printf("%s: entering main...\n", __FUNCTION__);
    delay(1000);
    main();
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    esp32_showstat();
    delay(5 * 1000);
}
#elif defined(ESP32NOKIA5110)
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
Adafruit_PCD8544 lcd = Adafruit_PCD8544(16, 17, 5, 18, 19);

int esp32_setpx(CANVAS_TYPE *cv, int x, int y, int c)
{
    if (c & 1)
    {
        lcd.drawPixel(x, y, BLACK);
        lcd.display();
    }
    return 0;
}

void setup(void)
{
    Serial.begin(115200);
    disableCore0WDT();
    disableCore1WDT();
    esp32_showstat();
    if (!lcd.begin())
        log_msg("%s: lcd init failed\n", __FUNCTION__);
    lcd.setContrast(50);
    pinMode(4, OUTPUT);
    digitalWrite(4, HIGH);
    lcd.clearDisplay();
    iter = 500;
    img_w = lcd.width();
    img_h = lcd.height();
    main();
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    esp32_showstat();
    delay(5 * 1000);
    lcd.clearDisplay();
}
#elif defined(ESP32CONSOLE)
void setup(void)
{
    Serial.begin(115200);
    disableCore0WDT();
    disableCore1WDT();
    img_w = 120;
    img_h = 48;
    main();
}

void esp32_zoomui(mandel<MTYPE> *m)
{
    esp32_showstat();
    delay(3 * 1000);
}

#else
#error "no esp32 arch defined"
#endif
void loop(void)
{
    Serial.println("loop...");
    delay(10 * 1000);
}
