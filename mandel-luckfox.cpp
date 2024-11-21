#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdint.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <linux/fb.h>
#include <sys/ioctl.h>

#include "mandel-arch.h"
extern void log_msg(const char *s, ...);

CANVAS_TYPE *tft_canvas; // must not be static?!
extern int img_w, img_h;

static int fd;

void init_luckfox(void)
{
    struct fb_fix_screeninfo fb_fix;
    struct fb_var_screeninfo fb_var;

    log_msg("init luckfox TFT\n");
    fd = open("/dev/fb0", O_RDWR);
    if (fd <= 0)
    {
        log_msg("open TFT framebuffer, errno = %d\n", errno);
        return;
    }

    ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
    ioctl(fd, FBIOGET_FSCREENINFO, &fb_fix);
    img_w = fb_var.xres;
    img_h = fb_var.yres;
    log_msg("%s: img = %dx%d\n", __FUNCTION__, img_w, img_h);
#ifdef FB_DEVICE    
    tft_canvas = (CANVAS_TYPE *)mmap(NULL, (img_w * img_h) << 1, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if (tft_canvas == MAP_FAILED)
    {
        log_msg("mmap failed - errno = %d\n", errno);
        tft_canvas = NULL;
        return;
    }
#else
    tft_canvas = new CANVAS_TYPE[img_w * img_h]();
    if (!tft_canvas)
    {
        log_msg("%s: canvas malloc() failed.\n", __FUNCTION__);
        return;
    }
#endif
    memset(tft_canvas, 0, (img_w * img_h) * sizeof(CANVAS_TYPE));
}

void luckfox_palette(int *col_pal)
{
#if 0    
    int c = 0;
    memset(col_pal, 0, 1024);
    for (int i = 0; i < 32; i++)
	col_pal[i] = i;
    col_pal[32] = 65535;
    c = 0;
    for (int i = 33; i < (33 + 64 + 1); i++, c++)
	col_pal[i] = (c<<5);
    col_pal[32 + 64 + 1] = 65535;
    c = 0;
    for (int i = 33+64 + 2; i < (33 + 64 + 32 + 2); i++, c++)
	col_pal[i] = (c<<11);
        
    return;
#endif
#ifdef COL16BIT
    uint16_t i, t;
    for (i = 0; i < 16; i++)
    {
        t = i << 1;
        col_pal[i] = t;
        col_pal[16 + i] = (31 - col_pal[i]) | (t << 6);
        col_pal[32 + i] = ((31 << 6) - (t << 6)) | (t << 11);
        col_pal[48 + i] = ((31 << 11) - (t << 11));
    }
    for (i = 1; i < 15; i++)
    {
        memcpy(col_pal + i * 64, col_pal, sizeof(uint16_t) * 64);
    }
#else
    int i, t;
    for (i = 0; i < 128; i++)
    {
        t = i << 1;
        col_pal[i] = t;
        col_pal[128 + i] = ((255 - t) | (t << 8));
        col_pal[256 + i] = ((255 - t) << 8) | (t << 16);
        col_pal[384 + i] = ((255 - t) << 16);
    }
    for (i = 1; i < 4; i++)
    {
        memcpy(col_pal + i * 512, col_pal, sizeof(int) * 512);
    }
#endif
}

void luckfox_rect(int x1, int y1, int x2, int y2, int c)
{
    for (int x = x1; x <= x2; x++)
	    for (int y = y1; y <= y2; y++)
	        luckfox_setpx(NULL, x, y, c);
}

void luckfox_setpx(void *canvas, int x, int y, int c)
{
    if (!tft_canvas)
	return;
//    pthread_mutex_lock(&logmutex);
    tft_canvas[x + y * IMG_W] = c;
    //log_msg("%s: (%d,%d) = %d\n", __FUNCTION__, x, y, c);
//    pthread_mutex_unlock(&logmutex);
}

uint16_t convertToBGR565(const cv::Vec3b& bgr_pixel) 
{
    uint16_t blue = (bgr_pixel[0] >> 3) & 0x1F;  // 5 bits for blue
    uint16_t green = (bgr_pixel[1] >> 2 ) & 0x3F; // 6 bits for green
    uint16_t red = (bgr_pixel[2] >> 3) & 0x1F;    // 5 bits for red

    return ((blue << 11) | (green << 5) | (red ));   // Pack into 16 bits
}

cv::Mat convertRGB565toCV8UC3(const uint16_t* data, int width, int height) {
    cv::Mat mat8bit(height, width, CV_8UC3);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            uint16_t pixel = data[y * width + x];
            uint8_t r = (pixel & 0x1F) << 3;         // Extract blue and scale to 8 bits
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;   // Extract green and scale to 8 bits
            uint8_t b = ((pixel >> 11) & 0x1F) << 3;  // Extract red and scale to 8 bits

            mat8bit.at<cv::Vec3b>(y, x) = cv::Vec3b(b, g, r);
        }
    }

    return mat8bit;
}

cv::Mat convert24bitBGRtoCV8UC3(const CANVAS_TYPE *data, int width, int height) {
    // Create the Mat header without copying data
    cv::Mat mat8bit(height, width, CV_8UC4, const_cast<CANVAS_TYPE *>(data));

    // Create a deep copy if you need to modify the Mat independently of the original data
    // This avoids potential issues if the original data is freed or modified later.
    return mat8bit.clone(); 
}

void luckfox_play(void)
{
    CANVAS_TYPE *mask = new CANVAS_TYPE[img_w * img_h];
    memcpy(mask, tft_canvas, img_h * img_w * sizeof(CANVAS_TYPE));
#if VIDEO_CAPTURE    
    cv::VideoCapture cap;
    cv::Mat bgr(img_h, img_w, CV_8UC3); 
    cv::Mat disp;
    cap.set(cv::CAP_PROP_FRAME_WIDTH,  img_w);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, img_h);
    cap.open(0);
    while (1)
    {
        cap >> bgr;

        bgr.forEach<cv::Vec3b>([&mask](cv::Vec3b &p, const int *pos) { 
                                    int idx = pos[0] * img_w + pos[1];
                                    if (!mask[idx])
                                        tft_canvas[idx] = convertToBGR565(p); });
    }
#else
#ifdef COL16BIT
    cv::Mat img = convertRGB565toCV8UC3(mask, img_w, img_h);
#else
    cv::Mat img = convert24bitBGRtoCV8UC3(mask, img_w, img_h);
#endif
    cv::imshow("Mandelbrot", img);                            
         
#endif
    cv::waitKey(0);

    delete[] mask;
}
