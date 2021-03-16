/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** nes.h
**
** NES hardware related definitions / prototypes
** $Id: nes.h,v 1.2 2001/04/27 14:37:11 neil Exp $
*/

#ifndef _NES_H_
#define _NES_H_

#include <nofrendo.h>
#include "apu.h"
#include "cpu.h"
#include "ppu.h"
#include "mmc.h"
#include "mem.h"
#include "rom.h"

#define NES_SCREEN_WIDTH      (256)
#define NES_SCREEN_HEIGHT     (240)
#define NES_SCREEN_OVERDRAW   (8)
#define NES_SCREEN_PITCH      (8 + 256 + 8)

#define NES_SCREEN_GETPTR(buf, x, y)     ((buf) + ((y) * NES_SCREEN_PITCH) + (x) + NES_SCREEN_OVERDRAW)

#define NES_CPU_CLOCK_NTSC      (1789772.72727)
#define NES_CPU_CLOCK_PAL       (1662607.03125)
#define NES_REFRESH_RATE_NTSC   (60)
#define NES_REFRESH_RATE_PAL    (50)
#define NES_SCANLINES_NTSC      (262)
#define NES_SCANLINES_PAL       (312)

#define NES_REFRESH_RATE        (nes_getptr()->refresh_rate)
#define NES_SCANLINES           (nes_getptr()->scanlines_per_frame)
#define NES_CPU_CLOCK           (nes_getptr()->cpu_clock)
#define NES_CYCLES_PER_SCANLINE (nes_getptr()->cycles_per_scanline)
#define NES_CURRENT_SCANLINE    (nes_getptr()->scanline)

typedef enum
{
    NES_AUTO,
    NES_NTSC,
    NES_PAL
} region_t;

typedef void (nes_timer_t)(int cycles);

typedef struct
{
    /* Hardware */
    nes6502_t *cpu;
    ppu_t *ppu;
    apu_t *apu;
    mem_t *mem;
    map_t *mapper;
    rom_t *cart;

    /* Video buffer */
    uint8 *framebuffers[2];
    uint8 *vidbuf;

    /* Misc */
    region_t region;
    int overscan;

    /* Timing constants */
    /* TO DO: re-check if float is still necessary...*/
    int refresh_rate;
    int scanlines_per_frame;
    float cycles_per_scanline;
    float cpu_clock;

    /* Timing counters */
    int scanline;
    float cycles;

    /* Periodic timer */
    nes_timer_t *timer_func;
    long timer_period;

    /* Control */
    bool autoframeskip;
    bool poweroff;
    bool pause;
    bool drawframe;

} nes_t;

extern nes_t *nes_getptr(void);
extern bool nes_init(region_t region, int sample_rate, bool stereo);
extern void nes_shutdown(void);
extern void nes_setregion(region_t region);
extern bool nes_insertcart(const char *filename);
extern bool nes_insertdisk(const char *filename);
extern void nes_settimer(nes_timer_t *func, long period);
extern void nes_emulate(void);
extern void nes_reset(bool hard_reset);
extern void nes_poweroff(void);
extern void nes_togglepause(void);

#endif /* _NES_H_ */
