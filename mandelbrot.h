/* -*-c++-*-
 * This file is part of pottendos-playground.
 *
 * FE playground is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FE playground is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FE playground.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef __MANDELBROT_H__
#define __MANDELBROT_H__

#include <stdlib.h>
#include <unistd.h>
#include <complex>
// #include <iostream>
#include <signal.h>

#include <time.h>
#include <cstdint>
typedef struct
{
    int x;
    int y;
} point_t;

const std::vector<frec_t> frecs = {
    {static_cast<MTYPE>(INTIFY(-1.500000)), static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(0.500000)), static_cast<MTYPE>(INTIFY(1.000000))},
   	{static_cast<MTYPE>(INTIFY(-0.712500)), static_cast<MTYPE>(INTIFY(0.350000)), static_cast<MTYPE>(INTIFY(-0.545000)), static_cast<MTYPE>(INTIFY(0.470833))},
	{static_cast<MTYPE>(INTIFY(-0.649478)), static_cast<MTYPE>(INTIFY(0.397578)), static_cast<MTYPE>(INTIFY(-0.631681)), static_cast<MTYPE>(INTIFY(0.410668))},
	{static_cast<MTYPE>(INTIFY(-0.643093)), static_cast<MTYPE>(INTIFY(0.401860)), static_cast<MTYPE>(INTIFY(-0.640224)), static_cast<MTYPE>(INTIFY(0.403660))},
	{static_cast<MTYPE>(INTIFY(-0.642477)), static_cast<MTYPE>(INTIFY(0.402538)), static_cast<MTYPE>(INTIFY(-0.642082)), static_cast<MTYPE>(INTIFY(0.402782))},
	{static_cast<MTYPE>(INTIFY(-0.642258)), static_cast<MTYPE>(INTIFY(0.402660)), static_cast<MTYPE>(INTIFY(-0.642214)), static_cast<MTYPE>(INTIFY(0.402682))},
	{static_cast<MTYPE>(INTIFY(-0.642249)), static_cast<MTYPE>(INTIFY(0.402669)), static_cast<MTYPE>(INTIFY(-0.642237)), static_cast<MTYPE>(INTIFY(0.402675))},
	{static_cast<MTYPE>(INTIFY(-0.642245)), static_cast<MTYPE>(INTIFY(0.402670)), static_cast<MTYPE>(INTIFY(-0.642242)), static_cast<MTYPE>(INTIFY(0.402672))},
    {static_cast<MTYPE>(INTIFY(-1.500000)), static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(0.500000)), static_cast<MTYPE>(INTIFY(1.000000))},
    {static_cast<MTYPE>(INTIFY(-1.350000)), static_cast<MTYPE>(INTIFY(-0.483333)), static_cast<MTYPE>(INTIFY(-1.022500)), static_cast<MTYPE>(INTIFY(-0.200000))},
    {static_cast<MTYPE>(INTIFY(-1.266078)), static_cast<MTYPE>(INTIFY(-0.390660)), static_cast<MTYPE>(INTIFY(-1.243153)), static_cast<MTYPE>(INTIFY(-0.374132))},
    {static_cast<MTYPE>(INTIFY(-1.256163)), static_cast<MTYPE>(INTIFY(-0.384393)), static_cast<MTYPE>(INTIFY(-1.253040)), static_cast<MTYPE>(INTIFY(-0.381879))},
    {static_cast<MTYPE>(INTIFY(-1.254211)), static_cast<MTYPE>(INTIFY(-0.383115)), static_cast<MTYPE>(INTIFY(-1.253535)), static_cast<MTYPE>(INTIFY(-0.382529))},
    {static_cast<MTYPE>(INTIFY(-1.253829)), static_cast<MTYPE>(INTIFY(-0.382716)), static_cast<MTYPE>(INTIFY(-1.253701)), static_cast<MTYPE>(INTIFY(-0.382611))},
    {static_cast<MTYPE>(INTIFY(-1.253755)), static_cast<MTYPE>(INTIFY(-0.382667)), static_cast<MTYPE>(INTIFY(-1.253742)), static_cast<MTYPE>(INTIFY(-0.382659))},
    {static_cast<MTYPE>(INTIFY(-1.253749)), static_cast<MTYPE>(INTIFY(-0.382663)), static_cast<MTYPE>(INTIFY(-1.253747)), static_cast<MTYPE>(INTIFY(-0.382662))},
    {static_cast<MTYPE>(INTIFY(-1.500000)), static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(0.500000)), static_cast<MTYPE>(INTIFY(1.000000))},
    {static_cast<MTYPE>(INTIFY(-1.500000)), static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(-0.500000)), static_cast<MTYPE>(INTIFY(0.000000))},
    {static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(-0.500000)), static_cast<MTYPE>(INTIFY(-0.506250)), static_cast<MTYPE>(INTIFY(-0.005000))},
    {static_cast<MTYPE>(INTIFY(-1.000000)), static_cast<MTYPE>(INTIFY(-0.376250)), static_cast<MTYPE>(INTIFY(-0.876563)), static_cast<MTYPE>(INTIFY(-0.252500))},
    {static_cast<MTYPE>(INTIFY(-0.938281)), static_cast<MTYPE>(INTIFY(-0.308187)), static_cast<MTYPE>(INTIFY(-0.907422)), static_cast<MTYPE>(INTIFY(-0.277250))},
    {static_cast<MTYPE>(INTIFY(-0.926709)), static_cast<MTYPE>(INTIFY(-0.296586)), static_cast<MTYPE>(INTIFY(-0.918994)), static_cast<MTYPE>(INTIFY(-0.288852))},
    {static_cast<MTYPE>(INTIFY(-0.923816)), static_cast<MTYPE>(INTIFY(-0.292332)), static_cast<MTYPE>(INTIFY(-0.921887)), static_cast<MTYPE>(INTIFY(-0.290398))},
    {static_cast<MTYPE>(INTIFY(-0.923093)), static_cast<MTYPE>(INTIFY(-0.291607)), static_cast<MTYPE>(INTIFY(-0.922610)), static_cast<MTYPE>(INTIFY(-0.291124))},
    {static_cast<MTYPE>(INTIFY(-0.922912)), static_cast<MTYPE>(INTIFY(-0.291426)), static_cast<MTYPE>(INTIFY(-0.922791)), static_cast<MTYPE>(INTIFY(-0.291305))},
    {static_cast<MTYPE>(INTIFY(-0.922882)), static_cast<MTYPE>(INTIFY(-0.291395)), static_cast<MTYPE>(INTIFY(-0.922852)), static_cast<MTYPE>(INTIFY(-0.291365))},
    {static_cast<MTYPE>(INTIFY(-0.922859)), static_cast<MTYPE>(INTIFY(-0.291384)), static_cast<MTYPE>(INTIFY(-0.922852)), static_cast<MTYPE>(INTIFY(-0.291377))}};

#ifdef PTHREADS
#include <pthread.h>
#if defined(ESP32)
#include <Arduino.h>
#define sem_t SemaphoreHandle_t
#define sem_wait(x) xSemaphoreTake(*x, portMAX_DELAY)
#define sem_post(x) xSemaphoreGive(*x)
#define sem_init(x, b, c) *x = xSemaphoreCreateCounting(NO_THREADS + 16, c)
#define sem_destroy(x) vSemaphoreDelete(*x)
#define pthread_attr_getstack(...) 0
#else
#include <semaphore.h>
#ifdef MANDEL_MQ
#include <mqueue.h>
#endif
#endif
extern pthread_mutex_t canvas_sem;
#define P P_
#define V V_
#define MQ_NAME "/mandel_feed"
#else
// make those calls dummies
#define pthread_mutex_init(...)
#define pthread_mutex_destroy(...) 0
#define pthread_mutex_lock(...)
#define pthread_mutex_unlock(...)
#define sem_wait(...)
#define sem_post(...)
#define sem_init(...)
#define sem_destroy(...)
#define pthread_getschedparam(...)
#define pthread_attr_setschedpolicy(...)
#define pthread_attr_init(...)
#define pthread_attr_destroy(...) 0
#define pthread_attr_setstack(...) 0
#define pthread_create(...) 0
#define pthread_detach(...) 0
#define pthread_exit(...)
#define sem_t int
#define pthread_t int
#define pthread_mutex_t int
#define sched_yield(...)
#define P(...)
#define V(...)
#endif /* PTHREADS */

template <typename myDOUBLE>
class mandel
{
    typedef CANVAS_TYPE *canvas_t;
    typedef uint16_t coord_t;
    typedef int color_t;
    struct tparam_t
    {
        int tno, width, height;
        myDOUBLE xl, yl, xh, yh, incx, incy;
        int xoffset, yoffset;
        pthread_mutex_t go;
        sem_t &sem;
        mandel<myDOUBLE> *mo;

        tparam_t(int t, int w, int h, myDOUBLE x1, myDOUBLE y1, myDOUBLE x2, myDOUBLE y2, myDOUBLE ix, myDOUBLE iy,
                 int xo, int yo, sem_t &se, mandel<myDOUBLE> *th)
            : tno(t), width(w), height(h), xl(x1), yl(y1), xh(x2), yh(y2), incx(ix), incy(iy), xoffset(xo), yoffset(yo), sem(se), mo(th)
        {
            pthread_mutex_init(&go, nullptr);
            P(go); // lock it
            // std::cout << *this << '\n';
        }
        ~tparam_t()
        {
            int res;
            //log_msg("cleaning up params for thread %d\n", tno);
            if ((res = pthread_mutex_destroy(&go)) != 0)
                log_msg("%s: pthread_mutex_destroy failed, may loose memory! res = %d\n", __FUNCTION__, res);
        }
        friend std::ostream &operator<<(std::ostream &ostr, tparam_t &t)
        {
            ostr << "t=" << t.tno << ", w=" << t.width << ",h=" << t.height
                 << ",xl=" << t.xl << ",yl=" << t.yl << ",xh=" << t.xh << ",yh=" << t.yh
                 << ",incx=" << t.incx << ",incy=" << t.incy
                 << ",xo=" << t.xoffset << ",yo=" << t.yoffset << ", go: " << t.go;
            return ostr;
        }
    };
    tparam_t *tp[NO_THREADS];
    // char *stacks[NO_THREADS];
    int &max_iter = MAX_ITER;
    uint8_t _mask;

    /* class local variables */
#ifdef PTHREADS
    struct tqparam_t
    {
        int tno;
        sem_t &sem;
        mandel<myDOUBLE> *mo;

        tqparam_t(int t, sem_t &se, mandel<myDOUBLE> *th)
            : tno(t), sem(se), mo(th)
        {}
    };
    tqparam_t *tpq[NO_THREADS];
    pthread_attr_t attr[NO_THREADS];
#if defined(MANDEL_MQ)    
    // mq attributes for message queue setup
    struct mq_attr mqattr;
#endif
    // Semaphores for prod/consumer setup
    sem_t pcempty;  // Counts empty slots in the buffer
    sem_t pcfull;   // Counts full slots in the buffer
    pthread_mutex_t pcmutex;
    int pcin = 0;
    int pcout = 0;
    #define PCBUFFER_SIZE (NO_THREADS + 16)
    point_t pcbuffer[PCBUFFER_SIZE];
#endif
    sem_t master_sem;

    canvas_t canvas;
    char *stacks;
    int xres, yres;
    color_t col_pal[PAL_SIZE];// = {0, 1, 2, 3};
    coord_t mark_x1, mark_y1, mark_x2, mark_y2;
    myDOUBLE last_xr, last_yr, ssw, ssh, transx, transy, xratio;
    int stop;
    struct timespec tstart, tend;

    static void P_(pthread_mutex_t &m)
    {
        pthread_mutex_lock(&m);
    }
    static void V_(pthread_mutex_t &m)
    {
        pthread_mutex_unlock(&m);
    }
    static void PSem(sem_t &s)
    {
        sem_wait(&s);
    }
    static void VSem(sem_t &s)
    {
        sem_post(&s);
    }

    int canvas_setpx_(canvas_t &canvas, coord_t x, coord_t y, color_t c)
    {
        uint32_t h = x % (8 / PIXELW);
        uint32_t shift = (8 / PIXELW - 1) - h;
        uint32_t val = (c << (shift * PIXELW));
        uint8_t mask = ~(_mask << (shift * PIXELW));
        const uint32_t lineb = IMG_W / 8 * 8; // line 8 bytes per 8x8 pixel
        const uint32_t colb = 8;              // 8 bytes per 8x8 pixel

        // log_msg("x/y %d/%d offs %d/%d\n", x, y, (x / (8 / PIXELW)) * colb, (y / 8) * lineb  + (y % 8));
        uint32_t cidx = (y / 8) * lineb + (y % 8) + (x / (8 / PIXELW)) * colb;
        if (cidx >= (uint32_t)CSIZE)
        {
            log_msg("Exceeding canvas!! %d, %d/%d\n", cidx, x, y);
            // delay (100 * 1000);
            return 0;
        }
        char t = canvas[cidx];
        t &= mask;
        t |= val;
        canvas[cidx] = t;
#ifdef __ZEPHYR__
        // volatile char *led= (char *)0xf0002000;
        //*led = ((*led) + 1);
#endif
        // sched_yield();
        return 0;
    }

    void dump_bits(uint8_t c, uint8_t *buf)
    {

        for (int i = 7; i >= 0; i--)
        {
            *buf++ = (c & (1 << i)) ? '*' : '.';
        }
    }

    void canvas_dump(canvas_t c)
    {
        static uint8_t *b, *s;
        s = b = new uint8_t[IMG_W * 8 + 1];
        for (int y = 0; y < IMG_H; y++)
        {
            log_msg("%03d: ", y);
            int offs = (y / 8) * IMG_W + (y % 8);
            for (auto i = 0; i < IMG_W / 1; i += 8)
            {
                // log_msg("idx: %032d ", offs + i);
                dump_bits(c[offs + i], b);
                b += 8;
            }
            *b = '\0';
            log_msg("%s\n", s);
            b = s;
        }
        delete[] s;
    }

    /* class private functinos */
    inline void timespec_diff(struct timespec *a, struct timespec *b, struct timespec *result)
    {
        result->tv_sec = a->tv_sec - b->tv_sec;
        result->tv_nsec = a->tv_nsec - b->tv_nsec;
        if (result->tv_nsec < 0)
        {
            --result->tv_sec;
            result->tv_nsec += 1000000000L;
        }
    }

    // abs() would calc sqr() as well, we don't need that for this fractal
    inline myDOUBLE abs2(std::complex<myDOUBLE> f)
    {
        myDOUBLE r = f.real(), i = f.imag();
        return r * r + i * i;
    }

    int mandel_calc_point(const std::complex<myDOUBLE> point)
    {
        std::complex<myDOUBLE> z = point;
        int nb_iter = 1;
        while (abs2(z) < INTIFY2(4) && nb_iter <= max_iter)
        {
            z = (DEINTIFY(z * z) + point);
            nb_iter++;
        }
        if (nb_iter < max_iter)
#if (_PAL_SIZE == 2)
            return nb_iter;
#else        
            return (col_pal[(nb_iter % (PAL_SIZE - 1)) + 1]);
#endif            
        else
            return 0;
    }

    int mandel_calc_point(myDOUBLE x, myDOUBLE y)
    {
        const std::complex<myDOUBLE> point{x, y};
        // std::cout << "calc: " << point << '\n';
        return mandel_calc_point(point);
    }

    void mandel_helper(myDOUBLE xl, myDOUBLE yl, myDOUBLE xh, myDOUBLE yh, myDOUBLE incx, myDOUBLE incy, int xo, int yo, int width, int height)
    {
        myDOUBLE x, y;
        int xk = xo;
        int yk = yo;
        if ((xl == xh) || (yl == yh))
        {
            //log_msg("assertion failed: xl=%d, xh=%d, yl=%d, yh=%d\n", xl, xh, yl, yh);
            log_msg("assertion failed - reached fixp/fpu limit!\n");
            return;
        }
        x = xl;
        for (xk = 0; xk < width; xk++)
        {
            y = yl;
            for (yk = 0; yk < height; yk++)
            {
                int d = mandel_calc_point(x, y);
                P(canvas_sem);
                if (stop || (stop = canvas_setpx(canvas, xk + xo, yk + yo, d)))
                {
                    V(canvas_sem);
                    goto out;
                }
                V(canvas_sem);
                y += incy;
            }
            x += incx;
        }
        out:
        ;
    }

    static void *mandel_wrapper(void *param)
    {
        tparam_t *p = static_cast<tparam_t *>(param);
        p->mo->mandel_wrapper_2(param);
        pthread_exit(nullptr);
        return nullptr;
    }

    int mandel_wrapper_2(void *param)
    {
        tparam_t *p = (tparam_t *)param;

#if defined(__ZEPHYR__) && defined(CONFIG_FPU)
        // make sure FPU regs are saved during context switch
        int r;
        if ((r = k_float_enable(k_current_get(), 0)) != 0)
        {
            log_msg("%s: k_float_enable() failed: %d.\n", __FUNCTION__, r);
        }
#endif
        P(p->go);
#ifdef PTHREADS
#ifdef __linux__
        sched_param sp;
        int pol = -1;
        int ret;
        ret = pthread_getschedparam(pthread_self(), &pol, &sp);
        if (ret != 0)
            log_msg("pthread_getschedparam()... failed: %d\n", ret);
        sp.sched_priority = sp.sched_priority + (p->tno % 3) + 1;
        if ((ret = pthread_setschedparam(pthread_self(), SCHED_RR, &sp)) != 0)
            log_msg("pthread setschedparam (pol=%d) failed for thread %d, %d - need sudo!\n", SCHED_RR, p->tno, ret);
        pthread_getschedparam(pthread_self(), &pol, &sp);
        log_msg(2, "starting thread %d with priority %d\n", p->tno, sp.sched_priority);
#else        
        log_msg(2, "starting thread %d...\n", p->tno);
#endif
        char *st; 
        size_t sts;
        volatile char c;
        if (pthread_attr_getstack(&attr[p->tno], (void **)&st, &sts) != 0)
            log_msg("%s:' pthread_attr_getstackaddr() failed %d\n", __FUNCTION__, errno);
        for (size_t i = 0; i < sts; i++)
        {
            c = st[i];
        }
        log_msg(1, "%s: thread %d's stack is at %p with size %d, %d\n", __FUNCTION__, p->tno, st, sts, c);
#endif
        sched_yield();
        mandel_helper(p->xl, p->yl, p->xh, p->yh, p->incx, p->incy, p->xoffset, p->yoffset, p->width, p->height);
        log_msg(2, "finished thread %d\n", p->tno);
        V(p->go);     // ensure mutex isn't locked, otherwise esp32 can't destroy without loosing memory
        VSem(p->sem); // report we've done our job
        return 0;
    }

    void mandel_setup(const int thread_no, myDOUBLE sx, myDOUBLE sy, myDOUBLE tx, myDOUBLE ty)
    {
        int t = 0;
        mandel_presetup(sx, sy, tx, ty);
        myDOUBLE stepx = (xres / thread_no) * ssw * xratio;
        myDOUBLE stepy = (yres / thread_no) * ssh;

        if (thread_no > 16)
        {
            if (thread_no != 100) /* 100 is a dummy to just initialize, don't confuse us with this log */
                log_msg("%s: too many threads(%d)... giving up.\n", __FUNCTION__, thread_no);
            return;
        }
        int w = (xres / thread_no);
        int h = (yres / thread_no);

        for (int tx = 0; tx < thread_no; tx++)
        {
            int xoffset = w * tx;
            for (int ty = 0; ty < thread_no; ty++)
            {
                int yoffset = h * ty;
                tp[t] = new tparam_t(t,
                                     w, h,
                                     tx * stepx + transx,
                                     ty * stepy + transy,
                                     tx * stepx + stepx + transx,
                                     ty * stepy + stepy + transy,
                                     ssw, ssh, xoffset, yoffset,
                                     master_sem, this);
#ifndef PTHREADS
                if (clock_gettime(CLOCK_REALTIME, &tstart) < 0)
                    perror("clock_gettime()");
                //log_msg("start at %ld.%06ld\n", tstart.tv_sec % 60, tstart.tv_nsec / 1000);
                mandel_wrapper(tp[t]);
#else                
                int ret;
                pthread_t th = (pthread_t)0;
                pthread_attr_init(&attr[t]);
#ifdef __ZEPHYR__                
                ret = pthread_attr_setstack(&attr[t], stacks + t * STACK_SIZE, STACK_SIZE);
                if (ret != 0)
                    log_msg("setstack: %d - ssize = %d\n", ret, STACK_SIZE);
#endif                    
                if ((ret = pthread_create(&th, &attr[t], mandel_wrapper, tp[t])) != 0)
                    log_msg("pthread create failed for thread %d, %d\n", t, ret);
                ret = pthread_detach(th);
                if (ret != 0)
                    log_msg("pthread detach failed for thread %d, %d\n", t, ret);
#endif
                t++;
            }
        }
    }

    void go(void)
    {
#ifdef PTHREADS
#if !defined(ESP32)
        memset(canvas, 0, CSIZE);
#endif        
        if (clock_gettime(CLOCK_REALTIME, &tstart) < 0)
            perror("clock_gettime()");
            // log_msg("start at %ld.%06ld\n", tstart.tv_sec % 60, tstart.tv_nsec / 1000);
        for (int i = 0; i < NO_THREADS; i++)
        {
            // usleep(250 * 1000);
            V(tp[i]->go);
        }
        //log_msg("main thread waiting for %i threads...\n", NO_THREADS);
        for (int i = NO_THREADS; i; i--)
        {
            log_msg(2, "main thread waiting for %i threads...\n", i);
            PSem(master_sem); // wait until all workers have finished
        }
#endif

        if (clock_gettime(CLOCK_REALTIME, &tend) < 0)
            perror("clock_gettime()");
        //log_msg("end at %ld.%06ld\n", tend.tv_sec % 60, tend.tv_nsec / 1000);
        log_msg(2, "all threads finished.\n");
        free_ressources();
        stop = 0;
    }

    void free_ressources(void)
    {
        for (int i = 0; i < NO_THREADS; i++)
        {
#ifdef PTHREADS            
            int ret;
            if ((ret = pthread_attr_destroy(&attr[i])) != 0)
                log_msg("pthread_attr_destroy failed: %d\n", ret);
#endif
            delete tp[i];
        }
    }

#if defined(PTHREADS)
#if defined (MANDEL_MQ)
    inline int produce(point_t &p, mqd_t mq = (mqd_t) -1)
    {
        return mq_send(mq, (char *) &p, sizeof(point_t), 0);
    }

    inline int consume(point_t &p, mqd_t mq = (mqd_t) -1)
    {
         return mq_receive(mq, (char *)&p, sizeof(point_t), NULL);
    }
#endif
    inline int pcproduce(point_t &p)
    {
        PSem(pcempty);
        P(pcmutex);
        pcbuffer[pcin] = p;
        pcin = ((pcin+1) % PCBUFFER_SIZE);
        V(pcmutex);
        VSem(pcfull);
        return 0;
    }

    inline int pcconsume(point_t &p)
    {
        PSem(pcfull);
        P(pcmutex);
        p = pcbuffer[pcout];
        pcout = ((pcout+1) % PCBUFFER_SIZE);
        V(pcmutex);        
        VSem(pcempty);
        return 0;
    }

    static void *mandel_qwrapper(void *param)
    {
        tqparam_t *p = static_cast<tqparam_t *>(param);
        p->mo->mandel_qwrapper_2(param);
        pthread_exit(nullptr);
        return nullptr;
    }

    int mandel_qwrapper_2(void *param)
    {
        tqparam_t *p = (tqparam_t *)param;
#ifdef __linux__
        sched_param sp;
        int pol = -1;
        int ret;
        ret = pthread_getschedparam(pthread_self(), &pol, &sp);
        if (ret != 0)
            log_msg("pthread_getschedparam()... failed: %d\n", ret);
        sp.sched_priority = sp.sched_priority - 10;
        if ((ret = pthread_setschedparam(pthread_self(), SCHED_RR, &sp)) != 0)
            log_msg("pthread setschedparam (pol=%d) failed for thread %d, %d - need sudo!\n", SCHED_RR, p->tno, ret);
        pthread_getschedparam(pthread_self(), &pol, &sp);
        log_msg(2, "starting thread %d with priority %d\n", p->tno, sp.sched_priority);
#endif

        char *st; 
        size_t sts;
        volatile char c;
        if (pthread_attr_getstack(&attr[p->tno], (void **)&st, &sts) != 0)
            log_msg("%s:' pthread_attr_getstackaddr() failed %d\n", __FUNCTION__, errno);
        for (size_t i = 0; i < sts; i++)
        {
            c = st[i];
        }
        log_msg(2, "%s: thread %d's stack is at %p with size %d, %d\n", __FUNCTION__, p->tno, st, sts, c);
#if defined(MANDEL_MQ)
        mqd_t mq = (mqd_t) -1;
        if (do_mq == 1)
        {
            mq = mq_open(MQ_NAME, O_CREAT | O_RDONLY, 0644, &mqattr);
            if (mq == (mqd_t)-1)
                log_msg("%s: mq_open failed: %d\n", __FUNCTION__, errno);
        }
#endif        
        VSem(p->sem);
        ssize_t res;
        point_t point;
        myDOUBLE stepx = ssw * xratio;
        myDOUBLE stepy = ssh;
        int tcount = 0;

        while (1)
        {
            res = 
#if defined(MANDEL_MQ)            
            ((do_mq == 1) ? consume(point, mq) : pcconsume(point));
#else
            pcconsume(point);
#endif            
            //log_msg("%s: thread %d received: %d,%d\n", __FUNCTION__, p->tno, point.x, point.y);
            if (res < 0)
                log_msg("%s: mq_receive failed: %d\n", __FUNCTION__, errno);
            if (point.x < 0)
                break;
            tcount++;
            P(canvas_sem);
            if (stop || (stop = canvas_setpx(canvas, point.x, point.y, 
                                mandel_calc_point(std::complex<myDOUBLE>(point.x * stepx + transx, point.y * stepy + transy)))))
            {
                V(canvas_sem);
                break;
            }
            V(canvas_sem);                         
            //sched_yield();
        }
        log_msg(1, "%s: thread %d delivered %d results\n", __FUNCTION__, p->tno, tcount);
        VSem(p->sem);

#if defined(MANDEL_MQ)
        if (do_mq == 1)
            mq_close(mq);
#endif            
        return 0;
    }

    void mandel_mq(const int thread_no, myDOUBLE sx, myDOUBLE sy, myDOUBLE tx, myDOUBLE ty)
    {
        pthread_t th;
        int ret;
        mandel_presetup(sx, sy, tx, ty);
#if defined(MANDEL_MQ)        
        mqd_t mq = (mqd_t) -1;;

        mqattr.mq_flags = 0;
        mqattr.mq_maxmsg = PCBUFFER_SIZE;
        mqattr.mq_msgsize = sizeof(point_t);
        mqattr.mq_curmsgs = 0;

        if (do_mq == 1)
        {
            mq = mq_open(MQ_NAME, O_CREAT | O_RDWR, 0644, &mqattr);
            if (mq == (mqd_t)-1)
            {
                log_msg("%s: mq_open failed: %d\n", __FUNCTION__, errno);
                exit(1);
            }
        }
#endif        
#ifdef __linux__
        sched_param sp;
        int pol = -1;
        ret = pthread_getschedparam(pthread_self(), &pol, &sp);
        if (ret != 0)
            log_msg("pthread_getschedparam()... failed: %d\n", ret);
        sp.sched_priority = sp.sched_priority + 90;
        if ((ret = pthread_setschedparam(pthread_self(), SCHED_RR, &sp)) != 0)
            log_msg("pthread setschedparam (pol=%d) failed for main thread, %d - need sudo!\n", SCHED_RR, ret);
        pthread_getschedparam(pthread_self(), &pol, &sp);
        log_msg(2, "main thread with priority %d\n", sp.sched_priority);
#endif

        for (auto t = 0; t < thread_no; t++)
        {
            tpq[t] = new tqparam_t(t, master_sem, this);
            pthread_attr_init(&attr[t]);
#ifdef __ZEPHYR__            
            ret = pthread_attr_setstack(&attr[t], stacks + t * STACK_SIZE, STACK_SIZE);
            if (ret != 0)
                log_msg("setstack: %d - ssize = %d\n", ret, STACK_SIZE);
#endif                
            if ((ret = pthread_create(&th, &attr[t], mandel_qwrapper, tpq[t])) != 0)
                log_msg("pthread create failed for thread %d, %d\n", t, ret);
            if ((ret = pthread_detach(th)) != 0)
                log_msg("pthread_detach() failed for thread %d, %d\n", t, ret);
        }
        // wait for all threads to be launched
        for (auto t = thread_no; t != 0; t--)
            PSem(master_sem);
        log_msg(2, "%s: feeding calc...\n", __FUNCTION__);
        if (clock_gettime(CLOCK_REALTIME, &tstart) < 0)
            perror("clock_gettime()");       
        for (int x = 0; x < IMG_W / PIXELW; x++) {
            for (auto y = 0; y < IMG_H; y++) {
                point_t p{x, y};
                ret = 
#if defined(MANDEL_MQ)                
                ((do_mq == 1) ? produce(p, mq) : pcproduce(p));
#else
                pcproduce(p);                
#endif                
                
                //log_msg("%s: main thread produced: %d,%d\n", __FUNCTION__, p.x, p.y);
                if (ret < 0)
                    log_msg("%s: mq_send failed: %d\n", __FUNCTION__, errno);
            }
        }
        if (clock_gettime(CLOCK_REALTIME, &tend) < 0)
            perror("clock_gettime()");

        // send end-marker '-1' in xcoord.
        point_t pe{-1, -1};
        for(int i = 0; i < thread_no; i++)
            ret = 
#if defined(MANDEL_MQ)            
            ((do_mq == 1) ? produce(pe, mq) : pcproduce(pe));
#else
            pcproduce(pe);
#endif            

        // end-sync & cleanup
        for (auto t = thread_no; t != 0; t--)
            PSem(master_sem);
        for (auto t = 0; t < thread_no; t++)
        {
            pthread_attr_destroy(&attr[t]);
            delete tpq[t];
        }
        log_msg(2, "%s: done.\n", __FUNCTION__);

#if defined(MANDEL_MQ)
        if (do_mq == 1)
            mq_close(mq);
#endif            
    }
#endif /* PTHREADS */

void action(myDOUBLE xl, myDOUBLE yl, myDOUBLE xh, myDOUBLE yh)
{
#if 0	
    log_msg("hit enter to start...\n");
    char c1;
    read(0, &c1, 1);
#endif
#if defined(PTHREADS)
        if (do_mq)
        {
            // Initialize semaphores and mutex
            sem_init(&pcempty, 0, PCBUFFER_SIZE); // Initially, all slots are empty
            sem_init(&pcfull, 0, 0);              // Initially, no slots are full
            pcout = pcin = 0;
            pthread_mutex_init(&pcmutex, NULL);
            mandel_mq(NO_THREADS, xl, yl, xh, yh);
            sem_destroy(&pcempty);
            sem_destroy(&pcfull);
            int res;
            if ((res = pthread_mutex_destroy(&pcmutex)) != 0)
            {
                log_msg("%s: pthread_mutex_destroy failed: %d\n", __FUNCTION__, res);
            } 
        }
        else
#endif
        {
            mandel_setup(sqrt(NO_THREADS), xl, yl, xh, yh); // initialize some stuff, but don't calculate
            go();
        }
        dump_result();
}

public:
    mandel(canvas_t c, char *st, myDOUBLE xl, myDOUBLE yl, myDOUBLE xh, myDOUBLE yh, int xr, int yr, myDOUBLE xrat = 1.0)
        : canvas(c), stacks(st), xres(xr), yres(yr), xratio(xrat), stop(0)
    {
	    init_palette(col_pal);
#if defined(__linuxX__) || defined(LUCKFOX)
	int c1 = 0;
	for (int i = 0; i < xr - 2; i += 2, c1++)
	    luckfox_rect(c, i, 0, i+1, yr - 1 , col_pal[c1]);
    zoom_ui(this);
#endif	
        _mask = 1;
        for (int i = 1; i < PIXELW; i++)
        {
            _mask = (_mask << 1) | 1;
        }

        pthread_mutex_init(&canvas_sem, nullptr);
        sem_init(&master_sem, 0, 0);
        //action(xl, yl, xh, yh);
    }

    ~mandel()
    {
        log_msg("%s destructor\n", __FUNCTION__);
#if defined(PTHREADS)        
        pthread_mutex_destroy(&canvas_sem);
#endif        
        sem_destroy(&master_sem);
    };

    char *get_stacks(void) { return stacks; }
    canvas_t get_canvas(void) { return canvas; }
    void dump_result(void)
    {
#if !defined(C64) && !defined(__amiga__) && !defined(VIDEO_CAPTURE) && !defined(ESP32) && !defined(ESP8266) || defined(ESP32CONSOLE)
        canvas_dump(canvas);
#endif
        struct timespec dt;
        timespec_diff(&tend, &tstart, &dt);
        if (!zoom_mode) log_msg("mandelbrot set done in: %lld.%06lds\n", (long long int)dt.tv_sec, dt.tv_nsec / 1000L);
    }

    void select_start(point_t &p)
    {
        mark_x1 = p.x; // - ((LV_HOR_RES_MAX - IMG_W) / 2);
        mark_y1 = p.y; // - ((LV_VER_RES_MAX - IMG_H) / 2);
        if (mark_x1 < 0)
            mark_x1 = 0;
        if (mark_y1 < 0)
            mark_y1 = 0;
    }

    void select_end(point_t &p)
    {
        mark_x2 = p.x; // - ((LV_HOR_RES_MAX - IMG_W) / 2);
        mark_y2 = p.y; // - ((LV_VER_RES_MAX - IMG_H) / 2);
        if (mark_x2 < 0)
            mark_x2 = 0;
        if (mark_y2 < 0)
            mark_y2 = 0;
        // log_msg("rect coord: [%d,%d]x[%d,%d] - ssw=%d,ssh=%d,trx=%d,try=%d\n", mark_x1, mark_y1, mark_x2, mark_y2, ssw, ssh, transx, transy);
        //log_msg("rect coord: [%d,%d]x[%d,%d]\n", mark_x1, mark_y1, mark_x2, mark_y2);
        /*
                mandel_setup(sqrt(NO_THREADS),
                             static_cast<myDOUBLE>(mark_x1 * ssw + transx),
                             static_cast<myDOUBLE>(mark_y1 * ssh + transy),
                             static_cast<myDOUBLE>(mark_x2 * ssw + transx),
                             static_cast<myDOUBLE>(mark_y2 * ssh + transy));
                go();
                dump_result();
        */
        action(static_cast<myDOUBLE>(mark_x1 * ssw + transx),
               static_cast<myDOUBLE>(mark_y1 * ssh + transy),
               static_cast<myDOUBLE>(mark_x2 * ssw + transx),
               static_cast<myDOUBLE>(mark_y2 * ssh + transy));

        mark_x1 = -1;
        mark_x2 = mark_x1;
        mark_y2 = mark_y1;
    }

    void zoom(myDOUBLE xl, myDOUBLE yl, myDOUBLE xh, myDOUBLE yh)
    {
        action(xl, yl, xh, yh);
    }

    void mandel_presetup(myDOUBLE sx, myDOUBLE sy, myDOUBLE tx, myDOUBLE ty)
    {
        log_msg("\t{static_cast<MTYPE>(INTIFY(%.24f)), static_cast<MTYPE>(INTIFY(%.24f)), static_cast<MTYPE>(INTIFY(%.24f)), static_cast<MTYPE>(INTIFY(%.24f))},\n", sx, sy, tx, ty);
        
        last_xr = (tx - sx);
        last_yr = (ty - sy);
        ssw = last_xr / xres;
        ssh = last_yr / yres;
        transx = sx;
        transy = sy;
    }
#if 0
    void select_update(point_t &p)
    {
        if (mark_x1 < 0)
            return;
        coord_t tx = mark_x2;
        coord_t ty = mark_y2;
        mark_x2 = p.x; // - ((LV_HOR_RES_MAX - IMG_W) / 2);
        mark_y2 = p.y; // - ((LV_VER_RES_MAX - IMG_H) / 2);
        if (mark_x2 < 0)
            mark_x2 = 0;
        if (mark_y2 < 0)
            mark_y2 = 0;
        if ((tx == mark_x2) && (ty == mark_y2))
            return;
        //log_msg("rect coord: %dx%d - %dx%d\n", mark_x1, mark_y1, mark_x2, mark_y2);
    }
#endif
};

#endif
