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

// haleyjd: removed WATCOMC
/*
#include <pspctrl.h>
#include <pspdebug.h>
#include <pspthreadman.h>
*/
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

//#include "config.h"
#include "doomfeatures.h"

#include "h2def.h"
//#include "ct_chat.h"
#include "d_iwad.h"
#include "d_mode.h"
#include "m_misc.h"
#include "s_sound.h"
//#include "i_joystick.h"
#include "i_system.h"
#include "i_timer.h"
#include "m_argv.h"
#include "m_config.h"
#include "m_controls.h"
//#include "net_client.h"
#include "p_local.h"
#include "v_video.h"
#include "w_main.h"
#include "w_wad.h"
#include "deh_str.h"
//#include "i_endoom.h"

// MACROS ------------------------------------------------------------------

#define PRESS_START_EN "PRESS START FOR MENU"

//#define printf pspDebugScreenPrintf
#define MAXWADFILES 20

/*
#define CT_KEY_BLUE         'b'
#define CT_KEY_RED          'r'
#define CT_KEY_YELLOW       'y'
#define CT_KEY_GREEN        'g'
#define CT_KEY_PLAYER5      'j'     // Jade
#define CT_KEY_PLAYER6      'w'     // White
#define CT_KEY_PLAYER7      'h'     // Hazel
#define CT_KEY_PLAYER8      'p'     // Purple
#define CT_KEY_ALL          't'
*/
// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

void R_ExecuteSetViewSize(void);
void D_ConnectNetGame(void);
void D_CheckNetGame(void);
boolean F_Responder(event_t * ev);
void I_StartupKeyboard(void);
void I_StartupJoystick(void);
void I_ShutdownKeyboard(void);
void S_InitScript(void);

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

void H2_ProcessEvents(void);
void H2_DoAdvanceDemo(void);
void H2_AdvanceDemo(void);
void H2_StartTitle(void);
void H2_PageTicker(void);

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

static void DrawMessage(void);
static void PageDrawer(void);
/*
static void HandleArgs(void);
static void CheckRecordFrom(void);
*/
static void DrawAndBlit(void);
static void CreateSavePath(void);
static void WarpCheck(void);

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

extern boolean automapactive;
extern boolean MenuActive;
extern boolean askforquit;
extern boolean use_alternate_startup;
extern boolean add_dk;

extern int loading_disk;
extern int warped;
//extern int ninty;

extern char calculated_md5_string[33];

// PUBLIC DATA DEFINITIONS -------------------------------------------------

GameMode_t gamemode;
skill_t startskill;
/*
FILE *debugfile;

SceCtrlData pad, lastpad;

boolean debugmode = false;      // checkparm of -debug
*/
boolean demos_are_disabled = true;
boolean do_not_repeat_music = false;

boolean nomonsters;             // checkparm of -nomonsters
boolean randomclass;            // checkparm of -randclass
boolean cmdfrag;                // true if a CMD_FRAG packet should be sent out
boolean singletics;             // debug flag to cancel adaptiveness
boolean artiskip;               // whether shift-enter skips an artifact
boolean autostart;
boolean advancedemo;

boolean HEXEN_BETA = false;
boolean HEXEN_BETA_DEMO = false;
boolean HEXEN_MACDEMO = false;
boolean HEXEN_MACFULL = false;
boolean HEXEN_DEMO = false;
boolean HEXEN_1_0 = false;
boolean HEXEN_1_1 = false;
boolean HEXDD_1_0 = false;
boolean HEXDD_1_1 = false;
boolean datadisc = false;
boolean main_loop_started = false;
/*
boolean respawnparm;            // checkparm of -respawn
boolean ravpic;                 // checkparm of -ravpic
boolean cdrom = false;          // true if cd-rom mode active
*/
int maxzone = 0x800000;         // Maximum allocated for zone heap (8meg default)
int startepisode;
int startmap;
int UpdateState;
//int exists;
//int existsdd;
int fsize = 0;
int fsizedd = 0;
int fsizerw = 0;
int resource_wad_exists = 0;
int show_endoom = 0;		// FIXME: DOESN'T WORK FOR THE WII

char *iwadfile;

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static int WarpMap;
static int demosequence;
static int pagetic;
static char *pagename;

// CODE --------------------------------------------------------------------

void D_BindVariables(void)
{
/*
    int i;

    M_ApplyPlatformDefaults();

    I_BindVideoVariables();
    I_BindJoystickVariables();
    I_BindSoundVariables();
*/
    M_BindBaseControls();
/*
    M_BindMapControls();
    M_BindMenuControls();
    M_BindWeaponControls();
    M_BindChatControls(MAXPLAYERS);
    M_BindHereticControls();
    M_BindHexenControls();

    key_multi_msgplayer[0] = CT_KEY_BLUE;
    key_multi_msgplayer[1] = CT_KEY_RED;
    key_multi_msgplayer[2] = CT_KEY_YELLOW;
    key_multi_msgplayer[3] = CT_KEY_GREEN;
    key_multi_msgplayer[4] = CT_KEY_PLAYER5;
    key_multi_msgplayer[5] = CT_KEY_PLAYER6;
    key_multi_msgplayer[6] = CT_KEY_PLAYER7;
    key_multi_msgplayer[7] = CT_KEY_PLAYER8;

    M_BindVariable("graphical_startup",      &graphical_startup);
    M_BindVariable("mouse_sensitivity",      &mouseSensitivity);
    M_BindVariable("sfx_volume",             &snd_MaxVolume);
    M_BindVariable("music_volume",           &snd_MusicVolume);
    M_BindVariable("messageson",             &messageson);
    M_BindVariable("screenblocks",           &screenblocks);
    M_BindVariable("snd_channels",           &snd_Channels);
    M_BindVariable("savedir",                &SavePath);

    // Multiplayer chat macros

    for (i=0; i<10; ++i)
    {
        char buf[12];

        sprintf(buf, "chatmacro%i", i);
        M_BindVariable(buf, &chat_macros[i]);
    }
*/
}

// Set the default directory where hub savegames are saved.

static void D_SetDefaultSavePath(void)
{
    SavePath = M_GetSaveGameDir("hexen.wad");

    // If we are not using a savegame path (probably because we are on
    // Windows and not using a config dir), behave like Vanilla Hexen
    // and use hexndata/:

    if (!strcmp(SavePath, ""))
    {
        SavePath = malloc(10);
	sprintf(SavePath, "hexndata%c", DIR_SEPARATOR);
    }
//    printf("savegamedir: %s\n",SavePath);	// ONLY FOR DEBUGGING
}

//
// D_GrabMouseCallback
//
// Called to determine whether to grab the mouse pointer
//
/*
static boolean D_GrabMouseCallback(void)
{
    // when menu is active or game is paused, release the mouse

    if (MenuActive || paused)
        return false;

    // only grab mouse when playing levels (but not demos)

    return (gamestate == GS_LEVEL) && !advancedemo && !demoplayback;
}

// Message displayed when quitting Hexen

static void D_HexenQuitMessage(void)
{
    printf("\nHexen: Beyond Heretic\n");
}
*/
static void D_AddFile(char *filename)
{
//    if(debugmode)
//        printf("  adding %s\n", filename);

    W_AddFile(filename);
}
/*
static void D_Endoom(void)
{
    byte *endoom;

    // Don't show ENDOOM if we have it disabled, or we're running
    // in screensaver or control test mode. Only show it once the
    // game has actually started.

    if (!show_endoom || !main_loop_started
     || screensaver_mode || M_CheckParm("-testcontrols") > 0)
    {
        return;
    }

    if(error_detected)
	endoom = W_CacheLumpName(DEH_String("ERROR"), PU_STATIC);
    else
	endoom = W_CacheLumpName(DEH_String("QUIT"), PU_STATIC);

    I_Endoom(endoom);
}
*/
//==========================================================================
//
// H2_Main
//
//==========================================================================
void InitMapMusicInfo(void);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

void D_DoomMain(void)
{
    FILE *fprw;
/*
    GameMission_t gamemission = hexen;
    int p;
*/
    if(debugmode)
    {
//	fsize = 20083672;
	fsize = 21078584;

	if(add_dk)
	    fsizedd = 4440584;
    }
/*
    I_AtExit(D_Endoom, false);
    I_AtExit(D_HexenQuitMessage, false);
*/
    startepisode = 1;

    if(debugmode)
	autostart = true;
    else
	autostart = false;

    startskill = sk_medium;
    startmap = 1;
    gamemode = commercial;
/*
    W_CheckSize(0);
    W_CheckSize(3);
*/
    if(usb)
	fprw = fopen("usb:/apps/wiihexen/psphexen.wad","rb");
    else if(sd)
	fprw = fopen("sd:/apps/wiihexen/psphexen.wad","rb");

    if(fprw)
    {
	resource_wad_exists = 1;

	fclose(fprw);

	W_CheckSize(/*2*/);
    }
    else
    {
    	resource_wad_exists = 0;
    }

    if(print_resource_pwad_error)
    {
	printf("\n\n\n\n\n");
	printf(" ===============================================================================");
	printf("                         !!! WRONG RESOURCE PWAD FILE !!!                       ");
	printf("                   PLEASE COPY THE WAD 'PSPHEXEN.WAD' THAT CAME                 ");
	printf("                    WITH THIS RELEASE, INTO THE GAME DIRECTORY                  \n");
   	printf("                                                                                \n");
	printf("                                 QUITTING NOW ...                               ");
	printf(" ===============================================================================");

	sleep(5);
//	sceKernelDelayThread(5000*1000);

	I_QuitSerialFail();
    }

    if (fsize != 20428208 &&
	fsize != 10615976 &&
	fsize != 13596228 &&
	fsize != 21078584 &&
	fsize != 10644136 &&
	fsize != 20128392 &&
	fsize != 20083672)
    {
	printf("\n\n\n\n\n");
	printf(" ===============================================================================");
	printf("        !!! HEXEN MAIN IWAD FILE MISSING, NOT SELECTED OR WRONG IWAD !!!        ");
   	printf("                                                                                \n");
	printf("                                 QUITTING NOW ...                               ");
	printf(" ===============================================================================");

	sleep(5);
//	sceKernelDelayThread(5000*1000);

	I_QuitSerialFail();
    }

    if(fsize == 20428208)
	HEXEN_BETA = true;
    else if(fsize == 10615976)
	HEXEN_BETA_DEMO = true;
    else if(fsize == 13596228)
	HEXEN_MACDEMO = true;
    else if(fsize == 21078584)
	HEXEN_MACFULL = true;
    else if(fsize == 10644136)
	HEXEN_DEMO = true;
    else if(fsize == 20128392)
	HEXEN_1_0 = true;
    else if(fsize == 20083672)
	HEXEN_1_1 = true;
    else
	fsize = 0;

    if(fsizedd == 4429700)
    {
	HEXDD_1_0 = true;
	datadisc = true;
    }
    else if(fsizedd == 4440584)
    {
	HEXDD_1_1 = true;
	datadisc = true;
    }
    else
	fsizedd = 0;
/*
    pspDebugScreenSetTextColor(0xD0D0D0);	// grey
    pspDebugScreenSetBackColor(0x000000);	// black
*/
    if (resource_wad_exists == 0)
    {
	printf("\n\n\n\n\n");
	printf(" ===============================================================================");
	printf("              WARNING: RESOURCE WAD FILE 'PSPHEXEN.WAD' MISSING!!!              ");
	printf("               PLEASE COPY THIS FILE INTO THE GAME'S DIRECTORY!!!               \n");
	printf("                                                                                \n");
	printf("                                QUITTING NOW ...                                ");
	printf(" ===============================================================================");

	sleep(5);
//	sceKernelDelayThread(5000*1000);

	I_QuitSerialFail();
    }

//    MD5_Check();		// FOR PSP: THIS FUNCTION DEFINITELY WORKS, BUT IT WAS NEVER USED
/*
    if(debugmode)
    {
	printf("\n                 HEXEN.WAD file size is : %d\n", fsize            );
	printf("\n                 HEXDD.WAD file size is :  %d", fsizedd          );
	printf("\n");
	printf("\n");
    }
*/
//    I_PrintBanner(PACKAGE_STRING);

    // Initialize subsystems

    printf("\n");

    if(debugmode)
    {
	ST_Message("DOS/4GW Professional Protected Mode Run-time  Version 1.97\n");
	ST_Message("Copyright (c) Rational Systems, Inc. 1990-1994\n");
    }

    if(debugmode)
	ST_Message("V_Init: allocate screens.\n");
    V_Init();

    // Load defaults before initing other systems
    if(debugmode)
	ST_Message("M_LoadDefaults: Load system defaults.\n");
    D_BindVariables();
/*
#ifdef _WIN32

    //!
    // @platform windows
    // @vanilla
    //
    // Save configuration data and savegames in c:\hexndata,
    // allowing play from CD.
    //

    cdrom = M_ParmExists("-cdrom");
#endif

    if (cdrom)
    {
        M_SetConfigDir("c:\\hexndata\\");
    }
    else
*/
    {
        M_SetConfigDir(NULL);
    }

    D_SetDefaultSavePath();

    M_SetConfigFilenames("hexen.cfg"/*, PROGRAM_PREFIX "hexen.cfg"*/);

    M_LoadDefaults();

    I_AtExit(M_SaveDefaults, false);

    if(debugmode)
	ST_Message("W_Init: Init WADfiles.\n");

    // Now that the savedir is loaded from .CFG, make sure it exists
    CreateSavePath();

    if(debugmode)
	ST_Message("Z_Init: Init zone memory allocation daemon.\n");
    Z_Init();

    // haleyjd: removed WATCOMC
/*
    iwadfile = D_FindIWAD(IWAD_MASK_HEXEN, &gamemission);

    if (iwadfile == NULL)
    {
        I_Error("Game mode indeterminate. No IWAD was found. Try specifying\n"
                "one with the '-iwad' command line parameter.");
    }
*/
    if(debugmode)
    {
	if(usb)
	    D_AddFile("usb:/apps/wiihexen/IWAD/MACFULL/HEXEN.WAD");
	else if(sd)
	    D_AddFile("sd:/apps/wiihexen/IWAD/MACFULL/HEXEN.WAD");
    }
    else
	D_AddFile(target);

//    W_CheckCorrectIWAD(hexen);

    if(HEXEN_MACFULL || HEXEN_1_0 || HEXEN_1_1)
    {
	if (load_extra_wad == 1)
	{
	    if(extra_wad_slot_1_loaded == 1)
	    {
		D_AddFile(extra_wad_1);

		if(debugmode)
		    printf("  adding %s\n", extra_wad_1);
	    }

	    if(extra_wad_slot_2_loaded == 1)
	    {
		D_AddFile(extra_wad_2);

		if(debugmode)
		    printf("  adding %s\n", extra_wad_2);
	    }

	    if(extra_wad_slot_3_loaded == 1)
	    {
		D_AddFile(extra_wad_3);

		if(debugmode)
		    printf("  adding %s\n", extra_wad_3);
	    }
	}
    }

    if(debugmode && add_dk)
    {
	if(usb)
	    D_AddFile("usb:/apps/wiihexen/IWAD/DK/Reg/v11/hexdd.wad");
	else if(sd)
	    D_AddFile("sd:/apps/wiihexen/IWAD/DK/Reg/v11/hexdd.wad");

	load_extra_wad = 1;
    }

    if(usb)
	D_AddFile("usb:/apps/wiihexen/psphexen.wad");
    else if(sd)
	D_AddFile("sd:/apps/wiihexen/psphexen.wad");

    if(debugmode)
    {
	ST_Message("  DPMI memory: 0x3c3d000, Maxzone: 0x800000.\n");
	ST_Message("  0x800000 allocated for zone, ZoneBase: 0x2C3038.\n");
    }
//    HandleArgs();

//    I_PrintStartupBanner("Hexen");

    if(debugmode)
	ST_Message("MN_Init: Init menu system.\n");
    MN_Init();

    if(debugmode)
	ST_Message("CT_Init: Init chat mode data.\n");
//    CT_Init();

    InitMapMusicInfo();         // Init music fields in mapinfo

    if(debugmode)
	ST_Message("S_InitScript\n");
    S_InitScript();

    if(debugmode)
	ST_Message("SN_InitSequenceScript: Registering sound sequences.\n");
    SN_InitSequenceScript();

    if(debugmode)
	ST_Message("I_Init: Setting up machine state.\n");
//    I_CheckIsScreensaver();
    I_InitTimer();
    I_Sleep(500);
//    I_InitJoystick();
/*
#ifdef FEATURE_MULTIPLAYER
    ST_Message("NET_Init: Init networking subsystem.\n");
    NET_Init();
#endif
    D_ConnectNetGame();
*/

    if(debugmode)
    {
	ST_Message("  I_StartupDPMI\n");
	ST_Message("  I_StartupMouse Mouse: detected\n");
	ST_Message("  CyberMan: Wrong mouse driver - no SWIFT support (AX=53c1).\n");
    }

    if(debugmode)
	ST_Message("  S_Init... I_StartupSound: Hope you hear a pop.\n");
    S_Init();
    S_Start();

    if(debugmode)
    {
	ST_Message("  I_StartupTimer()\n");
	ST_Message("  Sound cfg p=0x220, i=7, d=1\n");
	ST_Message("    SB_Detect returned p=0x220, i=7, d=1\n");
	ST_Message("    Music device #2 & dmxCode=2, Sfx device #3 & dmxCode=8\n");
	ST_Message("    Calling DMX_Init... DMX_Init() returned 10\n");
    }

    if(!debugmode)
    {
	if(use_alternate_startup)
	    I_InitGraphics();
    }
/*
    if(!debugmode)			// FIXME: DOESN'T WORK FOR THE WII (BUT IT DID WORK ON THE PSP)
    {
	if(HEXEN_MACDEMO || HEXEN_MACFULL)
	{
	    ST_LogoRaven();
	    ST_LogoPresage();
	    ST_LogoId();
	}
    }
*/
    if(debugmode)
	ST_Message("ST_Init: Init startup screen.\n");

    if(!use_alternate_startup)
	ST_Init();

    if(debugmode)
	ST_Message("Executable: Version 1.1 Mar 12 1996 (CBI).\n");

    // Show version message now, so it's visible during R_Init()
    if(debugmode)
    	ST_Message("R_Init: Init Hexen refresh daemon");
    R_Init();

    if(debugmode)
    	ST_Message("\n");

    //if (M_CheckParm("-net"))
    //    ST_NetProgress();       // Console player found

    if(debugmode)
    	ST_Message("P_Init: Init Playloop state.\n");
    P_Init();

    I_Sleep(1000);

    // Check for command line warping. Follows P_Init() because the
    // MAPINFO.TXT script must be already processed.
    WarpCheck();

    if(debugmode)
        ST_Message("D_CheckNetGame: Checking network game status.\n");
    D_CheckNetGame();
    I_Sleep(250);

    if(debugmode)
        ST_Message("SB_Init: Loading patches.\n");
    SB_Init();

//    ST_Done();					// FIXME: NO - THIS WON'T WORK ON THE WII

    if (autostart)
    {
        ST_Message("Warp to Map %d (\"%s\":%d), Skill %d\n",
                   WarpMap, P_GetMapName(startmap), startmap, startskill + 1);
    }

    if(debugmode /*&& use_alternate_startup*/)
	I_InitGraphics();
/*
    CheckRecordFrom();

    p = M_CheckParm("-record");
    if (p && p < myargc - 1)
    {
        G_RecordDemo(startskill, 1, startepisode, startmap, myargv[p + 1]);
        H2_GameLoop();          // Never returns
    }

    p = M_CheckParmWithArgs("-playdemo", 1);
    if (p)
    {
        singledemo = true;      // Quit after one demo
        G_DeferedPlayDemo(myargv[p + 1]);
        H2_GameLoop();          // Never returns
    }

    p = M_CheckParmWithArgs("-timedemo", 1);
    if (p)
    {
        G_TimeDemo(myargv[p + 1]);
        H2_GameLoop();          // Never returns
    }

    //!
    // @arg <s>
    // @vanilla
    //
    // Load the game in savegame slot s.
    //

    p = M_CheckParmWithArgs("-loadgame", 1);
    if (p)
    {
        G_LoadGame(atoi(myargv[p + 1]));
    }
*/
    if (gameaction != ga_loadgame)
    {
        UpdateState |= I_FULLSCRN;
        BorderNeedRefresh = true;
        if (autostart /*|| netgame*/)
        {
            G_StartNewInit();
            G_InitNew(startskill, startepisode, startmap);
        }
        else
        {
            H2_StartTitle();
        }
    }
    H2_GameLoop();              // Never returns
}

//==========================================================================
//
// HandleArgs
//
//==========================================================================
/*
static void HandleArgs(void)
{
    int p;

    //!
    // @vanilla
    //
    // Disable monsters.
    //

    nomonsters = M_ParmExists("-nomonsters");

    //!
    // @vanilla
    //
    // Monsters respawn after being killed.
    //

    respawnparm = M_ParmExists("-respawn");

    //!
    // @vanilla
    // @category net
    //
    // In deathmatch mode, change a player's class each time the
    // player respawns.
    //

    randomclass = M_ParmExists("-randclass");

    //!
    // @vanilla
    //
    // Take screenshots when F1 is pressed.
    //

    ravpic = M_ParmExists("-ravpic");

    //!
    // @vanilla
    //
    // Don't allow artifacts to be used when the run key is held down.
    //

    artiskip = M_ParmExists("-artiskip");

    debugmode = M_ParmExists("-debug");

    //!
    // @vanilla
    // @category net
    //
    // Start a deathmatch game.
    //

    deathmatch = M_ParmExists("-deathmatch");

    // currently broken or unused:
    cmdfrag = M_ParmExists("-cmdfrag");

    // Check WAD file command line options
//    W_ParseCommandLine();

    //!
    // @vanilla
    // @arg <path>
    //
    // Development option to specify path to level scripts.
    //

    p = M_CheckParmWithArgs("-scripts", 1);

    if (p)
    {
        sc_FileScripts = true;
        sc_ScriptsDir = myargv[p+1];
    }

    //!
    // @arg <skill>
    // @vanilla
    //
    // Set the game skill, 1-5 (1: easiest, 5: hardest).  A skill of
    // 0 disables all monsters.
    //

    p = M_CheckParmWithArgs("-skill", 1);

    if (p)
    {
        startskill = myargv[p+1][0] - '1';
        autostart = true;
    }

    //!
    // @arg <demo>
    // @category demo
    // @vanilla
    //
    // Play back the demo named demo.lmp.
    //

    p = M_CheckParmWithArgs("-playdemo", 1);

    if (!p)
    {
        //!
        // @arg <demo>
        // @category demo
        // @vanilla
        //
        // Play back the demo named demo.lmp, determining the framerate
        // of the screen.
        //

        p = M_CheckParmWithArgs("-timedemo", 1);
    }

    if (p)
    {
        char file[256];

        strncpy(file, myargv[p+1], sizeof(file));
        file[sizeof(file) - 6] = '\0';

        if (strcasecmp(file + strlen(file) - 4, ".lmp") != 0)
        {
            strcat(file, ".lmp");
        }

        W_AddFile(file);
        ST_Message("Playing demo %s.\n", file);
    }

    if (M_ParmExists("-testcontrols"))
    {
        autostart = true;
        testcontrols = true;
    }
}
*/
//==========================================================================
//
// WarpCheck
//
//==========================================================================

static void WarpCheck(void)
{
/*
    int p;
    int map;

    p = M_CheckParm("-warp");
    if (p && p < myargc - 1)
    {
        WarpMap = atoi(myargv[p + 1]);
        map = P_TranslateMap(WarpMap);
        if (map == -1)
        {                       // Couldn't find real map number
            startmap = 1;
            ST_Message("-WARP: Invalid map number.\n");
        }
        else
        {                       // Found a valid startmap
            startmap = map;
            autostart = true;
        }
    }
    else
*/
    {
        WarpMap = 1;
        startmap = P_TranslateMap(1);
        if (startmap == -1)
        {
            startmap = 1;
        }
    }
}

//==========================================================================
//
// H2_GameLoop
//
//==========================================================================

void H2_GameLoop(void)
{
//    if(debugmode)
    if(usb)
	debugfile = fopen("usb:/apps/wiihexen/debug.txt","w");
    else if(sd)
	debugfile = fopen("sd:/apps/wiihexen/debug.txt","w");

//    I_SetWindowTitle("Hexen");
//    I_GraphicsCheckCommandLine();
//    I_SetGrabMouseCallback(D_GrabMouseCallback);

    if(!use_alternate_startup)
	I_InitGraphics();

    main_loop_started = true;

    while (1)
    {
        // Frame syncronous IO operations
        I_StartFrame();

        // Process one or more tics
        // Will run at least one tic
        TryRunTics();

        // Move positional sounds
        S_UpdateSounds(players[displayplayer].mo);

        DrawAndBlit();
    }
}

//==========================================================================
//
// H2_ProcessEvents
//
// Send all the events of the given timestamp down the responder chain.
//
//==========================================================================

void H2_ProcessEvents(void)
{
    event_t *ev;

    for (;;)
    {
        ev = D_PopEvent();

        if (ev == NULL)
        {
            break;
        }
        if (F_Responder(ev))
        {
            continue;
        }
        if (MN_Responder(ev))
        {
            continue;
        }
        G_Responder(ev);
    }
}

//==========================================================================
//
// DrawAndBlit
//
//==========================================================================

static void DrawAndBlit(void)
{
    // Change the view size if needed
    if (setsizeneeded)
    {
        R_ExecuteSetViewSize();
    }

    // Do buffered drawing
    switch (gamestate)
    {
        case GS_LEVEL:
            if (!gametic)
            {
                break;
            }
            if (automapactive)
            {
                AM_Drawer();
            }
            else
            {
                R_RenderPlayerView(&players[displayplayer]);
            }
//            CT_Drawer();
            UpdateState |= I_FULLVIEW;
            SB_Drawer();

	    if(warped == 1)
		paused = true;

            break;
        case GS_INTERMISSION:
            IN_Drawer();
            break;
        case GS_FINALE:
            F_Drawer();
            break;
        case GS_DEMOSCREEN:
            PageDrawer();
            break;
    }
/*
    if (testcontrols)
    {
        V_DrawMouseSpeedBox(testcontrols_mousespeed);
    }
*/
    if (paused && !MenuActive && !askforquit)
    {
//        if (!netgame)
        {
/*
            V_DrawPatch(160, viewwindowy + 5, W_CacheLumpName("PAUSED",			// CHANGED FOR HIRES
                                                              PU_CACHE));		// CHANGED FOR HIRES
*/
	    V_DrawPatch(160, (viewwindowy >> hires) + 5, W_CacheLumpName("PAUSED",	// CHANGED FOR HIRES
							    PU_CACHE));			// CHANGED FOR HIRES
        }
/*
        else
        {
            V_DrawPatch(160, 70, 0, W_CacheLumpName("PAUSED", PU_CACHE));
        }
*/
    }

    // Draw current message
    DrawMessage();

    // Draw Menu
    MN_Drawer();

    // Send out any new accumulation
    NetUpdate();

    // Flush buffered stuff to screen
    I_FinishUpdate();
}

//==========================================================================
//
// DrawMessage
//
//==========================================================================

static void DrawMessage(void)
{
    player_t *player;

    player = &players[consoleplayer];
    if (player->messageTics <= 0 || !player->message)
    {                           // No message
        return;
    }
    if (player->yellowMessage)
    {
	if(HEXEN_DEMO || HEXEN_MACDEMO || HEXEN_MACFULL || HEXEN_1_0 || HEXEN_1_1)
	{
	    MN_DrTextAYellow(player->message, 160-MN_TextAWidth(player->message)/2, 1);
	}
	else if(HEXEN_BETA || HEXEN_BETA_DEMO)
	{
	    MN_DrTextA(player->message, 160-MN_TextAWidth(player->message)/2, 1);
	}
    }
    else
    {
        MN_DrTextA(player->message, 160 - MN_TextAWidth(player->message) / 2,
                   1);
    }
}

//==========================================================================
//
// H2_PageTicker
//
//==========================================================================

void H2_PageTicker(void)
{
    if (--pagetic < 0)
    {
        H2_AdvanceDemo();
    }
}

//==========================================================================
//
// PageDrawer
//
//==========================================================================

static void PageDrawer(void)
{
    V_DrawRawScreen(W_CacheLumpName(pagename, PU_CACHE));
    if (demosequence == 1)
    {
        V_DrawPatch(4, 160, W_CacheLumpName("ADVISOR", PU_CACHE));
    }
/*
    if(ninty == 1 && !MenuActive && leveltime&16 && gamestate != GS_LEVEL)
	MN_DrTextB(PRESS_START_EN,160-(MN_TextBWidth(PRESS_START_EN)>>1), 165);
*/
    UpdateState |= I_FULLSCRN;
}

//==========================================================================
//
// H2_AdvanceDemo
//
// Called after each demo or intro demosequence finishes.
//
//==========================================================================

void H2_AdvanceDemo(void)
{
    advancedemo = true;
}

//==========================================================================
//
// H2_DoAdvanceDemo
//
//==========================================================================

void H2_DoAdvanceDemo(void)
{
    players[consoleplayer].playerstate = PST_LIVE;      // don't reborn
    advancedemo = false;
    usergame = false;           // can't save/end game here
    paused = false;
    gameaction = ga_nothing;
    demosequence = (demosequence + 1) % 7;
    switch (demosequence)
    {
        case 0:
            pagetic = 280;
            gamestate = GS_DEMOSCREEN;
            pagename = "TITLE";

	    if(demos_are_disabled && !do_not_repeat_music)
	    {
//		S_StartSongName("hexen", true);		// ORIGINAL CODE
		S_StartSongName("hexen", false);	// MODIFIED CODE
		do_not_repeat_music = true;
	    }

            break;
        case 1:
            pagetic = 210;
            gamestate = GS_DEMOSCREEN;
            pagename = "TITLE";
            break;
        case 2:
            BorderNeedRefresh = true;
            UpdateState |= I_FULLSCRN;
//            G_DeferedPlayDemo("demo1");
            break;
        case 3:
            pagetic = 200;
            gamestate = GS_DEMOSCREEN;
            pagename = "CREDIT";
            break;
        case 4:
            BorderNeedRefresh = true;
            UpdateState |= I_FULLSCRN;
//            G_DeferedPlayDemo("demo2");
            break;
        case 5:
            pagetic = 200;
            gamestate = GS_DEMOSCREEN;

	    if(HEXEN_BETA || HEXEN_BETA_DEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1)
	    {
		pagename = "CREDIT";
	    }
	    else if(HEXEN_MACFULL || HEXEN_MACDEMO)
	    {
		pagename = "PRSGCRED";
	    }

            break;
        case 6:
            BorderNeedRefresh = true;
            UpdateState |= I_FULLSCRN;
//            G_DeferedPlayDemo("demo3");
            break;
    }
}

//==========================================================================
//
// H2_StartTitle
//
//==========================================================================

void H2_StartTitle(void)
{
    gameaction = ga_nothing;
    demosequence = -1;
    H2_AdvanceDemo();
}

//==========================================================================
//
// CheckRecordFrom
//
// -recordfrom <savegame num> <demoname>
//
//==========================================================================
/*
static void CheckRecordFrom(void)
{

    int p;

    p = M_CheckParm("-recordfrom");
    if (!p || p > myargc - 2)
    {                           // Bad args
        return;
    }

    G_LoadGame(atoi(myargv[p + 1]));
    G_DoLoadGame();             // Load the gameskill etc info from savegame
    G_RecordDemo(gameskill, 1, gameepisode, gamemap, myargv[p + 2]);

    H2_GameLoop();              // Never returns
}
*/
// haleyjd: removed WATCOMC
/*
void CleanExit(void)
{
	union REGS regs;

	I_ShutdownKeyboard();
	regs.x.eax = 0x3;
	int386(0x10, &regs, &regs);
	printf("Exited from HEXEN: Beyond Heretic.\n");
	exit(1);
}
*/

//==========================================================================
//
// CreateSavePath
//
//==========================================================================

static void CreateSavePath(void)
{
    char *savegamedir = NULL;
/*
    char *savegameroot;
    char *topdir = SavePathRoot1;

    M_MakeDirectory(topdir);

    savegameroot = SavePathRoot2;
    M_MakeDirectory(savegameroot);

    savegameroot = SavePathRoot3;
    M_MakeDirectory(savegameroot);

    savegameroot = SavePathRoot4;
    M_MakeDirectory(savegameroot);
*/
    if(usb)
    {
	if(!datadisc && HEXEN_BETA)
	    savegamedir = SavePathBetaUSB;

	if(!datadisc && HEXEN_BETA_DEMO)
	    savegamedir = SavePathBetaDemoUSB;

	if(!datadisc && HEXEN_MACDEMO)
	    savegamedir = SavePathMacDemoUSB;

	if(!datadisc && HEXEN_MACFULL)
	    savegamedir = SavePathMacFullUSB;

	if(!datadisc && HEXEN_DEMO)
	    savegamedir = SavePathDemoUSB;

	if(!datadisc && HEXEN_1_0)
	    savegamedir = SavePath10USB;

	if(!datadisc && HEXEN_1_1)
	    savegamedir = SavePath11USB;

	if(datadisc && HEXDD_1_0)
	    savegamedir = SavePathDD10USB;

	if(datadisc && HEXDD_1_1)
	    savegamedir = SavePathDD11USB;
    }
    else if(sd)
    {
	if(!datadisc && HEXEN_BETA)
	    savegamedir = SavePathBetaSD;

	if(!datadisc && HEXEN_BETA_DEMO)
	    savegamedir = SavePathBetaDemoSD;

	if(!datadisc && HEXEN_MACDEMO)
	    savegamedir = SavePathMacDemoSD;

	if(!datadisc && HEXEN_MACFULL)
	    savegamedir = SavePathMacFullSD;

	if(!datadisc && HEXEN_DEMO)
	    savegamedir = SavePathDemoSD;

	if(!datadisc && HEXEN_1_0)
	    savegamedir = SavePath10SD;

	if(!datadisc && HEXEN_1_1)
	    savegamedir = SavePath11SD;

	if(datadisc && HEXDD_1_0)
	    savegamedir = SavePathDD10SD;

	if(datadisc && HEXDD_1_1)
	    savegamedir = SavePathDD11SD;
    }

    M_MakeDirectory(savegamedir);
}

