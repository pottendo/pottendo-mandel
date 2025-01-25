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

static CANVAS_TYPE *cv;
static char *stacks;

#include "mandelbrot.h"

typedef struct
{
    point_t lu;
    point_t rd;
} rec_t;

typedef struct {
    MTYPE xl;
    MTYPE yl;
    MTYPE xh;
    MTYPE yh;
} frec_t;

#if !defined(__ZEPHYR__) && !defined(PICO) && !defined(ESP32) && !defined(ESP8266)
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        char opt;
        while ((opt = getopt(argc, argv, "i:l:bq:v:r:h")) != (char)-1)
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
                printf("Usage: %s [-b] [-q <0|1|2> select thread policy] [-v <video device nr>] [-r resolution in form <XxY> (e.g.: -r 800x480) -l <loglevel> -i <iter depth>]\n", argv[0]);
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
    log_msg("Welcome mandelbrot...\n");
    log_msg("blending %sactivated\n", blend ? "" : "de");
    log_msg("resolution set: %dx%d\n", IMG_W, IMG_H);
#ifdef PTHREADS    
    log_msg("using %s\n", 
        (do_mq == 0) ? "even allocation to threads" :
        (do_mq == 1) ? "message queue sync" :
        (do_mq > 1) ? "producer consumer sync": "unknown");
#endif        
    stacks = alloc_stack;
    cv = setup_screen();
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

const std::vector<frec_t> frecs = {
    //{-1.500000, -1.000000, 0.500000, 1.000000},
    {static_cast<MTYPE>(INTIFY(-1.500000)), static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(-0.500000)), static_cast<MTYPE>(INTIFY(0.000000))},
    {static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(-0.500000)), static_cast<MTYPE>(INTIFY(-0.506250)), static_cast<MTYPE>(INTIFY(-0.005000))},
    {static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(-0.376250)), static_cast<MTYPE>(INTIFY(-0.876563)), static_cast<MTYPE>(INTIFY(-0.252500))},
    {static_cast<MTYPE>(INTIFY(-0.938281)), static_cast<MTYPE>(INTIFY(-0.308187)), static_cast<MTYPE>(INTIFY(-0.907422)), static_cast<MTYPE>(INTIFY(-0.277250))},
    {static_cast<MTYPE>(INTIFY(-0.926709)), static_cast<MTYPE>(INTIFY(-0.296586)), static_cast<MTYPE>(INTIFY(-0.918994)), static_cast<MTYPE>(INTIFY(-0.288852))},
    {static_cast<MTYPE>(INTIFY(-0.923816)), static_cast<MTYPE>(INTIFY(-0.292332)), static_cast<MTYPE>(INTIFY(-0.921887)), static_cast<MTYPE>(INTIFY(-0.290398))},
    {static_cast<MTYPE>(INTIFY(-0.923093)), static_cast<MTYPE>(INTIFY(-0.291607)), static_cast<MTYPE>(INTIFY(-0.922610)), static_cast<MTYPE>(INTIFY(-0.291124))},
    {static_cast<MTYPE>(INTIFY(-0.922912)), static_cast<MTYPE>(INTIFY(-0.291426)), static_cast<MTYPE>(INTIFY(-0.922791)), static_cast<MTYPE>(INTIFY(-0.291305))},
    {static_cast<MTYPE>(INTIFY(-0.922882)), static_cast<MTYPE>(INTIFY(-0.291395)), static_cast<MTYPE>(INTIFY(-0.922852)), static_cast<MTYPE>(INTIFY(-0.291365))},
    {static_cast<MTYPE>(INTIFY(-0.922859)), static_cast<MTYPE>(INTIFY(-0.291384)), static_cast<MTYPE>(INTIFY(-0.922852)), static_cast<MTYPE>(INTIFY(-0.291377))}
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
        
        for (size_t i = 0; i < recs.size(); i++)
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
