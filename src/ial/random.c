/*
** $Id: random.c 7335 2007-08-16 03:38:27Z xgwang $
**
** random.c: A random IAL Engine
** 
** Copyright (C) 2005 ~ 2007 Feynman Software
**
** Created by Wei Yongming, 2005/06/08
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _RANDOM_IAL

#include "minigui.h"
#include "misc.h"
#include "ial.h"
#include "random.h"

static unsigned char kbd_state [NR_KEYS];

typedef struct {
    DWORD buttons;
    int x;
    int y;
} MOUSE_INFO;

static MOUSE_INFO mouse_info;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = mouse_info.x;
    *y = mouse_info.y;
}

static int mouse_getbutton (void)
{
    return mouse_info.buttons;
}

static int keyboard_update (void)
{
    return NR_KEYS;
}

static const char* keyboard_getstate (void)
{
    return (const char*) kbd_state;
}

#define EVENT_TYPE_LD   0
#define EVENT_TYPE_LU   1

#define EVENT_TYPE_RD   2
#define EVENT_TYPE_RU   3

#define EVENT_TYPE_MOVE 4

#define EVENT_TYPE_KD   5
#define EVENT_TYPE_KU   6

static int event_types_kbd [] =
{2, EVENT_TYPE_KD, EVENT_TYPE_KU};
static int event_types_pen [] =
{3, EVENT_TYPE_LD, EVENT_TYPE_LU, EVENT_TYPE_MOVE};
static int event_types_mice [] =
{5, EVENT_TYPE_LD, EVENT_TYPE_LU, EVENT_TYPE_RD, EVENT_TYPE_RU, EVENT_TYPE_MOVE};
static int event_types_pen_kbd [] =
{5, EVENT_TYPE_LD, EVENT_TYPE_LU, EVENT_TYPE_MOVE, EVENT_TYPE_KD, EVENT_TYPE_KU};
static int event_types_mice_kbd [] =
{7, EVENT_TYPE_LD, EVENT_TYPE_LU, EVENT_TYPE_RD, EVENT_TYPE_RU, EVENT_TYPE_MOVE, EVENT_TYPE_KD, EVENT_TYPE_KU};

/* defined by mtype */
static int* event_types;

static int event_generator (void)
{
    int event_type;
    int retvalue;
    static int last_down_key;
    
    while (1) {

        retvalue = 0;
        event_type = event_types [(rand () >> 8) % event_types [0] + 1];

        if (event_type == EVENT_TYPE_LD) {
            if (mouse_info.buttons & IAL_MOUSE_LEFTBUTTON)
                continue;
            mouse_info.buttons |= IAL_MOUSE_LEFTBUTTON;
            retvalue = IAL_MOUSEEVENT;
        }
        else if (event_type == EVENT_TYPE_LU) {
            if (!(mouse_info.buttons & IAL_MOUSE_LEFTBUTTON))
                continue;
            mouse_info.buttons &= ~IAL_MOUSE_LEFTBUTTON;
            retvalue = IAL_MOUSEEVENT;
        }
        else if (event_type == EVENT_TYPE_RD) {
            if (mouse_info.buttons & IAL_MOUSE_RIGHTBUTTON)
                continue;
            mouse_info.buttons |= IAL_MOUSE_RIGHTBUTTON;
            retvalue = IAL_MOUSEEVENT;
        }
        else if (event_type == EVENT_TYPE_RU) {
            if (!(mouse_info.buttons & IAL_MOUSE_RIGHTBUTTON))
                continue;
            mouse_info.buttons &= ~IAL_MOUSE_RIGHTBUTTON;
            retvalue = IAL_MOUSEEVENT;
        }
        else if (event_type == EVENT_TYPE_MOVE) {
            mouse_info.x = (rand () >> 8) % g_rcScr.right;
            mouse_info.y = (rand () >> 8) % g_rcScr.bottom;
            retvalue = IAL_MOUSEEVENT;
        }
        else if (event_type == EVENT_TYPE_KD) {
            int key = ((rand () >> 8) % (NR_KEYS - 1)) + 1;

            if (kbd_state [key]) {
                if (rand () & 0xFFFFF000) {
                    kbd_state [key] = 0;
                    retvalue = IAL_KEYEVENT;
                }
            }
            else {
                kbd_state [key] = 1;
                last_down_key = key;
                retvalue = IAL_KEYEVENT;
            }
        }
        else if (event_type == EVENT_TYPE_KU) {
            if (last_down_key && kbd_state [last_down_key]) {
                kbd_state [last_down_key] = 0;
                last_down_key = 0;
                retvalue = IAL_KEYEVENT;
            }
        }

        if (retvalue)
            break;
    }

    return retvalue;
}

#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int e;

    e = select (maxfd + 1, in, out, except, timeout);
    if (e < 0)
        return -1;

    return event_generator ();
}

#else

static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    __mg_os_time_delay (300);

    return event_generator ();
}

#endif

BOOL InitRandomInput (INPUT* input, const char* mdev, const char* mtype)
{
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    event_types = event_types_mice_kbd;
    if (mtype) {
        if (strcmp (mtype, "mice") == 0)
            event_types = event_types_mice;
        else if (strcmp (mtype, "pen") == 0)
            event_types = event_types_pen;
        else if (strcmp (mtype, "pen-kbd") == 0)
            event_types = event_types_pen_kbd;
        else if (strcmp (mtype, "kbd") == 0)
            event_types = event_types_kbd;
    }

    srand (__mg_os_get_random_seed ());
    return TRUE;
}

void TermRandomInput (void)
{
}

#endif /* _RANDOM_IAL */

