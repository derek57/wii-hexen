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
#include <psppower.h>
#include <pspsysmem_kernel.h>
*/
#include <ctype.h>
#include "h2def.h"
#include "doomkeys.h"
#include "i_system.h"
#include "i_swap.h"
#include "i_video.h"
#include "m_controls.h"
#include "p_local.h"
#include "r_local.h"
#include "s_sound.h"
#include "v_video.h"

// MACROS ------------------------------------------------------------------

#define LEFT_DIR 0
#define RIGHT_DIR 1
#define ITEM_HEIGHT 20
#define ITEM_HEIGHT_SMALL 10
#define SELECTOR_XOFFSET (-28)
#define SELECTOR_XOFFSET_SMALL (-15)
#define SELECTOR_YOFFSET (-1)
#define SELECTOR_YOFFSET_SMALL (1)
#define SLOTTEXTLEN	16 + 4 + 4
#define ASCII_CURSOR '['
#define FIRSTKEY_MAX	0

// TYPES -------------------------------------------------------------------

typedef enum
{
    ITT_EMPTY,
    ITT_EFUNC,
    ITT_LRFUNC,
    ITT_SETMENU,
    ITT_INERT,
    ITT_SETKEY
} ItemType_t;

typedef enum
{
    MENU_MAIN,
    MENU_CLASS,
    MENU_SKILL,
    MENU_OPTIONS,
    MENU_SCREEN,
    MENU_CONTROL,
    MENU_BINDINGS,
    MENU_SOUND,
    MENU_SYSTEM,
    MENU_GAME,
    MENU_DEBUG,
//    MENU_TEST,
    MENU_KEYS,
    MENU_ARMOR,
    MENU_WEAPONS,
    MENU_ARTIFACTS,
    MENU_PUZZLE,
    MENU_CHEATS,
    MENU_FILES,
    MENU_LOAD,
    MENU_SAVE,
    MENU_RECORD,
    MENU_NONE
} MenuType_t;

typedef struct
{
    ItemType_t type;
    char *text;
    void (*func) (int option);
    int option;
    MenuType_t menu;
} MenuItem_t;

typedef struct
{
    int x;
    int y;
    void (*drawFunc) (void);
    int itemCount;
    MenuItem_t *items;
    int oldItPos;
    MenuType_t prevMenu;
} Menu_t;

// EXTERNAL FUNCTION PROTOTYPES --------------------------------------------

// PUBLIC FUNCTION PROTOTYPES ----------------------------------------------

// PRIVATE FUNCTION PROTOTYPES ---------------------------------------------

static void InitFonts(void);
static void SetMenu(MenuType_t menu);
static void SCQuitGame(int option);
static void SCClass(int option);
static void SCSkill(int option);
//static void SCMouseSensi(int option);
static void SCSfxVolume(int option);
static void SCMusicVolume(int option);
static void SCScreenSize(int option);
static boolean SCNetCheck(int option);
static void SCNetCheck2(int option);
static void SCLoadGame(int option);
static void SCSaveGame(int option);
static void SCMessages(int option);
static void SCEndGame(int option);
static void SCInfo(int option);

static void SCWalkingSpeed(int option);
static void SCTurningSpeed(int option);
static void SCStrafingSpeed(int option);
static void SCMouseSpeed(int option);
static void SCNTrack(int option);
//static void SCNTrackLoop(int option);
static void SCWarp(int option);
static void SCWarpNow(int option);
static void SCBrightness(int option);
static void SCDetails(int option);
//static void SCNinty(int option);
static void SCWeaponChange(int option);
static void SCCrosshair(int option);
static void SCMouselook(int option);

static void SCArtifact(int option);
static void SCDefender(int option);
static void SCQuartz(int option);
static void SCUrn(int option);
static void SCIncant(int option);
static void SCServant(int option);
static void SCTorch(int option);
static void SCPorkalator(int option);
static void SCWings(int option);
static void SCRepulsion(int option);
static void SCFlechette(int option);
static void SCBanDev(int option);
static void SCBoots(int option);
static void SCVial(int option);
static void SCBracers(int option);
static void SCChaosDev(int option);
static void SCGod(int option);
static void SCNoclip(int option);
//static void SCCpu(int option);
static void SCTicker(int option);
static void SCFPS(int option);
static void SCWiiLight(int option);
static void SCAllArm(int option);
static void SCWarding(int option);
static void SCShield(int option);
static void SCArmor(int option);
static void SCHelm(int option);
static void SCWeaponA(int option);
static void SCWeaponB(int option);
static void SCWeaponC(int option);
static void SCWeaponD(int option);
static void SCManaA(int option);
static void SCManaB(int option);
static void SCPieceA(int option);
static void SCPieceB(int option);
static void SCPieceC(int option);
static void SCPieceD(int option);
static void SCPieceE(int option);
static void SCPieceF(int option);
static void SCPieceG(int option);
static void SCKeys(int option);
static void SCEmerald(int option);
static void SCSilver(int option);
static void SCSteel(int option);
static void SCFire(int option);
static void SCHorn(int option);
static void SCCave(int option);
static void SCSwamp(int option);
static void SCCastle(int option);
static void SCRusty(int option);
static void SCDungeon(int option);
static void SCAxe(int option);
static void SCFirstpuzzitem(int option);
static void SCPuzzle(int option);
static void SCPuzzgembig(int option);
static void SCPuzzgemred(int option);
static void SCPuzzgemgreen1(int option);
static void SCPuzzgemgreen2(int option);
static void SCPuzzgemblue1(int option);
static void SCPuzzgemblue2(int option);
static void SCPuzzbook1(int option);
static void SCPuzzbook2(int option);
static void SCPuzzgembig(int option);
static void SCPuzzskull2(int option);
static void SCPuzzfweapon(int option);
static void SCPuzzcweapon(int option);
static void SCPuzzmweapon(int option);
static void SCPuzzgear1(int option);
static void SCPuzzgear2(int option);
static void SCPuzzgear3(int option);
static void SCPuzzgear4(int option);
static void SCMap(int option);
static void SCKill(int option);
static void SCPClass(int option);
static void SCPig(int option);
static void SCSuicide(int option);
static void SCRestart(int option);
static void SCHealth(int option);
static void SCRSkill(int option);
static void SCRPClass(int option);
static void SCRMap(int option);
static void SCRecord(int option);
static void SCTimer(int option);
/*
static void SCOther(int option);
static void SCProcessor(int option);
static void SCShowMemory(int option);
static void SCBattery(int option);
*/
static void SCVersion(int option);
static void SCSound(int option);
static void SCCoords(int option);
static void SCMapname(int option);
static void SCGrid(int option);
static void SCFollow(int option);
static void SCRotate(int option);

//static void ButtonLayout(int option);
static void SCSetKey(int option);
static void ClearKeys(int option);
static void ResetKeys(int option);

static void DrawMainMenu(void);
static void DrawClassMenu(void);
static void DrawSkillMenu(void);
static void DrawOptionsMenu(void);
//static void DrawOptions2Menu(void);
static void DrawFileSlots(Menu_t * menu);
static void DrawFilesMenu(void);
static void MN_DrawInfo(void);
static void DrawLoadMenu(void);
static void DrawSaveMenu(void);
static void DrawSlider(Menu_t * menu, int item, int width, int slot);

static void DrawScreenMenu(void);
static void DrawControlMenu(void);
static void DrawBindingsMenu(void);
static void DrawSoundMenu(void);
static void DrawSystemMenu(void);
static void DrawGameMenu(void);
static void DrawDebugMenu(void);
//static void DrawTestMenu(void);
static void DrawKeysMenu(void);
static void DrawArmorMenu(void);
static void DrawWeaponsMenu(void);
static void DrawArtifactsMenu(void);
static void DrawPuzzleMenu(void);
static void DrawCheatsMenu(void);
static void DrawRecordMenu(void);

void MN_LoadSlotText(void);

// EXTERNAL DATA DECLARATIONS ----------------------------------------------

extern int screenblocks;
extern int mouselook;
extern default_t doom_defaults_list[];
//int mhz333;
int button_layout = 0;
extern boolean am_rotate;
//extern int loading_disk;
static int FirstKey = 0;
static boolean askforkey = false;
static int keyaskedfor;

//int ninty = 0;
int FramesPerSecond;
int timer, version, mapname, coords;
//extern boolean DebugSound;
//extern boolean DisplayTicker;
int followplayer = 1;
int drawgrid;
int crosshair = 0;
int tracknum = 1;
//boolean loop = true;
int map = 0;
int continuous=0;
//int selected = 0;
//int selectedx = 0;
//int actualmusvolume;
extern int cheating;
int cheeting;
int plcl = 0;
//extern boolean gamemode;
//extern boolean istnt;
//extern boolean isdd;
int ran = 0;
int warped = 0;
int rclass = 1;
int rmap = 1;
int rskill = 0;
int key_bindings_start_in_cfg_at_pos = 16;
int key_bindings_end_in_cfg_at_pos = 31;
/*
int memory_info = 0;
int battery_info = 0;
int cpu_info = 0;
int other_info = 0;
char unit_plugged_textbuffer[50];
char battery_present_textbuffer[50];
char battery_charging_textbuffer[50];
char battery_charging_status_textbuffer[50];
char battery_low_textbuffer[50];
char battery_lifetime_percent_textbuffer[50];
char battery_lifetime_int_textbuffer[50];
char battery_temp_textbuffer[50];
char battery_voltage_textbuffer[50];
char processor_clock_textbuffer[50];
char processor_bus_textbuffer[50];
char idle_time_textbuffer[50];
char allocated_ram_textbuffer[50];
char free_ram_textbuffer[50];
char max_free_ram_textbuffer[50];
extern int allocated_ram_size;
int max_free_ram = 0;
*/
boolean from_menu = false;
#define BETA_FLASH_TEXT "BETA"
#define PRESS_BUTTON	"RESTART: PRESS JUMP"

int	wiilight = 1;

extern int detailLevel;
extern boolean gamekeydown[256];        // The NUMKEYS macro is local to g_game

int	turnspeed = 7;		// ACTUAL MOVEMENT VALUE

fixed_t	forwardmove = 29;	// ACTUAL MOVEMENT VALUE

fixed_t	sidemove = 21;		// ACTUAL MOVEMENT VALUE

// PUBLIC DATA DEFINITIONS -------------------------------------------------

boolean MenuActive;
int InfoType;
int messageson/* = true*/;
//boolean mn_SuicideConsole;

// PRIVATE DATA DEFINITIONS ------------------------------------------------

static int FontABaseLump;
static int FontAYellowBaseLump;
static int FontBBaseLump;
static int MauloBaseLump;
/*
static int FontARedBaseLump;
static int FontCBaseLump;
static int FontCRedBaseLump;
static int FontCYellowBaseLump;
static int FontDBaseLump;
static int TorchBaseLump;
*/
static Menu_t *CurrentMenu;
static int CurrentItPos;
static int MenuPClass;
static int MenuTime;
static boolean soundchanged;

boolean askforquit;
boolean askforsave;
/*static*/ int typeofask;
/*static*/ int typeofask2;
static boolean FileMenuKeySteal;
static boolean slottextloaded;
static char SlotText[6][SLOTTEXTLEN + 2];
static char oldSlotText[SLOTTEXTLEN + 2];
static int SlotStatus[6];
static int slotptr;
static int currentSlot;
/*
static int quicksave;
static int quickload;
*/
char *songtextdd[] = {
    "00",
    "01",
    "02",
    "03",
    "04",
    "05",
    " ",	// additional but in game non selectable REQUIRED entry
    " ",
    " ",	// additional but in game non selectable REQUIRED entry
    "06",
    "07",
    " ",	// additional but in game non selectable REQUIRED entry
    " ",	// additional but in game non selectable REQUIRED entry
    "08",
    " ",
    " ",	// additional but in game non selectable REQUIRED entry
    " ",
    " ",	// additional but in game non selectable REQUIRED entry
    " ",	// additional but in game non selectable REQUIRED entry
    " ",
    " ",
    "09",
    "10",	// additional but in game non selectable REQUIRED entry
    " ",	// additional but in game non selectable REQUIRED entry
    " ",	// additional but in game non selectable REQUIRED entry
    " ",
    " ",
    "11",
    "12",
    "13",	// additional but in game non selectable REQUIRED entry
    "14",
    " ",	// additional but in game non selectable REQUIRED entry
    " ",	// additional but in game non selectable REQUIRED entry
    "15",	// additional but in game non selectable REQUIRED entry
    " ",	// additional but in game non selectable REQUIRED entry
    "16",	// additional but in game non selectable REQUIRED entry
    "17",
    "18",
    "19",
    " ",
    "20"
};
/*
char *songinfodd[] = {
    "MUSIC STOPPED",
    "SONG FROM RUINED VILLAGE PLAYING",
    "SONG FROM BLIGHT PLAYING",
    "SONG FROM ICE HOLD PLAYING",
    "SONG FROM TREASURY PLAYING",
    "SONG FROM ARMORY PLAYING",
    " ",				// additional but in game non selectable REQUIRED entry
    "SONG FROM CLOACA PLAYING",
    " ",
    "SONG FROM CATACOMB PLAYING",
    "SONG FROM BADLANDS PLAYING",
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    "SONG FROM SUMP PLAYING",
    "SONG FROM PYRE PLAYING",
    " ",				// additional but in game non selectable REQUIRED entry
    "SONG FROM NAVE PLAYING",
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    "SONG FROM CHANTRY PLAYING",
    "SONG FROM DARK WATCH PLAYING",
    "SONG FROM BRACKENWOOD PLAYING",
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    "SONG FROM CONSTABLE'S GATE PLAYING",
    "SONG FROM LOCUS REQUIESCAT PLAYING",
    "SONG FROM ABATTOIR PLAYING",
    "SONG FROM ORDEAL PLAYING",
    " ",				// additional but in game non selectable REQUIRED entry
    "SONG FROM DARK CITADEL PLAYING",
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    " ",				// additional but in game non selectable REQUIRED entry
    "SONG FROM MARKET PLACE PLAYING"
};

char *songinfo[] = {
    "MUSIC STOPPED",
    "SONG FROM WINNOWING HALL PLAYING",
    "SONG FROM SEVEN PORTALS PLAYING",
    "SONG FROM GUARDIAN OF ICE PLAYING",
    "SONG FROM GUARDIAN OF FIRE PLAYING",
    "SONG FROM GUARDIAN OF STEEL PLAYING",
    "SONG FROM BRIGHT CRUCIBLE PLAYING",
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    "SONG FROM DARKMERE PLAYING",
    "SONG FROM CAVES OF CIRCE PLAYING",
    "SONG FROM WASTELANDS PLAYING",
    "SONG FROM SACRED GROVE PLAYING",
    "SONG FROM HYPOSTYLE PLAYING",
    "SONG FROM SHADOW WOOD PLAYING",
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    "SONG FROM FORSAKEN OUTPOST PLAYING",
    "SONG FROM CASTLE OF GRIEF PLAYING",
    "SONG FROM GIBBET PLAYING",
    "SONG FROM EFFLUVIUM PLAYING",
    "SONG FROM DUNGEONS PLAYING",
    "SONG FROM DESOLATE GARDEN PLAYING",
    "SONG FROM HERESIARCH'S SEMINARY PLAYING",
    "SONG FROM DRAGON CHAPEL PLAYING",
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    "SONG FROM GRIFFIN CHAPEL PLAYING",
    "SONG FROM DEATHWIND CHAPEL PLAYING",
    "SONG FROM ORCHARD OF LAMENTATIONS PLAYING",
    "SONG FROM SILENT REFECTORY PLAYING",
    "SONG FROM WOLF CHAPEL PLAYING",
    "SONG FROM NECROPOLIS PLAYING",
    "SONG FROM ZEDEK'S TOMB PLAYING",
    "SONG FROM MENELKIR'S TOMB PLAYING",
    "SONG FROM TRADUCTUS' TOMB PLAYING",
    "SONG FROM VIVARIUM PLAYING",
    "SONG FROM DARK CRUCIBLE PLAYING"
};
*/
char *songtext[] = {
    "00",
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    " ",
    "07",
    "08",
    "09",
    "10",
    "11",
    "12",
    " ",
    " ",
    " ",
    " ",
    " ",
    " ",
    " ",
    "13",
    "14",
    "15",
    " ",
    "16",
    "17",
    "18",
    "19",
    " ",
    "20",
    "21",
    "22",
    "23",
    "24",
    "25",
    "26",
    "27",
    "28",
    "29",
    "30"
};

char *mlooktext[] = {
    "OFF",
    "NORMAL",
    "INVERSE"
};

char *enmaptext[] = {
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    "WINNOWING HALL",
    "SEVEN PORTALS",
    "GUARDIAN OF ICE",
    "GUARDIAN OF FIRE",
    "GUARDIAN OF STEEL",
    "BRIGHT CRUCIBLE",
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    "DARKMERE",
    "CAVES OF CIRCE",
    "WASTELANDS",
    "SACRED GROVE",
    "HYPOSTYLE",
    "SHADOW WOOD",
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    "FORSAKEN OUTPOST",
    "CASTLE OF GRIEF",
    "GIBBET",
    "EFFLUVIUM",
    "DUNGEONS",
    "DESOLATE GARDEN",
    "HERESIARCH'S SEMINARY",
    "DRAGON CHAPEL",
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN IWAD)
    "GRIFFIN CHAPEL",
    "DEATHWIND CHAPEL",
    "ORCHARD OF LAMENTATIONS",
    "SILENT REFECTORY",
    "WOLF CHAPEL",
    "NECROPOLIS",
    "ZEDEK'S TOMB",
    "MENELKIR'S TOMB",
    "TRADUCTUS' TOMB",
    "VIVARIUM",
    "DARK CRUCIBLE"
};

char *maptextdd[] = {
    " ",	// additional but in game non selectable REQUIRED entry (MAP 0 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 1 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 2 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 3 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 4 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 5 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 6 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 7 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 8 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 9 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 10 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 11 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 12 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 13 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 14 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 15 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 16 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 17 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 18 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 19 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 21 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 22 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 23 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 24 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 25 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 26 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 27 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 28 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 29 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 31 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 32 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 33 IS ONLY FOR DEATHMATCH)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 34 IS ONLY FOR DEATHMATCH)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 35 IS ONLY FOR DEATHMATCH)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 36 IS ONLY FOR DEATHMATCH)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 37 IS ONLY FOR DEATHMATCH)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 38 IS ONLY FOR DEATHMATCH)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 39 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry (MAP 40 DOESN'T EXIST IN PWAD)
    " ",	// additional but in game non selectable REQUIRED entry
    " ",	// additional but in game non selectable REQUIRED entry
    "RUINED VILLAGE",
    "BLIGHT", 
    "SUMP", 
    "CATACOMB", 
    "BADLANDS", 
    "BRACKENWOOD", 
    "PYRE", 
    "CONSTABLE'S GATE", 
    "TREASURY", 
    "MARKET PLACE", 
    "LOCUS REQUIESCAT", 
    "ORDEAL", 
    "ARMORY", 
    "NAVE", 
    "CHANTRY", 
    "ABATTOIR", 
    "DARK WATCH", 
    "CLOACA", 
    "ICE HOLD", 
    "DARK CITADEL"
};

static MenuItem_t MainItems[] = {
    {ITT_SETMENU, "NEW GAME", SCNetCheck2, 1, MENU_CLASS},
    {ITT_SETMENU, "OPTIONS", NULL, 0, MENU_OPTIONS},
    {ITT_SETMENU, "GAME FILES", NULL, 0, MENU_FILES},
    {ITT_EFUNC, "INFO", SCInfo, 0, MENU_NONE},
    {ITT_EFUNC, "QUIT GAME", SCQuitGame, 0, MENU_NONE}
};

static Menu_t MainMenu = {
    110, 56,
    DrawMainMenu,
    5, MainItems,
    0,
    MENU_NONE
};

static MenuItem_t ClassItems[] = {
    {ITT_EFUNC, "FIGHTER", SCClass, 0, MENU_NONE},
    {ITT_EFUNC, "CLERIC", SCClass, 1, MENU_NONE},
    {ITT_EFUNC, "MAGE", SCClass, 2, MENU_NONE}
};

static Menu_t ClassMenu = {
    66, 66,
    DrawClassMenu,
    3, ClassItems,
    0,
    MENU_MAIN
};

static MenuItem_t FilesItems[] = {
    {ITT_SETMENU, "LOAD GAME", SCNetCheck2, 2, MENU_LOAD},
    {ITT_SETMENU, "SAVE GAME", NULL, 0, MENU_SAVE},
    {ITT_EFUNC, "END GAME", SCEndGame, 0, MENU_NONE},
    {ITT_SETMENU, "CHEATS", NULL, 0, MENU_CHEATS},
    {ITT_SETMENU, "DEMO REC.", NULL, 0, MENU_RECORD}
};

static Menu_t FilesMenu = {
    110, 50,
    DrawFilesMenu,
    5, FilesItems,
    0,
    MENU_MAIN
};

static MenuItem_t LoadItems[] = {
    {ITT_EFUNC, NULL, SCLoadGame, 0, MENU_NONE},
    {ITT_EFUNC, NULL, SCLoadGame, 1, MENU_NONE},
    {ITT_EFUNC, NULL, SCLoadGame, 2, MENU_NONE},
    {ITT_EFUNC, NULL, SCLoadGame, 3, MENU_NONE},
    {ITT_EFUNC, NULL, SCLoadGame, 4, MENU_NONE},
    {ITT_EFUNC, NULL, SCLoadGame, 5, MENU_NONE}
};

static Menu_t LoadMenu = {
    70, 30,
    DrawLoadMenu,
    6, LoadItems,
    0,
    MENU_FILES
};

static MenuItem_t SaveItems[] = {
    {ITT_EFUNC, NULL, SCSaveGame, 0, MENU_NONE},
    {ITT_EFUNC, NULL, SCSaveGame, 1, MENU_NONE},
    {ITT_EFUNC, NULL, SCSaveGame, 2, MENU_NONE},
    {ITT_EFUNC, NULL, SCSaveGame, 3, MENU_NONE},
    {ITT_EFUNC, NULL, SCSaveGame, 4, MENU_NONE},
    {ITT_EFUNC, NULL, SCSaveGame, 5, MENU_NONE}
};

static Menu_t SaveMenu = {
    70, 30,
    DrawSaveMenu,
    6, SaveItems,
    0,
    MENU_FILES
};

static MenuItem_t SkillItems[] = {
    {ITT_EFUNC, NULL, SCSkill, sk_baby, MENU_NONE},
    {ITT_EFUNC, NULL, SCSkill, sk_easy, MENU_NONE},
    {ITT_EFUNC, NULL, SCSkill, sk_medium, MENU_NONE},
    {ITT_EFUNC, NULL, SCSkill, sk_hard, MENU_NONE},
    {ITT_EFUNC, NULL, SCSkill, sk_nightmare, MENU_NONE}
};

static Menu_t SkillMenu = {
    120, 44,
    DrawSkillMenu,
    5, SkillItems,
    2,
    MENU_CLASS
};

static MenuItem_t OptionsItems[] = {
/*
    {ITT_EFUNC, "END GAME", SCEndGame, 0, MENU_NONE},
    {ITT_EFUNC, "MESSAGES : ", SCMessages, 0, MENU_NONE},
    {ITT_LRFUNC, "MOUSE SENSITIVITY", SCMouseSensi, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_SETMENU, "MORE...", NULL, 0, MENU_OPTIONS2}
*/
    {ITT_SETMENU, "SCREEN SETTINGS", NULL, 0, MENU_SCREEN},
    {ITT_SETMENU, "CONTROL SETTINGS", NULL, 0, MENU_CONTROL},
    {ITT_SETMENU, "SOUND SETTINGS", NULL, 0, MENU_SOUND},
    {ITT_SETMENU, "SYSTEM SETTINGS", NULL, 0, MENU_SYSTEM},
    {ITT_SETMENU, "GAME SETTINGS", NULL, 0, MENU_GAME},
    {ITT_SETMENU, "DEBUG SETTINGS", NULL, 0, MENU_DEBUG}/*,
    {ITT_SETMENU, "TEST SETTINGS", NULL, 0, MENU_TEST}*/
};

static Menu_t OptionsMenu = {
    88, 30,
    DrawOptionsMenu,
    6, OptionsItems,
    0,
    MENU_MAIN
};

static MenuItem_t ScreenItems[] = {
    {ITT_LRFUNC, "BRIGHTNESS", SCBrightness, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_LRFUNC, "SCREEN SIZE", SCScreenSize, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_EFUNC, "DETAILS", SCDetails, 0, MENU_NONE}/*,

    {ITT_LRFUNC, "SFX VOLUME", SCSfxVolume, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_LRFUNC, "MUSIC VOLUME", SCMusicVolume, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE}
*/
};

static Menu_t ScreenMenu = {
    100, 40,
    DrawScreenMenu,
    5, ScreenItems,
    0,
    MENU_SCREEN
};

static MenuItem_t ControlItems[] = {
    {ITT_LRFUNC, "", SCWalkingSpeed, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_LRFUNC, "", SCTurningSpeed, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_LRFUNC, "", SCStrafingSpeed, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_LRFUNC, "", SCMouselook, 0, MENU_NONE},
    {ITT_LRFUNC, "", SCMouseSpeed, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_SETMENU, "", NULL, 0, MENU_BINDINGS}
};

static Menu_t ControlMenu = {
    38, 0,
    DrawControlMenu,
    10, ControlItems,
    0,
    MENU_CONTROL
};

static MenuItem_t BindingsItems[] = {
// see defaults[] in m_misc.c for the correct option number:
// key_right corresponds to defaults[3], which means that we
// are using the (index_number - 3) here.
//
    {ITT_SETKEY, "FIRE", SCSetKey, 0, MENU_NONE},
    {ITT_SETKEY, "USE / OPEN", SCSetKey, 1, MENU_NONE},
    {ITT_SETKEY, "MAIN MENU", SCSetKey, 2, MENU_NONE},
    {ITT_SETKEY, "WEAPON LEFT", SCSetKey, 3, MENU_NONE},
    {ITT_SETKEY, "SHOW AUTOMAP", SCSetKey, 4, MENU_NONE},
    {ITT_SETKEY, "WEAPON RIGHT", SCSetKey, 5, MENU_NONE},
    {ITT_SETKEY, "AUTOMAP ZOOM IN", SCSetKey, 6, MENU_NONE},
    {ITT_SETKEY, "AUTOMAP ZOOM OUT", SCSetKey, 7, MENU_NONE},
    {ITT_SETKEY, "INVENTORY LEFT", SCSetKey, 8, MENU_NONE},
    {ITT_SETKEY, "INVENTORY RIGHT", SCSetKey, 9, MENU_NONE},
    {ITT_SETKEY, "INVENTORY USE", SCSetKey, 10, MENU_NONE},
    {ITT_SETKEY, "FLY UP", SCSetKey, 11, MENU_NONE},
    {ITT_SETKEY, "FLY DOWN", SCSetKey, 12, MENU_NONE},
    {ITT_SETKEY, "LOOK CENTER", SCSetKey, 13, MENU_NONE},
    {ITT_SETKEY, "JUMP", SCSetKey, 14, MENU_NONE},
//    {ITT_EMPTY, NULL, NULL, 11, MENU_NONE},
//    {ITT_LRFUNC, "BUTTON LAYOUT :", ButtonLayout, 12, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 15, MENU_NONE},
    {ITT_SETKEY, "CLEAR ALL CONTROLS", ClearKeys, 16, MENU_NONE},
    {ITT_SETKEY, "RESET TO DEFAULTS", ResetKeys, 17, MENU_NONE}
};

static Menu_t BindingsMenu = {
    40, 20,
    DrawBindingsMenu,
    18, BindingsItems,
    0,
    MENU_BINDINGS
};

static MenuItem_t SoundItems[] = {
    {ITT_LRFUNC, "SFX VOLUME", SCSfxVolume, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_LRFUNC, "MUSIC VOLUME", SCMusicVolume, 0, MENU_NONE},
    {ITT_EMPTY, NULL, NULL, 0, MENU_NONE},
    {ITT_LRFUNC, "CHOOSE TRACK :", SCNTrack, 0, MENU_NONE}/*,
    {ITT_LRFUNC, "LOOP TRACK :", SCNTrackLoop, 0, MENU_NONE}*/
};

static Menu_t SoundMenu = {
    75, 30,
    DrawSoundMenu,
    5, SoundItems,
    0,
    MENU_SOUND
};

static MenuItem_t SystemItems[] = {
//    {ITT_EFUNC, "CPU SPEED :", SCCpu, 0, MENU_NONE},
    {ITT_EFUNC, "DISPLAY TICKER", SCTicker, 0, MENU_NONE},
    {ITT_EFUNC, "FPS COUNTER", SCFPS, 0, MENU_NONE},
    {ITT_EFUNC, "WII LIGHT FLASHING", SCWiiLight, 0, MENU_NONE}
};

static Menu_t SystemMenu = {
    70, 60,
    DrawSystemMenu,
    3, SystemItems,
    0,
    MENU_SYSTEM
};

static MenuItem_t GameItems[] = {
    {ITT_EFUNC, "MAP GRID", SCGrid, 0, MENU_NONE},
    {ITT_EFUNC, "FOLLOW MODE", SCFollow, 0, MENU_NONE},
    {ITT_LRFUNC, "ROTATION", SCRotate, 0, MENU_NONE},
    {},
    {ITT_EFUNC, "MESSAGES", SCMessages, 0, MENU_NONE},
    {ITT_EFUNC, "CROSSHAIR", SCCrosshair, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON CHANGE", SCWeaponChange, 0, MENU_NONE}/*,
    {ITT_EFUNC, "HEXEN 64 FUNCTIONS", SCNinty, 0, MENU_NONE}*/		// NOT YET (ONLY FOR DEBUGGING)
};

static Menu_t GameMenu = {
    50, 30,
    DrawGameMenu,
    7, GameItems,
    0,
    MENU_GAME
};

static MenuItem_t DebugItems[] = {
    {ITT_EFUNC, "COORDINATES INFO", SCCoords, 0, MENU_NONE},
    {ITT_EFUNC, "MAP NAME INFO", SCMapname, 0, MENU_NONE},
    {ITT_EFUNC, "SOUND INFO", SCSound, 0, MENU_NONE},
    {ITT_EFUNC, "TIMER INFO", SCTimer, 0, MENU_NONE},
    {ITT_EFUNC, "VERSION INFO", SCVersion, 0, MENU_NONE}/*,
    {ITT_EFUNC, "BATTERY INFO", SCBattery, 0, MENU_NONE},
    {ITT_EFUNC, "CPU INFO", SCProcessor, 0, MENU_NONE},
    {ITT_EFUNC, "MEMORY INFO", SCShowMemory, 0, MENU_NONE},
    {ITT_EFUNC, "OTHER INFO", SCOther, 0, MENU_NONE}*/
};

static Menu_t DebugMenu = {
    65, 50,
    DrawDebugMenu,
    5, DebugItems,
    0,
    MENU_DEBUG
};
/*
static MenuItem_t TestItems[] = {
    {ITT_EFUNC, "", NULL, 0, MENU_NONE}
};

static Menu_t TestMenu = {
    65, 10,
    DrawTestMenu,
    1, TestItems,
    0,
    MENU_TEST
};
*/
static MenuItem_t KeysItems[] = {
    {ITT_EFUNC, "GIVE THEM ALL AT ONCE", SCKeys, 0, MENU_NONE},
    {},
    {ITT_EFUNC, "EMERALD KEY", SCEmerald, 0, MENU_NONE},
    {ITT_EFUNC, "SILVER KEY", SCSilver, 0, MENU_NONE},
    {ITT_EFUNC, "STEEL KEY", SCSteel, 0, MENU_NONE},
    {ITT_EFUNC, "FIRE KEY", SCFire, 0, MENU_NONE},
    {ITT_EFUNC, "FORN KEY", SCHorn, 0, MENU_NONE},
    {ITT_EFUNC, "CAVE KEY", SCCave, 0, MENU_NONE},
    {ITT_EFUNC, "SWAMP KEY", SCSwamp, 0, MENU_NONE},
    {ITT_EFUNC, "CASTLE KEY", SCCastle, 0, MENU_NONE},
    {ITT_EFUNC, "RUSTY KEY", SCRusty, 0, MENU_NONE},
    {ITT_EFUNC, "DUNGEON KEY", SCDungeon, 0, MENU_NONE},
    {ITT_EFUNC, "AXE KEY", SCAxe, 0, MENU_NONE}
};

static MenuItem_t ArmorItems[] = {
    {ITT_EFUNC, "GIVE THEM ALL AT ONCE", SCAllArm, 0, MENU_NONE},
    {},
    {ITT_EFUNC, "AMULET OF WARDING", SCWarding, 0, MENU_NONE},
    {ITT_EFUNC, "FALCON SHIELD", SCShield, 0, MENU_NONE},
    {ITT_EFUNC, "MESH ARMOR", SCArmor, 0, MENU_NONE},
    {ITT_EFUNC, "PLATINUM HELMET", SCHelm, 0, MENU_NONE}
};

static MenuItem_t WeaponsItems[] = {
    {ITT_EFUNC, "GIVE THEM ALL AT ONCE", SCWeaponA, 0, MENU_NONE},
    {},
    {ITT_EFUNC, "", SCWeaponB, 0, MENU_NONE},
    {ITT_EFUNC, "", SCWeaponC, 0, MENU_NONE},
    {ITT_EFUNC, "", SCWeaponD, 0, MENU_NONE},
    {ITT_EFUNC, "GIVE BLUE MANA", SCManaA, 0, MENU_NONE},
    {ITT_EFUNC, "GIVE GREEN MANA", SCManaB, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON PIECE NO. 1", SCPieceA, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON PIECE NO. 2", SCPieceB, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON PIECE NO. 3", SCPieceC, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON PIECE NO. 4", SCPieceD, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON PIECE NO. 5", SCPieceE, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON PIECE NO. 6", SCPieceF, 0, MENU_NONE},
    {ITT_EFUNC, "WEAPON PIECE NO. 7", SCPieceG, 0, MENU_NONE}
};

static MenuItem_t ArtifactsItems[] = {
    {ITT_EFUNC, "GIVE THEM ALL AT ONCE", SCArtifact, 0, MENU_NONE},
    {},
    {ITT_EFUNC, "ICON OF THE DEFENDER", SCDefender, 0, MENU_NONE},
    {ITT_EFUNC, "QUARTZ FLASK", SCQuartz, 0, MENU_NONE},
    {ITT_EFUNC, "MYSTIC URN", SCUrn, 0, MENU_NONE},
    {ITT_EFUNC, "MYSTIC AMBIT INCANT", SCIncant, 0, MENU_NONE},
    {ITT_EFUNC, "DARK SERVANT", SCServant, 0, MENU_NONE},
    {ITT_EFUNC, "TORCH", SCTorch, 0, MENU_NONE},
    {ITT_EFUNC, "PORKALATOR", SCPorkalator, 0, MENU_NONE},
    {ITT_EFUNC, "WINGS OF WRATH", SCWings, 0, MENU_NONE},
    {ITT_EFUNC, "DISC OF REPULSION", SCRepulsion, 0, MENU_NONE},
    {ITT_EFUNC, "FLECHETTE", SCFlechette, 0, MENU_NONE},
    {ITT_EFUNC, "BANISHMENT DEVICE", SCBanDev, 0, MENU_NONE},
    {ITT_EFUNC, "BOOTS OF SPEED", SCBoots, 0, MENU_NONE},
    {ITT_EFUNC, "CRYSTAL VIAL", SCVial, 0, MENU_NONE},
    {ITT_EFUNC, "DRAGONSKIN BRACERS", SCBracers, 0, MENU_NONE},
    {ITT_EFUNC, "CHAOS DEVICE", SCChaosDev, 0, MENU_NONE}
};

static MenuItem_t PuzzleItems[] = {
    {ITT_EFUNC, "GIVE THEM ALL AT ONCE", SCPuzzle, 0, MENU_NONE},
    {},
    {ITT_EFUNC, "YORICK'S SKULL", SCFirstpuzzitem, 0, MENU_NONE},
    {ITT_EFUNC, "HEART OF D'SPARIL", SCPuzzgembig, 0, MENU_NONE},
    {ITT_EFUNC, "RUBY PLANET", SCPuzzgemred, 0, MENU_NONE},
    {ITT_EFUNC, "EMERALD PLANET NO. 1", SCPuzzgemgreen1, 0, MENU_NONE},
    {ITT_EFUNC, "EMERALD PLANET NO. 2", SCPuzzgemgreen2, 0, MENU_NONE},
    {ITT_EFUNC, "SAPPHIRE PLANET NO. 1", SCPuzzgemblue1, 0, MENU_NONE},
    {ITT_EFUNC, "SAPPHIRE PLANET NO. 2", SCPuzzgemblue2, 0, MENU_NONE},
    {ITT_EFUNC, "DAEMON CODEX", SCPuzzbook1, 0, MENU_NONE},
    {ITT_EFUNC, "LIBER OSCURA", SCPuzzbook2, 0, MENU_NONE},
    {ITT_EFUNC, "FLAME MASK", SCPuzzskull2, 0, MENU_NONE},
    {ITT_EFUNC, "GLAIVE SEAL", SCPuzzfweapon, 0, MENU_NONE},
    {ITT_EFUNC, "HOLY RELIC", SCPuzzcweapon, 0, MENU_NONE},
    {ITT_EFUNC, "SIGIL OF THE MAGUS", SCPuzzmweapon, 0, MENU_NONE},
    {ITT_EFUNC, "CLOCK GEAR NO. 1", SCPuzzgear1, 0, MENU_NONE},
    {ITT_EFUNC, "CLOCK GEAR NO. 2", SCPuzzgear2, 0, MENU_NONE},
    {ITT_EFUNC, "CLOCK GEAR NO. 3", SCPuzzgear3, 0, MENU_NONE},
    {ITT_EFUNC, "CLOCK GEAR NO. 4", SCPuzzgear4, 0, MENU_NONE}
};

static MenuItem_t CheatsItems[] = {
    {ITT_EFUNC, "GOD MODE", SCGod, 0, MENU_NONE},
    {ITT_EFUNC, "GIVE HEALTH", SCHealth, 0, MENU_NONE},
    {ITT_SETMENU, "GIVE ARMOR...", NULL, 0, MENU_ARMOR},
    {ITT_SETMENU, "GIVE WEAPONS...", NULL, 0, MENU_WEAPONS},
    {ITT_SETMENU, "GIVE ARTIFACTS...", NULL, 0, MENU_ARTIFACTS},
    {ITT_SETMENU, "GIVE KEYS...", NULL, 0, MENU_KEYS},
    {ITT_SETMENU, "GIVE PUZZLE ITEMS...", NULL, 0, MENU_PUZZLE},
    {ITT_EFUNC, "AUTOMAP REVEAL :", SCMap, 0, MENU_NONE},
    {},
    {ITT_LRFUNC, "WARP TO MAP :", SCWarp, 0, MENU_NONE},
    {},
    {},
    {ITT_EFUNC, "EXECUTE WARPING", SCWarpNow, 0, MENU_NONE},
    {},
    {ITT_EFUNC, "KILL ALL ENEMIES", SCKill, 0, MENU_NONE},
    {ITT_EFUNC, "NOCLIP", SCNoclip, 0, MENU_NONE},
    {ITT_LRFUNC, "PLAYER CLASS :", SCPClass, 0, MENU_NONE},
    {ITT_EFUNC, "MORPTH TO A PIG", SCPig, 0, MENU_NONE},
    {ITT_EFUNC, "COMMIT SUICIDE", SCSuicide, 0, MENU_NONE},
    {ITT_EFUNC, "RESTART MAP", SCRestart, 0, MENU_NONE}
};

static MenuItem_t RecordItems[] = {
    {ITT_LRFUNC, "PLAYER CLASS :", SCRPClass, 0, MENU_NONE},
    {},
    {ITT_LRFUNC, "CHOOSE MAP :", SCRMap, 0, MENU_NONE},
    {},
    {},
    {ITT_LRFUNC, "CHOOSE SKILL :", SCRSkill, 0, MENU_NONE},
    {ITT_EFUNC, "START RECORDING", SCRecord, 0, MENU_NONE}
};

static Menu_t KeysMenu = {
    85, 20,
    DrawKeysMenu,
    13, KeysItems,
    0,
    MENU_KEYS
};

static Menu_t ArmorMenu = {
    85, 50,
    DrawArmorMenu,
    6, ArmorItems,
    0,
    MENU_ARMOR
};

static Menu_t WeaponsMenu = {
    85, 10,
    DrawWeaponsMenu,
    14, WeaponsItems,
    0,
    MENU_WEAPONS
};

static Menu_t ArtifactsMenu = {
    95, 20,
    DrawArtifactsMenu,
    17, ArtifactsItems,
    0,
    MENU_ARTIFACTS
};

static Menu_t PuzzleMenu = {
    80, 10,
    DrawPuzzleMenu,
    19, PuzzleItems,
    0,
    MENU_PUZZLE
};

static Menu_t CheatsMenu = {
    50, 10,
    DrawCheatsMenu,
    19, CheatsItems,
    0,
    MENU_CHEATS
};

static Menu_t RecordMenu = {
    70, 10,
    DrawRecordMenu,
    7, RecordItems,
    0,
    MENU_RECORD
};

static Menu_t *Menus[] = {
    &MainMenu,
    &ClassMenu,
    &SkillMenu,
    &OptionsMenu,
    &ScreenMenu,
    &ControlMenu,
    &BindingsMenu,
    &SoundMenu,
    &SystemMenu,
    &GameMenu,
    &DebugMenu,
//    &TestMenu,
    &KeysMenu,
    &ArmorMenu,
    &WeaponsMenu,
    &ArtifactsMenu,
    &PuzzleMenu,
    &CheatsMenu,
    &FilesMenu,
    &LoadMenu,
    &SaveMenu,
    &RecordMenu
};

static char *GammaText[] = {
    TXT_GAMMA_LEVEL_OFF,
    TXT_GAMMA_LEVEL_1,
    TXT_GAMMA_LEVEL_2,
    TXT_GAMMA_LEVEL_3,
    TXT_GAMMA_LEVEL_4
};

// CODE --------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// PROC MN_Init
//
//---------------------------------------------------------------------------

void MN_Init(void)
{
    InitFonts();
    MenuActive = false;
//      messageson = true;              // Set by defaults in .CFG
    MauloBaseLump = W_GetNumForName("FBULA0");  // ("M_SKL00");
/*
    if(HEXEN_MACDEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
    {
	TorchBaseLump = W_GetNumForName("CNDLA0");
    }

    if(gameflags & GF_MULTI)
    { //disable cheats (crashing)
	FilesMenu.itemCount = 3;
	CurrentItPos = 0;
    }
*/
    if(!debugmode)
    {
	OptionsMenu.itemCount = 5;
	CurrentItPos = 0;
    }
    else
	OptionsMenu.itemCount = 6;
/*
    if(debugmode)
	GameMenu.itemCount = 7;
    else
	GameMenu.itemCount = 6;
*/
    if(HEXEN_BETA || HEXEN_DEMO || HEXEN_BETA_DEMO)	// "Retail Store Beta" & "4 Level Beta Release"
    {							// only	contain 1 "CLOCK GEAR" puzzle item
	PuzzleMenu.itemCount = 16;			// (all others have all 4 of them)
    }
}

//---------------------------------------------------------------------------
//
// PROC InitFonts
//
//---------------------------------------------------------------------------

static void InitFonts(void)
{
    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
    {
	FontAYellowBaseLump = W_GetNumForName("FONTAY_S")+1;
//	FontCYellowBaseLump = W_GetNumForName("FONTCY_S");
    }
/*
    FontARedBaseLump = W_GetNumForName("FONTAR_S")+1;
    FontCRedBaseLump = W_GetNumForName("FONTCR_S");
*/
    FontBBaseLump = W_GetNumForName("FONTB_S") + 1;
//    FontDBaseLump = W_GetNumForName("FONTD_S");

    FontABaseLump = W_GetNumForName("FONTA_S") + 1;
//    FontCBaseLump = W_GetNumForName("FONTC_S");
}

//---------------------------------------------------------------------------
//
// PROC MN_DrTextA
//
// Draw text using font A.
//
//---------------------------------------------------------------------------

void MN_DrTextA(char *text, int x, int y)
{
    char c;
    patch_t *p;

    while ((c = *text++) != 0)
    {
        if (c < 33)
        {
            x += 5;
        }
        else
        {
            p = W_CacheLumpNum(FontABaseLump + c - 33, PU_CACHE);
            V_DrawPatch(x, y, p);
            x += SHORT(p->width) - 1;
        }
/*		// FOR PSP: (NOT REQUIRED YET) (CHANGE GAME LANGUAGE WHEN USING HEXEN 64 FUNCTIONS)
	switch(c)
	{	// WE NEED EXTRA CHARS FOR THE GERMAN LANGUAGE
	    case '\x8E':	//	\x84 = 'ä'	||	\x8E = Ä
		p = W_CacheLumpNum(FontCBaseLump+1, PU_CACHE);
		V_DrawPatch(x-5, y, p);
//		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
	    case '\x99':	//	\x94 = 'ö'	||	\x99 = Ö
		p = W_CacheLumpNum(FontCBaseLump+2, PU_CACHE);
		V_DrawPatch(x-5, y, p);
//		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
	    case '\x9A':	//	\x81 = 'ü'	||	\x9A = Ü
		p = W_CacheLumpNum(FontCBaseLump+3, PU_CACHE);
		V_DrawPatch(x-5, y, p);
//		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
		// DISABLED AS IT'S NOT YET NEEDED AND THERE ARE NO LUMPS (YET) FOR 'ß' IN PSPHEXEN.WAD
//	    case '\xE1':	//	\xE1 = 'ß'
//		p = W_CacheLumpNum(FontCBaseLump+4, PU_CACHE);
//		V_DrawPatch(x, y, p);
//		x += p->width-6;		// NOT FOR THE WII
//		x += SHORT(p->width)-6;		// FOR THE WII
//		break;
	}
*/
    }
}
/*
void MN_DrTextARed(char *text, int x, int y)
{
    char c;
    patch_t *p;

    while((c = *text++) != 0)
    {
	if(c < 33)
	{
	    x += 5;
	}
	else
	{
	    p = W_CacheLumpNum(FontARedBaseLump+c-33, PU_CACHE);
	    V_DrawPatch(x, y, p);
	    x += SHORT(p->width)-1;
	}
		// FOR PSP: (NOT REQUIRED YET) (CHANGE GAME LANGUAGE WHEN USING HEXEN 64 FUNCTIONS)
	switch(c)
	{	// WE NEED EXTRA CHARS FOR THE GERMAN LANGUAGE
	    case '\x8E':	//	\x84 = 'ä'	||	\x8E = Ä
		p = W_CacheLumpNum(FontCRedBaseLump+1, PU_CACHE);
		V_DrawPatch(x-5, y, p);
		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
	    case '\x99':	//	\x94 = 'ö'	||	\x99 = Ö
		p = W_CacheLumpNum(FontCRedBaseLump+2, PU_CACHE);
		V_DrawPatch(x-5, y, p);
		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
	    case '\x9A':	//	\x81 = 'ü'	||	\x9A = Ü
		p = W_CacheLumpNum(FontCRedBaseLump+3, PU_CACHE);
		V_DrawPatch(x-5, y, p);
		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
		// DISABLED AS IT'S NOT YET NEEDED AND THERE ARE NO LUMPS (YET) FOR 'ß' IN PSPHEXEN.WAD
//	    case '\xE1':	//	\xE1 = 'ß'
//		p = W_CacheLumpNum(FontCRedBaseLump+4, PU_CACHE);
//		V_DrawPatch(x, y, p);
//		x += p->width-6;		// FOR THE WII
//		x += SHORT(p->width)-6;		// FOR THE WII
//		break;
	}
    }
}
*/
//==========================================================================
//
// MN_DrTextAYellow
//
//==========================================================================

void MN_DrTextAYellow(char *text, int x, int y)
{
    char c;
    patch_t *p;

    while ((c = *text++) != 0)
    {
        if (c < 33)
        {
            x += 5;
        }
        else
        {
            p = W_CacheLumpNum(FontAYellowBaseLump + c - 33, PU_CACHE);
            V_DrawPatch(x, y, p);
//            x += p->width - 1;		// FIXME: WTF... THIS LINE CAUSES A CRASH ON THE WII
            x += SHORT(p->width) - 1;		// FIX: THIS SHOULD'VE BEEN IT, MR. SIMON HOWARD ;o)
        }
/*		// FOR PSP: (NOT REQUIRED YET) (CHANGE GAME LANGUAGE WHEN USING HEXEN 64 FUNCTIONS)
	switch(c)
	{	// WE NEED EXTRA CHARS FOR THE GERMAN LANGUAGE
	    case '\x8E':	//	\x84 = 'ä'	||	\x8E = Ä
		p = W_CacheLumpNum(FontCYellowBaseLump+1, PU_CACHE);
		V_DrawPatch(x-5, y, p);
//		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
	    case '\x99':	//	\x94 = 'ö'	||	\x99 = Ö
		p = W_CacheLumpNum(FontCYellowBaseLump+2, PU_CACHE);
		V_DrawPatch(x-5, y, p);
//		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
	    case '\x9A':	//	\x81 = 'ü'	||	\x9A = Ü
		p = W_CacheLumpNum(FontCYellowBaseLump+3, PU_CACHE);
		V_DrawPatch(x-5, y, p);
//		x += p->width-16;		// NOT FOR THE WII
		x += SHORT(p->width)-16;	// FOR THE WII
		break;
		// DISABLED AS IT'S NOT YET NEEDED AND THERE ARE NO LUMPS (YET) FOR 'ß' IN PSPHEXEN.WAD
//	    case '\xE1':	//	\xE1 = 'ß'
//		p = W_CacheLumpNum(FontCYellowBaseLump+4, PU_CACHE);
//		V_DrawPatch(x, y, p);
//		x += p->width-6;		// NOT FOR THE WII
//		x += SHORT(p->width)-6;		// FOR THE WII
//		break;
	}
*/
    }
}

//---------------------------------------------------------------------------
//
// FUNC MN_TextAWidth
//
// Returns the pixel width of a string using font A.
//
//---------------------------------------------------------------------------

int MN_TextAWidth(char *text)
{
    char c;
    int width;
    patch_t *p;

    width = 0;
    while ((c = *text++) != 0)
    {
        if (c < 33)
        {
            width += 5;
        }
        else
        {
            p = W_CacheLumpNum(FontABaseLump + c - 33, PU_CACHE);
            width += SHORT(p->width) - 1;
        }
    }
    return (width);
}

//---------------------------------------------------------------------------
//
// PROC MN_DrTextB
//
// Draw text using font B.
//
//---------------------------------------------------------------------------

void MN_DrTextB(char *text, int x, int y)
{
    char c;
    patch_t *p;

    while ((c = *text++) != 0)
    {
        if (c < 33)
        {
            x += 8;
        }
        else
        {
            p = W_CacheLumpNum(FontBBaseLump + c - 33, PU_CACHE);
            V_DrawPatch(x, y, p);
            x += SHORT(p->width) - 1;
        }
/*		// FOR PSP: (NOT REQUIRED YET) (CHANGE GAME LANGUAGE WHEN USING HEXEN 64 FUNCTIONS)
	switch(c)
	{	// WE NEED EXTRA CHARS FOR THE GERMAN LANGUAGE
	    case '\x8E':	//	\x84 = 'ä'	||	\x8E = Ä
		p = W_CacheLumpNum(FontDBaseLump+1, PU_CACHE);
		V_DrawPatch(x-6, y, p);
//		x += p->width-22;		// NOT FOR THE WII
		x += SHORT(p->width)-22;	// FOR THE WII
		break;
	    case '\x99':	//	\x94 = 'ö'	||	\x99 = Ö
		p = W_CacheLumpNum(FontDBaseLump+2, PU_CACHE);
		V_DrawPatch(x-7, y, p);
//		x += p->width-23;		// NOT FOR THE WII
		x += SHORT(p->width)-23;	// FOR THE WII
		break;
	    case '\x9A':	//	\x81 = 'ü'	||	\x9A = Ü
		p = W_CacheLumpNum(FontDBaseLump+3, PU_CACHE);
		V_DrawPatch(x-7, y, p);
//		x += p->width-23;		// NOT FOR THE WII
		x += SHORT(p->width)-23;	// FOR THE WII
		break;
		// DISABLED AS IT'S NOT YET NEEDED AND THERE ARE NO LUMPS (YET) FOR 'ß' IN PSPHEXEN.WAD
//	    case '\xE1':	//	\xE1 = 'ß'
//		p = W_CacheLumpNum(FontDBaseLump+4, PU_CACHE);
//		V_DrawPatch(x, y, p);
//		x += p->width-6;		// NOT FOR THE WII
//		x += SHORT(p->width)-6;		// FOR THE WII
//		break;
	}
*/
    }
}

//---------------------------------------------------------------------------
//
// FUNC MN_TextBWidth
//
// Returns the pixel width of a string using font B.
//
//---------------------------------------------------------------------------

int MN_TextBWidth(char *text)
{
    char c;
    int width;
    patch_t *p;

    width = 0;
    while ((c = *text++) != 0)
    {
        if (c < 33)
        {
            width += 5;
        }
        else
        {
            p = W_CacheLumpNum(FontBBaseLump + c - 33, PU_CACHE);
            width += SHORT(p->width) - 1;
        }
    }
    return (width);
}

//---------------------------------------------------------------------------
//
// PROC MN_Ticker
//
//---------------------------------------------------------------------------

void MN_Ticker(void)
{
    if (MenuActive == false)
    {
        return;
    }
    MenuTime++;
}

//---------------------------------------------------------------------------
//
// PROC MN_Drawer
//
//---------------------------------------------------------------------------

char *QuitEndMsg[] = {
    "ARE YOU SURE YOU WANT TO QUIT?",
    "ARE YOU SURE YOU WANT TO END THE GAME?",
/*
    "DO YOU WANT TO QUICKSAVE THE GAME NAMED",
    "DO YOU WANT TO QUICKLOAD THE GAME NAMED",
    "ARE YOU SURE YOU WANT TO SUICIDE?"
*/
};

char *NotPlaying[] = {
    "YOU CAN'T SAVE IF YOU AREN'T PLAYING!",
};

#include <wiiuse/wpad.h>
#include <SDL/SDL.h>

char			fpsDisplay[100];
int			fps = 0;		// FOR PSP: calculating the frames per second

u64 GetTicks(void)
{
    return (u64)SDL_GetTicks();
}

void FPS(int FramesPerSecond)
{
    int tickfreq = 1000;

    static int fpsframecount = 0;
    static u64 fpsticks;

    fpsframecount++;

    if(GetTicks() >= fpsticks + tickfreq)
    {
	fps = fpsframecount;
	fpsframecount = 0;
	fpsticks = GetTicks();
    }
    sprintf( fpsDisplay, "FPS: %d", fps );

    if(FramesPerSecond)
    {
	MN_DrTextA(fpsDisplay, 0, 30);
    }
}

void MN_Drawer(void)
{
    int i;
    int x;
    int y;
    MenuItem_t *item;
    char *selName;

    static player_t* player;
    player = &players[consoleplayer];
/*
    if(other_info)
    	MN_DrTextA(idle_time_textbuffer, 0, 180);

    if(memory_info)
    {
    	MN_DrTextA(allocated_ram_textbuffer, 0, 40);
    	MN_DrTextA(free_ram_textbuffer, 0, 50);
    	MN_DrTextA(max_free_ram_textbuffer, 0, 60);
    }

    if(battery_info)
    {
  	MN_DrTextA(unit_plugged_textbuffer, 0, 70);
    	MN_DrTextA(battery_present_textbuffer, 0, 80);
    	MN_DrTextA(battery_charging_textbuffer, 0, 90);
    	MN_DrTextA(battery_charging_status_textbuffer, 0, 100);
    	MN_DrTextA(battery_low_textbuffer, 0, 110);
    	MN_DrTextA(battery_lifetime_percent_textbuffer, 0, 120);
    	MN_DrTextA(battery_lifetime_int_textbuffer, 0, 130);
    	MN_DrTextA(battery_temp_textbuffer, 0, 140);
    	MN_DrTextA(battery_voltage_textbuffer, 0, 150);
    }

    if(cpu_info)
    {
    	MN_DrTextA(processor_clock_textbuffer, 0, 160);
    	MN_DrTextA(processor_bus_textbuffer, 0, 170);
    }
*/
    if(FramesPerSecond==1)
    {
	FPS(1);
    }
    else if(FramesPerSecond==0)
    {
	FPS(0);
    }
/*
    if (player->playerstate == PST_DEAD	&&
	!MenuActive 			&&
	!askforquit			&&
	leveltime&16			&&
	gamestate == GS_LEVEL)
    {
	if(ninty==1 && !MenuActive && !askforquit && leveltime&16 && gamestate == GS_LEVEL)
	{
	    MN_DrTextB(PRESS_BUTTON,143-(MN_TextAWidth(PRESS_BUTTON)>>1), 150);		
	}
    }
*/
    // Beta blinker ***
    if((HEXEN_BETA			&&
	!MenuActive			&&
	!askforquit			&&
	leveltime&16			&&
	gamestate == GS_LEVEL)		||
	(HEXEN_BETA_DEMO		&&
	!MenuActive			&&
	!askforquit			&&
	leveltime&16			&&
	gamestate == GS_LEVEL))
    {
	MN_DrTextA(BETA_FLASH_TEXT,160-(MN_TextAWidth(BETA_FLASH_TEXT)>>1), 12);// DISPLAYS BLINKING
										// "BETA" MESSAGE
    }

    if((HEXEN_BETA && MenuActive) || (HEXEN_BETA_DEMO && MenuActive))
    {
	MN_DrTextA(" ",160, 12);						// DISABLES MESSAGE
    }
/*
    if((ninty == 1 && !MenuActive && demoplayback && players[consoleplayer].playerstate == PST_LIVE) ||
	(ninty == 1 && !MenuActive && demoplayback && players[consoleplayer].playerstate == PST_DEAD))
    {
	V_DrawPatch(88, 0, W_CacheLumpName("M_HTIC", PU_CACHE));
	if(HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACDEMO || HEXEN_MACFULL || HEXEN_DEMO)
	{	// ADD TIMER HERE TO SWITCH BETWEEN MESSAGES WHILE DEMO IS RUNNING
	    MN_DrTextAYellow("THE QUEST BEGINS ON THE SEVEN PORTALS", 30, 150);
	}
	else if(HEXEN_BETA || HEXEN_BETA_DEMO)
	{	// ADD TIMER HERE TO SWITCH BETWEEN MESSAGES WHILE DEMO IS RUNNING
	    MN_DrTextA("THE QUEST BEGINS ON THE SEVEN PORTALS", 30, 150);
	}
    }
*/
    if(!askforquit && gamestate == GS_LEVEL)
    {
	if(mapname==1)
	    MN_DrTextA(P_GetMapName(gamemap), 38, 144);	// DISPLAYS THE NAME OF A GAME MAP

	if(timer==1)
	    DrawWorldTimer();				// DISPLAYS THE GAME TIME

	if(coords==1)					// FIXME: DOES IT CRASH THE WII WHEN USED?
	{
	    char textBuffer[50];

	    sprintf(textBuffer, "MAP %d (%d)  X:%5d  Y:%5d  Z:%5d",
		    P_GetMapWarpTrans(gamemap), gamemap,	player->mo->x >> FRACBITS,
								player->mo->y >> FRACBITS,
								player->mo->z >> FRACBITS);

	    MN_DrTextA(textBuffer, 50, 24);			// DISPLAYS THE PLAYER'S COORDINATES
	}
	if(version==1)
	{
	    char date[50];

	    sprintf(date,"%d-%02d-%02d", YEAR, MONTH + 1, DAY);

	    MN_DrTextA(VERSIONTEXT,55, 36);			// DISPLAYS BINARY VERSION
	    MN_DrTextA(date, 195, 36);			// DISPLAYS THE DATE
	}
    }

    if (MenuActive == false)
    {
        if (askforquit)
        {
            MN_DrTextA(QuitEndMsg[typeofask - 1], 160 -
                       MN_TextAWidth(QuitEndMsg[typeofask - 1]) / 2, 80);
/*
            if (typeofask == 3)
            {
                MN_DrTextA(SlotText[quicksave - 1], 160 -
                           MN_TextAWidth(SlotText[quicksave - 1]) / 2, 90);
                MN_DrTextA("?", 160 +
                           MN_TextAWidth(SlotText[quicksave - 1]) / 2, 90);
            }

            if (typeofask == 4)
            {
                MN_DrTextA(SlotText[quickload - 1], 160 -
                           MN_TextAWidth(SlotText[quickload - 1]) / 2, 90);
                MN_DrTextA("?", 160 +
                           MN_TextAWidth(SlotText[quicksave - 1]) / 2, 90);
            }
*/
            UpdateState |= I_FULLSCRN;
        }
	if(askforsave)
	{
            MN_DrTextA(NotPlaying[typeofask2 - 1], 160 -
                       MN_TextAWidth(NotPlaying[typeofask2 - 1]) / 2, 80);

            UpdateState |= I_FULLSCRN;
	}
        return;
    }
    else
    {
        UpdateState |= I_FULLSCRN;
        if (InfoType)
        {
            MN_DrawInfo();
            return;
        }
        if (screenblocks < 10)
        {
            BorderNeedRefresh = true;
        }
        if (CurrentMenu->drawFunc != NULL)
        {
            CurrentMenu->drawFunc();
        }
        x = CurrentMenu->x;
        y = CurrentMenu->y;
        item = CurrentMenu->items;

	if (item->type == ITT_SETKEY)
	    item += FirstKey;

        for (i = 0; i < CurrentMenu->itemCount; i++)
        {
            if (item->type != ITT_EMPTY && item->text)
            {
		if(CurrentMenu==&ArmorMenu	||
		   CurrentMenu==&CheatsMenu	||
		   CurrentMenu==&ArtifactsMenu	||
		   CurrentMenu==&KeysMenu	||
		   CurrentMenu==&PuzzleMenu	||
		   CurrentMenu==&WeaponsMenu	||
		   CurrentMenu==&BindingsMenu)
			MN_DrTextA(item->text, x, y);
		else if(CurrentMenu)
		    MN_DrTextB(item->text, x, y);
            }

	    if (CurrentMenu==&ArmorMenu		||
		CurrentMenu==&CheatsMenu	||
		CurrentMenu==&ArtifactsMenu	||
		CurrentMenu==&KeysMenu		||
		CurrentMenu==&PuzzleMenu	||
		CurrentMenu==&WeaponsMenu	||
		CurrentMenu==&BindingsMenu)
			y += ITEM_HEIGHT_SMALL;
	    else if(CurrentMenu)
		y += ITEM_HEIGHT;

            item++;
        }
	if(CurrentMenu==&ArmorMenu		||
	   CurrentMenu==&CheatsMenu		||
	   CurrentMenu==&ArtifactsMenu		||
	   CurrentMenu==&KeysMenu		||
	   CurrentMenu==&PuzzleMenu		||
	   CurrentMenu==&WeaponsMenu		||
	   CurrentMenu==&BindingsMenu)
	{
	    y = CurrentMenu->y+(CurrentItPos*ITEM_HEIGHT_SMALL)+SELECTOR_YOFFSET_SMALL;
	    selName = MenuTime&16 ? "M_SLCTR3" : "M_SLCTR4";
	    V_DrawPatch(x+SELECTOR_XOFFSET_SMALL, y, 
		W_CacheLumpName(selName, PU_CACHE));
	}
	else if(CurrentMenu)
	{
	    y = CurrentMenu->y+(CurrentItPos*ITEM_HEIGHT)+SELECTOR_YOFFSET;
	    selName = MenuTime&16 ? "M_SLCTR1" : "M_SLCTR2";
	    V_DrawPatch(x+SELECTOR_XOFFSET, y, 
		W_CacheLumpName(selName, PU_CACHE));
	}
    }
}

//---------------------------------------------------------------------------
//
// PROC DrawMainMenu
//
//---------------------------------------------------------------------------

static void DrawMainMenu(void)
{
    int frame;

    frame = (MenuTime / 5) % 7;

//    if(ninty==0)
    {
    	V_DrawPatch(88, 0, W_CacheLumpName("M_HTIC", PU_CACHE));
// Old Gold skull positions: (40, 10) and (232, 10)
    	V_DrawPatch(37, 80, W_CacheLumpNum(MauloBaseLump + (frame + 2) % 7,
                                       PU_CACHE));
    	V_DrawPatch(278, 80, W_CacheLumpNum(MauloBaseLump + frame, PU_CACHE));
    }
/*
    else if(ninty==1 && paused)
    {
	MN_DrTextB("PAUSED", 160-MN_TextBWidth("PAUSED")/2, 10);

	if(HEXEN_MACDEMO || HEXEN_BETA || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
	{
	    V_DrawPatch(105, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	    V_DrawPatch(210, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	}
    }
*/
}

//==========================================================================
//
// DrawClassMenu
//
//==========================================================================

static void DrawClassMenu(void)
{
    pclass_t class;
    static char *boxLumpName[3] = {
        "m_fbox",
        "m_cbox",
        "m_mbox"
    };
    static char *walkLumpName[3] = {
        "m_fwalk1",
        "m_cwalk1",
        "m_mwalk1"
    };

    MN_DrTextB("CHOOSE CLASS:", 34, 24);
    class = (pclass_t) CurrentMenu->items[CurrentItPos].option;
    V_DrawPatch(174, 8, W_CacheLumpName(boxLumpName[class], PU_CACHE));
    V_DrawPatch(174 + 24, 8 + 12, 
                W_CacheLumpNum(W_GetNumForName(walkLumpName[class])
                               + ((MenuTime >> 3) & 3), PU_CACHE));
}

//---------------------------------------------------------------------------
//
// PROC DrawSkillMenu
//
//---------------------------------------------------------------------------

static void DrawSkillMenu(void)
{
    MN_DrTextB("CHOOSE SKILL LEVEL:", 74, 16);
}

//---------------------------------------------------------------------------
//
// PROC DrawFilesMenu
//
//---------------------------------------------------------------------------

static void DrawFilesMenu(void)
{
/*
    int frame;
    frame = (MenuTime/5)%7;
*/
// clear out the quicksave/quickload stuff
/*
    quicksave = 0;
    quickload = 0;
*/
    P_ClearMessage(&players[consoleplayer]);
/*
    if(ninty==1)
    {
	if(HEXEN_MACDEMO || HEXEN_BETA || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
	{
	    V_DrawPatch(95, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	    V_DrawPatch(230, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	}
	MN_DrTextB("GAME FILES", 160-MN_TextBWidth("GAME FILES")/2, 10);
    }
*/
    if(debugmode)
	FilesMenu.itemCount = 5;
    else if(!debugmode)
	FilesMenu.itemCount = 4;
}

//---------------------------------------------------------------------------
//
// PROC DrawLoadMenu
//
//---------------------------------------------------------------------------

static void DrawLoadMenu(void)
{
/*
    int frame;
    frame = (MenuTime/5)%7;
*/
    MN_DrTextB("LOAD GAME", 160 - MN_TextBWidth("LOAD GAME") / 2, 10);
    if (!slottextloaded)
    {
        MN_LoadSlotText();
    }
    DrawFileSlots(&LoadMenu);
/*
    if(ninty==1)
    {
	if(HEXEN_MACDEMO || HEXEN_BETA || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
	{
	    V_DrawPatch(95, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	    V_DrawPatch(230, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	}
    }
*/
    MN_DrTextA("* INDICATES A SAVEGAME THAT WAS", 55, 151);
    MN_DrTextA("CREATED USING AN OPTIONAL (P)WAD!", 52, 161);

    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

//---------------------------------------------------------------------------
//
// PROC DrawSaveMenu
//
//---------------------------------------------------------------------------

static void DrawSaveMenu(void)
{
/*
    int frame;
    frame = (MenuTime/5)%7;
*/
    if(!usergame || demoplayback)
    {
	MenuActive = false;
	askforsave = true;
	typeofask2 = 1;              //not in a game
	paused = false;
	return;
    }

    MN_DrTextB("SAVE GAME", 160 - MN_TextBWidth("SAVE GAME") / 2, 10);
    if (!slottextloaded)
    {
        MN_LoadSlotText();
    }
    DrawFileSlots(&SaveMenu);
/*
    if(ninty==1)
    {
	if(HEXEN_MACDEMO || HEXEN_BETA || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
	{
	    V_DrawPatch(95, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	    V_DrawPatch(230, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	}
    }
*/
    MN_DrTextA("* INDICATES A SAVEGAME THAT WAS", 55, 151);
    MN_DrTextA("CREATED USING AN OPTIONAL (P)WAD!", 52, 161);

    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static boolean ReadDescriptionForSlot(int slot, char *description)
{
    FILE *fp;
    boolean found;
    char name[100];
    char versionText[HXS_VERSION_TEXT_LENGTH];

    if(!datadisc && HEXEN_BETA)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePathBetaUSB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePathBetaSD, slot);
    }

    if(!datadisc && HEXEN_BETA_DEMO)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePathBetaDemoUSB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePathBetaDemoSD, slot);
    }

    if(!datadisc && HEXEN_MACDEMO)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePathMacDemoUSB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePathMacDemoSD, slot);
    }

    if(!datadisc && HEXEN_MACFULL)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePathMacFullUSB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePathMacFullSD, slot);
    }

    if(!datadisc && HEXEN_DEMO)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePathDemoUSB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePathDemoSD, slot);
    }

    if(!datadisc && HEXEN_1_0)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePath10USB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePath10SD, slot);
    }

    if(!datadisc && HEXEN_1_1)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePath11USB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePath11SD, slot);
    }

    if(datadisc && HEXDD_1_0)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePathDD10USB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePathDD10SD, slot);
    }

    if(datadisc && HEXDD_1_1)
    {
	if(usb)
	    sprintf(name, "%s/hex%d.hxs", SavePathDD11USB, slot);
	else if(sd)
	    sprintf(name, "%s/hex%d.hxs", SavePathDD11SD, slot);
    }

    fp = fopen(name, "rb");

    if (fp == NULL)
    {
        return false;
    }

    found = fread(description, HXS_DESCRIPTION_LENGTH, 1, fp) == 1
         && fread(versionText, HXS_VERSION_TEXT_LENGTH, 1, fp) == 1;

    found = found && strcmp(versionText, HXS_VERSION_TEXT) == 0;

    fclose(fp);

    return found;
}

//===========================================================================
//
// MN_LoadSlotText
//
// For each slot, looks for save games and reads the description field.
//
//===========================================================================

void MN_LoadSlotText(void)
{
    char description[HXS_DESCRIPTION_LENGTH];
    int slot;

    for (slot = 0; slot < 6; slot++)
    {
        if (ReadDescriptionForSlot(slot, description))
        {
            memcpy(SlotText[slot], description, SLOTTEXTLEN);
            SlotStatus[slot] = 1;
        }
        else
        {
            memset(SlotText[slot], 0, SLOTTEXTLEN);
            SlotStatus[slot] = 0;
        }
    }
    slottextloaded = true;
}

//---------------------------------------------------------------------------
//
// PROC DrawFileSlots
//
//---------------------------------------------------------------------------

static void DrawFileSlots(Menu_t * menu)
{
    int i;
    int x;
    int y;

    x = menu->x;
    y = menu->y;
    for (i = 0; i < 6; i++)
    {
        V_DrawPatch(x, y, W_CacheLumpName("M_FSLOT", PU_CACHE));

        if (SlotStatus[i])
            MN_DrTextA(SlotText[i], x + 5, y + 5);
	else if(!SlotStatus[i])
	    MN_DrTextA(TXT_EMPTYSTRING, x+5, y+5);

        y += ITEM_HEIGHT;
    }
}

//---------------------------------------------------------------------------
//
// PROC DrawOptionsMenu
//
//---------------------------------------------------------------------------

static void DrawOptionsMenu(void)
{
/*
    int frame;
    frame = (MenuTime/5)%7;

    if(ninty==1)
    {
	if(HEXEN_MACDEMO || HEXEN_BETA || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
	{
	    V_DrawPatch(105, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	    V_DrawPatch(220, 25, W_CacheLumpNum(TorchBaseLump+frame%3, PU_CACHE));
	}
    }

    if(!debugmode)
	OptionsMenu.itemCount = 5;
    else if(debugmode)
	OptionsMenu.itemCount = 6;
*/
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawSystemMenu(void)
{
/*
    if(mhz333)
	MN_DrTextB("333 MHZ", 190, 50);
    else
	MN_DrTextB("222 MHZ", 190, 50);
*/
    if(DisplayTicker)
	MN_DrTextB("ON", 240, 60);
    else
	MN_DrTextB("OFF", 240, 60);

    if(FramesPerSecond)
	MN_DrTextB("ON", 240, 80);
    else
	MN_DrTextB("OFF", 240, 80);

    if(wiilight)
	MN_DrTextB("ON", 240, 100);
    else
	MN_DrTextB("OFF", 240, 100);
}

static void DrawGameMenu(void)
{
    if(drawgrid == 1)
	MN_DrTextB("ON", 245, 30);
    else if(drawgrid == 0)
	MN_DrTextB("OFF", 245, 30);

    if(followplayer)
	MN_DrTextB("ON", 245, 50);
    else
	MN_DrTextB("OFF", 245, 50);

    if(am_rotate == true)
	MN_DrTextB("ON", 245, 70);
    else if(am_rotate == false)
	MN_DrTextB("OFF", 245, 70);

	MN_DrTextB("----------------------", 50, 90);

    if(messageson)
	MN_DrTextB("ON", 245, 110);
    else
	MN_DrTextB("OFF", 245, 110);

    if(crosshair)
	MN_DrTextB("ON", 245, 130);
    else
	MN_DrTextB("OFF", 245, 130);

    if(use_vanilla_weapon_change)
	MN_DrTextB("SLOW", 245, 150);
    else
	MN_DrTextB("FAST", 245, 150);
/*
    if(debugmode && (HEXEN_MACDEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL))
    {
	if(ninty)
	    MN_DrTextB("ON", 245, 150);
	else
	    MN_DrTextB("OFF", 245, 150);
    }
*/
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawDebugMenu(void)
{
    if(coords==1)
	MN_DrTextB("ON", 240, 50);
    else if(coords==0)
	MN_DrTextB("OFF", 240, 50);

    if(mapname==1)
	MN_DrTextB("ON", 240, 70);
    else if(mapname==0)
	MN_DrTextB("OFF", 240, 70);

    if(DebugSound)
	MN_DrTextB("ON", 240, 90);
    else if(!DebugSound)
	MN_DrTextB("OFF", 240, 90);

    if(timer==1)
	MN_DrTextB("ON", 240, 110);
    else if(timer==0)
	MN_DrTextB("OFF", 240, 110);

    if(version==1)
	MN_DrTextB("ON", 240, 130);
    else if(version==0)
	MN_DrTextB("OFF", 240, 130);
/*
    if(battery_info)
    {
	sprintf(unit_plugged_textbuffer,"UNIT IS PLUGGED IN: %d \n",scePowerIsPowerOnline());
	sprintf(battery_present_textbuffer,"BATTERY IS PRESENT: %d \n",scePowerIsBatteryExist());
	sprintf(battery_charging_textbuffer,"BATTERY IS CHARGING: %d \n",scePowerIsBatteryCharging());
	sprintf(battery_charging_status_textbuffer,"BATTERY CHARGING STATUS: %d \n",scePowerGetBatteryChargingStatus());
	sprintf(battery_low_textbuffer,"BATTERY IS LOW: %d \n",scePowerIsLowBattery());
	sprintf(battery_lifetime_percent_textbuffer,"BATTERY LIFETIME (PERC.): %d \n",scePowerGetBatteryLifePercent());
	sprintf(battery_lifetime_int_textbuffer,"BATTERY LIFETIME (INT.): %d \n",scePowerGetBatteryLifeTime());
	sprintf(battery_temp_textbuffer,"BATTERY TEMP.: %d \n",scePowerGetBatteryTemp());
	sprintf(battery_voltage_textbuffer,"BATTERY VOLTAGE: %d \n",scePowerGetBatteryVolt());

	MN_DrTextB("ON", 240, 110);
    }
    else
	MN_DrTextB("OFF", 240, 110);

    if(cpu_info)
    {
	sprintf(processor_clock_textbuffer,"PROCESSOR CLOCK FREQ.: %d \n",scePowerGetCpuClockFrequencyInt());
	sprintf(processor_bus_textbuffer,"PROCESSOR BUS FREQ.: %d \n",scePowerGetBusClockFrequencyInt());

	MN_DrTextB("ON", 240, 130);
    }
    else
	MN_DrTextB("OFF", 240, 130);

    if(memory_info)
    {
    	sprintf(allocated_ram_textbuffer,"ALLOCATED RAM: %d BYTES\n",allocated_ram_size);

    	sprintf(free_ram_textbuffer,"CURR. FREE RAM: %d BYTES\n",sceKernelTotalFreeMemSize());

    	max_free_ram = sceKernelMaxFreeMemSize();

    	sprintf(max_free_ram_textbuffer,"MAX. FREE RAM: %d BYTES\n",max_free_ram);

	MN_DrTextB("ON", 240, 150);
    }
    else
	MN_DrTextB("OFF", 240, 150);

    if(other_info)
    {
	sprintf(idle_time_textbuffer,"IDLE TIME: %d \n",scePowerGetIdleTimer());

	MN_DrTextB("ON", 240, 170);
    }
    else
	MN_DrTextB("OFF", 240, 170);
*/
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}
/*
static void DrawTestMenu(void)
{
    MN_DrTextA("1 2 3 4 5 6 7 8 9 0", 40, 40);
    MN_DrTextA("! $ % & / ( ) = ? - . , ; : @", 50, 50);
    MN_DrTextA("A B C D E F G H I", 60, 60);
    MN_DrTextA("J K L M N O P Q R", 70, 70);
    MN_DrTextAYellow("S T U V W X Y Z", 80, 80);
    MN_DrTextA("TES\x8E \bTOS\x99 \bTER\x9A \bONE", 90, 90);
    MN_DrTextA("TES\x8E \bTOS\x99 \bTER\x9A \bONE", 100, 100);
    MN_DrTextAYellow("TES\x8E \bTOS\x99 \bTER\x9A \bONE", 110, 110);
    MN_DrTextB("TES\x8E \bTOS\x99 \bTER\x9A \bONE", 30, 130);
}
*/
static void DrawScreenMenu(void)
{
/*
    if(ninty==1)
    {
	CurrentItPos = 0;
	ScreenMenu.itemCount = 2;
    }
*/
    DrawSlider(&ScreenMenu, 1, 5, usegamma);

//    if(ninty==0)
    {
	if(!debugmode)
	    ScreenMenu.itemCount = 4;
	else if(debugmode)
	{
	    ScreenMenu.itemCount = 5;

	    if(detailLevel)
		MN_DrTextB("LOW", 170, 120);
	    else
		MN_DrTextB("HIGH", 170, 120);
	}
	DrawSlider(&ScreenMenu, 3, 9, screenblocks-3);
    }
}

static void DrawControlMenu(void)
{
    MN_DrTextB("WALKING SPEED", 38, 0);
    MN_DrTextB("TURNING SPEED", 38, 40);
    MN_DrTextB("STRAFING SPEED", 38, 80);
    MN_DrTextB("FREELOOK :", 38, 124);
    MN_DrTextB("FREELOOK SPEED", 38, 142);
    MN_DrTextB("KEY BINDINGS", 38, 179);

    DrawSlider(&ControlMenu, 1, 29, forwardmove-19);
    DrawSlider(&ControlMenu, 3, 6, turnspeed-5);
    DrawSlider(&ControlMenu, 5, 17, sidemove-16);

    MN_DrTextB(mlooktext[mouselook], 150, 124);

    DrawSlider(&ControlMenu, 8, 9, mspeed-2);

    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}
static char *stupidtable[] =
{
    "A","B","C","D","E",
    "F","G","H","I","J",
    "K","L","M","N","O",
    "P","Q","R","S","T",
    "U","V","W","X","Y",
    "Z"
};

#define CLASSIC_CONTROLLER_A		0x1
#define CLASSIC_CONTROLLER_R		0x2
#define CLASSIC_CONTROLLER_PLUS		0x4
#define CLASSIC_CONTROLLER_L		0x8
#define CLASSIC_CONTROLLER_MINUS	0x10
#define CLASSIC_CONTROLLER_B		0x20
#define CLASSIC_CONTROLLER_LEFT		0x40
#define CLASSIC_CONTROLLER_DOWN		0x80
#define CLASSIC_CONTROLLER_RIGHT	0x100
#define CLASSIC_CONTROLLER_UP		0x200
#define CLASSIC_CONTROLLER_ZR		0x400
#define CLASSIC_CONTROLLER_ZL		0x800
#define CLASSIC_CONTROLLER_HOME		0x1000
#define CLASSIC_CONTROLLER_X		0x2000
#define CLASSIC_CONTROLLER_Y		0x4000

char *Key2String (int ch)
{
// S.A.: return "[" or "]" or "\"" doesn't work
// because there are no lumps for these chars,
// therefore we have to go with "RIGHT BRACKET"
// and similar for much punctuation.  Probably
// won't work with international keyboards and
// dead keys, either.
//
    switch (ch)
    {
	case CLASSIC_CONTROLLER_UP:	return "UP ARROW";
	case CLASSIC_CONTROLLER_DOWN:	return "DOWN ARROW";
	case CLASSIC_CONTROLLER_LEFT:	return "LEFT ARROW";
	case CLASSIC_CONTROLLER_RIGHT:	return "RIGHT ARROW";
	case CLASSIC_CONTROLLER_MINUS:	return "MINUS";
	case CLASSIC_CONTROLLER_PLUS:	return "PLUS";
	case CLASSIC_CONTROLLER_HOME:	return "HOME";
	case CLASSIC_CONTROLLER_A:	return "A";
	case CLASSIC_CONTROLLER_B:	return "B";
	case CLASSIC_CONTROLLER_X:	return "X";
	case CLASSIC_CONTROLLER_Y:	return "Y";
	case CLASSIC_CONTROLLER_ZL:	return "ZL";
	case CLASSIC_CONTROLLER_ZR:	return "ZR";
	case CLASSIC_CONTROLLER_L:	return "LEFT TRIGGER";
	case CLASSIC_CONTROLLER_R:	return "RIGHT TRIGGER";
    }

    // Handle letter keys
    // S.A.: could also be done with toupper
    if (ch >= 'a' && ch <= 'z')
	return stupidtable[(ch - 'a')];

    return "?";		// Everything else
}

static void ClearControls (int cctrlskey)
{
    int cctrls;

    for (cctrls = key_bindings_start_in_cfg_at_pos; cctrls < key_bindings_end_in_cfg_at_pos; cctrls++)
    {
    	if (*doom_defaults_list[cctrls].location == cctrlskey)
	    *doom_defaults_list[cctrls].location = 0;
    }
}

static void ClearKeys (int option)
{
    *doom_defaults_list[16].location = 0;
    *doom_defaults_list[17].location = 0;
    *doom_defaults_list[18].location = 0;
    *doom_defaults_list[19].location = 0;
    *doom_defaults_list[20].location = 0;
    *doom_defaults_list[21].location = 0;
    *doom_defaults_list[22].location = 0;
    *doom_defaults_list[23].location = 0;
    *doom_defaults_list[24].location = 0;
    *doom_defaults_list[25].location = 0;
    *doom_defaults_list[26].location = 0;
    *doom_defaults_list[27].location = 0;
    *doom_defaults_list[28].location = 0;
    *doom_defaults_list[29].location = 0;
    *doom_defaults_list[30].location = 0;
}

static void ResetKeys (int option)
{
    *doom_defaults_list[16].location = CLASSIC_CONTROLLER_HOME;
    *doom_defaults_list[17].location = CLASSIC_CONTROLLER_R;
    *doom_defaults_list[18].location = CLASSIC_CONTROLLER_L;
    *doom_defaults_list[19].location = CLASSIC_CONTROLLER_MINUS;
    *doom_defaults_list[20].location = CLASSIC_CONTROLLER_LEFT;
    *doom_defaults_list[21].location = CLASSIC_CONTROLLER_DOWN;
    *doom_defaults_list[22].location = CLASSIC_CONTROLLER_RIGHT;
    *doom_defaults_list[23].location = CLASSIC_CONTROLLER_ZL;
    *doom_defaults_list[24].location = CLASSIC_CONTROLLER_ZR;
    *doom_defaults_list[25].location = CLASSIC_CONTROLLER_Y;
    *doom_defaults_list[26].location = CLASSIC_CONTROLLER_A;
    *doom_defaults_list[27].location = CLASSIC_CONTROLLER_PLUS;
    *doom_defaults_list[28].location = CLASSIC_CONTROLLER_X;
    *doom_defaults_list[29].location = CLASSIC_CONTROLLER_B;
    *doom_defaults_list[30].location = CLASSIC_CONTROLLER_UP;
}
/*
static void ButtonLayout(int option)
{
    if(option == RIGHT_DIR)
    {
	if(button_layout < 1)
	    button_layout++;
    }
    else if(button_layout)
	button_layout--;
}
*/
static void DrawBindingsMenu(void)
{
    int ctrls;

    for (ctrls = 0; ctrls < 15; ctrls++)
    {
	if (askforkey && keyaskedfor == ctrls)
	    MN_DrTextA("???", 195, (ctrls*10+20));
	else
	    MN_DrTextA(Key2String(*(doom_defaults_list[ctrls+FirstKey+16].location)),195,(ctrls*10+20));
    }
/*
    if(button_layout == 0)
    {
	MN_DrTextA("TURN LEFT", 40, 40);
	MN_DrTextA("TURN RIGHT", 40, 50);
    	MN_DrTextA("PS VITA", 195, 150);
    }
    else if(button_layout == 1)
    {
	MN_DrTextA("STRAFE LEFT", 40, 40);
	MN_DrTextA("STRAFE RIGHT", 40, 50);
    	MN_DrTextA("PSP", 195, 150);
    }
*/
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawSoundMenu(void)
{
    DrawSlider(&SoundMenu, 1, 16, snd_MaxVolume);
    DrawSlider(&SoundMenu, 3, 16, snd_MusicVolume);

    if(tracknum == 0)
	tracknum = 1;

    if(!datadisc)
	MN_DrTextB(songtext[tracknum], 220, 110);
    else if(datadisc)
	MN_DrTextB(songtextdd[tracknum], 220, 110);

    P_SetMessage(&players[consoleplayer], "MUSIC CHANGE", true);
/*
    if(loop)
	MN_DrTextB("YES", 220, 130);
    else
	MN_DrTextB("NO", 220, 130);
*/
}

static void DrawKeysMenu(void)
{
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawArmorMenu(void)
{
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawWeaponsMenu(void)
{
    if(PlayerClass[consoleplayer] == PCLASS_FIGHTER)
    {
	MN_DrTextA("TIMON'S AXE", 85, 30);
	MN_DrTextA("HAMMER OF RETRIBUTION", 85, 40);
	MN_DrTextA("QUIETUS", 85, 50);
    }
    else if(PlayerClass[consoleplayer] == PCLASS_CLERIC)
    {
	MN_DrTextA("SERPENT STAFF", 85, 30);
	MN_DrTextA("FIRESTORM", 85, 40);
	MN_DrTextA("WRAITHVERGE", 85, 50);
    }
    else if(PlayerClass[consoleplayer] == PCLASS_MAGE)
    {
	MN_DrTextA("FROST SHARDS", 85, 30);
	MN_DrTextA("ARC OF DEATH", 85, 40);
	MN_DrTextA("BLOODSCOURGE", 85, 50);
    }
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawArtifactsMenu(void)
{
    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawPuzzleMenu(void)
{
    if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
    {
	MN_DrTextA("CLOCK GEAR NO. 1", 80, 160);
	MN_DrTextA("CLOCK GEAR NO. 2", 80, 170);
	MN_DrTextA("CLOCK GEAR NO. 3", 80, 180);
	MN_DrTextA("CLOCK GEAR NO. 4", 80, 190);
    }
    else if(HEXEN_BETA || HEXEN_BETA_DEMO)
	MN_DrTextA("CLOCK GEAR", 80, 160);

    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void DrawCheatsMenu(void)
{
    static player_t* player;
    player = &players[consoleplayer];

    if (player->cheats & CF_GODMODE)
	MN_DrTextA("ON", 205, 10);
    else
	MN_DrTextA("OFF", 205, 10);

    if(!cheating)
	MN_DrTextA("OFF", 205, 80);
    else if (cheating && cheeting!=2)
	MN_DrTextA("WALLS", 205, 80);
    else if (cheating && cheeting==2)
	MN_DrTextA("WALLS / ITEMS", 205, 80);

    if(!datadisc)
    {
	if(map == 0)
	    map = 1;

	MN_DrTextA(enmaptext[map], 50, 110);

	if(map == 41)
	    MN_DrTextA("MAZE", 50, 110);
    }
    else if(datadisc)
    {
	if(map == 0)
	    map = 41;

	MN_DrTextA(maptextdd[map], 50, 110);
    }

    if (player->cheats & CF_NOCLIP)
	MN_DrTextA("ON", 205, 160);
    else
	MN_DrTextA("OFF", 205, 160);

    if(plcl==0)
	MN_DrTextA("FIGHTER", 205, 170);
    else if(plcl==1)
	MN_DrTextA("CLERIC", 205, 170);
    else if(plcl==2)
	MN_DrTextA("MAGE", 205, 170);

    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

void DetectState(void)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_DEAD)
	P_SetMessage(&players[consoleplayer], "CHEATING NOT ALLOWED - YOU ARE DEAD", true);
    else if(demoplayback && gamestate == GS_LEVEL && players[consoleplayer].playerstate == PST_LIVE)
	P_SetMessage(&players[consoleplayer], "CHEATING NOT ALLOWED IN DEMO MODE", true);
    else if(demoplayback && gamestate == GS_LEVEL && players[consoleplayer].playerstate == PST_DEAD)
	P_SetMessage(&players[consoleplayer], "CHEATING NOT ALLOWED IN DEMO MODE", true);
    else if(demoplayback && gamestate != GS_LEVEL)
	P_SetMessage(&players[consoleplayer], "CHEATING NOT ALLOWED IN DEMO MODE", true);
    else if(!demoplayback && gamestate != GS_LEVEL)
	P_SetMessage(&players[consoleplayer], "CHEATING NOT ALLOWED IN DEMO MODE", true);
/*
    else if(netgame)
	P_SetMessage(&players[consoleplayer], "CHEATING NOT ALLOWED FOR NET GAMES", true);
*/
    if(gameskill == sk_nightmare)
	P_SetMessage(&players[consoleplayer], "CHEATING NOT ALLOWED FOR NIGHTMARE SKILL", true);
}

static void SCPClass(int option)
{
    static player_t* player;
    player = &players[consoleplayer];

    if(option == RIGHT_DIR && !demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	if(plcl < 2)
	{
	    S_StartSound(NULL, SFX_TELEPORT);
	    plcl++;
	}
    }
    else if(plcl && !demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	S_StartSound(NULL, SFX_TELEPORT);
	plcl--;
    }

    if(player->morphTics)
    { // don't change class if the player is morphed
	return;
    }

    if(!demoplayback)
    {
	player->class = plcl;
	PlayerClass[consoleplayer] = plcl;
	P_PostMorphWeapon(player, WP_FIRST);
	SB_SetClassData();
	SB_state = -1;
	UpdateState |= I_FULLSCRN;
    }

    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPig(int option)
{
    static player_t* player;
    player = &players[consoleplayer];

    extern boolean P_UndoPlayerMorph(player_t *player);

    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && player->morphTics && players[consoleplayer].playerstate == PST_LIVE)
    {
	S_StartSound(NULL, SFX_TELEPORT);
	P_UndoPlayerMorph(player);
	P_SetMessage(player, "SQUEAL!!", true);
    }
    else if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	S_StartSound(NULL, SFX_TELEPORT);
	P_MorphPlayer(player);
    }

    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCSuicide(int option)
{
    static player_t* player;
    player = &players[consoleplayer];

    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	if(ran==0)
	{
	    P_SetMessage(player, "TRYING TO CHEAT?  THAT'S ONE....", true);
	    ran = 1;
	    goto end;
	}
	if(ran==1)
	{
	    P_SetMessage(player, "THAT'S TWO....", true);
	    ran = 2;
	    goto end;
	}
	if(ran==2)
	{
	    P_DamageMobj(player->mo, NULL, player->mo, 10000);
	    P_SetMessage(player, "THAT'S THREE!  TIME TO DIE.", true);
	    ran = 0;
	}
    }

    DetectState();

    end:
    {
    }

    S_StartSound(NULL, SFX_CHAT);
}

static void SCRestart(int option)
{
    static player_t* player;
    player = &players[consoleplayer];

    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	warped = 1;
	G_DeferedInitNew(gameskill, gameepisode, gamemap);
	P_SetMessage(player, TXT_CHEATWARP, true);
    }

    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCHealth(int option)
{
    static player_t* player;
    player = &players[consoleplayer];

    if(player->morphTics && !demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	player->health = player->mo->health = MAXMORPHHEALTH;
	P_SetMessage(player, TXT_CHEATHEALTH, true);
    }
    else if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	player->health = player->mo->health = MAXHEALTH;
	P_SetMessage(player, TXT_CHEATHEALTH, true);
    }

    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCRPClass(int option)
{
    if(option == RIGHT_DIR && !demoplayback)
    {
	if(rclass < 3)
	    rclass++;
    }
    else if(rclass && !demoplayback)
	rclass--;

    if(demoplayback)
    {
	P_SetMessage(&players[consoleplayer], "CANNOT CHANGE STATE WHILE A DEMO IS RUNNING", true);
	S_StartSound(NULL, SFX_CHAT);
    }
}

static void SCRMap(int option)
{
    if(option == RIGHT_DIR && !demoplayback)
    {
	if	((  !datadisc && HEXEN_BETA && rmap < 41	)  ||
	   	 (  !datadisc && HEXEN_BETA_DEMO && rmap < 4)	   ||
	   	 (  !datadisc && HEXEN_DEMO && rmap < 4	)	   ||
	   	 (  !datadisc && HEXEN_MACDEMO && rmap < 4	)  ||
	   	 (  !datadisc && HEXEN_1_0 && rmap < 41	)	   ||
	   	 (  !datadisc && HEXEN_1_1 && rmap < 40	)	   ||
	   	 (  !datadisc && HEXEN_MACFULL && rmap < 40	)  ||
	   	 (   datadisc && rmap < 60)			)
	{
	    if(datadisc && rmap == 0) rmap = 40;
	    	rmap++;

	    if	   ((!datadisc && HEXEN_BETA && rmap == 7)	   ||
		    (!datadisc && HEXEN_1_0 && rmap == 7 )	   ||
		    (!datadisc && HEXEN_MACFULL && rmap == 7 )	   ||
		    (!datadisc && HEXEN_1_1 && rmap == 7))
		rmap = 8;
	    else if((!datadisc && HEXEN_BETA && rmap == 14)	   ||
		    (!datadisc && HEXEN_1_0 && rmap == 14 )	   ||
		    (!datadisc && HEXEN_MACFULL && rmap == 14 )	   ||
		    (!datadisc && HEXEN_1_1 && rmap == 14))
		rmap = 21;
	    else if((!datadisc && HEXEN_BETA && rmap == 29)	   ||
		    (!datadisc && HEXEN_1_0 && rmap == 29 )	   ||
		    (!datadisc && HEXEN_MACFULL && rmap == 29 )	   ||
		    (!datadisc && HEXEN_1_1 && rmap == 29))
		rmap = 30;
	}
    }
    else if((!datadisc && rmap >= 2) || (datadisc && rmap >= 42))
    {
	if(!demoplayback)
	{
	    rmap--;
	    if	   ((!datadisc && HEXEN_BETA && rmap == 7)		||
		    (!datadisc && HEXEN_1_0 && rmap == 7 )		||
		    (!datadisc && HEXEN_MACFULL && rmap == 7 )		||
		    (!datadisc && HEXEN_1_1 && rmap == 7))
		rmap = 6;
	    else if((!datadisc && HEXEN_BETA && rmap == 20)		||
		    (!datadisc && HEXEN_1_0 && rmap == 20 )		||
		    (!datadisc && HEXEN_MACFULL && rmap == 20 )		||
		    (!datadisc && HEXEN_1_1 && rmap == 20))
		rmap = 13;
	    else if((!datadisc && HEXEN_BETA && rmap == 29)		||
		    (!datadisc && HEXEN_1_0 && rmap == 29 )		||
		    (!datadisc && HEXEN_MACFULL && rmap == 29 )		||
		    (!datadisc && HEXEN_1_1 && rmap == 29))
		rmap = 28;
	}
    }
    if(demoplayback)
    {
	P_SetMessage(&players[consoleplayer], "CANNOT CHANGE STATE WHILE A DEMO IS RUNNING", true);
	S_StartSound(NULL, SFX_CHAT);
    }
}

static void SCRSkill(int option)
{
    if(option == RIGHT_DIR && !demoplayback)
    {
	if(rskill < 4)
	    rskill++;
    }
    else if(rskill && !demoplayback)
	rskill--;

    if(demoplayback)
    {
	P_SetMessage(&players[consoleplayer], "CANNOT CHANGE STATE WHILE A DEMO IS RUNNING", true);
	S_StartSound(NULL, SFX_CHAT);
    }
}

static void SCRecord(int option)
{
    if(!demoplayback)
    {
	SB_SetClassData();
	MN_DeactivateMenu();
	SB_state = -1;
	BorderNeedRefresh = true;
	G_StartNewInit();
	G_RecordDemo(rskill, rclass, 1, 1, rmap);
    }
    else if(demoplayback)
    {
	P_SetMessage(&players[consoleplayer], "CANNOT CHANGE STATE WHILE A DEMO IS RUNNING", true);
	S_StartSound(NULL, SFX_CHAT);
    }
}

static void SCMouselook(int option)
{
    if(option == RIGHT_DIR)
    {
	if(mouselook < 2)
	    mouselook++;
    }
    else if(mouselook)
	mouselook--;
}

static void SCTimer(int option)
{
    if(timer < 1)
    {
	timer++;
	P_SetMessage(&players[consoleplayer], "IN GAME TIMER INFO ON", true);
    }
    else if(timer)
    {
	timer--;
	P_SetMessage(&players[consoleplayer], "IN GAME TIMER INFO OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}

static void SCVersion(int option)
{
    if(version < 1)
    {
	version++;
	P_SetMessage(&players[consoleplayer], "VERSION INFO ON", true);
    }
    else if(version)
    {
	version--;
	P_SetMessage(&players[consoleplayer], "VERSION INFO OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}
/*
static void SCProcessor(int option)
{
    if(cpu_info < 1)
	cpu_info++;
    else if(cpu_info)
	cpu_info--;
}

static void SCBattery(int option)
{
    if(battery_info < 1)
	battery_info++;
    else if(battery_info)
	battery_info--;
}

static void SCOther(int option)
{
    if(other_info < 1)
	other_info++;
    else if(other_info)
	other_info--;
}

static void SCShowMemory(int option)
{
    if(memory_info < 1)
	memory_info++;
    else if(memory_info)
	memory_info--;
}
*/
static void SCSound(int option)
{
    if(!DebugSound)
    {
	DebugSound = true;
	P_SetMessage(&players[consoleplayer], "SOUND INFO ON", true);
    }
    else if(DebugSound)
    {
	DebugSound = false;
	P_SetMessage(&players[consoleplayer], "SOUND INFO OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}

static void SCCoords(int option)
{
    if(coords < 1)
    {
	coords++;
	P_SetMessage(&players[consoleplayer], "COORDINATES INFO ON", true);
    }
    else if(coords)
    {
	coords--;
	P_SetMessage(&players[consoleplayer], "COORDINATES INFO OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}

static void SCMapname(int option)
{
    if(mapname < 1)
    {
	mapname++;
	P_SetMessage(&players[consoleplayer], "IN GAME MAP NAME INFO ON", true);
    }
    else if(mapname)
    {
	mapname--;
	P_SetMessage(&players[consoleplayer], "IN GAME MAP NAME INFO OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}

static void DrawRecordMenu(void)
{
    if(rclass==0)
    {
	MN_DrTextB("FIGHTER", 200, 10);
	rclass = 1;
    }
    else if(rclass==1)
    {
	MN_DrTextB("FIGHTER", 200, 10);
	PlayerClass[consoleplayer] = PCLASS_FIGHTER;
    }
    else if(rclass==2)
    {
	MN_DrTextB("CLERIC", 200, 10);
	PlayerClass[consoleplayer] = PCLASS_CLERIC;
    }
    else if(rclass==3)
    {
	MN_DrTextB("MAGE", 200, 10);
	PlayerClass[consoleplayer] = PCLASS_MAGE;
    }

    if(rskill == 0)
	MN_DrTextB("BABY", 200, 110);
    else if(rskill == 1)
	MN_DrTextB("EASY", 200, 110);		
    else if(rskill == 2)
	MN_DrTextB("MEDIUM", 200, 110);
    else if(rskill == 3)
	MN_DrTextB("HARD", 200, 110);
    else if(rskill == 4)
	MN_DrTextB("NIGHTMARE", 200, 110);

    if(!datadisc)
    {
	if(rmap == 0)
	    rmap = 1;

	MN_DrTextB(enmaptext[rmap], 70, 70);

	if(rmap == 41)
	    MN_DrTextB("MAZE", 70, 70);
    }
    else if(datadisc)
    {
	if(rmap == 0)
	    rmap = 41;

	MN_DrTextB(maptextdd[rmap], 85, 70);
    }
}

static void SCGrid(int option)
{
    if(drawgrid < 1)
    {
	drawgrid++;
	P_SetMessage(&players[consoleplayer], "AUTOMAP GRID ON", true);
    }
    else if(drawgrid)
    {
	drawgrid--;
	P_SetMessage(&players[consoleplayer], "AUTOMAP GRID OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}

static void SCFollow(int option)
{
    followplayer ^= 1;

    if(followplayer)
	P_SetMessage(&players[consoleplayer], "FOLLOW MODE ON", true);
    else
	P_SetMessage(&players[consoleplayer], "FOLLOW MODE OFF", true);

    S_StartSound(NULL, SFX_CHAT);
}

static void SCRotate(int option)
{
    switch(option)
    {
    case 0:
        if (am_rotate)
            am_rotate = false;
	P_SetMessage(&players[consoleplayer], "AUTOMAP ROTATION OFF", true);
        break;
    case 1:
        if (am_rotate == false)
            am_rotate = true;
	P_SetMessage(&players[consoleplayer], "AUTOMAP ROTATION ON", true);
        break;
    }
}

static void SCSetKey(int option)
{
    askforkey = true;
    keyaskedfor = option;

    if (!demoplayback)
	paused = true;
}

//---------------------------------------------------------------------------
//
// PROC DrawOptions2Menu
//
//---------------------------------------------------------------------------
/*
static void DrawOptions2Menu(void)
{
    DrawSlider(&Options2Menu, 1, 9, screenblocks - 3);
    DrawSlider(&Options2Menu, 3, 16, snd_MaxVolume);
    DrawSlider(&Options2Menu, 5, 16, snd_MusicVolume);
}
*/
//---------------------------------------------------------------------------
//
// PROC SCQuitGame
//
//---------------------------------------------------------------------------

static void SCQuitGame(int option)
{
    MenuActive = false;
    askforquit = true;
    typeofask = 1;              //quit game
    if (/*!netgame &&*/ !demoplayback)
    {
        paused = true;
    }
}

//---------------------------------------------------------------------------
//
// PROC SCEndGame
//
//---------------------------------------------------------------------------

static void SCEndGame(int option)
{
    if (demoplayback)
    {
        return;
    }
    if (SCNetCheck(3))
    {
        MenuActive = false;
        askforquit = true;
        typeofask = 2;          //endgame
        if (/*!netgame &&*/ !demoplayback)
        {
            paused = true;
        }
    }
}

//---------------------------------------------------------------------------
//
// PROC SCMessages
//
//---------------------------------------------------------------------------

static void SCMessages(int option)
{
    messageson ^= 1;
    if (messageson)
    {
        P_SetMessage(&players[consoleplayer], "MESSAGES ON", true);
    }
    else
    {
        P_SetMessage(&players[consoleplayer], "MESSAGES OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}
/*
static void SCNinty(int option)
{
    if(ninty < 1)
    {
	ninty++;
	screenblocks = 11;
	R_SetViewSize(screenblocks, 0);
	P_SetMessage(&players[consoleplayer], "HEXEN 64 FUNCTIONS ENABLED", true);
    }
    else if(ninty)
    {
	ninty--;
	P_SetMessage(&players[consoleplayer], "HEXEN 64 FUNCTIONS DISABLED", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}
*/
static void SCWeaponChange(int option)
{
    if(use_vanilla_weapon_change < 1)
    {
	use_vanilla_weapon_change++;
	P_SetMessage(&players[consoleplayer], "ORIGINAL WEAPON CHANGING STYLE DISABLED", true);
    }
    else if(use_vanilla_weapon_change)
    {
	use_vanilla_weapon_change--;
	P_SetMessage(&players[consoleplayer], "ORIGINAL WEAPON CHANGING STYLE ENABLED", true);
    }
}

static void SCCrosshair(int option)
{
    if(crosshair < 1)
	crosshair++;
    else if(crosshair)
	crosshair--;
}
/*
static void SCCpu(int option)
{
    mhz333 ^= 1;

    if (mhz333)
    {
	P_SetMessage(&players[consoleplayer], "CLOCK NOW AT 333MHZ", true);
	scePowerSetClockFrequency(333, 333, 166);
    }
    else
    {
	P_SetMessage(&players[consoleplayer], "CLOCK NOW AT 222MHZ", true);
	scePowerSetClockFrequency(222, 222, 111);
    }
    S_StartSound(NULL, SFX_CHAT);
}
*/
void SCWalkingSpeed(int option)
{
    switch(option)
    {
      case 0:
	if(forwardmove > 19)
	    forwardmove--;
	break;
      case 1:
	if(forwardmove < 47)
	    forwardmove++;
	break;
    }
}

void SCTurningSpeed(int option)
{
    switch(option)
    {
      case 0:
	if(turnspeed > 5)
	    turnspeed--;
	break;
      case 1:
	if(turnspeed < 10)
	    turnspeed++;
	break;
    }
}

void SCStrafingSpeed(int option)
{
    switch(option)
    {
      case 0:
	if(sidemove > 16)
	    sidemove--;
	break;
      case 1:
	if (sidemove < 32)
	    sidemove++;
	break;
    }
}

static void SCTicker(int option)
{
    DisplayTicker = !DisplayTicker;

    if (DisplayTicker)
	P_SetMessage(&players[consoleplayer], TXT_CHEATTICKERON, true);
    else
	P_SetMessage(&players[consoleplayer], TXT_CHEATTICKEROFF, true);

    I_DisplayFPSDots(DisplayTicker);

    S_StartSound(NULL, SFX_CHAT);

    SB_state = -1;
    UpdateState |= I_FULLSCRN;
}

static void SCFPS(int option)
{
    if(FramesPerSecond < 1)
    {
	FramesPerSecond++;
	P_SetMessage(&players[consoleplayer], "FPS COUNTER ON", true);
    }
    else if(FramesPerSecond)
    {
	FramesPerSecond--;
	P_SetMessage(&players[consoleplayer], "FPS COUNTER OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}

static void SCWiiLight(int option)
{
    if(wiilight < 1)
    {
	wiilight++;
	P_SetMessage(&players[consoleplayer], "WII LIGHT ON", true);
    }
    else if(wiilight)
    {
	wiilight--;
	P_SetMessage(&players[consoleplayer], "WII LIGHT OFF", true);
    }
    S_StartSound(NULL, SFX_CHAT);
}

static void SCNoclip(int option)
{
    static player_t* player;
    player = &players[consoleplayer];

    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	if (player->cheats & CF_NOCLIP)
	{
	    player->cheats -= CF_NOCLIP;
	    P_SetMessage(&players[consoleplayer], "NOCLIP MODE OFF", true);
	}
	else
	{
	    player->cheats += CF_NOCLIP;
	    P_SetMessage(&players[consoleplayer], "NOCLIP MODE ON", true);
	}
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCAllArm(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArmor(player, ARMOR_AMULET, -1);
	P_GiveArmor(player, ARMOR_SHIELD, -1);
	P_GiveArmor(player, ARMOR_ARMOR, -1);
	P_GiveArmor(player, ARMOR_HELMET, -1);

	P_SetMessage(&players[consoleplayer], "ALL ARMOR ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCWarding(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArmor(player, ARMOR_AMULET, -1);

	P_SetMessage(&players[consoleplayer], "AMULET OF WARDING ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCShield(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArmor(player, ARMOR_SHIELD, -1);

	P_SetMessage(&players[consoleplayer], "FALCON SHIELD ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCArmor(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArmor(player, ARMOR_ARMOR, -1);

	P_SetMessage(&players[consoleplayer], "ARMOR ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCHelm(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArmor(player, ARMOR_HELMET, -1);

	P_SetMessage(&players[consoleplayer], "PLATINUM HELMET ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}


static void SCWeaponA(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->weaponowned[0]=1;
	player->weaponowned[1]=1;
	player->weaponowned[2]=1;
	player->weaponowned[3]=1;
	player->pieces = 7;
	player->mana[0] = 200;
	player->mana[1] = 200;

	P_SetMessage(&players[consoleplayer], "ALL WEAPONS + MANA ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCWeaponB(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->weaponowned[1]=1;

	if(PlayerClass[consoleplayer] == PCLASS_FIGHTER)
	    P_SetMessage(&players[consoleplayer], "TIMON'S AXE ADDED", true);
	else if(PlayerClass[consoleplayer] == PCLASS_CLERIC)
	    P_SetMessage(&players[consoleplayer], "SERPENT STAFF ADDED", true);
	else if(PlayerClass[consoleplayer] == PCLASS_MAGE)
	    P_SetMessage(&players[consoleplayer], "FROST SHARDS ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCWeaponC(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->weaponowned[2]=1;

	if(PlayerClass[consoleplayer] == PCLASS_FIGHTER)
	    P_SetMessage(&players[consoleplayer], "HAMMER OF RETRIBUTION ADDED", true);
	else if(PlayerClass[consoleplayer] == PCLASS_CLERIC)
	    P_SetMessage(&players[consoleplayer], "FIRESTORM ADDED", true);
	else if(PlayerClass[consoleplayer] == PCLASS_MAGE)
	    P_SetMessage(&players[consoleplayer], "ARC OF DEATH ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCWeaponD(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->weaponowned[3]=1;
	player->pieces = 7;

	if(PlayerClass[consoleplayer] == PCLASS_FIGHTER)
	    P_SetMessage(&players[consoleplayer], "QUIETUS ADDED", true);
	else if(PlayerClass[consoleplayer] == PCLASS_CLERIC)
	    P_SetMessage(&players[consoleplayer], "WRAITHVERGE ADDED", true);
	else if(PlayerClass[consoleplayer] == PCLASS_MAGE)
	    P_SetMessage(&players[consoleplayer], "BLOODSCOURGE ADDED", true);
    }	
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCManaA(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->mana[0] = 200;

	P_SetMessage(&players[consoleplayer], "BLUE MANA ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCManaB(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->mana[1] = 200;

	P_SetMessage(&players[consoleplayer], "GREEN MANA ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPieceA(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->pieces = 1;

	P_SetMessage(&players[consoleplayer], "WEAPON PIECE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPieceB(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->pieces = 2;

	P_SetMessage(&players[consoleplayer], "WEAPON PIECE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPieceC(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->pieces = 3;

	P_SetMessage(&players[consoleplayer], "WEAPON PIECE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPieceD(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->pieces = 4;

	P_SetMessage(&players[consoleplayer], "WEAPON PIECE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPieceE(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->pieces = 5;

	P_SetMessage(&players[consoleplayer], "WEAPON PIECE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPieceF(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->pieces = 6;

	P_SetMessage(&players[consoleplayer], "WEAPON PIECE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPieceG(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	player->pieces = 7;

	P_SetMessage(&players[consoleplayer], "WEAPON PIECE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCCave(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_2);

	P_SetMessage(&players[consoleplayer], "CAVE KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCHorn(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_9);

	P_SetMessage(&players[consoleplayer], "HORN KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCAxe(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_3);

	P_SetMessage(&players[consoleplayer], "AXE KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCSteel(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_1);

	P_SetMessage(&players[consoleplayer], "STEEL KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCKeys(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_1);
	P_GiveKey(player, KEY_2);
	P_GiveKey(player, KEY_3);
	P_GiveKey(player, KEY_4);
	P_GiveKey(player, KEY_5);
	P_GiveKey(player, KEY_6);
	P_GiveKey(player, KEY_7);
	P_GiveKey(player, KEY_8);
	P_GiveKey(player, KEY_9);
	P_GiveKey(player, KEY_A);
	P_GiveKey(player, KEY_B);

	P_SetMessage(&players[consoleplayer], "ALL KEYS ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCEmerald(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_5);

	P_SetMessage(&players[consoleplayer], "EMERALD KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCSilver(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_7);

	P_SetMessage(&players[consoleplayer], "SILVER KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCFire(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_4);

	P_SetMessage(&players[consoleplayer], "FIRE KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}


static void SCSwamp(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_A);

	P_SetMessage(&players[consoleplayer], "SWAMP KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCDungeon(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_6);

	P_SetMessage(&players[consoleplayer], "DUNGEON KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCCastle(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_B);

	P_SetMessage(&players[consoleplayer], "CASTLE KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCRusty(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveKey(player, KEY_8);

	P_SetMessage(&players[consoleplayer], "RUSTY KEY ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCArtifact(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_invulnerability, NULL);
	P_GiveArtifact(player, arti_health, NULL);
	P_GiveArtifact(player, arti_superhealth, NULL);
	P_GiveArtifact(player, arti_healingradius, NULL);
	P_GiveArtifact(player, arti_summon, NULL);
	P_GiveArtifact(player, arti_torch, NULL);
	P_GiveArtifact(player, arti_egg, NULL);
	P_GiveArtifact(player, arti_fly, NULL);
	P_GiveArtifact(player, arti_blastradius, NULL);
	P_GiveArtifact(player, arti_poisonbag, NULL);
	P_GiveArtifact(player, arti_teleportother, NULL);
	P_GiveArtifact(player, arti_speed, NULL);
	P_GiveArtifact(player, arti_boostmana, NULL);
	P_GiveArtifact(player, arti_teleport, NULL);

	P_SetMessage(&players[consoleplayer], "ALL ARTIFACTS ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCDefender(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_invulnerability, NULL);

	P_SetMessage(&players[consoleplayer], "ICON OF THE DEFENDER", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCQuartz(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_health, NULL);

	P_SetMessage(&players[consoleplayer], "QUARTZ FLASK ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCUrn(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_superhealth, NULL);

	P_SetMessage(&players[consoleplayer], "MYSTIC URN ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCIncant(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_healingradius, NULL);

	P_SetMessage(&players[consoleplayer], "MYSTIC AMBIT INCANT ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCServant(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_summon, NULL);

	P_SetMessage(&players[consoleplayer], "DARK SERVANT ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCTorch(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_torch, NULL);

	P_SetMessage(&players[consoleplayer], "TORCH ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPorkalator(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_egg, NULL);

	P_SetMessage(&players[consoleplayer], "PORKALATOR ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCWings(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_fly, NULL);

	P_SetMessage(&players[consoleplayer], "WINGS OF WRATH ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCRepulsion(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_blastradius, NULL);

	P_SetMessage(&players[consoleplayer], "DISC OF REPULSION ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCFlechette(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_poisonbag, NULL);

	P_SetMessage(&players[consoleplayer], "FLECHETTE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCBanDev(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_teleportother, NULL);

	P_SetMessage(&players[consoleplayer], "BANISHMENT DEVICE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCBoots(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_speed, NULL);

	P_SetMessage(&players[consoleplayer], "BOOTS OF SPEED ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCVial(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_boostmana, NULL);

	P_SetMessage(&players[consoleplayer], "CRYSTAL VIAL ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCBracers(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_boostarmor, NULL);

	P_SetMessage(&players[consoleplayer], "DRAGONSKIN BRACERS ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCChaosDev(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_teleport, NULL);

	P_SetMessage(&players[consoleplayer], "CHAOS DEVICE ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzle(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_firstpuzzitem, NULL);
	P_GiveArtifact(player, arti_puzzgembig, NULL);
	P_GiveArtifact(player, arti_puzzgemred, NULL);
	P_GiveArtifact(player, arti_puzzgemgreen1, NULL);
	P_GiveArtifact(player, arti_puzzgemgreen2, NULL);
	P_GiveArtifact(player, arti_puzzgemblue1, NULL);
	P_GiveArtifact(player, arti_puzzgemblue2, NULL);
	P_GiveArtifact(player, arti_puzzbook1, NULL);
	P_GiveArtifact(player, arti_puzzbook2, NULL);
	P_GiveArtifact(player, arti_puzzskull2, NULL);
	P_GiveArtifact(player, arti_puzzfweapon, NULL);
	P_GiveArtifact(player, arti_puzzcweapon, NULL);
	P_GiveArtifact(player, arti_puzzmweapon, NULL);
	P_GiveArtifact(player, arti_puzzgear1, NULL);

	if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
	{
	    P_GiveArtifact(player, arti_puzzgear2, NULL);	// HACK: These three artifacts DO NOT
								// work for the "Retail Store Beta" and
								// the "4 Level Beta Release" 'cuz...
	    P_GiveArtifact(player, arti_puzzgear3, NULL);	// ...their IWAD's DO NOT contain those
								// artifacts. The game would even crash
								// upon first giving and then trying to
	    P_GiveArtifact(player, arti_puzzgear4, NULL);	// select them from within the
								// inventory. So we make an exception
								// to the IWAD's that definitely
								// contain these artifacts.
	}
	P_SetMessage(&players[consoleplayer], "ALL PUZZLE ITEMS ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCFirstpuzzitem(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_firstpuzzitem, NULL);

	P_SetMessage(&players[consoleplayer], "YORICK'S SKULL ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgembig(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgembig, NULL);

	P_SetMessage(&players[consoleplayer], "HEART OF D'SPARIL ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgemred(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgemred, NULL);

	P_SetMessage(&players[consoleplayer], "RUBY PLANET ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgemgreen1(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgemgreen1, NULL);

	P_SetMessage(&players[consoleplayer], "EMERALD PLANET #1 ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgemgreen2(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgemgreen2, NULL);

	P_SetMessage(&players[consoleplayer], "EMERALD PLANET #2 ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgemblue1(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgemblue1, NULL);

	P_SetMessage(&players[consoleplayer], "SAPPHIRE PLANET #1 ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgemblue2(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgemblue2, NULL);

	P_SetMessage(&players[consoleplayer], "SAPPHIRE PLANET #2 ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzbook1(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzbook1, NULL);

	P_SetMessage(&players[consoleplayer], "DAEMON CODEX ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzbook2(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzbook2, NULL);

	P_SetMessage(&players[consoleplayer], "LIBER OSCURA ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzskull2(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzskull2, NULL);

	P_SetMessage(&players[consoleplayer], "FLAME MASK ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzfweapon(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzfweapon, NULL);

	P_SetMessage(&players[consoleplayer], "GLAIVE SEAL ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzcweapon(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzcweapon, NULL);

	P_SetMessage(&players[consoleplayer], "HOLY RELIC ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzmweapon(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzmweapon, NULL);

	P_SetMessage(&players[consoleplayer], "SIGIL OF THE MAGUS ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgear1(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgear1, NULL);

	if(HEXEN_MACDEMO || HEXEN_DEMO || HEXEN_1_0 || HEXEN_1_1 || HEXEN_MACFULL)
	    P_SetMessage(&players[consoleplayer], "CLOCK GEAR #1 ADDED", true);
	else if(HEXEN_BETA || HEXEN_BETA_DEMO)
	    P_SetMessage(&players[consoleplayer], "CLOCK GEAR ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgear2(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgear2, NULL);

	P_SetMessage(&players[consoleplayer], "CLOCK GEAR #2 ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgear3(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgear3, NULL);

	P_SetMessage(&players[consoleplayer], "CLOCK GEAR #3 ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCPuzzgear4(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	static player_t* player;
	player = &players[consoleplayer];

	P_GiveArtifact(player, arti_puzzgear4, NULL);

	P_SetMessage(&players[consoleplayer], "CLOCK GEAR #4 ADDED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCGod(int option)
{
    static player_t* player;
    player = &players[consoleplayer];

    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	if (player->cheats & CF_GODMODE)
	{
	    player->cheats -= CF_GODMODE;
	    P_SetMessage(&players[consoleplayer], "GOD MODE OFF", true);
	}
	else
	{
	    player->cheats += CF_GODMODE;
	    P_SetMessage(&players[consoleplayer], "GOD MODE ON", true);
	}
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCMap(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	cheating = (cheating+1) % 3;
	cheeting = (cheeting+1) % 3;

	if(!cheating)
	    P_SetMessage(&players[consoleplayer], "MAP BACK TO NORMAL", true);
	else if (cheating && cheeting!=2)
	    P_SetMessage(&players[consoleplayer], "ALL WALLS REVEALED", true);
	else if (cheating && cheeting==2)
	    P_SetMessage(&players[consoleplayer], "ALL WALLS / ITEMS REVEALED", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCKill(int option)
{
    if(!demoplayback && gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE)
    {
	int count;

	char buffer[80];

	static player_t* player;
	player = &players[consoleplayer];

	count = P_Massacre();

	if(HEXEN_1_0 || HEXEN_1_1 || HEXEN_DEMO || HEXEN_MACDEMO || HEXEN_MACFULL)
	{
	    sprintf(buffer, "%d MONSTERS KILLED\n", count);
	    P_SetMessage(player, buffer, true);
	}
	else if(HEXEN_BETA || HEXEN_BETA_DEMO)
	    P_SetMessage(&players[consoleplayer], "CARNAGE", true);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

static void SCWarp(int option)
{
    if(option == RIGHT_DIR)
    {
	if    ((	!datadisc && HEXEN_BETA && map < 41	)	||
	       (	!datadisc && HEXEN_BETA_DEMO && map < 4	)	||
	       (	!datadisc && HEXEN_DEMO && map < 4	)	||
	       (	!datadisc && HEXEN_MACDEMO && map < 4	)	||
	       (	!datadisc && HEXEN_1_0 && map < 41	)	||
	       (	!datadisc && HEXEN_1_1 && map < 40	)	||
	       (	!datadisc && HEXEN_MACFULL && map < 40	)	||
	       (	 datadisc && map < 60		        ))
	{
	    if(datadisc && map == 0) map = 40;
		map++;
	    if((	!datadisc && HEXEN_BETA && map == 7	)	||
	       (	!datadisc && HEXEN_1_0 && map == 7 	)	||
	       (	!datadisc && HEXEN_MACFULL && map == 7	)	||
	       (	!datadisc && HEXEN_1_1 && map == 7      ))
		map = 8;
	    else if((	!datadisc && HEXEN_BETA && map == 14	)	||
	            (	!datadisc && HEXEN_1_0 && map == 14	)	||
	            (	!datadisc && HEXEN_MACFULL && map == 14 )	||
	            (	!datadisc && HEXEN_1_1 && map == 14     ))
		map = 21;
	    else if((	!datadisc && HEXEN_BETA && map == 29	)	||
	            (	!datadisc && HEXEN_1_0 && map == 29	)	||
	            (	!datadisc && HEXEN_MACFULL && map == 29 )	||
	            (	!datadisc && HEXEN_1_1 && map == 29     ))
		map = 30;
	}
    }
    else if((!datadisc && map >= 2) || (datadisc && map >= 42))
    {
	map--;
	if     ((!datadisc && HEXEN_BETA && map == 7	)		||
	        (!datadisc && HEXEN_1_0 && map == 7	)		||
	        (!datadisc && HEXEN_MACFULL && map == 7 )		||
	        (!datadisc && HEXEN_1_1 && map == 7     ))
	    map = 6;
	else if((!datadisc && HEXEN_BETA && map == 20	)		||
		(!datadisc && HEXEN_1_0 && map == 20	)		||
		(!datadisc && HEXEN_MACFULL && map == 20)		||
		(!datadisc && HEXEN_1_1 && map == 20    ))
	    map = 13;
	else if((!datadisc && HEXEN_BETA && map == 29	)		||
		(!datadisc && HEXEN_1_0 && map == 29	)		||
		(!datadisc && HEXEN_MACFULL && map == 29)		||
		(!datadisc && HEXEN_1_1 && map == 29    ))
	    map = 28;
    }
}

static void SCWarpNow(int option)
{
    if(gamestate == GS_LEVEL && gameskill != sk_nightmare && players[consoleplayer].playerstate == PST_LIVE && !demoplayback)
    {
	warped = 1;
	G_TeleportNewMap(map, 0);
    }
    DetectState();
    S_StartSound(NULL, SFX_CHAT);
}

//===========================================================================
//
// SCNetCheck
//
//===========================================================================

static boolean SCNetCheck(int option)
{
//    if (!netgame)
    {
        return true;
    }
    switch (option)
    {
        case 1:                // new game
            P_SetMessage(&players[consoleplayer],
                         "YOU CAN'T START A NEW GAME IN NETPLAY!", true);
            break;
        case 2:                // load game
            P_SetMessage(&players[consoleplayer],
                         "YOU CAN'T LOAD A GAME IN NETPLAY!", true);
            break;
        case 3:                // end game
            P_SetMessage(&players[consoleplayer],
                         "YOU CAN'T END A GAME IN NETPLAY!", true);
            break;
    }
    MenuActive = false;
    S_StartSound(NULL, SFX_CHAT);
    return false;
}

//===========================================================================
//
// SCNetCheck2
//
//===========================================================================

static void SCNetCheck2(int option)
{
    SCNetCheck(option);
    return;
}

//---------------------------------------------------------------------------
//
// PROC SCLoadGame
//
//---------------------------------------------------------------------------

static void SCLoadGame(int option)
{
    if (!SlotStatus[option])
    {                           // Don't try to load from an empty slot
        return;
    }
    G_LoadGame(option);
    MN_DeactivateMenu();
    BorderNeedRefresh = true;
/*
    if (quickload == -1)
    {
        quickload = option + 1;
        P_ClearMessage(&players[consoleplayer]);
    }
*/
}

//---------------------------------------------------------------------------
//
// PROC SCSaveGame
//
//---------------------------------------------------------------------------

static void SCSaveGame(int option)
{
    char *ptr;

    if (!FileMenuKeySteal)
    {
        FileMenuKeySteal = true;
        strcpy(oldSlotText, SlotText[option]);
        ptr = SlotText[option];
        while (*ptr)
        {
            ptr++;
        }
        *ptr = '[';
        *(ptr + 1) = 0;
        SlotStatus[option]++;
        currentSlot = option;
        slotptr = ptr - SlotText[option];
        return;
    }
    else
    {
        G_SaveGame(option, SlotText[option]);
        FileMenuKeySteal = false;
        MN_DeactivateMenu();
    }
    BorderNeedRefresh = true;
/*
    if (quicksave == -1)
    {
        quicksave = option + 1;
        P_ClearMessage(&players[consoleplayer]);
    }
*/
}

//==========================================================================
//
// SCClass
//
//==========================================================================

static void SCClass(int option)
{
/*
    if (netgame)
    {
        P_SetMessage(&players[consoleplayer],
                     "YOU CAN'T START A NEW GAME FROM WITHIN A NETGAME!",
                     true);
        return;
    }
*/
    MenuPClass = option;
    switch (MenuPClass)
    {
        case PCLASS_FIGHTER:
            SkillMenu.x = 120;
            SkillItems[0].text = "SQUIRE";
            SkillItems[1].text = "KNIGHT";
            SkillItems[2].text = "WARRIOR";
            SkillItems[3].text = "BERSERKER";
            SkillItems[4].text = "TITAN";
            break;
        case PCLASS_CLERIC:
            SkillMenu.x = 116;
            SkillItems[0].text = "ALTAR BOY";
            SkillItems[1].text = "ACOLYTE";
            SkillItems[2].text = "PRIEST";
            SkillItems[3].text = "CARDINAL";
            SkillItems[4].text = "POPE";
            break;
        case PCLASS_MAGE:
            SkillMenu.x = 112;
            SkillItems[0].text = "APPRENTICE";
            SkillItems[1].text = "ENCHANTER";
            SkillItems[2].text = "SORCERER";
            SkillItems[3].text = "WARLOCK";
            SkillItems[4].text = "ARCHIMAGE";
            break;
    }
    SetMenu(MENU_SKILL);
}

//---------------------------------------------------------------------------
//
// PROC SCSkill
//
//---------------------------------------------------------------------------

static void SCSkill(int option)
{
    PlayerClass[consoleplayer] = MenuPClass;
    G_DeferredNewGame(option);
    SB_SetClassData();
    SB_state = -1;
    MN_DeactivateMenu();
}

//---------------------------------------------------------------------------
//
// PROC SCMouseSensi
//
//---------------------------------------------------------------------------
/*
static void SCMouseSensi(int option)
{
    if (option == RIGHT_DIR)
    {
        if (mouseSensitivity < 9)
        {
            mouseSensitivity++;
        }
    }
    else if (mouseSensitivity)
    {
        mouseSensitivity--;
    }
}
*/
static void SCMouseSpeed(int option)
{
    if(option == RIGHT_DIR)
    {
	if(mspeed < 8)
	    mspeed++;
    }
    else if(mspeed)
	mspeed--;
}

//---------------------------------------------------------------------------
//
// PROC SCSfxVolume
//
//---------------------------------------------------------------------------

static void SCSfxVolume(int option)
{
    if (option == RIGHT_DIR)
    {
        if (snd_MaxVolume < 15)
        {
            snd_MaxVolume++;
        }
    }
    else if (snd_MaxVolume)
    {
        snd_MaxVolume--;
    }
    soundchanged = true;        // we'll set it when we leave the menu
}

//---------------------------------------------------------------------------
//
// PROC SCMusicVolume
//
//---------------------------------------------------------------------------

static void SCMusicVolume(int option)
{
    if (option == RIGHT_DIR)
    {
        if (snd_MusicVolume < 15)
        {
            snd_MusicVolume++;
        }
    }
    else if (snd_MusicVolume)
    {
        snd_MusicVolume--;
    }
    S_SetMusicVolume();
}

static void SCNTrack(int option)
{
    if(option == RIGHT_DIR && !demoplayback)
    {
//	selected = 1;
	if	((	!datadisc && HEXEN_BETA && tracknum < 40    )	||
	  	 (	!datadisc && HEXEN_BETA_DEMO && tracknum < 4)	||
	  	 (	!datadisc && HEXEN_DEMO && tracknum < 4	    )	||
	  	 (	!datadisc && HEXEN_MACDEMO && tracknum < 4  )	||
	  	 (	!datadisc && HEXEN_1_0 && tracknum < 40	    )	||
	  	 (	!datadisc && HEXEN_1_1 && tracknum < 40	    )	||
	  	 (	!datadisc && HEXEN_MACFULL && tracknum < 40 )	||
	  	 (	datadisc && tracknum < 40)		    )
	{
	    tracknum++;
	    if(datadisc)
	    {
		if(tracknum == 6)
		    tracknum = 9;
		else if(tracknum == 11)
		    tracknum = 13;
		else if(tracknum == 14)
		    tracknum = 21;
		else if(tracknum == 23)
		    tracknum = 27;
		else if(tracknum == 31)
		    tracknum = 33;
		else if(tracknum == 34)
		    tracknum = 35;
		else if(tracknum == 39)
		    tracknum = 40;
	    }
	    else if(!datadisc)
	    {
		if(tracknum == 7)
		    tracknum = 8;
		else if(tracknum == 14)
		    tracknum = 21;
		else if(tracknum == 24)
		    tracknum = 25;
		else if(tracknum == 29)
		    tracknum = 30;
	    }
	}
    }
    else if(tracknum > 1)
    {
	tracknum--;
	if (datadisc)
	{
	    if(tracknum == 39)
		tracknum = 38;
	    else if(tracknum == 34)
		tracknum = 33;
	    else if(tracknum == 32)
		tracknum = 30;
	    else if(tracknum == 26)
		tracknum = 22;
	    else if(tracknum == 20)
		tracknum = 13;
	    else if(tracknum == 12)
		tracknum = 10;
	    else if(tracknum == 8)
		tracknum = 5;
	}
	else if(!datadisc)
	{
	    if(tracknum == 29)
		tracknum = 28;
	    else if(tracknum == 24)
		tracknum = 23;
	    else if(tracknum == 20)
		tracknum = 13;
	    else if(tracknum == 7)
		tracknum = 6;
	}
    }
    if(demoplayback)
    {
	P_SetMessage(&players[consoleplayer], "CANNOT CHANGE STATE WHILE NOT IN A GAME", true);
	goto skipchange;
    }
/*
    if(datadisc)
	S_StartSong(tracknum + 40, true);
    else if(!datadisc)
*/
    from_menu = true;

    S_StartSong(tracknum, true);
    skipchange:
    {
    }
}
/*
static void SCNTrackLoop(int option)
{
    switch(option)
    {
    case 0:
        if (loop)
            loop = false;
        break;
    case 1:
        if (loop == false)
            loop = true;
        break;
    }
}
*/
//---------------------------------------------------------------------------
//
// PROC SCScreenSize
//
//---------------------------------------------------------------------------

static void SCScreenSize(int option)
{
    if (option == RIGHT_DIR)
    {
        if (screenblocks < 11)
        {
            screenblocks++;
        }
    }
    else if (screenblocks > 3)
    {
        screenblocks--;
    }
    R_SetViewSize(screenblocks, detailLevel);
}

static void SCBrightness(int option)
{
    if(option == RIGHT_DIR)
    {
	if(usegamma < 4)
	    usegamma++;
    }
    else if(usegamma)
	usegamma--;

    SB_PaletteFlash(true);
    P_SetMessage(&players[consoleplayer], GammaText[usegamma], false);
}

static void SCDetails(int option)
{
    option = 0;
    detailLevel = 1 - detailLevel;

    R_SetViewSize (screenblocks, detailLevel);

    if (!detailLevel)
	P_SetMessage(&players[consoleplayer],
		"HIGH DETAIL", true);
    else
	P_SetMessage(&players[consoleplayer],
		"LOW DETAIL", true);
}

//---------------------------------------------------------------------------
//
// PROC SCInfo
//
//---------------------------------------------------------------------------

static void SCInfo(int option)
{
    InfoType = 1;
    S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
    if (/*!netgame &&*/ !demoplayback)
    {
        paused = true;
    }
}

//---------------------------------------------------------------------------
//
// FUNC MN_Responder
//
//---------------------------------------------------------------------------

boolean MN_Responder(event_t * event)
{
    int /*key,*/ ch;
//    int charTyped;
//    int i;
    MenuItem_t *item;
//    extern boolean automapactive;
    extern void H2_StartTitle(void);
    extern void G_CheckDemoStatus(void);
//    char *textBuffer;

    ch = -1; // will be changed to a legit char if we're going to use it here

    // Process joystick input
    // For some reason, polling ev.data for joystick input here in the menu code doesn't work when
    // using the twilight hack to launch wiidoom. At the same time, it works fine if you're using the
    // homebrew channel. I don't know why this is so for the meantime I'm polling the wii remote directly.

    WPADData *data = WPAD_Data(0);
    
    if ((data->btns_d & WPAD_BUTTON_UP) /*&& (joywait < I_GetTime())*/)
    {
        ch = key_menu_up;                                // phares 3/7/98

//        joywait = I_GetTime() + 5;    
    }

    if ((data->btns_d & WPAD_BUTTON_DOWN) /*&& (joywait < I_GetTime())*/)
    {
        ch = key_menu_down;                              // phares 3/7/98

//        joywait = I_GetTime() + 5;    
    }

    if ((data->btns_d & WPAD_BUTTON_LEFT) /*&& (joywait < I_GetTime())*/)
    {
        ch = key_menu_left;                              // phares 3/7/98

//        joywait = I_GetTime() + 10;
    }

    if ((data->btns_d & WPAD_BUTTON_RIGHT) /*&& (joywait < I_GetTime())*/)
    {
        ch = key_menu_right;                             // phares 3/7/98

//        joywait = I_GetTime() + 10;
    }

    if ((data->btns_d & WPAD_BUTTON_A) /*&& (joywait < I_GetTime())*/)
    {
        ch = key_menu_forward;                             // phares 3/7/98

//        joywait = I_GetTime() + 10;
    }

    if ((data->btns_d & WPAD_BUTTON_B) /*&& (joywait < I_GetTime())*/)
    {
        ch = key_menu_back;                         // phares 3/7/98

//        joywait = I_GetTime() + 10;
    }

    if ((data->btns_d & WPAD_BUTTON_HOME) /*&& (joywait < I_GetTime())*/)
    {
        ch = key_menu_activate;                         // phares 3/7/98

//        joywait = I_GetTime() + 10;
    }

    //Classic Controls
    if(data->exp.type == WPAD_EXP_CLASSIC)
    {
	if ((data->btns_d & WPAD_CLASSIC_BUTTON_UP) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_up;                                // phares 3/7/98

//	    joywait = I_GetTime() + 5;
	}

	if ((data->btns_d & WPAD_CLASSIC_BUTTON_DOWN) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_down;                              // phares 3/7/98

//	    joywait = I_GetTime() + 5;
	}

	if ((data->btns_d & WPAD_CLASSIC_BUTTON_LEFT) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_left;                              // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}

	if ((data->btns_d & WPAD_CLASSIC_BUTTON_RIGHT) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_right;                             // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}

	if ((data->btns_d & WPAD_CLASSIC_BUTTON_B) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_forward;                             // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}

	if ((data->btns_d & WPAD_CLASSIC_BUTTON_A) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_back;                         // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}
/*
	if (data->btns_d & WPAD_CLASSIC_BUTTON_MINUS)
	{
	    ch = key_menu_activate;                         // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}
*/
	if ((data->exp.classic.ljs.pos.y > (data->exp.classic.ljs.center.y + 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_up;

//	    joywait = I_GetTime() + 5;
	}
	else if ((data->exp.classic.ljs.pos.y < (data->exp.classic.ljs.center.y - 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_down;

//	    joywait = I_GetTime() + 5;
	}

	if ((data->exp.classic.ljs.pos.x > (data->exp.classic.ljs.center.x + 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_right;

//	    joywait = I_GetTime() + 5;
	}
	else if ((data->exp.classic.ljs.pos.x < (data->exp.classic.ljs.center.x - 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_left;

//	    joywait = I_GetTime() + 5;
	}
    }

    //Nunchuk Controls
    if(data->exp.type == WPAD_EXP_NUNCHUK)
    {
	if ((data->exp.nunchuk.js.pos.y > (data->exp.nunchuk.js.center.y + 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_up;

//	    joywait = I_GetTime() + 5;
	}
	else if ((data->exp.nunchuk.js.pos.y < (data->exp.nunchuk.js.center.y - 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_down;

//	    joywait = I_GetTime() + 5;
	}

	if ((data->exp.nunchuk.js.pos.x > (data->exp.nunchuk.js.center.x + 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_right;

//	    joywait = I_GetTime() + 5;
	}
	else if ((data->exp.nunchuk.js.pos.x < (data->exp.nunchuk.js.center.x - 50)) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_left;

//	    joywait = I_GetTime() + 5;
	}
    }
	
    //GC Controls
    if(data->exp.type!=WPAD_EXP_NUNCHUK && data->exp.type!=WPAD_EXP_CLASSIC)
    {
	s32 pad_stickx = PAD_StickX(0);
	s32 pad_sticky = PAD_StickY(0);
		
	if ((PAD_ButtonsHeld(0) & PAD_BUTTON_UP) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_up;                                // phares 3/7/98

//	    joywait = I_GetTime() + 5;
	}

	if ((PAD_ButtonsHeld(0) & PAD_BUTTON_DOWN) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_down;                              // phares 3/7/98

//	    joywait = I_GetTime() + 5;
	}

	if ((PAD_ButtonsHeld(0) & PAD_BUTTON_LEFT) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_left;                              // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}

	if ((PAD_ButtonsHeld(0) & PAD_BUTTON_RIGHT) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_right;                             // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}

	if ((PAD_ButtonsHeld(0) & PAD_BUTTON_A) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_forward;                             // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}

	if ((PAD_ButtonsHeld(0) & PAD_BUTTON_B) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_back;                         // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}

	if ((PAD_ButtonsHeld(0) & PAD_BUTTON_START) /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_activate;                         // phares 3/7/98

//	    joywait = I_GetTime() + 10;
	}
	  
	if(pad_sticky > 20  /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_up;

//	    joywait = I_GetTime() + 5;
	}
	else if(pad_sticky < -20 /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_down;

//	    joywait = I_GetTime() + 5;
	}

	if(pad_stickx > 20 /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_right;

//	    joywait = I_GetTime() + 5;
	}

	if(pad_stickx < -20 /*&& (joywait < I_GetTime())*/)
	{
	    ch = key_menu_left;

//	    joywait = I_GetTime() + 5;
	}
    }

    // In testcontrols mode, none of the function keys should do anything
    // - the only key is escape to quit.
/*
    if (testcontrols)
    {
        if (event->type == ev_quit
         || (event->type == ev_keydown
          && (event->data1 == key_menu_activate
           || event->data1 == key_menu_quit)))
        {
            I_Quit();
            return true;
        }

        return false;
    }
*/
    if (askforkey && data->btns_d)		// KEY BINDINGS
    {
	ClearControls(event->data1);
	*doom_defaults_list[keyaskedfor + 16 + FirstKey].location = event->data1;
	askforkey = false;
	return true;
    }

    if (askforkey && event->type == ev_mouse)
    {
	if (event->data1 & 1)
	    return true;
	if (event->data1 & 2)
	    return true;
	if (event->data1 & 4)
	    return true;
	return false;
    }
/*
    // "close" button pressed on window?
    if (event->type == ev_quit)
    {
        // First click on close = bring up quit confirm message.
        // Second click = confirm quit.

        if (!MenuActive && askforquit && typeofask == 1)
        {
            G_CheckDemoStatus();
            I_Quit();
        }
        else
        {
            SCQuitGame(0);
            S_StartSound(NULL, SFX_CHAT);
        }

        return true;
    }

    // Only care about keypresses beyond this point.

    if (event->type != ev_keydown)
    {
        return false;
    }

    key = event->data1;
    charTyped = event->data2;
*/
    if (InfoType)
    {
	if(ch == key_menu_forward)		// FIX FOR NINTENDO WII
	{
	    if (gamemode == shareware)
	    {
		InfoType = (InfoType + 1) % 5;
	    }
	    else
	    {
		InfoType = (InfoType + 1) % 4;
	    }
	    if (ch == KEY_ESCAPE)
	    {
		InfoType = 0;
	    }
	    if (!InfoType)
	    {
            	if (/*!netgame &&*/ !demoplayback)
            	{
            	    paused = false;
            	}
            	MN_DeactivateMenu();
            	SB_state = -1;      //refresh the statbar
            	BorderNeedRefresh = true;
	    }
    	    S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
        }
        return (true);          //make the info screen eat the keypress
    }
/*
    if (ravpic && key == KEY_F1)
    {
        G_ScreenShot();
        return (true);
    }
*/
    if (askforquit)
    {
        if (ch == key_menu_forward)		// FIX FOR NINTENDO WII
        {
            switch (typeofask)
            {
                case 1:
                    G_CheckDemoStatus();
                    I_Quit();
                    return false;
                case 2:
                    P_ClearMessage(&players[consoleplayer]);
                    askforquit = false;
                    typeofask = 0;
                    paused = false;
                    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));
                    H2_StartTitle();    // go to intro/demo mode.
                    return false;
/*
                case 3:
                    P_SetMessage(&players[consoleplayer],
                                 "QUICKSAVING....", false);
                    FileMenuKeySteal = true;
                    SCSaveGame(quicksave - 1);
                    BorderNeedRefresh = true;
                    break;
                case 4:
                    P_SetMessage(&players[consoleplayer],
                                 "QUICKLOADING....", false);
                    SCLoadGame(quickload - 1);
                    BorderNeedRefresh = true;
                    break;
                case 5:
                    BorderNeedRefresh = true;
                    mn_SuicideConsole = true;
                    break;
*/
                default:
                    break;
            }

            askforquit = false;
            typeofask = 0;

            return true;
        }
        else if (ch == key_menu_abort || ch == KEY_ESCAPE)
        {
            players[consoleplayer].messageTics = 0;
            askforquit = false;
            typeofask = 0;
            paused = false;
            UpdateState |= I_FULLSCRN;
            BorderNeedRefresh = true;
            return true;
        }

        return false;           // don't let the keys filter thru
    }
    if(askforsave)
    {
        if (ch == key_menu_forward)		// FIX FOR NINTENDO WII
        {
            switch (typeofask2)
            {
                case 1:
                    P_ClearMessage(&players[consoleplayer]);
                    askforsave = false;
                    typeofask2 = 0;
                    paused = false;
                    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));
//                    H2_StartTitle();    // go to intro/demo mode.
                    return false;

                default:
                    break;
	    }
            askforsave = false;
            typeofask2 = 0;

            return true;
	}
        else if (ch == key_menu_abort || ch == KEY_ESCAPE)
        {
            players[consoleplayer].messageTics = 0;
            askforsave = false;
            typeofask2 = 0;
            paused = false;
            UpdateState |= I_FULLSCRN;
            BorderNeedRefresh = true;
            return true;
        }

        return false;           // don't let the keys filter thru
    }
    if (!MenuActive /*&& !chatmodeon*/)
    {
/*
        if (key == key_menu_decscreen)
        {
            if (automapactive)
            {               // Don't screen size in automap
                return (false);
            }
            SCScreenSize(LEFT_DIR);
            S_StartSound(NULL, SFX_PICKUP_KEY);
            BorderNeedRefresh = true;
            UpdateState |= I_FULLSCRN;
            return (true);
        }
        else if (key == key_menu_incscreen)
        {
            if (automapactive)
            {               // Don't screen size in automap
                return (false);
            }
            SCScreenSize(RIGHT_DIR);
            S_StartSound(NULL, SFX_PICKUP_KEY);
            BorderNeedRefresh = true;
            UpdateState |= I_FULLSCRN;
            return (true);
        }
        else if (key == key_menu_help)           // F1 (help screen)
        {
            SCInfo(0);      // start up info screens
            MenuActive = true;
            return (true);
        }
        else if (key == key_menu_save)           // F2 (save game)
        {
            if (gamestate == GS_LEVEL && !demoplayback)
            {
                MenuActive = true;
                FileMenuKeySteal = false;
                MenuTime = 0;
                CurrentMenu = &SaveMenu;
                CurrentItPos = CurrentMenu->oldItPos;
                if (!netgame && !demoplayback)
                {
                    paused = true;
                }
                S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
                slottextloaded = false;     //reload the slot text, when needed
            }
            return true;
        }
        else if (key == key_menu_load)           // F3 (load game)
        {
            if (SCNetCheck(2))
            {
                MenuActive = true;
                FileMenuKeySteal = false;
                MenuTime = 0;
                CurrentMenu = &LoadMenu;
                CurrentItPos = CurrentMenu->oldItPos;
                if (!netgame && !demoplayback)
                {
                    paused = true;
                }
                S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
                slottextloaded = false;     //reload the slot text, when needed
            }
            return true;
        }
        else if (key == key_menu_volume)         // F4 (volume)
        {
            MenuActive = true;
            FileMenuKeySteal = false;
            MenuTime = 0;
            CurrentMenu = &Options2Menu;
            CurrentItPos = CurrentMenu->oldItPos;
            if (!netgame && !demoplayback)
            {
                paused = true;
            }
            S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
            slottextloaded = false; //reload the slot text, when needed
            return true;
        }
        else if (key == key_menu_detail)         // F5 (suicide)
        {
            MenuActive = false;
            askforquit = true;
            typeofask = 5;  // suicide
            return true;
        }
        else if (key == key_menu_qsave)          // F6 (quicksave)
        {
            if (gamestate == GS_LEVEL && !demoplayback)
            {
                if (!quicksave || quicksave == -1)
                {
                    MenuActive = true;
                    FileMenuKeySteal = false;
                    MenuTime = 0;
                    CurrentMenu = &SaveMenu;
                    CurrentItPos = CurrentMenu->oldItPos;
                    if (!netgame && !demoplayback)
                    {
                        paused = true;
                    }
                    S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
                    slottextloaded = false; //reload the slot text
                    quicksave = -1;
                    P_SetMessage(&players[consoleplayer],
                                 "CHOOSE A QUICKSAVE SLOT", true);
                }
                else
                {
                    askforquit = true;
                    typeofask = 3;
                    if (!netgame && !demoplayback)
                    {
                        paused = true;
                    }
                    S_StartSound(NULL, SFX_CHAT);
                }
            }
            return true;
        }
        else if (key == key_menu_endgame)        // F7 (end game)
        {
            if (SCNetCheck(3))
            {
                if (gamestate == GS_LEVEL && !demoplayback)
                {
                    S_StartSound(NULL, SFX_CHAT);
                    SCEndGame(0);
                }
            }
            return true;
        }
        else if (key == key_menu_messages)       // F8 (toggle messages)
        {
            SCMessages(0);
            return true;
        }
        else if (key == key_menu_qload)          // F9 (quickload)
        {
            if (SCNetCheck(2))
            {
                if (!quickload || quickload == -1)
                {
                    MenuActive = true;
                    FileMenuKeySteal = false;
                    MenuTime = 0;
                    CurrentMenu = &LoadMenu;
                    CurrentItPos = CurrentMenu->oldItPos;
                    if (!netgame && !demoplayback)
                    {
                        paused = true;
                    }
                    S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
                    slottextloaded = false; // reload the slot text
                    quickload = -1;
                    P_SetMessage(&players[consoleplayer],
                                 "CHOOSE A QUICKLOAD SLOT", true);
                }
                else
                {
                    askforquit = true;
                    if (!netgame && !demoplayback)
                    {
                        paused = true;
                    }
                    typeofask = 4;
                    S_StartSound(NULL, SFX_CHAT);
                }
            }
            return true;
        }
        else if (key == key_menu_quit)           // F10 (quit)
        {
            if (gamestate == GS_LEVEL || gamestate == GS_FINALE)
            {
                SCQuitGame(0);
                S_StartSound(NULL, SFX_CHAT);
            }
            return true;
        }
        else if (key == key_menu_gamma)          // F11 (gamma correction)
        {
            usegamma++;
            if (usegamma > 4)
            {
                usegamma = 0;
            }
            SB_PaletteFlash(true);  // force change
            P_SetMessage(&players[consoleplayer], GammaText[usegamma],
                         false);
            return true;
        }
        else if (key == KEY_F12)                 // F12 (???)
        {
            // F12 - reload current map (devmaps mode)

            if (netgame)
            {
                return false;
            }
            if (gamekeydown[key_speed])
            {               // Monsters ON
                nomonsters = false;
            }
            if (gamekeydown[key_strafe])
            {               // Monsters OFF
                nomonsters = true;
            }
            G_DeferedInitNew(gameskill, gameepisode, gamemap);
            P_SetMessage(&players[consoleplayer], TXT_CHEATWARP, false);
            return true;
        }
*/
    }

    if (!MenuActive)
    {
//        if (key == key_menu_activate || gamestate == GS_DEMOSCREEN || demoplayback)
        if (ch == key_menu_activate /*|| gamestate == GS_DEMOSCREEN || demoplayback*/)
        {
            MN_ActivateMenu();
            return (true);
        }
        return (false);
    }
    if (!FileMenuKeySteal)
    {
        item = &CurrentMenu->items[CurrentItPos];

        if (ch == key_menu_down)                // Next menu item
        {
            do
            {
		if (CurrentMenu->items[CurrentItPos].type == ITT_SETKEY
			&& CurrentItPos+1 > CurrentMenu->itemCount-1)
		{
		    if (FirstKey == FIRSTKEY_MAX)
		    {
			CurrentItPos = 0; // End of Key menu
			FirstKey = 0;
		    }
		    else
			FirstKey++;
		}
                else if (CurrentItPos + 1 > CurrentMenu->itemCount - 1)
                {
                    CurrentItPos = 0;
                }
                else
                {
                    CurrentItPos++;
                }
            }
            while (CurrentMenu->items[CurrentItPos].type == ITT_EMPTY);
            S_StartSound(NULL, SFX_FIGHTER_HAMMER_HITWALL);
            return (true);
        }
        else if (ch == key_menu_up)             // Previous menu item
        {
            do
            {
		if (CurrentMenu->items[CurrentItPos].type == ITT_SETKEY && CurrentItPos == 0)
		{
		    if (FirstKey == 0)
		    {
			CurrentItPos = 18; // End of Key menu (14 == 15 (max lines on a page) - 1)
			FirstKey = FIRSTKEY_MAX;
		    }
		    else
			FirstKey--;
		}
                if (CurrentItPos == 0)
                {
                    CurrentItPos = CurrentMenu->itemCount - 1;
                }
                else
                {
                    CurrentItPos--;
                }
            }
            while (CurrentMenu->items[CurrentItPos].type == ITT_EMPTY);
            S_StartSound(NULL, SFX_FIGHTER_HAMMER_HITWALL);
            return (true);
        }
        else if (ch == key_menu_left)           // Slider left
        {
            if (item->type == ITT_LRFUNC && item->func != NULL)
            {
                item->func(LEFT_DIR);
                S_StartSound(NULL, SFX_PICKUP_KEY);
            }
            return (true);
        }
        else if (ch == key_menu_right)          // Slider right
        {
            if (item->type == ITT_LRFUNC && item->func != NULL)
            {
                item->func(RIGHT_DIR);
                S_StartSound(NULL, SFX_PICKUP_KEY);
            }
            return (true);
        }
        else if (ch == key_menu_forward)        // Activate item (enter)
        {
            if (item->type == ITT_SETMENU)
            {
                if (item->func != NULL)
                {
                    item->func(item->option);
                }
                SetMenu(item->menu);
            }
            else if (item->func != NULL)
            {
                CurrentMenu->oldItPos = CurrentItPos;
                if (item->type == ITT_LRFUNC)
                {
                    item->func(RIGHT_DIR);
                }
                else if (item->type == ITT_EFUNC)
                {
                    item->func(item->option);
                }
		else if (item->type == ITT_SETKEY)
		{
		    item->func(item->option);
		}
            }
            S_StartSound(NULL, SFX_DOOR_LIGHT_CLOSE);
            return (true);
        }
        else if (ch == key_menu_activate)
        {
            MN_DeactivateMenu();
            return (true);
        }
/*
        else if (ch == key_menu_back)
        {
            S_StartSound(NULL, SFX_PICKUP_KEY);

            if (CurrentMenu->prevMenu == MENU_NONE)
            {
                MN_DeactivateMenu();
            }
            else
            {
                SetMenu(CurrentMenu->prevMenu);
            }
            return (true);
        }
        else if (charTyped != 0)
        {
            for (i = 0; i < CurrentMenu->itemCount; i++)
            {
                if (CurrentMenu->items[i].text)
                {
                    if (toupper(charTyped)
                        == toupper(CurrentMenu->items[i].text[0]))
                    {
                        CurrentItPos = i;
                        return (true);
                    }
                }
            }
        }
*/
        return (false);
    }
    else
    {                           // Editing file names
/*
        textBuffer = &SlotText[currentSlot][slotptr];
        if (key == KEY_BACKSPACE)
        {
            if (slotptr)
            {
                *textBuffer-- = 0;
                *textBuffer = ASCII_CURSOR;
                slotptr--;
            }
            return (true);
        }
        if (key == KEY_ESCAPE)
        {
            memset(SlotText[currentSlot], 0, SLOTTEXTLEN + 2);
            strcpy(SlotText[currentSlot], oldSlotText);
            SlotStatus[currentSlot]--;
            MN_DeactivateMenu();
            return (true);
        }
*/
        if (ch == key_menu_forward)
        {
            SlotText[currentSlot][slotptr] = 0; // clear the cursor
            item = &CurrentMenu->items[CurrentItPos];
            CurrentMenu->oldItPos = CurrentItPos;
            if (item->type == ITT_EFUNC)
            {
                item->func(item->option);
                if (item->menu != MENU_NONE)
                {
                    SetMenu(item->menu);
                }
            }
            return (true);
        }
/*
        if (slotptr < SLOTTEXTLEN && key != KEY_BACKSPACE)
        {
            if (isalpha(charTyped))
            {
                *textBuffer++ = toupper(charTyped);
                *textBuffer = ASCII_CURSOR;
                slotptr++;
                return (true);
            }
            if (isdigit(charTyped) || charTyped == ' '
             || charTyped == ',' || charTyped == '.' || charTyped == '-'
             || charTyped == '!')
            {
                *textBuffer++ = charTyped;
                *textBuffer = ASCII_CURSOR;
                slotptr++;
                return (true);
            }
        }
*/
        return (true);
    }
    return (false);
}

//---------------------------------------------------------------------------
//
// PROC MN_ActivateMenu
//
//---------------------------------------------------------------------------

void MN_ActivateMenu(void)
{
    if (MenuActive)
    {
        return;
    }
    if (paused)
    {
        S_ResumeSound();
    }
    MenuActive = true;
    FileMenuKeySteal = false;
    MenuTime = 0;
    CurrentMenu = &MainMenu;
    CurrentItPos = CurrentMenu->oldItPos;
    if (/*!netgame &&*/ !demoplayback)
    {
        paused = true;
    }
    S_StartSound(NULL, SFX_PLATFORM_STOP);
    slottextloaded = false;     //reload the slot text, when needed
}

//---------------------------------------------------------------------------
//
// PROC MN_DeactivateMenu
//
//---------------------------------------------------------------------------

void MN_DeactivateMenu(void)
{
    if (CurrentMenu != NULL)
    {
        CurrentMenu->oldItPos = CurrentItPos;
    }

    if(MenuActive)
	warped = 0;

    MenuActive = false;
//    if (!netgame)
    {
        paused = false;
    }
    S_StartSound(NULL, SFX_PLATFORM_STOP);
    P_ClearMessage(&players[consoleplayer]);
}

//---------------------------------------------------------------------------
//
// PROC MN_DrawInfo
//
//---------------------------------------------------------------------------

void MN_DrawInfo(void)
{
    I_SetPalette(W_CacheLumpName("PLAYPAL", PU_CACHE));
/*
    memcpy(I_VideoBuffer,							// CHANGED FOR HIRES
           (byte *) W_CacheLumpNum(W_GetNumForName("TITLE") + InfoType,		// CHANGED FOR HIRES
                                   PU_CACHE), SCREENWIDTH * SCREENHEIGHT);	// CHANGED FOR HIRES
*/
    V_CopyScaledBuffer(I_VideoBuffer,						// CHANGED FOR HIRES
	    (byte *) W_CacheLumpNum(W_GetNumForName("TITLE") + InfoType,	// CHANGED FOR HIRES
				    PU_CACHE), ORIGWIDTH * ORIGHEIGHT);		// CHANGED FOR HIRES
//      V_DrawPatch(0, 0, W_CacheLumpNum(W_GetNumForName("TITLE")+InfoType,
//              PU_CACHE));
}


//---------------------------------------------------------------------------
//
// PROC SetMenu
//
//---------------------------------------------------------------------------

static void SetMenu(MenuType_t menu)
{
    CurrentMenu->oldItPos = CurrentItPos;
    CurrentMenu = Menus[menu];
    CurrentItPos = CurrentMenu->oldItPos;
}

//---------------------------------------------------------------------------
//
// PROC DrawSlider
//
//---------------------------------------------------------------------------

static void DrawSlider(Menu_t * menu, int item, int width, int slot)
{
    int x;
    int y;
    int x2;
    int count;

    x = menu->x + 24;
    y = menu->y + 2 + (item * ITEM_HEIGHT);
    V_DrawPatch(x - 32, y, W_CacheLumpName("M_SLDLT", PU_CACHE));
    for (x2 = x, count = width; count--; x2 += 8)
    {
        V_DrawPatch(x2, y, W_CacheLumpName(count & 1 ? "M_SLDMD1"
                                           : "M_SLDMD2", PU_CACHE));
    }
    V_DrawPatch(x2, y, W_CacheLumpName("M_SLDRT", PU_CACHE));
    V_DrawPatch(x + 4 + slot * 8, y + 7, 
                W_CacheLumpName("M_SLDKB", PU_CACHE));
}


