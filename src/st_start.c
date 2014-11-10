// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2008 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.
//
//-----------------------------------------------------------------------------



// HEADER FILES ------------------------------------------------------------
/*
#include <pspkernel.h>
#include <psprtc.h>
#include <pspdebug.h>
*/
#include <stdarg.h>

#include "config.h"

#include "h2def.h"
#include "i_system.h"
#include "i_video.h"
#include "i_videohr.h"
#include "s_sound.h"
#include "st_start.h"
#include "v_video.h"

// MACROS ------------------------------------------------------------------

//#define printf pspDebugScreenPrintf

#define ST_MAX_NOTCHES		32
#define ST_NOTCH_WIDTH		16
#define ST_NOTCH_HEIGHT		23
#define ST_PROGRESS_X		64      // Start of notches x screen pos.
#define ST_PROGRESS_Y		441     // Start of notches y screen pos.

#define ST_NETPROGRESS_X		288
#define ST_NETPROGRESS_Y		32
#define ST_NETNOTCH_WIDTH		8
#define ST_NETNOTCH_HEIGHT		16
#define ST_MAX_NETNOTCHES		8

extern boolean use_alternate_startup;

byte *ST_LoadScreen(void);
void ST_UpdateNotches(int notchPosition);
void ST_UpdateNetNotches(int notchPosition);

static int notchPosition = 0;

patch_t *logo;
patch_t *logomac;
/*
u32 stResolution;
u64 stTickNow;
u64 stTickLast;
*/
// PRIVATE DATA DEFINITIONS ------------------------------------------------
static const byte *bitmap = NULL;
int graphical_startup = 1;
static boolean using_graphical_startup;

static const byte notchTable[] = {
    // plane 0
    0x00, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40,
    0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x03, 0xC0,
    0x0F, 0x90, 0x1B, 0x68, 0x3D, 0xBC, 0x3F, 0xFC, 0x20, 0x08, 0x20, 0x08,
    0x2F, 0xD8, 0x37, 0xD8, 0x37, 0xF8, 0x1F, 0xF8, 0x1C, 0x50,

    // plane 1
    0x00, 0x80, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x02, 0x40, 0x02, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0xA0,
    0x30, 0x6C, 0x24, 0x94, 0x42, 0x4A, 0x60, 0x0E, 0x60, 0x06, 0x7F, 0xF6,
    0x7F, 0xF6, 0x7F, 0xF6, 0x5E, 0xF6, 0x38, 0x16, 0x23, 0xAC,

    // plane 2
    0x00, 0x80, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00, 0x02, 0x40, 0x02, 0x40,
    0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x02, 0x40, 0x03, 0xE0,
    0x30, 0x6C, 0x24, 0x94, 0x52, 0x6A, 0x7F, 0xFE, 0x60, 0x0E, 0x60, 0x0E,
    0x6F, 0xD6, 0x77, 0xD6, 0x56, 0xF6, 0x38, 0x36, 0x23, 0xAC,

    // plane 3
    0x00, 0x00, 0x00, 0x00, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80, 0x01, 0x80,
    0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0x80, 0x02, 0x40,
    0x0F, 0x90, 0x1B, 0x68, 0x3D, 0xB4, 0x1F, 0xF0, 0x1F, 0xF8, 0x1F, 0xF8,
    0x10, 0x28, 0x08, 0x28, 0x29, 0x08, 0x07, 0xE8, 0x1C, 0x50
};


// Red Network Progress notches
static const byte netnotchTable[] = {
    // plane 0
    0x80, 0x50, 0xD0, 0xf0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xD0, 0xF0, 0xC0,
    0x70, 0x50, 0x80, 0x60,

    // plane 1
    0x60, 0xE0, 0xE0, 0xA0, 0xA0, 0xA0, 0xE0, 0xA0, 0xA0, 0xA0, 0xE0, 0xA0,
    0xA0, 0xE0, 0x60, 0x00,

    // plane 2
    0x80, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00,
    0x10, 0x10, 0x80, 0x60,

    // plane 3
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

// CODE --------------------------------------------------------------------



//--------------------------------------------------------------------------
//
// Startup Screen Functions
//
//--------------------------------------------------------------------------


//==========================================================================
//
// ST_Init - Do the startup screen
//
//==========================================================================

void ST_Init(void)
{
    if(!use_alternate_startup)
    {
	byte *pal;
	byte *buffer;
    
	using_graphical_startup = false;

	if (graphical_startup && !debugmode /*&& !testcontrols*/)
	{
//	    I_SetWindowTitleHR("Hexen startup - " PACKAGE_STRING);

	    // Set 640x480x16 mode
	    if (I_SetVideoModeHR())
	    {
		using_graphical_startup = true;
//		I_InitWindowIcon();

		if(!debugmode && (HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACDEMO || HEXEN_MACFULL))
		    S_StartSongName("orb", true);

            	I_ClearScreenHR();
            	I_InitPaletteHR();
            	I_BlackPaletteHR();

            	// Load graphic
            	buffer = ST_LoadScreen();
            	pal = buffer;
            	bitmap = buffer + 16 * 3;

            	I_SlamHR(bitmap);
            	I_FadeToPaletteHR(pal);
            	Z_Free(buffer);
	    }
        }
    }
}

void ST_Done(void)
{
    if (using_graphical_startup)
    {
        I_ClearScreenHR();
        I_UnsetVideoModeHR();
    }
}


//==========================================================================
//
// ST_UpdateNotches
//
//==========================================================================

void ST_UpdateNotches(int notchPosition)
{
    int x = ST_PROGRESS_X + notchPosition * ST_NOTCH_WIDTH;
    int y = ST_PROGRESS_Y;
    I_SlamBlockHR(x, y, ST_NOTCH_WIDTH, ST_NOTCH_HEIGHT, notchTable);
}


//==========================================================================
//
// ST_UpdateNetNotches - indicates network progress
//
//==========================================================================

void ST_UpdateNetNotches(int notchPosition)
{
    int x = ST_NETPROGRESS_X + notchPosition * ST_NETNOTCH_WIDTH;
    int y = ST_NETPROGRESS_Y;
    I_SlamBlockHR(x, y, ST_NETNOTCH_WIDTH, ST_NETNOTCH_HEIGHT, netnotchTable);
}


//==========================================================================
//
// ST_Progress - increments progress indicator
//
//==========================================================================

void ST_Progress(void)
{
    // Check for ESC press -- during startup all events eaten here
/*
    if (I_CheckAbortHR())
    {
        I_Quit();
    }
*/
    if (using_graphical_startup)
    {
        static int notchPosition = 0;

        if (notchPosition < ST_MAX_NOTCHES)
        {
            ST_UpdateNotches(notchPosition);
            S_StartSound(NULL, SFX_STARTUP_TICK);
            //I_Sleep(1000);
            notchPosition++;
        }
    }

    if(debugmode)
	printf(".");
}

void ST_Progress_Alternate(void)
{
    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));

    if(!debugmode)
	S_StartSound(NULL, SFX_STARTUP_TICK);

    if(HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACDEMO || HEXEN_MACFULL)
    {
    	if(notchPosition == 0)
	    logo = W_CacheLumpName("START001", PU_CACHE);

    	if(notchPosition == 1)
	    logo = W_CacheLumpName("START002", PU_CACHE);

    	if(notchPosition == 2)
	    logo = W_CacheLumpName("START003", PU_CACHE);

    	if(notchPosition == 3)
	    logo = W_CacheLumpName("START004", PU_CACHE);

    	if(notchPosition == 4)
	    logo = W_CacheLumpName("START005", PU_CACHE);

    	if(notchPosition == 5)
	    logo = W_CacheLumpName("START006", PU_CACHE);

    	if(notchPosition == 6)
	    logo = W_CacheLumpName("START007", PU_CACHE);

    	if(notchPosition == 7)
	    logo = W_CacheLumpName("START008", PU_CACHE);

    	if(notchPosition == 8)
	    logo = W_CacheLumpName("START009", PU_CACHE);

    	if(notchPosition == 9)
	    logo = W_CacheLumpName("START010", PU_CACHE);

    	if(notchPosition == 10)
	    logo = W_CacheLumpName("START011", PU_CACHE);

    	if(notchPosition == 11)
	    logo = W_CacheLumpName("START012", PU_CACHE);

    	if(notchPosition == 12)
	    logo = W_CacheLumpName("START013", PU_CACHE);

    	if(notchPosition == 13)
	    logo = W_CacheLumpName("START014", PU_CACHE);

    	if(notchPosition == 14)
	    logo = W_CacheLumpName("START015", PU_CACHE);

    	if(notchPosition == 15)
	    logo = W_CacheLumpName("START016", PU_CACHE);

    	if(notchPosition == 16)
	    logo = W_CacheLumpName("START017", PU_CACHE);

    	if(notchPosition == 17)
	    logo = W_CacheLumpName("START018", PU_CACHE);

    	if(notchPosition == 18)
	    logo = W_CacheLumpName("START019", PU_CACHE);

    	if(notchPosition == 19)
	    logo = W_CacheLumpName("START020", PU_CACHE);

    	if(notchPosition == 20)
	    logo = W_CacheLumpName("START021", PU_CACHE);

    	if(notchPosition == 21)
	    logo = W_CacheLumpName("START022", PU_CACHE);

    	if(notchPosition == 22)
	    logo = W_CacheLumpName("START023", PU_CACHE);

    	if(notchPosition == 23)
	    logo = W_CacheLumpName("START024", PU_CACHE);

    	if(notchPosition == 24)
	    logo = W_CacheLumpName("START025", PU_CACHE);

    	if(notchPosition == 25)
	    logo = W_CacheLumpName("START026", PU_CACHE);

    	if(notchPosition == 26)
	    logo = W_CacheLumpName("START027", PU_CACHE);

    	if(notchPosition == 27)
	    logo = W_CacheLumpName("START028", PU_CACHE);

    	if(notchPosition == 28)
	    logo = W_CacheLumpName("START029", PU_CACHE);

    	if(notchPosition == 29)
	    logo = W_CacheLumpName("START030", PU_CACHE);

    	if(notchPosition == 30)
	    logo = W_CacheLumpName("START031", PU_CACHE);

    	if(notchPosition == 31)
	    logo = W_CacheLumpName("START032", PU_CACHE);

    	if(notchPosition == 32)
	    logo = W_CacheLumpName("START033", PU_CACHE);

	if(notchPosition == 33)
	    logo = W_CacheLumpName("START033", PU_CACHE);
    }
    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
    {
    	if(notchPosition == 0)
	    logo = W_CacheLumpName("STARTB01", PU_CACHE);

    	if(notchPosition == 1)
	    logo = W_CacheLumpName("STARTB02", PU_CACHE);

    	if(notchPosition == 2)
	    logo = W_CacheLumpName("STARTB03", PU_CACHE);

    	if(notchPosition == 3)
	    logo = W_CacheLumpName("STARTB04", PU_CACHE);

    	if(notchPosition == 4)
	    logo = W_CacheLumpName("STARTB05", PU_CACHE);

    	if(notchPosition == 5)
	    logo = W_CacheLumpName("STARTB06", PU_CACHE);

    	if(notchPosition == 6)
	    logo = W_CacheLumpName("STARTB07", PU_CACHE);

    	if(notchPosition == 7)
	    logo = W_CacheLumpName("STARTB08", PU_CACHE);

    	if(notchPosition == 8)
	    logo = W_CacheLumpName("STARTB09", PU_CACHE);

    	if(notchPosition == 9)
	    logo = W_CacheLumpName("STARTB10", PU_CACHE);

    	if(notchPosition == 10)
	    logo = W_CacheLumpName("STARTB11", PU_CACHE);

    	if(notchPosition == 11)
	    logo = W_CacheLumpName("STARTB12", PU_CACHE);

    	if(notchPosition == 12)
	    logo = W_CacheLumpName("STARTB13", PU_CACHE);

    	if(notchPosition == 13)
	    logo = W_CacheLumpName("STARTB14", PU_CACHE);

    	if(notchPosition == 14)
	    logo = W_CacheLumpName("STARTB15", PU_CACHE);

    	if(notchPosition == 15)
	    logo = W_CacheLumpName("STARTB16", PU_CACHE);

    	if(notchPosition == 16)
	    logo = W_CacheLumpName("STARTB17", PU_CACHE);

    	if(notchPosition == 17)
	    logo = W_CacheLumpName("STARTB18", PU_CACHE);

    	if(notchPosition == 18)
	    logo = W_CacheLumpName("STARTB19", PU_CACHE);

    	if(notchPosition == 19)
	    logo = W_CacheLumpName("STARTB20", PU_CACHE);

    	if(notchPosition == 20)
	    logo = W_CacheLumpName("STARTB21", PU_CACHE);

    	if(notchPosition == 21)
	    logo = W_CacheLumpName("STARTB22", PU_CACHE);

    	if(notchPosition == 22)
	    logo = W_CacheLumpName("STARTB23", PU_CACHE);

    	if(notchPosition == 23)
	    logo = W_CacheLumpName("STARTB24", PU_CACHE);

    	if(notchPosition == 24)
	    logo = W_CacheLumpName("STARTB25", PU_CACHE);

    	if(notchPosition == 25)
	    logo = W_CacheLumpName("STARTB26", PU_CACHE);

    	if(notchPosition == 26)
	    logo = W_CacheLumpName("STARTB27", PU_CACHE);

    	if(notchPosition == 27)
	    logo = W_CacheLumpName("STARTB28", PU_CACHE);

    	if(notchPosition == 28)
	    logo = W_CacheLumpName("STARTB29", PU_CACHE);

    	if(notchPosition == 29)
	    logo = W_CacheLumpName("STARTB30", PU_CACHE);

    	if(notchPosition == 30)
	    logo = W_CacheLumpName("STARTB31", PU_CACHE);

    	if(notchPosition == 31)
	    logo = W_CacheLumpName("STARTB32", PU_CACHE);

    	if(notchPosition == 32)
	    logo = W_CacheLumpName("STARTB33", PU_CACHE);

	if(notchPosition == 33)
	    logo = W_CacheLumpName("STARTB33", PU_CACHE);
    }

    if(!debugmode)
    {
	V_DrawPatch(0, 0, logo);

	if((!datadisc && HEXEN_DEMO) || (!datadisc && HEXEN_MACDEMO))
	    MN_DrTextB("DEMO VERSION",100,5);

	if(!datadisc && HEXEN_1_0)
	    MN_DrTextB("VERSION 1.0",110,5);
	else if((!datadisc && HEXEN_1_1) || (!datadisc && HEXEN_MACFULL))
	    MN_DrTextB("VERSION 1.1",110,5);

	if(datadisc && HEXDD_1_0 && HEXEN_1_0)
	    MN_DrTextB("VERSION 1.0",110,5);

	if(datadisc && HEXDD_1_1 && HEXEN_1_0)
	    MN_DrTextB("VERSION 1.0",110,5);

	if((datadisc && HEXDD_1_0 && HEXEN_1_1) || (datadisc && HEXDD_1_0 && HEXEN_MACFULL))
	    MN_DrTextB("VERSION 1.1",110,5);

	if((datadisc && HEXDD_1_1 && HEXEN_1_1) || (datadisc && HEXDD_1_1 && HEXEN_MACFULL))
	    MN_DrTextB("VERSION 1.1",110,5);
    }
    I_FinishUpdate();

    notchPosition++;

    if(debugmode)
	printf(".");
}



//==========================================================================
//
// ST_NetProgress - indicates network progress
//
//==========================================================================

void ST_NetProgress(void)
{
    printf("*");

    if (using_graphical_startup)
    {
        static int netnotchPosition = 0;

        if (netnotchPosition < ST_MAX_NETNOTCHES)
        {
            ST_UpdateNetNotches(netnotchPosition);
            S_StartSound(NULL, SFX_DRIP);
            netnotchPosition++;
        }
    }
}

//==========================================================================
//
// ST_NetDone - net progress complete
//
//==========================================================================
void ST_NetDone(void)
{
    if (using_graphical_startup)
    {
        S_StartSound(NULL, SFX_PICKUP_WEAPON);
    }
}

//==========================================================================
//
// ST_Message - gives debug message
//
//==========================================================================

void ST_Message(char *message, ...)
{
    char buffer[80];

    va_list argptr;

    va_start(argptr, message);
//    vprintf(message, argptr);

    vsprintf(buffer, message, argptr);

    va_end(argptr);

    if ( strlen(buffer) >= 80 )
    {
	I_Error("Long debug message has overwritten memory");
    }

    printf(buffer);
}

//==========================================================================
//
// ST_RealMessage - gives user message
//
//==========================================================================

void ST_RealMessage(char *message, ...)
{
    va_list argptr;

    va_start(argptr, message);
    vprintf(message, argptr);
    va_end(argptr);
}



//==========================================================================
//
// ST_LoadScreen - loads startup graphic
//
//==========================================================================


byte *ST_LoadScreen(void)
{
    int length, lump;
    byte *buffer;

    lump = W_GetNumForName("STARTUP");
    length = W_LumpLength(lump);
    buffer = (byte *) Z_Malloc(length, PU_STATIC, NULL);
    W_ReadLump(lump, buffer);
    return (buffer);
}

void ST_ProgressDebug(void)
{
    printf(".");
}
/*
void ST_LogoRaven(void)
{
    int i;

    sceRtcGetCurrentTick( &stTickLast );

    stResolution = sceRtcGetTickResolution();

    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));

    logomac = W_CacheLumpName("RAVEN", PU_CACHE);

    for( i = 0; i < 1500000; i++)
    {
	if( ((stTickNow - stTickLast)/((float)stResolution)) >= 1.0f )
	{
	    stTickLast = stTickNow;

	    V_DrawPatch(0, 0, logomac);

    	    I_FinishUpdate(1);

    	    if(i == 1500000)
    		break;
    	}
    }
}

void ST_LogoPresage(void)
{
    int i;

    sceRtcGetCurrentTick( &stTickLast );

    stResolution = sceRtcGetTickResolution();

    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));

    logomac = W_CacheLumpName("PRESAGE", PU_CACHE);

    for(i = 0; i < 1500000; i++)
    {
	if( ((stTickNow - stTickLast)/((float)stResolution)) >= 1.0f )
	{
	    stTickLast = stTickNow;

	    V_DrawPatch(0, 0, logomac);

	    I_FinishUpdate(1);

	    if(i == 1500000)
		break;
	}
    }
}

void ST_LogoId(void)
{
    int i;

    sceRtcGetCurrentTick( &stTickLast );

    stResolution = sceRtcGetTickResolution();

    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));

    logomac = W_CacheLumpName("ID", PU_CACHE);

    for(i = 0 ; i < 1500000; i++)
    {
	if( ((stTickNow - stTickLast)/((float)stResolution)) >= 1.0f )
	{
	    stTickLast = stTickNow;

	    V_DrawPatch(0, 0, logomac);

	    I_FinishUpdate(1);

	    if(i==1500000)
		break;
	}
    }
}
*/
