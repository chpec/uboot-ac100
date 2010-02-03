/*
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
 * 
 * (C) Copyright 2003
 * Texas Instruments <www.ti.com>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002-2004
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * (C) Copyright 2004
 * Philippe Robin, ARM Ltd. <philippe.robin@arm.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>


#define TIMER_LOAD_VAL 0xffffffff

/* macro to read the 32 bit timer */
#define READ_TIMER    IO_READ32(AGPT_COUNT_VAL)

static ulong timestamp;
static ulong lastinc;

/* nothing really to do with interrupts, just starts up a counter. */
int timer_init (void)
{
	/* Stop the timer if running */
	IO_WRITE32(AGPT_ENABLE, 0x00000000);
	/* Clear the timer */
	IO_WRITE32(AGPT_CLEAR, 0x00000001);
	/* Load timer with initial value */
	IO_WRITE32(AGPT_MATCH_VAL, TIMER_LOAD_VAL);
	/* Enable timer */
	IO_WRITE32(AGPT_ENABLE, (AGPT_ENABLE__EN___M | AGPT_ENABLE__CLR_ON_MATCH_EN___M));

	/* init the timestamp and lastinc value */
	reset_timer_masked();

	return 0;
}

/*
 * GPT timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

/* delay x useconds AND perserve advance timstamp value */
void udelay (unsigned long usec)
{
	ulong tmo, tmp;

	if(usec >= 1000){               /* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;      /* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;   /* find number of "ticks" to wait to achieve target */
		tmo /= 1000;            /* finish normalize. */
	}else{                          /* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000*1000);
	}

    if (tmo == 0)               /* Minimum 1 timer tick if timer resolution is low */
        tmo = 1;

	tmp = get_timer (0);		/* get current timestamp */
	if( (tmo + tmp + 1) < tmp ) 	/* if setting this fordward will roll time stamp */
		reset_timer_masked ();	/* reset "advancing" timestamp to 0, set lastinc value */
	else
		tmo += tmp;		/* else, set advancing stamp wake up time */

	while (get_timer_masked () < tmo)/* loop till event */
		/*NOP*/;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastinc = READ_TIMER;  /* capure current decrementer value time */
	timestamp = 0;         /* start "advancing" time stamp from 0 */
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER;		/* current tick value */

	if (lastinc <= now) {		/* normal mode (non roll) */
		/* normal mode */
		timestamp += now - lastinc; /* move stamp forward with absolute diff ticks */
	} else {			/* we have overflow of the count down timer */
		timestamp += now + (TIMER_LOAD_VAL - lastinc);
	}
	lastinc = now;

	return timestamp;
}

/* waits specified delay value and resets timestamp */
void udelay_masked (unsigned long usec)
{
	ulong tmo;

	if(usec >= 1000){               /* if "big" number, spread normalization to seconds */
		tmo = usec / 1000;      /* start to normalize for usec to ticks per sec */
		tmo *= CONFIG_SYS_HZ;   /* find number of "ticks" to wait to achieve target */
		tmo /= 1000;            /* finish normalize. */
	}else{                          /* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ;
		tmo /= (1000*1000);
	}

	reset_timer_masked ();	/* set "advancing" timestamp to 0, set lastinc value */

	while (get_timer_masked () < tmo) /* wait for time stamp to overtake tick number.*/
		/*NOP*/;
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CONFIG_SYS_HZ;
	return tbclk;
}
