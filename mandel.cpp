#include <stdarg.h>
#include <cstring>
#include <math.h>
#include <vector>

#include "mandel-arch.h"

#ifdef PTHREADS
pthread_mutex_t canvas_sem;
pthread_mutex_t logmutex;
void log_msg(const char *s, ...)
{
    pthread_mutex_lock(&logmutex);
    {
        va_list args;
        va_start(args, s);
        vprintf(s, args);
        va_end(args);
    }
    pthread_mutex_unlock(&logmutex);
}
#endif  /* PTHREADS */

// globals
int img_w = 800, img_h=480;   // used by luckfox
int iter = MAX_ITER_INIT;     // used by Amiga
int video_device, blend, do_mq = 1;      // used by opencv
MTYPE xrat = 1.0;

static CANVAS_TYPE *cv;
static char *stacks;

#include "mandelbrot.h"

typedef struct
{
    point_t lu;
    point_t rd;
} rec_t;

#ifndef __ZEPHYR__
int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        char opt;
        while ((opt = getopt(argc, argv, "bq:v:r:h")) != (char)-1)
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
                printf("Usage: %s [-b] [-q <0|1|2> select thread policy] [-v <video device nr>] [-r resolution in form <XxY> (e.g.: -r 800x480)]\n", argv[0]);
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
    usleep(1000*50); // wait for boot banner
#endif /* ZEPHYR */    
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
    log_msg("%s: stack_size per thread = %d, no threads=%d, iter = %d, palette = %ld, stacks = %p, cv = %p, CSIZE = %d\n", 
        __FUNCTION__, STACK_SIZE, NO_THREADS, iter, PAL_SIZE, stacks, cv, CSIZE);
#if 0
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
#else
std::vector<rec_t> recs = {
//    {{00, IMG_H / 4}, {IMG_W / 2, IMG_H / 4 * 3}},
//    {{00, IMG_H / 4}, {IMG_W / 2, IMG_H / 4 * 3}},
//    {{IMG_W / 4, IMG_H / 4}, {IMG_W / 4 + 200, IMG_H / 4 * 3}},
};
#endif

    for (int i = 0; i < 1; i++)
    {
        hook1();
        mandel<MTYPE> *m = new mandel<MTYPE>{cv, stacks, 
                                            static_cast<MTYPE>(INTIFY(-1.5)), static_cast<MTYPE>(INTIFY(-1.0)), 
                                            static_cast<MTYPE>(INTIFY(0.5)), static_cast<MTYPE>(INTIFY(1.0)), 
                                            IMG_W / PIXELW, IMG_H, xrat};
        zoom_ui(m);
        
        for (size_t i = 0; i < recs.size(); i++)
        {
            auto it = &recs[i];
            log_msg("%d/%d, zooming into [%d,%d]x[%d,%d]...stacks=%p\n", (int)i, (int)recs.size(), it->lu.x, it->lu.y, it->rd.x, it->rd.y, m->get_stacks());
            m->select_start(it->lu);
            m->select_end(it->rd);
        }
        delete m;
        hook2();
    }
#ifndef __ZEPHYR__
    delete[] cv;
    delete[] stacks;
#else
    log_msg("system halted");
    while (1)
    {   
//        log_msg(".");
//        fflush(stdout);
//        sleep(5);
    }
#endif
    return 0;
}
