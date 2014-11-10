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
*/
#include <math.h>
#include <stdlib.h>
#include "h2def.h"
#include "i_system.h"
#include "m_argv.h"
#include "m_bbox.h"
#include "i_swap.h"
#include "s_sound.h"
#include "p_local.h"

#include "v_video.h"

// MACROS ------------------------------------------------------------------

#define MAPINFO_SCRIPT_NAME "MAPINFO"
#define MCMD_SKY1 1
#define MCMD_SKY2 2
#define MCMD_LIGHTNING 3
#define MCMD_FADETABLE 4
#define MCMD_DOUBLESKY 5
#define MCMD_CLUSTER 6
#define MCMD_WARPTRANS 7
#define MCMD_NEXT 8
#define MCMD_CDTRACK 9
#define MCMD_CD_STARTTRACK 10
#define MCMD_CD_END1TRACK 11
#define MCMD_CD_END2TRACK 12
#define MCMD_CD_END3TRACK 13
#define MCMD_CD_INTERTRACK 14
#define MCMD_CD_TITLETRACK 15

#define UNKNOWN_MAP_NAME "DEVELOPMENT MAP"

#define DEFAULT_SKY_NAME_1 "SKY1"
#define DEFAULT_SKY_NAME_2 "SKY2"

#define DEFAULT_SONG_LUMP "DEFSONG"
#define DEFAULT_FADE_TABLE "COLORMAP"

extern boolean from_menu;

// TYPES -------------------------------------------------------------------

typedef struct mapInfo_s mapInfo_t;
struct mapInfo_s
{
    short cluster;
    short warpTrans;
    short nextMap;
    short cdTrack;
    char name[32];
    short sky1Texture;
    short sky2Texture;
    fixed_t sky1ScrollDelta;
    fixed_t sky2ScrollDelta;
    boolean doubleSky;
    boolean lightning;
    int fadetable;
    char songLump[10];
};

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

void P_SpawnMapThing(mapthing_t * mthing);

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

static int QualifyMap(int map);

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

// PUBLIC DATA DEFINITIONS -------------------------------------------------

int MapCount;
//mapthing_t deathmatchstarts[MAXDEATHMATCHSTARTS], *deathmatch_p;
mapthing_t playerstarts[MAX_PLAYER_STARTS][MAXPLAYERS];
int numvertexes;
vertex_t *vertexes;
int numsegs;
seg_t *segs;
int numsectors;
sector_t *sectors;
int numsubsectors;
subsector_t *subsectors;
int numnodes;
node_t *nodes;
int numlines;
line_t *lines;
int numsides;
side_t *sides;
short *blockmaplump;            // offsets in blockmap are from here
short *blockmap;
int bmapwidth, bmapheight;      // in mapblocks
fixed_t bmaporgx, bmaporgy;     // origin of block map
mobj_t **blocklinks;            // for thing chains
byte *rejectmatrix;             // for fast sight rejection
/*
patch_t *blank;
u32 ninResolution;
u64 ninTickNow;
u64 ninTickLast;
extern int ninty;
*/
// PRIVATE DATA DEFINITIONS ------------------------------------------------

static mapInfo_t MapInfo[99];
static char *MapCmdNames[] = {
    "SKY1",
    "SKY2",
    "DOUBLESKY",
    "LIGHTNING",
    "FADETABLE",
    "CLUSTER",
    "WARPTRANS",
    "NEXT",
    "CDTRACK",
    "CD_START_TRACK",
    "CD_END1_TRACK",
    "CD_END2_TRACK",
    "CD_END3_TRACK",
    "CD_INTERMISSION_TRACK",
    "CD_TITLE_TRACK",
    NULL
};
static int MapCmdIDs[] = {
    MCMD_SKY1,
    MCMD_SKY2,
    MCMD_DOUBLESKY,
    MCMD_LIGHTNING,
    MCMD_FADETABLE,
    MCMD_CLUSTER,
    MCMD_WARPTRANS,
    MCMD_NEXT,
    MCMD_CDTRACK,
    MCMD_CD_STARTTRACK,
    MCMD_CD_END1TRACK,
    MCMD_CD_END2TRACK,
    MCMD_CD_END3TRACK,
    MCMD_CD_INTERTRACK,
    MCMD_CD_TITLETRACK
};

static int cd_NonLevelTracks[6];        // Non-level specific song cd track numbers 

// CODE --------------------------------------------------------------------

/*
=================
=
= P_LoadVertexes
=
=================
*/

void P_LoadVertexes(int lump)
{
    byte *data;
    int i;
    mapvertex_t *ml;
    vertex_t *li;

    numvertexes = W_LumpLength(lump) / sizeof(mapvertex_t);
    vertexes = Z_Malloc(numvertexes * sizeof(vertex_t), PU_LEVEL, 0);
    data = W_CacheLumpNum(lump, PU_STATIC);

    ml = (mapvertex_t *) data;
    li = vertexes;
    for (i = 0; i < numvertexes; i++, li++, ml++)
    {
        li->x = SHORT(ml->x) << FRACBITS;
        li->y = SHORT(ml->y) << FRACBITS;
    }

    W_ReleaseLumpNum(lump);
}


/*
=================
=
= P_LoadSegs
=
=================
*/

void P_LoadSegs(int lump)
{
    byte *data;
    int i;
    mapseg_t *ml;
    seg_t *li;
    line_t *ldef;
    int linedef, side;

    numsegs = W_LumpLength(lump) / sizeof(mapseg_t);
    segs = Z_Malloc(numsegs * sizeof(seg_t), PU_LEVEL, 0);
    memset(segs, 0, numsegs * sizeof(seg_t));
    data = W_CacheLumpNum(lump, PU_STATIC);

    ml = (mapseg_t *) data;
    li = segs;
    for (i = 0; i < numsegs; i++, li++, ml++)
    {
        li->v1 = &vertexes[SHORT(ml->v1)];
        li->v2 = &vertexes[SHORT(ml->v2)];

        li->angle = (SHORT(ml->angle)) << 16;
        li->offset = (SHORT(ml->offset)) << 16;
        linedef = SHORT(ml->linedef);
        ldef = &lines[linedef];
        li->linedef = ldef;
        side = SHORT(ml->side);
        li->sidedef = &sides[ldef->sidenum[side]];
        li->frontsector = sides[ldef->sidenum[side]].sector;
        if (ldef->flags & ML_TWOSIDED)
            li->backsector = sides[ldef->sidenum[side ^ 1]].sector;
        else
            li->backsector = 0;
    }

    W_ReleaseLumpNum(lump);
}


/*
=================
=
= P_LoadSubsectors
=
=================
*/

void P_LoadSubsectors(int lump)
{
    byte *data;
    int i;
    mapsubsector_t *ms;
    subsector_t *ss;

    numsubsectors = W_LumpLength(lump) / sizeof(mapsubsector_t);
    subsectors = Z_Malloc(numsubsectors * sizeof(subsector_t), PU_LEVEL, 0);
    data = W_CacheLumpNum(lump, PU_STATIC);

    ms = (mapsubsector_t *) data;
    memset(subsectors, 0, numsubsectors * sizeof(subsector_t));
    ss = subsectors;
    for (i = 0; i < numsubsectors; i++, ss++, ms++)
    {
        ss->numlines = SHORT(ms->numsegs);
        ss->firstline = SHORT(ms->firstseg);
    }

    W_ReleaseLumpNum(lump);
}


/*
=================
=
= P_LoadSectors
=
=================
*/

void P_LoadSectors(int lump)
{
    byte *data;
    int i;
    mapsector_t *ms;
    sector_t *ss;

    numsectors = W_LumpLength(lump) / sizeof(mapsector_t);
    sectors = Z_Malloc(numsectors * sizeof(sector_t), PU_LEVEL, 0);
    memset(sectors, 0, numsectors * sizeof(sector_t));
    data = W_CacheLumpNum(lump, PU_STATIC);

    ms = (mapsector_t *) data;
    ss = sectors;

    for (i = 0; i < numsectors; i++, ss++, ms++)
    {
        ss->floorheight = SHORT(ms->floorheight) << FRACBITS;
        ss->ceilingheight = SHORT(ms->ceilingheight) << FRACBITS;
        ss->floorpic = R_FlatNumForName(ms->floorpic);
        ss->ceilingpic = R_FlatNumForName(ms->ceilingpic);
        ss->lightlevel = SHORT(ms->lightlevel);
        ss->special = SHORT(ms->special);
        ss->tag = SHORT(ms->tag);
        ss->thinglist = NULL;
        ss->seqType = SEQTYPE_STONE;    // default seqType
    }
    W_ReleaseLumpNum(lump);
}


/*
=================
=
= P_LoadNodes
=
=================
*/

void P_LoadNodes(int lump)
{
    byte *data;
    int i, j, k;
    mapnode_t *mn;
    node_t *no;

    numnodes = W_LumpLength(lump) / sizeof(mapnode_t);
    nodes = Z_Malloc(numnodes * sizeof(node_t), PU_LEVEL, 0);
    data = W_CacheLumpNum(lump, PU_STATIC);

    mn = (mapnode_t *) data;
    no = nodes;
    for (i = 0; i < numnodes; i++, no++, mn++)
    {
        no->x = SHORT(mn->x) << FRACBITS;
        no->y = SHORT(mn->y) << FRACBITS;
        no->dx = SHORT(mn->dx) << FRACBITS;
        no->dy = SHORT(mn->dy) << FRACBITS;
        for (j = 0; j < 2; j++)
        {
            no->children[j] = SHORT(mn->children[j]);
            for (k = 0; k < 4; k++)
                no->bbox[j][k] = SHORT(mn->bbox[j][k]) << FRACBITS;
        }
    }
    W_ReleaseLumpNum(lump);
}

//==========================================================================
//
// P_LoadThings
//
//==========================================================================

void P_LoadThings(int lump)
{
    byte *data;
    int i;
    mapthing_t spawnthing;
    mapthing_t *mt;
    int numthings;
    int playerCount;
//    int deathSpotsCount;

    data = W_CacheLumpNum(lump, PU_STATIC);
    numthings = W_LumpLength(lump) / sizeof(mapthing_t);

    mt = (mapthing_t *) data;
    for (i = 0; i < numthings; i++, mt++)
    {
        spawnthing.tid = SHORT(mt->tid);
        spawnthing.x = SHORT(mt->x);
        spawnthing.y = SHORT(mt->y);
        spawnthing.height = SHORT(mt->height);
        spawnthing.angle = SHORT(mt->angle);
        spawnthing.type = SHORT(mt->type);
        spawnthing.options = SHORT(mt->options);

        spawnthing.special = mt->special;
        spawnthing.arg1 = mt->arg1;
        spawnthing.arg2 = mt->arg2;
        spawnthing.arg3 = mt->arg3;
        spawnthing.arg4 = mt->arg4;
        spawnthing.arg5 = mt->arg5;

        P_SpawnMapThing(&spawnthing);
    }
    P_CreateTIDList();
    P_InitCreatureCorpseQueue(false);   // false = do NOT scan for corpses
    W_ReleaseLumpNum(lump);

//    if (!deathmatch)
    {                           // Don't need to check deathmatch spots
        return;
    }
    playerCount = 0;
    for (i = 0; i < MAXPLAYERS; i++)
    {
        playerCount += playeringame[i];
    }
/*
    deathSpotsCount = deathmatch_p - deathmatchstarts;
    if (deathSpotsCount < playerCount)
    {
        I_Error("P_LoadThings: Player count (%d) exceeds deathmatch "
                "spots (%d)", playerCount, deathSpotsCount);
    }
*/
}

/*
=================
=
= P_LoadLineDefs
=
=================
*/

void P_LoadLineDefs(int lump)
{
    byte *data;
    int i;
    maplinedef_t *mld;
    line_t *ld;
    vertex_t *v1, *v2;

    numlines = W_LumpLength(lump) / sizeof(maplinedef_t);
    lines = Z_Malloc(numlines * sizeof(line_t), PU_LEVEL, 0);
    memset(lines, 0, numlines * sizeof(line_t));
    data = W_CacheLumpNum(lump, PU_STATIC);

    mld = (maplinedef_t *) data;
    ld = lines;
    for (i = 0; i < numlines; i++, mld++, ld++)
    {
        ld->flags = SHORT(mld->flags);

        // Old line special info ...
        //ld->special = SHORT(mld->special);
        //ld->tag = SHORT(mld->tag);

        // New line special info ...
        ld->special = mld->special;
        ld->arg1 = mld->arg1;
        ld->arg2 = mld->arg2;
        ld->arg3 = mld->arg3;
        ld->arg4 = mld->arg4;
        ld->arg5 = mld->arg5;

        v1 = ld->v1 = &vertexes[SHORT(mld->v1)];
        v2 = ld->v2 = &vertexes[SHORT(mld->v2)];
        ld->dx = v2->x - v1->x;
        ld->dy = v2->y - v1->y;
        if (!ld->dx)
            ld->slopetype = ST_VERTICAL;
        else if (!ld->dy)
            ld->slopetype = ST_HORIZONTAL;
        else
        {
            if (FixedDiv(ld->dy, ld->dx) > 0)
                ld->slopetype = ST_POSITIVE;
            else
                ld->slopetype = ST_NEGATIVE;
        }

        if (v1->x < v2->x)
        {
            ld->bbox[BOXLEFT] = v1->x;
            ld->bbox[BOXRIGHT] = v2->x;
        }
        else
        {
            ld->bbox[BOXLEFT] = v2->x;
            ld->bbox[BOXRIGHT] = v1->x;
        }
        if (v1->y < v2->y)
        {
            ld->bbox[BOXBOTTOM] = v1->y;
            ld->bbox[BOXTOP] = v2->y;
        }
        else
        {
            ld->bbox[BOXBOTTOM] = v2->y;
            ld->bbox[BOXTOP] = v1->y;
        }
        ld->sidenum[0] = SHORT(mld->sidenum[0]);
        ld->sidenum[1] = SHORT(mld->sidenum[1]);
        if (ld->sidenum[0] != -1)
            ld->frontsector = sides[ld->sidenum[0]].sector;
        else
            ld->frontsector = 0;
        if (ld->sidenum[1] != -1)
            ld->backsector = sides[ld->sidenum[1]].sector;
        else
            ld->backsector = 0;
    }

    W_ReleaseLumpNum(lump);
}


/*
=================
=
= P_LoadSideDefs
=
=================
*/

void P_LoadSideDefs(int lump)
{
    byte *data;
    int i;
    mapsidedef_t *msd;
    side_t *sd;

    numsides = W_LumpLength(lump) / sizeof(mapsidedef_t);
    sides = Z_Malloc(numsides * sizeof(side_t), PU_LEVEL, 0);
    memset(sides, 0, numsides * sizeof(side_t));
    data = W_CacheLumpNum(lump, PU_STATIC);

    msd = (mapsidedef_t *) data;
    sd = sides;

    for (i = 0; i < numsides; i++, msd++, sd++)
    {
        sd->textureoffset = SHORT(msd->textureoffset) << FRACBITS;
        sd->rowoffset = SHORT(msd->rowoffset) << FRACBITS;
        sd->toptexture = R_TextureNumForName(msd->toptexture);
        sd->bottomtexture = R_TextureNumForName(msd->bottomtexture);
        sd->midtexture = R_TextureNumForName(msd->midtexture);
        sd->sector = &sectors[SHORT(msd->sector)];
    }
    W_ReleaseLumpNum(lump);
}

/*
=================
=
= P_LoadBlockMap
=
=================
*/

void P_LoadBlockMap(int lump)
{
    int i, count;
    int lumplen;

    lumplen = W_LumpLength(lump);

    blockmaplump = Z_Malloc(lumplen, PU_LEVEL, NULL);
    W_ReadLump(lump, blockmaplump);
    blockmap = blockmaplump + 4;

    // Swap all short integers to native byte ordering:

    count = lumplen / 2;

    for (i = 0; i < count; i++)
        blockmaplump[i] = SHORT(blockmaplump[i]);

    bmaporgx = blockmaplump[0] << FRACBITS;
    bmaporgy = blockmaplump[1] << FRACBITS;
    bmapwidth = blockmaplump[2];
    bmapheight = blockmaplump[3];

    // clear out mobj chains

    count = sizeof(*blocklinks) * bmapwidth * bmapheight;
    blocklinks = Z_Malloc(count, PU_LEVEL, 0);
    memset(blocklinks, 0, count);
}




/*
=================
=
= P_GroupLines
=
= Builds sector line lists and subsector sector numbers
= Finds block bounding boxes for sectors
=================
*/

void P_GroupLines(void)
{
    line_t **linebuffer;
    int i, j, total;
    line_t *li;
    sector_t *sector;
    subsector_t *ss;
    seg_t *seg;
    fixed_t bbox[4];
    int block;

// look up sector number for each subsector
    ss = subsectors;
    for (i = 0; i < numsubsectors; i++, ss++)
    {
        seg = &segs[ss->firstline];
        ss->sector = seg->sidedef->sector;
    }

// count number of lines in each sector
    li = lines;
    total = 0;
    for (i = 0; i < numlines; i++, li++)
    {
        total++;
        li->frontsector->linecount++;
        if (li->backsector && li->backsector != li->frontsector)
        {
            li->backsector->linecount++;
            total++;
        }
    }

// build line tables for each sector
    linebuffer = Z_Malloc(total * sizeof(line_t *), PU_LEVEL, 0);
    sector = sectors;
    for (i = 0; i < numsectors; i++, sector++)
    {
        M_ClearBox(bbox);
        sector->lines = linebuffer;
        li = lines;
        for (j = 0; j < numlines; j++, li++)
        {
            if (li->frontsector == sector || li->backsector == sector)
            {
                *linebuffer++ = li;
                M_AddToBox(bbox, li->v1->x, li->v1->y);
                M_AddToBox(bbox, li->v2->x, li->v2->y);
            }
        }
        if (linebuffer - sector->lines != sector->linecount)
            I_Error("P_GroupLines: miscounted");

        // set the degenmobj_t to the middle of the bounding box
        sector->soundorg.x = (bbox[BOXRIGHT] + bbox[BOXLEFT]) / 2;
        sector->soundorg.y = (bbox[BOXTOP] + bbox[BOXBOTTOM]) / 2;

        // adjust bounding box to map blocks
        block = (bbox[BOXTOP] - bmaporgy + MAXRADIUS) >> MAPBLOCKSHIFT;
        block = block >= bmapheight ? bmapheight - 1 : block;
        sector->blockbox[BOXTOP] = block;

        block = (bbox[BOXBOTTOM] - bmaporgy - MAXRADIUS) >> MAPBLOCKSHIFT;
        block = block < 0 ? 0 : block;
        sector->blockbox[BOXBOTTOM] = block;

        block = (bbox[BOXRIGHT] - bmaporgx + MAXRADIUS) >> MAPBLOCKSHIFT;
        block = block >= bmapwidth ? bmapwidth - 1 : block;
        sector->blockbox[BOXRIGHT] = block;

        block = (bbox[BOXLEFT] - bmaporgx - MAXRADIUS) >> MAPBLOCKSHIFT;
        block = block < 0 ? 0 : block;
        sector->blockbox[BOXLEFT] = block;
    }

}

//=============================================================================


/*
=================
=
= P_SetupLevel
=
=================
*/

void P_SetupLevel(int episode, int map, int playermask, skill_t skill)
{
    int i/*, j*/;
//    int parm;
    char lumpname[9];
    int lumpnum;
//    mobj_t *mobj;

//    blank = W_CacheLumpName("BLANK", PU_CACHE);

    for (i = 0; i < MAXPLAYERS; i++)
    {
        players[i].killcount = players[i].secretcount
            = players[i].itemcount = 0;
    }
    players[consoleplayer].viewz = 1;   // will be set by player think

    // Waiting-for-level-load song; not played if playing music from CD
    // (the seek time will be so long it will just make loading take
    // longer)

    if (/*ninty == 0 ||*/ HEXEN_MACFULL || HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1)
    {
	if(!HEXEN_BETA_DEMO)
	    S_StartSongName("chess", true);
    }

    Z_FreeTags(PU_LEVEL, PU_PURGELEVEL - 1);

    P_InitThinkers();
    leveltime = 0;

    sprintf(lumpname, "MAP%02d", map);
    lumpnum = W_GetNumForName(lumpname);
    //
    // Begin processing map lumps
    // Note: most of this ordering is important
    //
    P_LoadBlockMap(lumpnum + ML_BLOCKMAP);
    P_LoadVertexes(lumpnum + ML_VERTEXES);
    P_LoadSectors(lumpnum + ML_SECTORS);
    P_LoadSideDefs(lumpnum + ML_SIDEDEFS);
    P_LoadLineDefs(lumpnum + ML_LINEDEFS);
    P_LoadSubsectors(lumpnum + ML_SSECTORS);
    P_LoadNodes(lumpnum + ML_NODES);
    P_LoadSegs(lumpnum + ML_SEGS);
    rejectmatrix = W_CacheLumpNum(lumpnum + ML_REJECT, PU_LEVEL);
    P_GroupLines();
    bodyqueslot = 0;
    po_NumPolyobjs = 0;
//    deathmatch_p = deathmatchstarts;
    P_LoadThings(lumpnum + ML_THINGS);
    PO_Init(lumpnum + ML_THINGS);       // Initialize the polyobjs
    P_LoadACScripts(lumpnum + ML_BEHAVIOR);     // ACS object code
    //
    // End of map lump processing
    //

    // If deathmatch, randomly spawn the active players
    TimerGame = 0;
/*
    if (deathmatch)
    {
        for (i = 0; i < MAXPLAYERS; i++)
        {
            if (playeringame[i])
            {                   // must give a player spot before deathmatchspawn
                mobj = P_SpawnMobj(playerstarts[0][i].x << 16,
                                   playerstarts[0][i].y << 16, 0,
                                   MT_PLAYER_FIGHTER);
                players[i].mo = mobj;
                G_DeathMatchSpawnPlayer(i);
                P_RemoveMobj(mobj);
            }
        }

        //!
        // @arg <n>
        // @category net
        // @vanilla
        //
        // For multiplayer games: exit each level after n minutes.
        //
        parm = M_CheckParmWithArgs("-timer", 1);
        if (parm)
        {
            TimerGame = atoi(myargv[parm + 1]) * 35 * 60;
        }
    }
*/
// set up world state
    P_SpawnSpecials();

// build subsector connect matrix
//      P_ConnectSubsectors ();

// Load colormap and set the fullbright flag
    i = P_GetMapFadeTable(gamemap);
    W_ReadLump(i, colormaps);
    if (i == W_GetNumForName("COLORMAP"))
    {
        LevelUseFullBright = true;
    }
    else
    {                           // Probably fog ... don't use fullbright sprites
        LevelUseFullBright = false;
    }

// preload graphics
    if (precache)
        R_PrecacheLevel();

    // Check if the level is a lightning level
    P_InitLightning();

    S_StopAllSound();
    SN_StopAllSequences();
/*
    if(ninty == 1)
    {
	I_StopSong();

	sceRtcGetCurrentTick( &ninTickLast );

	ninResolution = sceRtcGetTickResolution();

	I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));

	for(j=0;j<6000000;j++)
	{
	    if( ((ninTickNow - ninTickLast)/((float)ninResolution)) >= 1.0f )
	    {
		ninTickLast = ninTickNow;

		V_DrawPatch(0, 0, 0, blank);

		#define MAIN1 "APPROACHING"
		#define MAIN2 "PRESS ANY BUTTON TO CONTINUE"

		MN_DrTextA(MAIN1,160-(MN_TextAWidth(MAIN1)>>1), 40);
		MN_DrTextA(MAIN2,160-(MN_TextAWidth(MAIN2)>>1), 190);

		if(!datadisc && map==1)
		{
		    #define WH1 "WINNOWING HALL"
		    #define WH2 "AMONG THE FIRST PLACES SET UPON BY"
		    #define WH3 "KORAX'S MINIONS, THE CHAPELS BELL"
		    #define WH4 "SOUNDS AS A DEATH KNELL FOR THE DOOM"
		    #define WH5 "ABOUT TO BEFALL THE REST OF HEXEN."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(WH1,160-(MN_TextAWidth(WH1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(WH1,160-(MN_TextAWidth(WH1)>>1), 50);

		    MN_DrTextA(WH2,160-(MN_TextAWidth(WH2)>>1), 70);
		    MN_DrTextA(WH3,160-(MN_TextAWidth(WH3)>>1), 80);
		    MN_DrTextA(WH4,160-(MN_TextAWidth(WH4)>>1), 90);
		    MN_DrTextA(WH5,160-(MN_TextAWidth(WH5)>>1), 100);
		}
		else if(!datadisc && map==2)
		{
		    #define SP1 "SEVEN PORTALS"
		    #define SP2 "THE SEVEN DOORS ONCE GUARDED HEXEN"
		    #define SP3 "FROM OUTSIDE INVASION. FORCES THAT"
		    #define SP4 "HAD SERVED THE WORLD NOW BAR YOUR WAY"
		    #define SP5 "AND HIDE A VALUABLE SECRET."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(SP1,160-(MN_TextAWidth(SP1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(SP1,160-(MN_TextAWidth(SP1)>>1), 50);

		    MN_DrTextA(SP2,160-(MN_TextAWidth(SP2)>>1), 70);
		    MN_DrTextA(SP3,160-(MN_TextAWidth(SP3)>>1), 80);
		    MN_DrTextA(SP4,160-(MN_TextAWidth(SP4)>>1), 90);
		    MN_DrTextA(SP5,160-(MN_TextAWidth(SP5)>>1), 100);
		}
		else if(!datadisc && map==3)
		{
		    #define GI1 "GUARDIAN OF ICE"
		    #define GI2 "IN THESE FROZEN CAVERNS, RELENTLESS"
		    #define GI3 "ICE IS THE SENTINEL THAT WILL CRUSH"
		    #define GI4 "ANY WHO ATTEMPT TO SLIP BY AND UNLOCK"
		    #define GI5 "ITS SECRETS."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(GI1,160-(MN_TextAWidth(GI1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(GI1,160-(MN_TextAWidth(GI1)>>1), 50);

		    MN_DrTextA(GI2,160-(MN_TextAWidth(GI2)>>1), 70);
		    MN_DrTextA(GI3,160-(MN_TextAWidth(GI3)>>1), 80);
		    MN_DrTextA(GI4,160-(MN_TextAWidth(GI4)>>1), 90);
		    MN_DrTextA(GI5,160-(MN_TextAWidth(GI5)>>1), 100);
		}
		else if(!datadisc && map==4)
		{
		    #define GF1 "GUARDIAN OF FIRE"
		    #define GF2 "IN THE MOLTEN DEPTHS OF THE WORLD,"
		    #define GF3 "DEATH WEARS A MASK OF LAVA AND FIRE."
		    #define GF4 "YOU MUST BRAVE SEARING DESTRUCTION"
		    #define GF5 "IN ORDER TO CONTINUE ONWARD."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(GF1,160-(MN_TextAWidth(GF1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(GF1,160-(MN_TextAWidth(GF1)>>1), 50);

		    MN_DrTextA(GF2,160-(MN_TextAWidth(GF2)>>1), 70);
		    MN_DrTextA(GF3,160-(MN_TextAWidth(GF3)>>1), 80);
		    MN_DrTextA(GF4,160-(MN_TextAWidth(GF4)>>1), 90);
		    MN_DrTextA(GF5,160-(MN_TextAWidth(GF5)>>1), 100);
		}
		else if(!datadisc && map==5)
		{
		    #define GS1 "GUARDIAN OF STEEL"
		    #define GS2 "COLD STEEL, ONCE SERVING THE MIGHT OF"
		    #define GS3 "HEXEN, NOW STANDS ARRAYED AGAINST"
		    #define GS4 "THOSE WHO WOULD LIBERATE THE WORLD"
		    #define GS5 "FROM KORAX."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(GS1,160-(MN_TextAWidth(GS1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(GS1,160-(MN_TextAWidth(GS1)>>1), 50);

		    MN_DrTextA(GS2,160-(MN_TextAWidth(GS2)>>1), 70);
		    MN_DrTextA(GS3,160-(MN_TextAWidth(GS3)>>1), 80);
		    MN_DrTextA(GS4,160-(MN_TextAWidth(GS4)>>1), 90);
		    MN_DrTextA(GS5,160-(MN_TextAWidth(GS5)>>1), 100);
		}
		else if(!datadisc && map==6)
		{
		    #define BC1 "BRIGHT CRUCIBLE"
		    #define BC2 "OTHER HORSEMEN HAVE ATTEMPTED TO"
		    #define BC3 "CONQUER WORLDS ONLY TO BE DEFEATED."
		    #define BC4 "HERE IN DEADLY RADIANCE LIES THE"
		    #define BC5 "HEART OF D'SPARIL, FIRST OF THE"
		    #define BC6 "SERPENT RIDERS TO FALL IN BATTLE."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(BC1,160-(MN_TextAWidth(BC1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(BC1,160-(MN_TextAWidth(BC1)>>1), 50);

		    MN_DrTextA(BC2,160-(MN_TextAWidth(BC2)>>1), 70);
		    MN_DrTextA(BC3,160-(MN_TextAWidth(BC3)>>1), 80);
		    MN_DrTextA(BC4,160-(MN_TextAWidth(BC4)>>1), 90);
		    MN_DrTextA(BC5,160-(MN_TextAWidth(BC5)>>1), 100);
		    MN_DrTextA(BC6,160-(MN_TextAWidth(BC6)>>1), 110);
		}
		else if(!datadisc && map==8)
		{
		    #define DM1 "DARKMERE"
		    #define DM2 "TREACHEROUS SWAMPS HAVE BECOME EVEN"
		    #define DM3 "DEADLIER. THE ROT AND THE MIRE"
		    #define DM4 "OBSCURES MANY DANGERS AS WELL AS THE"
		    #define DM5 "MEANS OF PROGRESSION."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(DM1,160-(MN_TextAWidth(DM1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(DM1,160-(MN_TextAWidth(DM1)>>1), 50);

		    MN_DrTextA(DM2,160-(MN_TextAWidth(DM2)>>1), 70);
		    MN_DrTextA(DM3,160-(MN_TextAWidth(DM3)>>1), 80);
		    MN_DrTextA(DM4,160-(MN_TextAWidth(DM4)>>1), 90);
		    MN_DrTextA(DM5,160-(MN_TextAWidth(DM5)>>1), 100);
		}
		else if(!datadisc && map==9)
		{
		    #define CC1 "CAVES OF CIRCE"
		    #define CC2 "COUNTLESS TRAVELERS HAVE LOST THEIR"
		    #define CC3 "WAY IN THE CAVERNS, NEVER TO BE HEARD"
		    #define CC4 "FROM AGAIN. TO FIND THE KEY, THE"
		    #define CC5 "CAVES MUST BE EXPLORED AND THE"
		    #define CC6 "DANGERS FACED."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(CC1,160-(MN_TextAWidth(CC1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(CC1,160-(MN_TextAWidth(CC1)>>1), 50);

		    MN_DrTextA(CC2,160-(MN_TextAWidth(CC2)>>1), 70);
		    MN_DrTextA(CC3,160-(MN_TextAWidth(CC3)>>1), 80);
		    MN_DrTextA(CC4,160-(MN_TextAWidth(CC4)>>1), 90);
		    MN_DrTextA(CC5,160-(MN_TextAWidth(CC5)>>1), 100);
		    MN_DrTextA(CC6,160-(MN_TextAWidth(CC6)>>1), 110);
		}
		else if(!datadisc && map==10)
		{
		    #define WL1 "WASTELANDS"
		    #define WL2 "THESE ONCE FERTILE LANDS HAVE BEEN"
		    #define WL3 "RENDERED BARREN AND LIFELESS EXCEPT"
		    #define WL4 "FOR THOSE CREATURES SERVING KORAX."
		    #define WL5 "THE PLANET HAS BEEN SPLIT ASUNDER"
		    #define WL6 "HERE AND PITFALLS ABOUND TO SWALLOW"
		    #define WL7 "THE UNWARY."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(WL1,160-(MN_TextAWidth(WL1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(WL1,160-(MN_TextAWidth(WL1)>>1), 50);

		    MN_DrTextA(WL2,160-(MN_TextAWidth(WL2)>>1), 70);
		    MN_DrTextA(WL3,160-(MN_TextAWidth(WL3)>>1), 80);
		    MN_DrTextA(WL4,160-(MN_TextAWidth(WL4)>>1), 90);
		    MN_DrTextA(WL5,160-(MN_TextAWidth(WL5)>>1), 100);
		    MN_DrTextA(WL6,160-(MN_TextAWidth(WL6)>>1), 110);
		    MN_DrTextA(WL7,160-(MN_TextAWidth(WL7)>>1), 120);
		}
		else if(!datadisc && map==11)
		{
		    #define SG1 "SACRED GROVE"
		    #define SG2 "EVEN THE MOST HALLOWED SPOTS HAVE NOT"
		    #define SG3 "BEEN SPARED DESECRATION. HOWEVER,"
		    #define SG4 "DIVINE AID LIES HIDDEN FOR HE THAT"
		    #define SG5 "REDEEMS THIS PLACE AND FORCES SET IN"
		    #define SG6 "MOTION HERE WILL REVEAL FUTURE"
		    #define SG7 "SECRETS."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(SG1,160-(MN_TextAWidth(SG1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(SG1,160-(MN_TextAWidth(SG1)>>1), 50);

		    MN_DrTextA(SG2,160-(MN_TextAWidth(SG2)>>1), 70);
		    MN_DrTextA(SG3,160-(MN_TextAWidth(SG3)>>1), 80);
		    MN_DrTextA(SG4,160-(MN_TextAWidth(SG4)>>1), 90);
		    MN_DrTextA(SG5,160-(MN_TextAWidth(SG5)>>1), 100);
		    MN_DrTextA(SG6,160-(MN_TextAWidth(SG6)>>1), 110);
		    MN_DrTextA(SG7,160-(MN_TextAWidth(SG7)>>1), 120);
		}
		else if(!datadisc && map==12)
		{
		    #define HS1 "HYPOSTYLE"
		    #define HS2 "LOCKED AWAY, A SIEGE BEAST,"
		    #define HS3 "SLUMBERING FOR AGES, HAS AWOKEN. THE"
		    #define HS4 "FOUR POINTS OF THE STAR MUST BE"
		    #define HS5 "CONQUERED BEFORE WINGED DEATH CAN BE"
		    #define HS6 "CONFRONTED."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(HS1,160-(MN_TextAWidth(HS1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(HS1,160-(MN_TextAWidth(HS1)>>1), 50);

		    MN_DrTextA(HS2,160-(MN_TextAWidth(HS2)>>1), 70);
		    MN_DrTextA(HS3,160-(MN_TextAWidth(HS3)>>1), 80);
		    MN_DrTextA(HS4,160-(MN_TextAWidth(HS4)>>1), 90);
		    MN_DrTextA(HS5,160-(MN_TextAWidth(HS5)>>1), 100);
		    MN_DrTextA(HS6,160-(MN_TextAWidth(HS6)>>1), 110);
		}
		else if(!datadisc && map==13)
		{
		    #define SW1 "SHADOW WOOD"
		    #define SW2 "NATURE ITSELF HAS BEEN CORRUPTED INTO"
		    #define SW3 "SOMETHING SINISTER. TO ESCAPE THIS"
		    #define SW4 "DARK FOREST, YOU MUST RECOVER THE"
		    #define SW5 "SECRETS OF THE HORN, THE SWAMP, AND"
		    #define SW6 "THE CAVES."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(SW1,160-(MN_TextAWidth(SW1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(SW1,160-(MN_TextAWidth(SW1)>>1), 50);

		    MN_DrTextA(SW2,160-(MN_TextAWidth(SW2)>>1), 70);
		    MN_DrTextA(SW3,160-(MN_TextAWidth(SW3)>>1), 80);
		    MN_DrTextA(SW4,160-(MN_TextAWidth(SW4)>>1), 90);
		    MN_DrTextA(SW5,160-(MN_TextAWidth(SW5)>>1), 100);
		    MN_DrTextA(SW6,160-(MN_TextAWidth(SW6)>>1), 110);
		}
		else if(!datadisc && map==21)
		{
		    #define FO1 "FORSAKEN OUTPOST"
		    #define FO2 "TWO TOMES OF OBSCURE AND ANCIENT"
		    #define FO3 "KNOWLEDGE LIE FORGOTTEN WITHIN THIS"
		    #define FO4 "BASE ABANDONED IN DESPAIR THAT WILL"
		    #define FO5 "HELP YOU IN YOUR QUEST."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(FO1,160-(MN_TextAWidth(FO1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(FO1,160-(MN_TextAWidth(FO1)>>1), 50);

		    MN_DrTextA(FO2,160-(MN_TextAWidth(FO2)>>1), 70);
		    MN_DrTextA(FO3,160-(MN_TextAWidth(FO3)>>1), 80);
		    MN_DrTextA(FO4,160-(MN_TextAWidth(FO4)>>1), 90);
		    MN_DrTextA(FO5,160-(MN_TextAWidth(FO5)>>1), 100);
		}
		else if(!datadisc && map==22)
		{
		    #define CG1 "CASTLE OF GRIEF"
		    #define CG2 "IN THIS FORTRESS OF PAIN, SUFFERING"
		    #define CG3 "LASTS AN ETERNITY AND ONLY TIME CAN"
		    #define CG4 "TELL IF YOU WILL PERSEVERE TO VENTURE"
		    #define CG5 "ONWARD."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(CG1,160-(MN_TextAWidth(CG1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(CG1,160-(MN_TextAWidth(CG1)>>1), 50);

		    MN_DrTextA(CG2,160-(MN_TextAWidth(CG2)>>1), 70);
		    MN_DrTextA(CG3,160-(MN_TextAWidth(CG3)>>1), 80);
		    MN_DrTextA(CG4,160-(MN_TextAWidth(CG4)>>1), 90);
		    MN_DrTextA(CG5,160-(MN_TextAWidth(CG5)>>1), 100);
		}
		else if(!datadisc && map==23)
		{
		    #define GT1 "GIBBET"
		    #define GT2 "MANY HAVE LOST THEIR HEADS AND THEIR"
		    #define GT3 "LIVES IN THIS KILLING GROUND."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(GT1,160-(MN_TextAWidth(GT1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(GT1,160-(MN_TextAWidth(GT1)>>1), 50);

		    MN_DrTextA(GT2,160-(MN_TextAWidth(GT2)>>1), 70);
		    MN_DrTextA(GT3,160-(MN_TextAWidth(GT3)>>1), 80);
		}
		else if(!datadisc && map==24)
		{
		    #define EF1 "EFFLUVIUM"
		    #define EF2 "THE MUCK AND SEWAGE PROPHESY WHAT"
		    #define EF3 "WILL BECOME OF HEXEN UNLESS KORAX IS"
		    #define EF4 "STOPPED. YET EVEN HERE, IT IS"
		    #define EF5 "POSSIBLE TO FIND A KEY TO MOVING ON."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(EF1,160-(MN_TextAWidth(EF1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(EF1,160-(MN_TextAWidth(EF1)>>1), 50);

		    MN_DrTextA(EF2,160-(MN_TextAWidth(EF2)>>1), 70);
		    MN_DrTextA(EF3,160-(MN_TextAWidth(EF3)>>1), 80);
		    MN_DrTextA(EF4,160-(MN_TextAWidth(EF4)>>1), 90);
		    MN_DrTextA(EF5,160-(MN_TextAWidth(EF5)>>1), 100);
		}
		else if(!datadisc && map==25)
		{
		    #define DS1 "DUNGEONS"
		    #define DS2 "IN THIS PRISON'S LONG AND BLOODY"
		    #define DS3 "HISTORY, ONLY A SINGLE CAPTIVE EVER"
		    #define DS4 "ESCAPED. WITH NO WITNESSES SAVE FOR"
		    #define DS5 "THE PRISONER'S CAGE, THE MEANS OF THE"
		    #define DS6 "BREAKOUT REMAINS A MYSTERY."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(DS1,160-(MN_TextAWidth(DS1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(DS1,160-(MN_TextAWidth(DS1)>>1), 50);

		    MN_DrTextA(DS2,160-(MN_TextAWidth(DS2)>>1), 70);
		    MN_DrTextA(DS3,160-(MN_TextAWidth(DS3)>>1), 80);
		    MN_DrTextA(DS4,160-(MN_TextAWidth(DS4)>>1), 90);
		    MN_DrTextA(DS5,160-(MN_TextAWidth(DS5)>>1), 100);
		    MN_DrTextA(DS6,160-(MN_TextAWidth(DS6)>>1), 110);
		}
		else if(!datadisc && map==26)
		{
		    #define DG1 "DESOLATE GARDEN"
		    #define DG2 "THOUGH DEVOID OF VEGETATION, THINGS"
		    #define DG3 "STILL SPROUT WITHIN THE GARDEN. IF"
		    #define DG4 "YOU SURVIVE THE DEADLY CROP, YOU WILL"
		    #define DG5 "FIND PRECIOUS AID TO YOUR QUEST."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(DG1,160-(MN_TextAWidth(DG1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(DG1,160-(MN_TextAWidth(DG1)>>1), 50);

		    MN_DrTextA(DG2,160-(MN_TextAWidth(DG2)>>1), 70);
		    MN_DrTextA(DG3,160-(MN_TextAWidth(DG3)>>1), 80);
		    MN_DrTextA(DG4,160-(MN_TextAWidth(DG4)>>1), 90);
		    MN_DrTextA(DG5,160-(MN_TextAWidth(DG5)>>1), 100);
		}
		else if(!datadisc && map==27)
		{
		    #define HSS1 "HERESIARCH'S SEMINARY"
		    #define HSS2 "HEXEN'S RELIGIOUS ORDER HAS BEEN"
		    #define HSS3 "TURNED TOWARD DARKNESS AND THE"
		    #define HSS4 "HEAVENS ARE IN CHAOS. THE HERESIARCH"
		    #define HSS5 "NOW STANDS AGAINST ALL WHO WOULD"
		    #define HSS6 "BRING SALVATION TO HER PEOPLE."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(HSS1,160-(MN_TextAWidth(HSS1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(HSS1,160-(MN_TextAWidth(HSS1)>>1), 50);

		    MN_DrTextA(HSS2,160-(MN_TextAWidth(HSS2)>>1), 70);
		    MN_DrTextA(HSS3,160-(MN_TextAWidth(HSS3)>>1), 80);
		    MN_DrTextA(HSS4,160-(MN_TextAWidth(HSS4)>>1), 90);
		    MN_DrTextA(HSS5,160-(MN_TextAWidth(HSS5)>>1), 100);
		    MN_DrTextA(HSS6,160-(MN_TextAWidth(HSS6)>>1), 110);
		}
		else if(!datadisc && map==28)
		{
		    #define DC1 "DRAGON CHAPEL"
		    #define DC2 "DEATH IS STRAIGHTFORWARD AND DIRECT"
		    #define DC3 "WITHIN THE BASILICA OF THE GREAT"
		    #define DC4 "WORM."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(DC1,160-(MN_TextAWidth(DC1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(DC1,160-(MN_TextAWidth(DC1)>>1), 50);

		    MN_DrTextA(DC2,160-(MN_TextAWidth(DC2)>>1), 70);
		    MN_DrTextA(DC3,160-(MN_TextAWidth(DC3)>>1), 80);
		    MN_DrTextA(DC4,160-(MN_TextAWidth(DC4)>>1), 90);
		}
		else if(!datadisc && map==30)
		{
		    #define GC1 "GRIFFIN CHAPEL"
		    #define GC2 "IN THE TEMPLE DEDICATED TO THE WINGED"
		    #define GC3 "LION, AN ACT OF FAITH IS REQUIRED OF"
		    #define GC4 "ALL WHO WOULD SPAN THE BOUNDARIES OF"
		    #define GC5 "LIFE AND DEATH."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(GC1,160-(MN_TextAWidth(GC1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(GC1,160-(MN_TextAWidth(GC1)>>1), 50);

		    MN_DrTextA(GC2,160-(MN_TextAWidth(GC2)>>1), 70);
		    MN_DrTextA(GC3,160-(MN_TextAWidth(GC3)>>1), 80);
		    MN_DrTextA(GC4,160-(MN_TextAWidth(GC4)>>1), 90);
		    MN_DrTextA(GC5,160-(MN_TextAWidth(GC5)>>1), 100);
		}
		else if(!datadisc && map==31)
		{
		    #define DWC1 "DEATHWIND CHAPEL"
		    #define DWC2 "WITH THE HERESIARCH DEFEATED, WHAT"
		    #define DWC3 "TREASURES LIE WITHIN HIS INNER"
		    #define DWC4 "SANCTUM? TO DISCOVER THIS, YOU MUST"
		    #define DWC5 "OVERCOME FORCES WITH HEARTS AS BLACK"
		    #define DWC6 "AS THEIR FORMER MASTER."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(DWC1,160-(MN_TextAWidth(DWC1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(DWC1,160-(MN_TextAWidth(DWC1)>>1), 50);

		    MN_DrTextA(DWC2,160-(MN_TextAWidth(DWC2)>>1), 70);
		    MN_DrTextA(DWC3,160-(MN_TextAWidth(DWC3)>>1), 80);
		    MN_DrTextA(DWC4,160-(MN_TextAWidth(DWC4)>>1), 90);
		    MN_DrTextA(DWC5,160-(MN_TextAWidth(DWC5)>>1), 100);
		    MN_DrTextA(DWC6,160-(MN_TextAWidth(DWC6)>>1), 110);
		}
		else if(!datadisc && map==32)
		{
		    #define OL1 "ORCHARD OF LAMENTATIONS"
		    #define OL2 "IN THIS GROVE OF SORROW, TWO PLANETS,"
		    #define OL3 "ONE GREEN ONE BLUE, CLEVERLY"
		    #define OL4 "CONCEALED FROM THE LIKES OF YOU..."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(OL1,160-(MN_TextAWidth(OL1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(OL1,160-(MN_TextAWidth(OL1)>>1), 50);

		    MN_DrTextA(OL2,160-(MN_TextAWidth(OL2)>>1), 70);
		    MN_DrTextA(OL3,160-(MN_TextAWidth(OL3)>>1), 80);
		    MN_DrTextA(OL4,160-(MN_TextAWidth(OL4)>>1), 90);
		}
		else if(!datadisc && map==33)
		{
		    #define SF1 "SILENT REFECTORY"
		    #define SF2 "WITHIN THIS REPOSITORY OF MYSTIC"
		    #define SF3 "ARTIFACTS AND TREASURE, THE SAPPHIRE,"
		    #define SF4 "THE EMERALD, AND THE RUBY ARE YOURS"
		    #define SF5 "TO COLLECT IF YOU CAN SURVIVE."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(SF1,160-(MN_TextAWidth(SF1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(SF1,160-(MN_TextAWidth(SF1)>>1), 50);

		    MN_DrTextA(SF2,160-(MN_TextAWidth(SF2)>>1), 70);
		    MN_DrTextA(SF3,160-(MN_TextAWidth(SF3)>>1), 80);
		    MN_DrTextA(SF4,160-(MN_TextAWidth(SF4)>>1), 90);
		    MN_DrTextA(SF5,160-(MN_TextAWidth(SF5)>>1), 100);
		}
		else if(!datadisc && map==34)
		{
		    #define WC1 "WOLF CHAPEL"
		    #define WC2 "THE SAVAGERY AND CUNNING OF THE WOLF"
		    #define WC3 "ARE WORSHIPPED HERE. TO TRIUMPH HERE,"
		    #define WC4 "ONE MUST MATCH THE FEROCITY OF THE"
		    #define WC5 "ENEMIES ENCOUNTERED."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(WC1,160-(MN_TextAWidth(WC1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(WC1,160-(MN_TextAWidth(WC1)>>1), 50);

		    MN_DrTextA(WC2,160-(MN_TextAWidth(WC2)>>1), 70);
		    MN_DrTextA(WC3,160-(MN_TextAWidth(WC3)>>1), 80);
		    MN_DrTextA(WC4,160-(MN_TextAWidth(WC4)>>1), 90);
		    MN_DrTextA(WC5,160-(MN_TextAWidth(WC5)>>1), 100);
		}
		else if(!datadisc && map==35)
		{
		    #define NS1 "NECROPOLIS"
		    #define NS2 "WITHIN THE HALLS OF THE DEAD, YOUR"
		    #define NS3 "FORMER MASTERS AWAIT YOU. IN THE END,"
		    #define NS4 "YOU MUST EMERGE VICTORIOUS OR ALL"
		    #define NS5 "YOUR STRUGGLES WILL HAVE BEEN FOR"
		    #define NS6 "NAUGHT."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(NS1,160-(MN_TextAWidth(NS1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(NS1,160-(MN_TextAWidth(NS1)>>1), 50);

		    MN_DrTextA(NS2,160-(MN_TextAWidth(NS2)>>1), 70);
		    MN_DrTextA(NS3,160-(MN_TextAWidth(NS3)>>1), 80);
		    MN_DrTextA(NS4,160-(MN_TextAWidth(NS4)>>1), 90);
		    MN_DrTextA(NS5,160-(MN_TextAWidth(NS5)>>1), 100);
		    MN_DrTextA(NS6,160-(MN_TextAWidth(NS6)>>1), 110);
		}
		else if(!datadisc && map==36)
		{
		    #define ZT1 "ZEDEK'S TOMB"
		    #define ZT2 "ONLY THOSE WHO KNOW THE WARRIOR'S"
		    #define ZT3 "CODE ARE WORTHY OF FACING ZEDEK,"
		    #define ZT4 "MARSHALL OF THE LEGION, IN PERSONAL"
		    #define ZT5 "COMBAT."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(ZT1,160-(MN_TextAWidth(ZT1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(ZT1,160-(MN_TextAWidth(ZT1)>>1), 50);

		    MN_DrTextA(ZT2,160-(MN_TextAWidth(ZT2)>>1), 70);
		    MN_DrTextA(ZT3,160-(MN_TextAWidth(ZT3)>>1), 80);
		    MN_DrTextA(ZT4,160-(MN_TextAWidth(ZT4)>>1), 90);
		    MN_DrTextA(ZT5,160-(MN_TextAWidth(ZT5)>>1), 100);
		}
		else if(!datadisc && map==37)
		{
		    #define MT1 "MENELKIR'S TOMB"
		    #define MT2 "ARCANE RUNES CHART THE WAY TO THE"
		    #define MT3 "ARCH-MAGE OF THE ARCANUM, MENELKIR,"
		    #define MT4 "WHO WAITS TO UNLEASH HIS SORCEROUS"
		    #define MT5 "MIGHT UPON ANY WHO DARE CONFRONT HIM."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(MT1,160-(MN_TextAWidth(MT1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(MT1,160-(MN_TextAWidth(MT1)>>1), 50);

		    MN_DrTextA(MT2,160-(MN_TextAWidth(MT2)>>1), 70);
		    MN_DrTextA(MT3,160-(MN_TextAWidth(MT3)>>1), 80);
		    MN_DrTextA(MT4,160-(MN_TextAWidth(MT4)>>1), 90);
		    MN_DrTextA(MT5,160-(MN_TextAWidth(MT5)>>1), 100);
		}
		else if(!datadisc && map==38)
		{
		    #define TT1 "TRADUCTUS' TOMB"
		    #define TT2 "TRADUCTUS, GRAND PATRIARCH OF THE"
		    #define TT3 "CHURCH, STANDS READY TO SMITE WITH"
		    #define TT4 "UNHOLY RETRIBUTION HE WHO DIVINES THE"
		    #define TT5 "CORRECT PATH."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(TT1,160-(MN_TextAWidth(TT1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(TT1,160-(MN_TextAWidth(TT1)>>1), 50);

		    MN_DrTextA(TT2,160-(MN_TextAWidth(TT2)>>1), 70);
		    MN_DrTextA(TT3,160-(MN_TextAWidth(TT3)>>1), 80);
		    MN_DrTextA(TT4,160-(MN_TextAWidth(TT4)>>1), 90);
		    MN_DrTextA(TT5,160-(MN_TextAWidth(TT5)>>1), 100);
		}
		else if(!datadisc && map==39)
		{
		    #define VV1 "VIVARIUM"
		    #define VV2 "VALUABLE ITEMS IN THE STRUGGLE FOR"
		    #define VV3 "LIFE ARE AVAILABLE TO THOSE WHOSE AIM"
		    #define VV4 "IS TRUE, BUT ONCE YOU MOVE ON, THERE"
		    #define VV5 "IS NO TURNING BACK."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(VV1,160-(MN_TextAWidth(VV1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(VV1,160-(MN_TextAWidth(VV1)>>1), 50);

		    MN_DrTextA(VV2,160-(MN_TextAWidth(VV2)>>1), 70);
		    MN_DrTextA(VV3,160-(MN_TextAWidth(VV3)>>1), 80);
		    MN_DrTextA(VV4,160-(MN_TextAWidth(VV4)>>1), 90);
		    MN_DrTextA(VV5,160-(MN_TextAWidth(VV5)>>1), 100);
		}
		else if(!datadisc && map==40)
		{
		    #define DCB1 "DARK CRUCIBLE"
		    #define DCB2 "AT LAST, YOU NOW STAND POISED TO"
		    #define DCB3 "BATTLE KORAX, THE SECOND SERPENT"
		    #define DCB4 "RIDER. YOUR FATE SHALL BE ETERNAL"
		    #define DCB5 "GLORY OR EXCRUCIATING DEATH."

		    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
			MN_DrTextAYellow(DCB1,160-(MN_TextAWidth(DCB1)>>1), 50);
		    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
			MN_DrTextA(DCB1,160-(MN_TextAWidth(DCB1)>>1), 50);

		    MN_DrTextA(DCB2,160-(MN_TextAWidth(DCB2)>>1), 70);
		    MN_DrTextA(DCB3,160-(MN_TextAWidth(DCB3)>>1), 80);
		    MN_DrTextA(DCB4,160-(MN_TextAWidth(DCB4)>>1), 90);
		    MN_DrTextA(DCB5,160-(MN_TextAWidth(DCB5)>>1), 100);
		}

		I_FinishUpdate(1);

		if(j==6000000)
		    break;
	    }
	}
    }
*/
    from_menu = false;
    S_StartSong(gamemap, true);

//printf ("free memory: 0x%x\n", Z_FreeMemory());

}

//==========================================================================
//
// InitMapInfo
//
//==========================================================================

static void InitMapInfo(void)
{
    int map;
    int mapMax;
    int mcmdValue;
    mapInfo_t *info;
    char songMulch[10];

    mapMax = 1;

    // Put defaults into MapInfo[0]
    info = MapInfo;
    info->cluster = 0;
    info->warpTrans = 0;
    info->nextMap = 1;          // Always go to map 1 if not specified
    info->cdTrack = 1;

    if(HEXEN_DEMO || HEXEN_BETA_DEMO)
	info->sky1Texture = R_TextureNumForName(DEFAULT_SKY_NAME_2);
    else
	info->sky1Texture = R_TextureNumForName(DEFAULT_SKY_NAME_1);

    info->sky2Texture = info->sky1Texture;
    info->sky1ScrollDelta = 0;
    info->sky2ScrollDelta = 0;
    info->doubleSky = false;
    info->lightning = false;
    info->fadetable = W_GetNumForName(DEFAULT_FADE_TABLE);
    strcpy(info->name, UNKNOWN_MAP_NAME);

//      strcpy(info->songLump, DEFAULT_SONG_LUMP);
    SC_Open(MAPINFO_SCRIPT_NAME);
    while (SC_GetString())
    {
        if (SC_Compare("MAP") == false)
        {
            SC_ScriptError(NULL);
        }
        SC_MustGetNumber();
        if (sc_Number < 1 || sc_Number > 99)
        {                       // 
            SC_ScriptError(NULL);
        }
        map = sc_Number;

        info = &MapInfo[map];

        // Save song lump name
        strcpy(songMulch, info->songLump);

        // Copy defaults to current map definition
        memcpy(info, &MapInfo[0], sizeof(*info));

        // Restore song lump name
        strcpy(info->songLump, songMulch);

        // The warp translation defaults to the map number
        info->warpTrans = map;

        // Map name must follow the number
        SC_MustGetString();
        strcpy(info->name, sc_String);

        // Process optional tokens
        while (SC_GetString())
        {
            if (SC_Compare("MAP"))
            {                   // Start next map definition
                SC_UnGet();
                break;
            }
            mcmdValue = MapCmdIDs[SC_MustMatchString(MapCmdNames)];
            switch (mcmdValue)
            {
                case MCMD_CLUSTER:
                    SC_MustGetNumber();
                    info->cluster = sc_Number;
                    break;
                case MCMD_WARPTRANS:
                    SC_MustGetNumber();
                    info->warpTrans = sc_Number;
                    break;
                case MCMD_NEXT:
                    SC_MustGetNumber();
                    info->nextMap = sc_Number;
                    break;
                case MCMD_CDTRACK:
                    SC_MustGetNumber();
                    info->cdTrack = sc_Number;
                    break;
                case MCMD_SKY1:
                    SC_MustGetString();
                    info->sky1Texture = R_TextureNumForName(sc_String);
                    SC_MustGetNumber();
                    info->sky1ScrollDelta = sc_Number << 8;
                    break;
                case MCMD_SKY2:
                    SC_MustGetString();
                    info->sky2Texture = R_TextureNumForName(sc_String);
                    SC_MustGetNumber();
                    info->sky2ScrollDelta = sc_Number << 8;
                    break;
                case MCMD_DOUBLESKY:
                    info->doubleSky = true;
                    break;
                case MCMD_LIGHTNING:
                    info->lightning = true;
                    break;
                case MCMD_FADETABLE:
                    SC_MustGetString();
                    info->fadetable = W_GetNumForName(sc_String);
                    break;
                case MCMD_CD_STARTTRACK:
                case MCMD_CD_END1TRACK:
                case MCMD_CD_END2TRACK:
                case MCMD_CD_END3TRACK:
                case MCMD_CD_INTERTRACK:
                case MCMD_CD_TITLETRACK:
                    SC_MustGetNumber();
                    cd_NonLevelTracks[mcmdValue - MCMD_CD_STARTTRACK] =
                        sc_Number;
                    break;
            }
        }
        mapMax = map > mapMax ? map : mapMax;
    }
    SC_Close();
    MapCount = mapMax;
}

//==========================================================================
//
// P_GetMapCluster
//
//==========================================================================

int P_GetMapCluster(int map)
{
    return MapInfo[QualifyMap(map)].cluster;
}

//==========================================================================
//
// P_GetMapCDTrack
//
//==========================================================================

int P_GetMapCDTrack(int map)
{
    return MapInfo[QualifyMap(map)].cdTrack;
}

//==========================================================================
//
// P_GetMapWarpTrans
//
//==========================================================================

int P_GetMapWarpTrans(int map)
{
    return MapInfo[QualifyMap(map)].warpTrans;
}

//==========================================================================
//
// P_GetMapNextMap
//
//==========================================================================

int P_GetMapNextMap(int map)
{
    return MapInfo[QualifyMap(map)].nextMap;
}

//==========================================================================
//
// P_TranslateMap
//
// Returns the actual map number given a warp map number.
//
//==========================================================================

int P_TranslateMap(int map)
{
    int i;

    for (i = 1; i < 99; i++)    // Make this a macro
    {
        if (MapInfo[i].warpTrans == map)
        {
            return i;
        }
    }
    // Not found
    return -1;
}

//==========================================================================
//
// P_GetMapSky1Texture
//
//==========================================================================

int P_GetMapSky1Texture(int map)
{
    return MapInfo[QualifyMap(map)].sky1Texture;
}

//==========================================================================
//
// P_GetMapSky2Texture
//
//==========================================================================

int P_GetMapSky2Texture(int map)
{
    return MapInfo[QualifyMap(map)].sky2Texture;
}

//==========================================================================
//
// P_GetMapName
//
//==========================================================================

char *P_GetMapName(int map)
{
    return MapInfo[QualifyMap(map)].name;
}

//==========================================================================
//
// P_GetMapSky1ScrollDelta
//
//==========================================================================

fixed_t P_GetMapSky1ScrollDelta(int map)
{
    return MapInfo[QualifyMap(map)].sky1ScrollDelta;
}

//==========================================================================
//
// P_GetMapSky2ScrollDelta
//
//==========================================================================

fixed_t P_GetMapSky2ScrollDelta(int map)
{
    return MapInfo[QualifyMap(map)].sky2ScrollDelta;
}

//==========================================================================
//
// P_GetMapDoubleSky
//
//==========================================================================

boolean P_GetMapDoubleSky(int map)
{
    return MapInfo[QualifyMap(map)].doubleSky;
}

//==========================================================================
//
// P_GetMapLightning
//
//==========================================================================

boolean P_GetMapLightning(int map)
{
    return MapInfo[QualifyMap(map)].lightning;
}

//==========================================================================
//
// P_GetMapFadeTable
//
//==========================================================================

boolean P_GetMapFadeTable(int map)
{
    return MapInfo[QualifyMap(map)].fadetable;
}

//==========================================================================
//
// P_GetMapSongLump
//
//==========================================================================

char *P_GetMapSongLump(int map)
{
    if (!strcasecmp(MapInfo[QualifyMap(map)].songLump, DEFAULT_SONG_LUMP))
    {
        return NULL;
    }
    else
    {
        return MapInfo[QualifyMap(map)].songLump;
    }
}

//==========================================================================
//
// P_PutMapSongLump
//
//==========================================================================

void P_PutMapSongLump(int map, char *lumpName)
{
    if (map < 1 || map > MapCount)
    {
        return;
    }
    strcpy(MapInfo[map].songLump, lumpName);
}

//==========================================================================
//
// P_GetCDStartTrack
//
//==========================================================================

int P_GetCDStartTrack(void)
{
    return cd_NonLevelTracks[MCMD_CD_STARTTRACK - MCMD_CD_STARTTRACK];
}

//==========================================================================
//
// P_GetCDEnd1Track
//
//==========================================================================

int P_GetCDEnd1Track(void)
{
    return cd_NonLevelTracks[MCMD_CD_END1TRACK - MCMD_CD_STARTTRACK];
}

//==========================================================================
//
// P_GetCDEnd2Track
//
//==========================================================================

int P_GetCDEnd2Track(void)
{
    return cd_NonLevelTracks[MCMD_CD_END2TRACK - MCMD_CD_STARTTRACK];
}

//==========================================================================
//
// P_GetCDEnd3Track
//
//==========================================================================

int P_GetCDEnd3Track(void)
{
    return cd_NonLevelTracks[MCMD_CD_END3TRACK - MCMD_CD_STARTTRACK];
}

//==========================================================================
//
// P_GetCDIntermissionTrack
//
//==========================================================================

int P_GetCDIntermissionTrack(void)
{
    return cd_NonLevelTracks[MCMD_CD_INTERTRACK - MCMD_CD_STARTTRACK];
}

//==========================================================================
//
// P_GetCDTitleTrack
//
//==========================================================================

int P_GetCDTitleTrack(void)
{
    return cd_NonLevelTracks[MCMD_CD_TITLETRACK - MCMD_CD_STARTTRACK];
}

//==========================================================================
//
// QualifyMap
//
//==========================================================================

static int QualifyMap(int map)
{
    return (map < 1 || map > MapCount) ? 0 : map;
}

//==========================================================================
//
// P_Init
//
//==========================================================================

void P_Init(void)
{
    InitMapInfo();
    P_InitSwitchList();
    P_InitFTAnims();            // Init flat and texture animations
    P_InitTerrainTypes();
    P_InitLava();
    R_InitSprites(sprnames);
}


// Special early initializer needed to start sound before R_Init()
void InitMapMusicInfo(void)
{
    int i;

    for (i = 0; i < 99; i++)
    {
        strcpy(MapInfo[i].songLump, DEFAULT_SONG_LUMP);
    }
    MapCount = 98;
}

/*
void My_Debug(void)
{
	int i;

	printf("My debug stuff ----------------------\n");
	printf("gamemap=%d\n",gamemap);
	for (i=0; i<10; i++)
	{
		printf("i=%d  songlump=%s\n",i,MapInfo[i].songLump);
	}
}
*/
