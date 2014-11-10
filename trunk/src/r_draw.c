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


#include "h2def.h"
#include "i_system.h"
#include "i_video.h"
#include "r_local.h"
#include "v_video.h"

#define FUZZTABLE		50 
#define FUZZOFF	(SCREENWIDTH)

/*

All drawing to the view buffer is accomplished in this file.  The other refresh
files only know about ccordinates, not the architecture of the frame buffer.

*/

byte *viewimage;
int viewwidth, scaledviewwidth, viewheight, viewwindowx, viewwindowy;
byte *ylookup[MAXHEIGHT];
int columnofs[MAXWIDTH];
//byte translations[3][256]; // color tables for different players

byte *tinttable; // used for translucent sprites

int	fuzzoffset[FUZZTABLE] =
{
    FUZZOFF,-FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,
    FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,
    FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,
    FUZZOFF,-FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,
    FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,-FUZZOFF,FUZZOFF,
    FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,
    FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF,FUZZOFF,-FUZZOFF,FUZZOFF 
}; 

int	fuzzpos = 0; 

/*
==================
=
= R_DrawColumn
=
= Source is the top of the column to scale
=
==================
*/

lighttable_t *dc_colormap;
int dc_x;
int dc_yl;
int dc_yh;
fixed_t dc_iscale;
fixed_t dc_texturemid;
byte *dc_source;                // first pixel in a column (possibly virtual)

int dccount;                    // just for profiling

void R_DrawColumn(void)
{
    int count;
    byte *dest;
    fixed_t frac, fracstep;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    dest = ylookup[dc_yl] + columnofs[dc_x];

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    do
    {
        *dest = dc_colormap[dc_source[(frac >> FRACBITS) & 127]];
        dest += SCREENWIDTH;
        frac += fracstep;
    }
    while (count--);
}

void R_DrawColumnLow (void)
{
    int                 count;
    byte*               dest;
    byte*               dest2;
    fixed_t             frac;
    fixed_t             fracstep;        
    int                 x;
 
    count = dc_yh - dc_yl;
 
    // Zero length.
    if (count < 0)
        return;
                                 
#ifdef RANGECHECK
    if ((unsigned)dc_x >= SCREENWIDTH
        || dc_yl < 0
        || dc_yh >= SCREENHEIGHT)
    {
       
        I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
    }
    //  dccount++;
#endif
    // Blocky mode, need to multiply by 2.
    x = dc_x << 1;
   
    dest = ylookup[dc_yl] + columnofs[x];
    dest2 = ylookup[dc_yl] + columnofs[x+1];
   
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl-centery)*fracstep;
   
    do
    {
        // Hack. Does not work corretly.
        *dest2 = *dest = dc_colormap[dc_source[(frac>>FRACBITS)&127]];
        dest += SCREENWIDTH;
        dest2 += SCREENWIDTH;
        frac += fracstep;
 
    } while (count--);
}

void R_DrawTLColumn(void)
{
    int count;
    byte *dest;
    fixed_t frac, fracstep;

    if (!dc_yl)
        dc_yl = 1;
    if (dc_yh == viewheight - 1)
        dc_yh = viewheight - 2;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
        I_Error("R_DrawTLColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    dest = ylookup[dc_yl] + columnofs[dc_x];

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    do
    {
        *dest = tinttable[*dest +
                          (dc_colormap[dc_source[(frac >> FRACBITS) & 127]] <<
                           8)];
        dest += SCREENWIDTH;
        frac += fracstep;
    }
    while (count--);
}

//============================================================================
//
// R_DrawAltTLColumn
//
//============================================================================

void R_DrawAltTLColumn(void)
{
    int count;
    byte *dest;
    fixed_t frac, fracstep;

    if (!dc_yl)
        dc_yl = 1;
    if (dc_yh == viewheight - 1)
        dc_yh = viewheight - 2;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
        I_Error("R_DrawAltTLColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    dest = ylookup[dc_yl] + columnofs[dc_x];

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    do
    {
        *dest = tinttable[((*dest) << 8)
                          + dc_colormap[dc_source[(frac >> FRACBITS) & 127]]];
        dest += SCREENWIDTH;
        frac += fracstep;
    }
    while (count--);
}

/*
========================
=
= R_DrawTranslatedColumn
=
========================
*/

byte *dc_translation;
byte *translationtables;

void R_DrawTranslatedColumn(void)
{
    int count;
    byte *dest;
    fixed_t frac, fracstep;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    dest = ylookup[dc_yl] + columnofs[dc_x];

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    do
    {
        *dest = dc_colormap[dc_translation[dc_source[frac >> FRACBITS]]];
        dest += SCREENWIDTH;
        frac += fracstep;
    }
    while (count--);
}

//============================================================================
//
// R_DrawTranslatedTLColumn
//
//============================================================================

void R_DrawTranslatedTLColumn(void)
{
    int count;
    byte *dest;
    fixed_t frac, fracstep;

    count = dc_yh - dc_yl;
    if (count < 0)
        return;

#ifdef RANGECHECK
    if ((unsigned) dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
        I_Error("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

    dest = ylookup[dc_yl] + columnofs[dc_x];

    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl - centery) * fracstep;

    do
    {
        *dest = tinttable[((*dest) << 8)
                          +
                          dc_colormap[dc_translation
                                      [dc_source[frac >> FRACBITS]]]];
        dest += SCREENWIDTH;
        frac += fracstep;
    }
    while (count--);
}

//============================================================================
//
// R_DrawTranslatedAltTLColumn
//
//============================================================================

/*
void R_DrawTranslatedAltTLColumn (void)
{
	int			count;
	byte		*dest;
	fixed_t		frac, fracstep;	

	count = dc_yh - dc_yl;
	if (count < 0)
		return;
				
#ifdef RANGECHECK
	if ((unsigned)dc_x >= SCREENWIDTH || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
		I_Error ("R_DrawColumn: %i to %i at %i", dc_yl, dc_yh, dc_x);
#endif

	dest = ylookup[dc_yl] + columnofs[dc_x];
	
	fracstep = dc_iscale;
	frac = dc_texturemid + (dc_yl-centery)*fracstep;

	do
	{
		*dest = tinttable[*dest
			+(dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]]<<8)];
		dest += SCREENWIDTH;
		frac += fracstep;
	} while (count--);
}
*/

//--------------------------------------------------------------------------
//
// PROC R_InitTranslationTables
//
//--------------------------------------------------------------------------

void R_InitTranslationTables(void)
{
    int i;
    byte *transLump;
    int lumpnum;

    V_LoadTintTable();

    // Allocate translation tables
    translationtables = Z_Malloc(256 * 3 * (MAXPLAYERS - 1), PU_STATIC, 0);

    for (i = 0; i < 3 * (MAXPLAYERS - 1); i++)
    {
        lumpnum = W_GetNumForName("trantbl0") + i;
        transLump = W_CacheLumpNum(lumpnum, PU_STATIC);
        memcpy(translationtables + i * 256, transLump, 256);
        W_ReleaseLumpNum(lumpnum);
    }
}

/*
================
=
= R_DrawSpan
=
================
*/

int ds_y;
int ds_x1;
int ds_x2;
lighttable_t *ds_colormap;
fixed_t ds_xfrac;
fixed_t ds_yfrac;
fixed_t ds_xstep;
fixed_t ds_ystep;
byte *ds_source;                // start of a 64*64 tile image

int dscount;                    // just for profiling

void R_DrawSpan(void)
{
    fixed_t xfrac, yfrac;
    byte *dest;
    int count, spot;

#ifdef RANGECHECK
    if (ds_x2 < ds_x1 || ds_x1 < 0 || ds_x2 >= SCREENWIDTH
        || (unsigned) ds_y > SCREENHEIGHT)
        I_Error("R_DrawSpan: %i to %i at %i", ds_x1, ds_x2, ds_y);
//      dscount++;
#endif

    xfrac = ds_xfrac;
    yfrac = ds_yfrac;

    dest = ylookup[ds_y] + columnofs[ds_x1];
    count = ds_x2 - ds_x1;
    do
    {
        spot = ((yfrac >> (16 - 6)) & (63 * 64)) + ((xfrac >> 16) & 63);
        *dest++ = ds_colormap[ds_source[spot]];
        xfrac += ds_xstep;
        yfrac += ds_ystep;
    }
    while (count--);
}

void R_DrawSpanLow (void)
{
    unsigned int position, step;
    unsigned int xtemp, ytemp;
    byte *dest;
    int count;
    int spot;

#ifdef RANGECHECK
    if (ds_x2 < ds_x1
	|| ds_x1<0
	|| ds_x2>=SCREENWIDTH
	|| (unsigned)ds_y>SCREENHEIGHT)
    {
	I_Error( "R_DrawSpan: %i to %i at %i",
		 ds_x1,ds_x2,ds_y);
    }
//	dscount++; 
#endif

    position = ((ds_xfrac << 10) & 0xffff0000)
             | ((ds_yfrac >> 6)  & 0x0000ffff);
    step = ((ds_xstep << 10) & 0xffff0000)
         | ((ds_ystep >> 6)  & 0x0000ffff);

    count = (ds_x2 - ds_x1);

    // Blocky mode, need to multiply by 2.
    ds_x1 <<= 1;
    ds_x2 <<= 1;

    dest = ylookup[ds_y] + columnofs[ds_x1];

    do
    {
	// Calculate current texture index in u,v.
        ytemp = (position >> 4) & 0x0fc0;
        xtemp = (position >> 26);
        spot = xtemp | ytemp;

	// Lowres/blocky mode does it twice,
	//  while scale is adjusted appropriately.
	*dest++ = ds_colormap[ds_source[spot]];
	*dest++ = ds_colormap[ds_source[spot]];

	position += step;

    } while (count--);
}


/*
================
=
= R_InitBuffer
=
=================
*/

void R_InitBuffer(int width, int height)
{
    int i;

    viewwindowx = (SCREENWIDTH - width) >> 1;
    for (i = 0; i < width; i++)
        columnofs[i] = viewwindowx + i;
    if (width == SCREENWIDTH)
        viewwindowy = 0;
    else
        viewwindowy = (SCREENHEIGHT - SBARHEIGHT - height) >> 1;
    for (i = 0; i < height; i++)
        ylookup[i] = I_VideoBuffer + (i + viewwindowy) * SCREENWIDTH;
}


/*
==================
=
= R_DrawViewBorder
=
= Draws the border around the view for different size windows
==================
*/

boolean BorderNeedRefresh;

void R_DrawViewBorder(void)
{
    byte *src, *dest;
    int x, y;

    if (scaledviewwidth == SCREENWIDTH)
        return;

    src = W_CacheLumpName("F_022", PU_CACHE);
    dest = I_VideoBuffer;

    for (y = 0; y < SCREENHEIGHT - SBARHEIGHT; y++)
    {
        for (x = 0; x < SCREENWIDTH / 64; x++)
        {
            memcpy(dest, src + ((y & 63) << 6), 64);
            dest += 64;
        }
        if (SCREENWIDTH & 63)
        {
            memcpy(dest, src + ((y & 63) << 6), SCREENWIDTH & 63);
            dest += (SCREENWIDTH & 63);
        }
    }
//    for (x = viewwindowx; x < viewwindowx + viewwidth; x += 16)		// CHANGED FOR HIRES
    for (x = (viewwindowx >> hires); x < (viewwindowx >> hires) + (viewwidth >> hires); x += 16)	// CHANGED FOR HIRES
    {
/*
        V_DrawPatch(x, viewwindowy - 4, W_CacheLumpName("bordt", PU_CACHE));	// CHANGED FOR HIRES
        V_DrawPatch(x, viewwindowy + viewheight, W_CacheLumpName("bordb",
                                                                 PU_CACHE));	// CHANGED FOR HIRES
*/
	V_DrawPatch(x, (viewwindowy >> hires) - 4, W_CacheLumpName("bordt", PU_CACHE));	// CHANGED FOR HIRES
	V_DrawPatch(x, (viewwindowy >> hires) + (viewheight >> hires), W_CacheLumpName("bordb",	// CHANGED FOR HIRES
								PU_CACHE));	// CHANGED FOR HIRES
    }
//    for (y = viewwindowy; y < viewwindowy + viewheight; y += 16)		// CHANGED FOR HIRES
    for (y = (viewwindowy >> hires); y < (viewwindowy >> hires) + (viewheight >> hires); y += 16)	// CHANGED FOR HIRES
    {
/*
        V_DrawPatch(viewwindowx - 4, y, W_CacheLumpName("bordl", PU_CACHE));	// CHANGED FOR HIRES
        V_DrawPatch(viewwindowx + viewwidth, y, W_CacheLumpName("bordr",	// CHANGED FOR HIRES
                                                                PU_CACHE));	// CHANGED FOR HIRES
*/
	V_DrawPatch((viewwindowx >> hires) - 4, y, W_CacheLumpName("bordl", PU_CACHE));	// CHANGED FOR HIRES
	V_DrawPatch((viewwindowx >> hires) + (viewwidth >> hires), y, W_CacheLumpName("bordr",	// CHANGED FOR HIRES
								PU_CACHE));	// CHANGED FOR HIRES
    }
/*
    V_DrawPatch(viewwindowx - 4, viewwindowy - 4, W_CacheLumpName("bordtl",	// CHANGED FOR HIRES
                                                                  PU_CACHE));	// CHANGED FOR HIRES
    V_DrawPatch(viewwindowx + viewwidth, viewwindowy - 4, 			// CHANGED FOR HIRES
                W_CacheLumpName("bordtr", PU_CACHE));				// CHANGED FOR HIRES
    V_DrawPatch(viewwindowx + viewwidth, viewwindowy + viewheight, 		// CHANGED FOR HIRES
                W_CacheLumpName("bordbr", PU_CACHE));				// CHANGED FOR HIRES
    V_DrawPatch(viewwindowx - 4, viewwindowy + viewheight, 			// CHANGED FOR HIRES
                W_CacheLumpName("bordbl", PU_CACHE));				// CHANGED FOR HIRES
*/
    V_DrawPatch((viewwindowx >> hires) - 4, (viewwindowy >> hires) - 4, W_CacheLumpName("bordtl",	// CHANGED FOR HIRES
			PU_CACHE));						// CHANGED FOR HIRES
    V_DrawPatch((viewwindowx >> hires) + (viewwidth >> hires), (viewwindowy >> hires) - 4,	// CHANGED FOR HIRES
			W_CacheLumpName("bordtr", PU_CACHE));			// CHANGED FOR HIRES
    V_DrawPatch((viewwindowx >> hires) + (viewwidth >> hires), (viewwindowy >> hires) + (viewheight >> hires),	// CHANGED FOR HIRES
			W_CacheLumpName("bordbr", PU_CACHE));			// CHANGED FOR HIRES
    V_DrawPatch((viewwindowx >> hires) - 4, (viewwindowy >> hires) + (viewheight >> hires),	// CHANGED FOR HIRES
			W_CacheLumpName("bordbl", PU_CACHE));			// CHANGED FOR HIRES
}

/*
==================
=
= R_DrawTopBorder
=
= Draws the top border around the view for different size windows
==================
*/

boolean BorderTopRefresh;

void R_DrawTopBorder(void)
{
    byte *src, *dest;
    int x, y;

    if (scaledviewwidth == SCREENWIDTH)
        return;

/*	if(gamemode == shareware)
	{
		src = W_CacheLumpName ("FLOOR04", PU_CACHE);
	}
	else
	{
		src = W_CacheLumpName ("FLAT513", PU_CACHE);
	}
*/
    src = W_CacheLumpName("F_022", PU_CACHE);
    dest = I_VideoBuffer;

    for (y = 0; y < 34; y++)
    {
        for (x = 0; x < SCREENWIDTH / 64; x++)
        {
            memcpy(dest, src + ((y & 63) << 6), 64);
            dest += 64;
        }
        if (SCREENWIDTH & 63)
        {
            memcpy(dest, src + ((y & 63) << 6), SCREENWIDTH & 63);
            dest += (SCREENWIDTH & 63);
        }
    }
    if (viewwindowy < 35)
    {
//        for (x = viewwindowx; x < viewwindowx + viewwidth; x += 16)		// CHANGED FOR HIRES
	for (x = (viewwindowx >> hires); x < (viewwindowx >> hires) + (viewwidth >> hires); x += 16)	// CHANGED FOR HIRES
        {
/*
            V_DrawPatch(x, viewwindowy - 4, 					// CHANGED FOR HIRES
                        W_CacheLumpName("bordt", PU_CACHE));			// CHANGED FOR HIRES
*/
	    V_DrawPatch(x, (viewwindowy >> hires) - 4,				// CHANGED FOR HIRES
			W_CacheLumpName("bordt", PU_CACHE));			// CHANGED FOR HIRES
        }
/*
        V_DrawPatch(viewwindowx - 4, viewwindowy, W_CacheLumpName("bordl",	// CHANGED FOR HIRES
                                                                  PU_CACHE));	// CHANGED FOR HIRES
        V_DrawPatch(viewwindowx + viewwidth, viewwindowy, 			// CHANGED FOR HIRES
                    W_CacheLumpName("bordr", PU_CACHE));			// CHANGED FOR HIRES
        V_DrawPatch(viewwindowx - 4, viewwindowy + 16, 				// CHANGED FOR HIRES
                    W_CacheLumpName("bordl", PU_CACHE));			// CHANGED FOR HIRES
        V_DrawPatch(viewwindowx + viewwidth, viewwindowy + 16, 			// CHANGED FOR HIRES
                    W_CacheLumpName("bordr", PU_CACHE));			// CHANGED FOR HIRES

        V_DrawPatch(viewwindowx - 4, viewwindowy - 4, 				// CHANGED FOR HIRES
                    W_CacheLumpName("bordtl", PU_CACHE));			// CHANGED FOR HIRES
        V_DrawPatch(viewwindowx + viewwidth, viewwindowy - 4, 			// CHANGED FOR HIRES
                    W_CacheLumpName("bordtr", PU_CACHE));			// CHANGED FOR HIRES
*/
	V_DrawPatch((viewwindowx >> hires) - 4, (viewwindowy >> hires),		// CHANGED FOR HIRES
			W_CacheLumpName("bordl", PU_CACHE));			// CHANGED FOR HIRES
	V_DrawPatch((viewwindowx >> hires) + (viewwidth >> hires), (viewwindowy >> hires),	// CHANGED FOR HIRES
			W_CacheLumpName("bordr", PU_CACHE));			// CHANGED FOR HIRES
	V_DrawPatch((viewwindowx >> hires) - 4, (viewwindowy >> hires) + 16,	// CHANGED FOR HIRES
			W_CacheLumpName("bordl", PU_CACHE));			// CHANGED FOR HIRES
	V_DrawPatch((viewwindowx >> hires) + (viewwidth >> hires), (viewwindowy >> hires) + 16,	// CHANGED FOR HIRES
			W_CacheLumpName("bordr", PU_CACHE));			// CHANGED FOR HIRES

	V_DrawPatch((viewwindowx >> hires) - 4, (viewwindowy >> hires) - 4,	// CHANGED FOR HIRES
			W_CacheLumpName("bordtl", PU_CACHE));			// CHANGED FOR HIRES
	V_DrawPatch((viewwindowx >> hires) + (viewwidth >> hires), (viewwindowy >> hires) - 4,	// CHANGED FOR HIRES
			W_CacheLumpName("bordtr", PU_CACHE));			// CHANGED FOR HIRES
    }
}

void R_DrawTranslatedColumnLow (void)
{
    int                 count;
    byte*               dest;
    byte*               dest2;
    fixed_t             frac;
    fixed_t             fracstep;        
    int                 x;
 
    count = dc_yh - dc_yl;
    if (count < 0)
        return;
 
    // low detail, need to scale by 2
    x = dc_x << 1;
                                 
#ifdef RANGECHECK
    if ((unsigned)x >= SCREENWIDTH
        || dc_yl < 0
        || dc_yh >= SCREENHEIGHT)
    {
        I_Error ( "R_DrawColumn: %i to %i at %i",
                  dc_yl, dc_yh, x);
    }
   
#endif
 
 
    dest = ylookup[dc_yl] + columnofs[x];
    dest2 = ylookup[dc_yl] + columnofs[x+1];
 
    // Looks familiar.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl-centery)*fracstep;
 
    // Here we do an additional index re-mapping.
    do
    {
        // Translation tables are used
        //  to map certain colorramps to other ones,
        //  used with PLAY sprites.
        // Thus the "green" ramp of the player 0 sprite
        //  is mapped to gray, red, black/indigo.
        *dest = dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]];
        *dest2 = dc_colormap[dc_translation[dc_source[frac>>FRACBITS]]];
        dest += SCREENWIDTH;
        dest2 += SCREENWIDTH;
       
        frac += fracstep;
    } while (count--);
} 

void R_DrawTLColumnLow (void)
{
    int                 count;
    byte*               dest;
    byte*               dest2;
    fixed_t             frac;
    fixed_t             fracstep;        
    int x;
 
    // Adjust borders. Low...
    if (!dc_yl)
        dc_yl = 1;
 
    // .. and high.
    if (dc_yh == viewheight-1)
        dc_yh = viewheight - 2;
                 
    count = dc_yh - dc_yl;
 
    // Zero length.
    if (count < 0)
        return;
 
    // low detail mode, need to multiply by 2
   
    x = dc_x << 1;
   
#ifdef RANGECHECK
    if ((unsigned)x >= SCREENWIDTH
        || dc_yl < 0 || dc_yh >= SCREENHEIGHT)
    {
        I_Error ("R_DrawFuzzColumn: %i to %i at %i",
                 dc_yl, dc_yh, dc_x);
    }
#endif
   
    dest = ylookup[dc_yl] + columnofs[x];
    dest2 = ylookup[dc_yl] + columnofs[x+1];
 
    // Looks familiar.
    fracstep = dc_iscale;
    frac = dc_texturemid + (dc_yl-centery)*fracstep;
 
    // Looks like an attempt at dithering,
    //  using the colormap #6 (of 0-31, a bit
    //  brighter than average).
    do
    {
        // Lookup framebuffer, and retrieve
        //  a pixel that is either one column
        //  left or right of the current one.
        // Add index from colormap to index.
        *dest = colormaps[6*256+dest[fuzzoffset[fuzzpos]]];
        *dest2 = colormaps[6*256+dest2[fuzzoffset[fuzzpos]]];
 
        // Clamp table lookup index.
        if (++fuzzpos == FUZZTABLE)
            fuzzpos = 0;
       
        dest += SCREENWIDTH;
        dest2 += SCREENWIDTH;
 
        frac += fracstep;
    } while (count--);
} 

