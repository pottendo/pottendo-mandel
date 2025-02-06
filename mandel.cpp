#include <stdarg.h>
#include <cstring>
#include <math.h>
#include <vector>
#include <stdio.h>

#include "mandel-arch.h"

#ifdef PTHREADS
pthread_mutex_t canvas_sem;
pthread_mutex_t logmutex;
void log_msg(const char *s, ...)
{
    va_list args;
    va_start(args, s);
    pthread_mutex_lock(&logmutex);
    vprintf(s, args);
    pthread_mutex_unlock(&logmutex);
    va_end(args);
}

#else
void log_msg(const char *s, ...)
{
    va_list args;
    va_start(args, s);
    vprintf(s, args);
    va_end(args);
}
#endif  /* PTHREADS */

int log_level = 0;
void log_msg(int lv, const char *s, ...)
{
    if (lv <= log_level)
    {
        char buf[256];
        va_list args;
        va_start(args, s);
        vsnprintf(buf, 255, s, args);
        log_msg(buf);
        va_end(args);
    }
}

#if !defined(MANDEL_PC)
#define MANDEL_PC 0
#endif
// globals
int img_w = 800, img_h=480;   // used by luckfox
int iter = MAX_ITER_INIT;     // used by Amiga
int video_device, blend, do_mq = MANDEL_PC;      // used by opencv
MTYPE xrat = 1.0;
bool zoom_mode = false;
int dwell_time = 3;

static CANVAS_TYPE *cv;
static char *stacks;

#include "mandelbrot.h"

typedef struct
{
    point_t lu;
    point_t rd;
} rec_t;

void init_palette(int *col_pal)
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
#if defined(COL16BIT)
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
#elif defined(COL32BIT)
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
        //memcpy(col_pal + i * 512, col_pal, sizeof(int) * 512);
    }
#else
    for (auto i = 0; i < PAL_SIZE; i++)
        col_pal[i] = i;
#endif
}

#if !defined(__ZEPHYR__) && !defined(PICO) && !defined(ESP32) && !defined(ESP8266)
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        char opt;
        while ((opt = getopt(argc, argv, "i:l:bzq:v:r:t:h")) != (char)-1)
        {
            switch (opt)
            {
            case 'b':
                blend = 1;
                break;
            case 'q':
                if (optarg)
                    do_mq = strtol(optarg, NULL, 10);
#ifndef PTHREADS
                log_msg("not useful without threads - ignoring -q \n");
#endif
                break;
            case 'v':
                if (optarg)
                {
                    video_device = strtol(optarg, NULL, 10);
                }
                log_msg("video-device set to %d\n", video_device);
                break;
            case 'l':
                log_level = strtol(optarg, NULL, 10);
                break;
            case 'i':
                iter = strtol(optarg, NULL, 10);
                if (iter < 16) { iter = 16; break; }
                break;
            case 'z':
                zoom_mode = 1;
                break;
            case 't':
                dwell_time = strtol(optarg, NULL, 10);
                break;
            case 'r':
                if (optarg)
                {
                    char *x;
                    img_w = strtol(optarg, &x, 10);
                    if (*x == 'x')
                    {
                        img_h = strtol(x + 1, NULL, 10);
                        if ((img_w > 0) && (img_w > 0))
                            break;
                    }
                } // else fallthrough
            case 'h':
            case '?':
                // Print help message and exit
                printf("Usage: %s [-b] [-q <0|1|2> select thread policy] [-v <video device nr>]"
                        " [-r resolution in form <XxY> (e.g.: -r 800x480) -l <loglevel> -i <iter depth> -z -t <dwell time>]\n", argv[0]);
                return 0;
            default:
                fprintf(stderr, "Unexpected error in getopt: %c/%d\n", (isprint(opt)?opt:'.'), opt);
                return 1;
            }
        }
    }
#else
int main(void)
{
#if defined(__ZEPHYR__)
    usleep(1000*50); // wait for boot banner
#endif    
#endif /* ZEPHYR && PICO && ESPs*/    
#ifdef PTHREADS
    pthread_mutex_init(&logmutex, NULL);
#endif
    stacks = alloc_stack;
    cv = setup_screen();
    log_msg("Welcome mandelbrot...\n");
    log_msg("blending %sactivated\n", blend ? "" : "de");
    log_msg("resolution set: %dx%d\n", IMG_W, IMG_H);
#ifdef PTHREADS    
    log_msg("using %s\n", 
        (do_mq == 0) ? "even allocation to threads" :
        (do_mq == 1) ? "message queue sync" :
        (do_mq > 1) ? "producer consumer sync": "unknown");
#endif        
    if (!cv) cv = alloc_canvas;
    log_msg("%s: stack_size per thread = %d, no threads=%d, iter = %d, palsize = %ld, stacks = %p, cv = %p, CSIZE = %d\n", 
        __FUNCTION__, STACK_SIZE, NO_THREADS, iter, PAL_SIZE, stacks, cv, CSIZE);
#if 1
std::vector<rec_t> recs = { 
        {{00, 00},{80,100}}, 
        {{80, 100},{159,199}}, 
        {{00, 50},{40,100}},        
        {{80, 110}, {120, 160}},
        {{60,75}, {100, 125}},
        {{60,110}, {100, 160}},
        {{60,75}, {100, 125}},
        {{60,75}, {100, 125}},
        {{40,50}, {80, 100}},
        {{120,75}, {159, 125}},
    };

#endif
    for (;;)
    {
        hook1();
        mandel<MTYPE> *m = new mandel<MTYPE>{cv, stacks, 
                                            static_cast<MTYPE>(INTIFY(-1.5)), static_cast<MTYPE>(INTIFY(-1.0)), 
                                            static_cast<MTYPE>(INTIFY(0.5)), static_cast<MTYPE>(INTIFY(1.0)), 
                                            IMG_W / PIXELW, IMG_H, xrat};
        zoom_ui(m);
        
        for (size_t i = 0; i < frecs.size(); i++)
        {
            auto it = &frecs[i];
            m->zoom(it->xl, it->yl, it->xh, it->yh);
            zoom_ui(m);
        }
        delete m;
        hook2();
    }
#ifndef __ZEPHYR__
    delete[] cv;
    delete[] stacks;
#else
    log_msg("system halted.\n");
    while (1)
    {   
        //log_msg(".");
        //fflush(stdout);
        //usleep(50 * 1000);
    }
#endif
    return 0;
}
