#include "mandel-arch.h"

#ifdef __amiga__
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
extern "C"
{
    void run_setupAnimation(struct Window *w);
    void run_stepAnimation(void);
    struct GelsInfo *run_setupDisplay(struct Window *win,
                                      SHORT dbufing,
                                      struct BitMap **myBitMaps);
}
#ifdef PTHREADS
static pthread_mutex_t anim_ctrl;
#endif
struct Library *ConsoleDevice;
struct IOStdReq ioreq;
static struct Screen *myScreen;
static struct Window *myWindow;
static struct RastPort *rp;
static struct SimpleSprite sprite_ul = {0};
static struct SimpleSprite sprite_lr = {0};
static char title[24] = "Mandelbrot";
static bool animation = true;

#if 1
static struct NewScreen Screen1 = {
    0, 0, IMG_W, IMG_H + 20, SCRDEPTH, /* Screen of 640 x 480 of depth 8 (2^8 = 256 colours)    */
    DETAILPEN, BLOCKPEN,
    SCRMODE, /* see graphics/view.h for view modes */
    CUSTOMSCREEN | SCREENQUIET,    /* Screen types */
    NULL,            /* Text attributes (use defaults) */
    (char *)title,
    NULL,
    NULL};
#else
static struct NewScreen Screen1 = {
    0, 0, IMG_W, IMG_H + 20, 4, /* Screen of 640 x 480 of depth 8 (2^8 = 256 colours)    */
    DETAILPEN, BLOCKPEN,
    LACE | HIRES, /* see graphics/view.h for view modes */
    CUSTOMSCREEN| SCREENQUIET,    /* Screen types */
    NULL,            /* Text attributes (use defaults) */
    (char *)title,
    NULL,
    NULL};
#endif
static struct NewWindow param_dialog = {
    WINX-200-1, 30,
    200, 50,
    0, 1,
    IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY,
    WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_ACTIVATE,
    NULL, NULL,
    (char *)"Depth:",
    NULL, NULL,
    0, 0,
    WINX, WINY,
    CUSTOMSCREEN};

static struct NewWindow menu_window = {
    WINX-200-1, 10,
    200, 150,
    0, 1,
    IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY,
    WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_ACTIVATE | WFLG_CLOSEGADGET,
    NULL, NULL,
    (char *)"Menu:",
    NULL, NULL,
    0, 0,
    WINX, WINY,
    CUSTOMSCREEN};

/* real boring sprite data */
UWORD __chip sprite_data_ul[] = {
    0, 0,           /* position control           */
    0xffff, 0x0000, /* image data line 1, color 1 */
    0xffff, 0x0000, /* image data line 2, color 1 */
    0xC000, 0x0000, /* image data line 3, color 2 */
    0xC000, 0x0000, /* image data line 4, color 2 */
    0xC000, 0x0000, /* image data line 5, transparent */
    0xC000, 0x0000, /* image data line 6, color 2 */
    0xC000, 0x0000, /* image data line 7, color 2 */
    0xC000, 0x0000, /* image data line 8, color 3 */
    0xC000, 0x0000, /* image data line 9, color 3 */
    0xC000, 0x0000, /* image data line 10, color 3 */
    0xC000, 0x0000, /* image data line 11, color 3 */
    0xC000, 0x0000, /* image data line 12, color 3 */
    0xC000, 0x0000, /* image data line 13, color 3 */
    0xC000, 0x0000, /* image data line 14, color 3 */
    0xC000, 0x0000, /* image data line 15, color 3 */
    0xC000, 0x0000, /* image data line 16, color 3 */
    0, 0            /* reserved, must init to 0 0 */
};

UWORD __chip sprite_data_lr[] = {
    0, 0,           /* position control           */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0x0003, 0x0000, /* image data line 1, color 1 */
    0xffff, 0x0000, /* image data line 1, color 1 */
    0xffff, 0x0000, /* image data line 2, color 1 */
    0, 0            /* reserved, must init to 0 0 */
};

void sprite_setup(struct Screen *myScreen)
{
    WORD sprnum;
    SHORT color_reg;
    struct ViewPort *viewport;
    viewport = &myScreen->ViewPort;

    sprnum = GetSprite(&sprite_ul, 2);
    // printf("sprite ul num = %d\n", sprnum);
    /* Calculate the correct base color register number, */
    /* set up the color registers.                       */
    color_reg = 16 + ((sprnum & 0x06) << 1);
    // printf("color_reg=%d\n", color_reg);
    SetRGB4(viewport, color_reg + 1, 0xf, 0x0, 0x0);

    sprite_ul.x = 0;       /* initialize position and size info    */
    sprite_ul.y = 0;       /* to match that shown in sprite_data   */
    sprite_ul.height = 16; /* so system knows layout of data later */

    /* install sprite data and move sprite to start position. */
    ChangeSprite(NULL, &sprite_ul, sprite_data_ul);
    MoveSprite(NULL, &sprite_ul, -1, 20 / SCMOUSE);

    sprnum = GetSprite(&sprite_lr, 3);
    // printf("sprite lr num = %d\n", sprnum);
    /* Calculate the correct base color register number, */
    /* set up the color registers.                       */
    color_reg = 16 + ((sprnum & 0x06) << 1);
    // printf("color_reg=%d\n", color_reg);
    SetRGB4(viewport, color_reg + 1, 0xf, 0x0, 0x0);

    sprite_lr.x = 0;       /* initialize position and size info    */
    sprite_lr.y = 0;       /* to match that shown in sprite_data   */
    sprite_lr.height = 16; /* so system knows layout of data later */

    /* install sprite data and move sprite to start position. */
    ChangeSprite(NULL, &sprite_lr, sprite_data_lr);
    MoveSprite(NULL, &sprite_lr, WINX / SCMOUSE - 16, WINY / SCMOUSE + 4);
}

void *anim_thread(void *arg)
{
    log_msg("%s: starting animation thread...\n", __FUNCTION__);
    Delay(20);
    while(animation)
    {
        pthread_mutex_lock(&anim_ctrl);
        run_stepAnimation();
        pthread_mutex_unlock(&anim_ctrl);
        Delay(2);
    }
    pthread_mutex_unlock(&anim_ctrl);
    log_msg("%s: terminating animation thread.\n", __FUNCTION__);
    return NULL;
}

void amiga_setup_screen(void)
{
    myScreen = OpenScreen(&Screen1); /* & (ampersand) means address of */
    ScreenToFront(myScreen);
    ShowTitle(myScreen, TRUE);
    MakeScreen(myScreen);
    sprite_setup(myScreen);
    struct NewWindow winlayout = {
        0 * WINX / 4, 0 * WINY / 4 + 10,
        WINX, WINY + 10,
        0, 1,
        IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_MOUSEMOVE | IDCMP_RAWKEY,
        WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE,
        NULL, NULL,
        (char *)title,
        myScreen, NULL,
        0, 0,
        WINX, WINY,
        CUSTOMSCREEN};
    myWindow = OpenWindow(&winlayout);
    rp = myWindow->RPort;
    param_dialog.Screen = myScreen;
    menu_window.Screen = myScreen;

    if (0 == OpenDevice("console.device",-1,(struct IORequest *)&ioreq,0)) {
        ConsoleDevice = (struct Library *)ioreq.io_Device;  
    }
    run_setupDisplay(myWindow, 0, NULL);
    run_setupAnimation(myWindow);
#ifdef PTHREADS
    pthread_t ath;
    static pthread_attr_t pattr;
    static char *anim_stack[4096];
    pthread_mutex_init(&anim_ctrl, NULL);
    pthread_attr_init(&pattr);
    pthread_attr_setstack(&pattr, anim_stack, 4096);
    if (pthread_create(&ath, &pattr, anim_thread, NULL) != 0)
        log_msg("%s: couldn't start animation thread\n", __FUNCTION__);
    pthread_detach(ath);
#endif    
}

/* Convert RAWKEYs into VANILLAKEYs, also shows special keys like HELP, Cursor Keys,
** FKeys, etc.  It returns:
**   -2 if not a RAWKEY event.
**   -1 if not enough room in the buffer, try again with a bigger buffer.
**   otherwise, returns the number of characters placed in the buffer.
*/
LONG deadKeyConvert(struct IntuiMessage *msg, UBYTE *kbuffer,
                    LONG kbsize, struct KeyMap *kmap, struct InputEvent *ievent)
{
    if (msg->Class != IDCMP_RAWKEY)
        return (-2);
    ievent->ie_Class = IECLASS_RAWKEY;
    ievent->ie_Code = msg->Code;
    ievent->ie_Qualifier = msg->Qualifier;
    ievent->ie_position.ie_addr = *((APTR *)msg->IAddress);

    return (RawKeyConvert(ievent, (STRPTR)kbuffer, kbsize, kmap));
}

char fetch_key(struct IntuiMessage *pIMsg, struct Window *win)
{
    struct InputEvent *ievent;
    struct IntuiMessage *m;
    UBYTE buffer[8];
    char ret = '\0';

    ievent = (struct InputEvent *)AllocMem(sizeof(struct InputEvent), MEMF_CLEAR);
    if (ievent)
    {
        if (!(pIMsg->Code & 0x80))
        {
            deadKeyConvert(pIMsg, buffer, 7, NULL, ievent);
            ret = buffer[0];
            while (1) // wait for up-key event
            {
                WaitPort(win->UserPort);
                if ((m = (struct IntuiMessage *)GetMsg(win->UserPort)) != NULL)
                {
                    ReplyMsg((struct Message *) m);
                    if ((m->Class == IDCMP_RAWKEY) &&
                        (m->Code & 0x80))
                    break;
                }
            }
        }
        FreeMem(ievent, sizeof(struct InputEvent));
    }
    return ret;
}

int amiga_setpixel(void *not_used, int x, int y, int col)
{
    //    log_msg("%s: %dx%d->%d\n", __FUNCTION__, x, y, col);
    SetAPen(rp, col);
    WritePixel(rp, x, y + 10);
    //    Move(rp, x, y+10);
    //    Draw(rp, WINX, y + 10);
    struct Message *pMsg;
    int ret = 0;

    if ((pMsg = GetMsg(myWindow->UserPort)) != NULL)
    {
        struct IntuiMessage *pIMsg = (struct IntuiMessage *)pMsg;
        ReplyMsg(pMsg);
        switch (pIMsg->Class)
        {
        case IDCMP_MOUSEBUTTONS:
        case IDCMP_RAWKEY:
            ret = 1;
            while (1) // loop until button up is seen
            {
                pMsg = GetMsg(myWindow->UserPort);
                if (pMsg)
                {
                    ReplyMsg(pMsg);

                    pIMsg = (struct IntuiMessage *)pMsg;
                    if (pIMsg->Code == SELECTUP)
                        goto out;
                    if (pIMsg->Code & 0x80)
                        goto out;
                }
            }
            break;
        case IDCMP_CLOSEWINDOW:
            ret = 1;
            break;
        default:
            //log_msg("%s: class = %ld\n", __FUNCTION__, pIMsg->Class);
            break;
        }
    }
out:
    return ret;
}

int fetch_param(void)
{
    struct Window *paramd = OpenWindow(&param_dialog);
    bool closewin = FALSE;
    long data;
    int new_iter = -1;
    char buf[32];
    memset(buf, 0, 32);
    struct IntuiText reqtext = { 1, 0, JAM1, 0, 0, NULL, (STRPTR) "Depth: ", NULL};
    struct Requester req;
    struct StringInfo reqstringinfo = {
        &buf[0], NULL,
        0, 6,
        0, 
        0, 0, 0, 0, 0,
        NULL,
        iter,
        NULL};
    struct Gadget gad = {
        NULL,
        0, 0,
        180, 35,
        0,
        GACT_ENDGADGET | GACT_STRINGLEFT | GACT_LONGINT,
        GTYP_REQGADGET | GTYP_STRGADGET,
        NULL, NULL,
        NULL, /* text */
        0,
        &reqstringinfo,
        0,
        &data};

    sprintf(buf, "%d", MAX_ITER);
    InitRequester(&req);
    req.LeftEdge = 10;
    req.TopEdge = 30;
    req.Width = 200;
    req.Height = 40;
    req.ReqText = &reqtext;
    req.ReqGadget = &gad;
    up:     
    if (!Request(&req, paramd))
        log_msg("req failed.\n");
   
    while (closewin == FALSE) {
        WaitPort(paramd->UserPort);
        struct Message *pMsg;
        while ((pMsg = GetMsg(paramd->UserPort)) != NULL)
        {
            struct IntuiMessage *pIMsg = (struct IntuiMessage *)pMsg;
            //log_msg("%s: class = %ld\n", __FUNCTION__, pIMsg->Class);
            switch (pIMsg->Class)
            {
            case IDCMP_CLOSEWINDOW:
                closewin = TRUE;
                break;
            case IDCMP_RAWKEY:
                //log_msg("%s: RAWKEY event2: buf = %s\n", __FUNCTION__, buf);
                if (pIMsg->Code & 0x80) // only when key-up is seen.
                {
                    new_iter = strtol(buf, NULL, 10);
                    if ((errno != 0) ||
                        (new_iter < 16) || (new_iter > 1024))
                    {
                        DisplayBeep(myScreen);
                        log_msg("%s: iter out of range: %d\n", __FUNCTION__, new_iter);
                        goto up;
                    }
                    log_msg("%s: iter set to %d\n", __FUNCTION__, new_iter);
                    iter = new_iter;
                    closewin = TRUE;
                }
                break;
            default:
                break;
            }
            ReplyMsg(pMsg);
        }
    }
    EndRequest(&req, paramd);
    CloseWindow(paramd);
    return 0;
}


int show_menu(void)
{
    struct Window *menu = OpenWindow(&menu_window);
    bool closewin = FALSE;
    int ret = 1;
    struct IntuiText t = {
        1, 0, JAM1, 5, 5, NULL, 
        NULL, NULL};
    std::vector<STRPTR> menu_str = {(STRPTR)"i...Iteration", (STRPTR)"r...Recalc", (STRPTR)"s...Restart", (STRPTR)"q...Quit"};
    int j = 0;
    for (auto i : menu_str) {
        t.IText = i;
        PrintIText(menu->RPort, &t, 1, 15 + j);
        j += 12;
    }

    while (closewin == FALSE)
    {
        WaitPort(menu->UserPort);
        struct Message *pMsg;
        while ((pMsg = GetMsg(menu->UserPort)) != NULL)
        {
            struct IntuiMessage *pIMsg = (struct IntuiMessage *)pMsg;
            // log_msg("%s: class = %ld\n", __FUNCTION__, pIMsg->Class);
            ReplyMsg(pMsg);
            switch (pIMsg->Class)
            {
            case IDCMP_CLOSEWINDOW:
                closewin = TRUE;
                break;
            case IDCMP_RAWKEY:
                if (!(pIMsg->Code & 0x80))
                {
                    switch (fetch_key(pIMsg, menu))
                    {
                    case 'q':
                        ret = 0;
                        closewin = TRUE;
                        break;
                    case 'i':
                        fetch_param();
                        closewin = FALSE; // keep menu to decide
                        break;
                    case 'r':
                        ret = 2;
                        closewin = TRUE;
                        break;
                    case 's':
                        ret = 3;
                    default:
                        closewin = TRUE;
                        break;
                    }
                }
            default:
                break;
            }
        }
    }
    CloseWindow(menu);
    return ret;
}

void amiga_zoom_ui(mandel<MTYPE> *m)
{
    uint16_t stx = 0, sty = 10;
    bool closewin = FALSE;
    while (closewin == FALSE)
    {
#ifndef PTHREADS
        run_stepAnimation(); // FIXME!
#else
        WaitPort(myWindow->UserPort);
#endif
        struct Message *pMsg;
        while ((pMsg = GetMsg(myWindow->UserPort)) != NULL)
        {
            struct IntuiMessage *pIMsg = (struct IntuiMessage *)pMsg;
            ReplyMsg(pMsg);
            switch (pIMsg->Class)
            {
            case IDCMP_CLOSEWINDOW:
                closewin = TRUE;
                break;
            case IDCMP_MOUSEMOVE:
                if (pIMsg->MouseX >= WINX)
                    break;
                if (pIMsg->MouseY >= WINY + 10)
                    break;
                if (pIMsg->MouseY < 10)
                    break;
                MoveSprite(NULL, &sprite_lr, pIMsg->MouseX / SCMOUSE - 16, pIMsg->MouseY / SCMOUSE - 4 * SCMOUSE);  // weird
#if 0		
		SetDrMd(rp, COMPLEMENT);
		SetAPen(rp, 0);
		RectFill(rp, stx, sty, pIMsg->MouseX, pIMsg->MouseY);
		SetDrMd(rp, JAM1);
#endif
                if ((stx < pIMsg->MouseX) &&
                    (sty < pIMsg->MouseY))
                    break;
                if (stx >= pIMsg->MouseX)
                    stx = pIMsg->MouseX;
                if (sty >= pIMsg->MouseY)
                    sty = pIMsg->MouseY;
                MoveSprite(NULL, &sprite_ul, (stx - 1) / SCMOUSE, (sty + 10) / SCMOUSE);
                break;
            case IDCMP_MOUSEBUTTONS:
                if (pIMsg->Code == SELECTDOWN)
                {
                    stx = pIMsg->MouseX;
                    sty = pIMsg->MouseY;
                    // log_msg("mouse select start: (%d,%d)\n", stx, sty);
                    ReportMouse(TRUE, myWindow);
                    MoveSprite(NULL, &sprite_ul, (pIMsg->MouseX - 1) / SCMOUSE, (pIMsg->MouseY + 10) / SCMOUSE);
                }
                if (pIMsg->Code == SELECTUP)
                {
                    ReportMouse(FALSE, myWindow);
                    MoveSprite(NULL, &sprite_ul, (-1 / SCMOUSE), 20 / SCMOUSE);
                    MoveSprite(NULL, &sprite_lr, (WINX - 16) / SCMOUSE, (WINY + 4) / SCMOUSE);
                    if ((stx == pIMsg->MouseX) ||
                        (sty == pIMsg->MouseY) ||
                        (sty < 10) ||
                        (pIMsg->MouseX > WINX) ||
                        (pIMsg->MouseY > WINY))
                    {
                        // log_msg("stx=%d, sty=%d, Mx=%d, My=%d\n", stx, sty, pIMsg->MouseX, pIMsg->MouseY);
                        DisplayBeep(myScreen);
                        break;
                    }
                    point_t lu{stx, (uint16_t)(sty - 10)}, rd{(uint16_t)pIMsg->MouseX, (uint16_t)(pIMsg->MouseY - 10)};
                    m->select_start(lu);
                    m->select_end(rd);
                    DisplayBeep(myScreen);
                    ReportMouse(FALSE, myWindow);
                }
                break;
            case IDCMP_RAWKEY:
                if (!(pIMsg->Code & 0x80))
                {
                    switch (show_menu())
                    {
                    case 0:
                        closewin = TRUE;
                        break;
                    case 3:
                        m->mandel_presetup(static_cast<MTYPE>(INTIFY(-1.5)), static_cast<MTYPE>(INTIFY(-1.0)),
                                           static_cast<MTYPE>(INTIFY(0.5)), static_cast<MTYPE>(INTIFY(1.0)));
                    case 2:
                    {
                        point_t lu{0, 0}, rd{WINX, WINY};
                        m->select_start(lu);
                        m->select_end(rd);
                        DisplayBeep(myScreen);
                        ReportMouse(FALSE, myWindow);
                    }
                    break;
                    }
                }
                break;
            }
        }
    }
#ifdef PTHREADS
    //pthread_mutex_lock(&anim_ctrl);
    animation = false;
    //pthread_mutex_unlock(&anim_ctrl);
    Delay(10);
    pthread_mutex_lock(&anim_ctrl);
#endif    
    CloseWindow(myWindow);
    if (myScreen)
        CloseScreen(myScreen); /* Close screen using myScreen pointer */
}

#else
#define setup_screen()
#endif // __amiga__
