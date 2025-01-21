#ifndef __MANDEL_ARCH_H__
#define __MANDEL_ARCH_H__
/* definition section for globals to adapt for some variation */
#define MTYPE double //long long int  // double
//#define INTMATH              // goes along with int above, on Intels or other fast FPUs, double/float can be faster
#define MAX_ITER_INIT 160
//#define C64   // build for C64 GFX output

// some global internals, no need to change normally
#ifdef INTMATH
#define INTSCALE 102400000LL
#define INTIFY(a) ((a) * INTSCALE)
#define INTIFY2(a) ((a) * INTSCALE * INTSCALE)
#define DEINTIFY(a) ((a) / INTSCALE)
#else
#define INTIFY(a) (a)
#define INTIFY2(a) (a)
#define DEINTIFY(a) (a)
#endif

#include <stdint.h>
#define alloc_stack new char[STACK_SIZE * NO_THREADS]()
#define alloc_canvas new CANVAS_TYPE[CSIZE]()

void log_msg(const char *s, ...);
void log_msg(int lv, const char *s, ...);
//#define NO_LOG
#ifdef NO_LOG
#define log_msg(...)
#endif

#ifdef PTHREADS
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#define NO_THREADS 16 // max 16 for Orangecart!
#ifdef PTHREAD_STACK_MIN
#define STACK_SIZE PTHREAD_STACK_MIN
#else
#define STACK_SIZE (1024)
#endif
extern pthread_mutex_t logmutex;
#else
#define NO_THREADS 1 // singlethreaded
#define STACK_SIZE 1024
#endif
#define MAX_ITER iter   // relict

#if defined(__amiga__) //-------------------------------------------------------------------
#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/io.h>
#include <inline/timer.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <graphics/sprite.h>
#include <exec/memory.h>
#include <devices/inputevent.h>
#include <clib/console_protos.h>
#ifdef __cplusplus
#include <vector>
#endif

#define CANVAS_TYPE char
#define SCRDEPTH 6  // or 6 for 64cols lesser resolution
#define PAL_SIZE (1L << SCRDEPTH)
#define PIXELW 1

#if (SCRDEPTH > 6)
#error "pixeldepth too large, must be <= 6"
#endif
#if (SCRDEPTH <= 4)
#define HALF 1
//#define HALF 2
#define SCRMODE (HIRES|LACE)
//#define SCRMODE EXTRA_HALFBRITE
#define SCMOUSE 2
//#define SCMOUSE 1
#else
#define HALF 2
#define SCRMODE (EXTRA_HALFBRITE)
#define SCMOUSE 1
#endif

#define IMG_W (640 / HALF)      // 320
#define IMG_H (512 / HALF - 20) // 200
#define CSIZE (IMG_W * IMG_H) / 8
#define WINX (IMG_W / 1)
#define WINY (IMG_H / 1)

#ifdef __cplusplus
extern int iter;
#include "mandelbrot.h"
CANVAS_TYPE *amiga_setup_screen(void);
void amiga_zoom_ui(mandel<MTYPE> *m);
int amiga_setpixel(void *, int x, int y, int col);

#define setup_screen amiga_setup_screen
#define canvas_setpx amiga_setpixel
#define zoom_ui amiga_zoom_ui
#define hook1(...)
#define hook2(...)
#endif /* __cplusplus */

#elif defined(__linux__) //-------------------------------------------------------------------
#ifdef LUCKFOX
#define VIDEO_CAPTURE
#define FB_DEVICE
#define COL16BIT
#define TOUCH
#define CANVAS_TYPE uint16_t
#define CVCOL CV_16UC1
#else
#define VIDEO_CAPTURE
#define CANVAS_TYPE uint32_t
#define CVCOL CV_8UC4
#endif
//#define BENCHMARK
#define IMG_W img_w
#define IMG_H img_h
//#define SCRDEPTH 24  // or 6 for 64cols lesser resolution
#define PAL_SIZE (512 * 16)
#define PIXELW 1
#define CSIZE (img_w * img_h) /// 8
#ifndef PTHREAD_STACK_MIN
#undef STACK_SIZE
#define STACK_SIZE 16384
#endif
#define MANDEL_MQ
extern CANVAS_TYPE *tft_canvas;		// must not be static?!
void luckfox_setpx(CANVAS_TYPE *canvas, int x, int y, int c);
CANVAS_TYPE *init_luckfox(void);
void luckfox_palette(int *p);
void luckfox_rect(CANVAS_TYPE *c, int x1, int y1, int x2, int y2, int col);
#define zoom_ui luckfox_play
#define setup_screen init_luckfox
#define canvas_setpx luckfox_setpx
#define hook1(...)
#define hook2(...)
extern int iter, video_device, blend, do_mq;
extern int img_w, img_h;
#include "mandelbrot.h"
void luckfox_play(mandel<MTYPE> *mandel);

#elif defined(C64) || defined (__ZEPHYR__) //--------------------------------------------
#if (NO_THREADS > 16)
#error "too many threads for Orangencart's STACK_SIZE"
#endif
#define CANVAS_TYPE char
#define SCRDEPTH 2
#define CSIZE ((IMG_W/8) * IMG_H)
#define PAL_SIZE (1L << SCRDEPTH)
#define PIXELW 2 // 2
#undef STACK_SIZE
#define STACK_SIZE 1024
#define canvas_setpx canvas_setpx_
#define setup_screen(...) NULL;
#define zoom_ui(...)

#include "c64-lib.h"
extern c64_t c64;
extern char *c64_stack;
extern char *c64_screen_init(void);
#undef alloc_stack
#define alloc_stack c64_stack
#ifdef C64
#define IMG_W 320
#define IMG_H 200
#undef setup_screen
#define setup_screen c64_screen_init
#undef alloc_canvas
#define alloc_canvas (char *)&c64.get_mem()[0x4000]
#define hook1 c64_hook1
#define hook2 c64_hook2
#else
#define IMG_W 120
#define IMG_H 48
#define hook1(...)
#define hook2(...)
#endif

#elif defined(PICO) // PICO ------------------------------------------------
#define clock_gettime(...) 0
#define COL16BIT
#define TOUCH
#define CANVAS_TYPE uint16_t
#define IMG_W 240
#define IMG_H 240
#define PAL_SIZE (512 * 16)
#define PIXELW 1
#define CSIZE (IMG_W * IMG_H)

#define canvas_setpx pico_setpx
#define setup_screen pico_init
#define alloc_stacks NULL
#define zoom_ui(...)
#define hook1(...)
#define hook2(...)
void pico_setpx(CANVAS_TYPE *canvas, int x, int y, int c);
CANVAS_TYPE *pico_init(void);

#elif defined(ESP32) || defined(ESP8266)   // ESP32-------------------------------------------

#if defined(ESP32CONSOLE)
#define SCRDEPTH 2
#define PAL_SIZE (1L << SCRDEPTH)
#define PIXELW 2 // 2
#define CSIZE ((IMG_W/8) * IMG_H * PIXELW)
#define CANVAS_TYPE char
#define canvas_setpx canvas_setpx_
#else
#define SCRDEPTH 3
#define PAL_SIZE (_PAL_SIZE)
#define PIXELW 1 // 2
#define CSIZE ((IMG_W/8) * IMG_H * PIXELW)
#define CANVAS_TYPE char
#undef alloc_canvas
#define alloc_canvas NULL
#define canvas_setpx esp32_setpx
#endif  /* ESP32CONSOLE */
#undef alloc_stack
#define alloc_stack NULL
#define setup_screen(...) NULL
#define zoom_ui esp32_zoomui
#define hook1(...)
#define hook2(...)

#ifdef ESP8266
#define clock_gettime(...) 0
#endif
#define IMG_W img_w
#define IMG_H img_h
int esp32_setpx(CANVAS_TYPE *canvas, int x, int y, int c);
extern int iter;
extern int img_w, img_h;
#include "mandelbrot.h"
void esp32_zoomui(mandel<MTYPE> *m);

#else // non-specific architectures-----------------------------------------
#define IMG_W 120
#define IMG_H 48
#define SCRDEPTH 2
#define PAL_SIZE (1L << SCRDEPTH)
#define PIXELW 2 // 2
#define CSIZE ((IMG_W/8) * IMG_H * PIXELW)
#define CANVAS_TYPE char

#define canvas_setpx canvas_setpx_
#define setup_screen(...) NULL

#define zoom_ui(...)
#define hook1(...)
#define hook2(...)

#endif /* __amiga__ */
#endif /* __MANDEL_ARCH_H__*/