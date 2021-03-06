/*
** $Id: bitmap.h 7957 2007-10-25 04:37:28Z weiym $
**
** readbmp.h: Low Level bitmap file read/save routines.
**
** Copyright (C) 2003 ~ 2007 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Create date: 2001/xx/xx
*/

#ifndef GUI_GDI_BITMAP_H
    #define GUI_GDI_BITMAP_H

#include "endianrw.h"
#include "gal.h"
#include "cliprect.h"
#include "internals.h"
#include "dc.h"

struct _SCALER_INFO_FILLBMP
{
    PDC pdc;
    RECT dst_rc;
    BYTE* line_buff;
    const BITMAP* bmp;
};

typedef struct {
    struct _SCALER_INFO_FILLBMP scaler_info;
    GAL_Rect dst_rect;
    int old_bkmode;

    BYTE* decoded_buff;
    const BYTE* encoded_bits;
} FILLINFO;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int bmpComputePitch (int bpp, Uint32 width, Uint32 *pitch, BOOL does_round);

PDC _begin_fill_bitmap (HDC hdc, int x, int y, int w, int h,
                const BITMAP* bmp, FILLINFO *fill_info);
void _fill_bitmap_scanline (PDC pdc, const BITMAP* bmp, 
                FILLINFO *fill_info, int y);
void _end_fill_bitmap (PDC pdc, const BITMAP* bmp, FILLINFO *fill_info);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_GDI_BITMAP_H

