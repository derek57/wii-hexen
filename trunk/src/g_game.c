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


#include <string.h>
#include <unistd.h>
/*
#include <pspkernel.h>
#include <psprtc.h>
*/
#include "m_random.h"
#include "h2def.h"
#include "s_sound.h"
#include "doomkeys.h"
#include "i_video.h"
#include "i_system.h"
#include "i_timer.h"
#include "m_controls.h"
#include "m_misc.h"
#include "p_local.h"
#include "v_video.h"

//#define AM_STARTKEY	9

// External functions

extern void R_InitSky(int map);
extern void P_PlayerNextArtifact(player_t * player);

// Functions

boolean G_CheckDemoStatus(void);
void G_ReadDemoTiccmd(ticcmd_t * cmd);
void G_WriteDemoTiccmd(ticcmd_t * cmd);

void G_DoReborn(int playernum);

void G_DoLoadLevel(void);
void G_DoInitNew(void);
void G_DoNewGame(void);
void G_DoPlayDemo(void);
void G_DoTeleportNewMap(void);
void G_DoCompleted(void);
void G_DoVictory(void);
void G_DoWorldDone(void);
void G_DoSaveGame(void);
void G_DoSingleReborn(void);

void H2_PageTicker(void);
void H2_AdvanceDemo(void);

//extern boolean mn_SuicideConsole;

gameaction_t gameaction;
gamestate_t gamestate;
skill_t gameskill;
//boolean         respawnmonsters;
int gameepisode;
int gamemap;
int prevmap;

boolean paused;
boolean sendpause;              // send a pause event next tic
boolean sendsave;               // send a save event next tic
boolean usergame;               // ok to save / end game

boolean timingdemo;             // if true, exit with report on completion
int starttime;                  // for comparative timing purposes      

boolean viewactive;

//boolean deathmatch;             // only if started as net death
//boolean netgame;                // only true if packets are broadcast
boolean playeringame[MAXPLAYERS];
player_t players[MAXPLAYERS];
pclass_t PlayerClass[MAXPLAYERS];

// Position indicator for cooperative net-play reborn
int RebornPosition;

int consoleplayer;              // player taking events and displaying
int displayplayer;              // view being displayed
int gametic;
int levelstarttic;              // gametic at level start

char demoname[32];
boolean demorecording;
boolean demoplayback;
byte *demobuffer, *demo_p;
boolean singledemo;             // quit after playing a demo from cmdline

boolean precache = true;        // if true, load all graphics at start

// TODO: Hexen uses 16-bit shorts for consistancy?
byte consistancy[MAXPLAYERS][BACKUPTICS];

int mouseSensitivity = 5;

int LeaveMap;
static int LeavePosition;

int key_strafe;
int key_speed;
/*
int key_straferight;
int key_flycenter;
int key_lookup;
int key_lookdown;
int key_lookcenter;

int mousebfire;
int mousebstrafe;
int mousebforward;
int mousebjump;
*/
int	mouselook;

extern int rclass;
extern int extra_wad_loaded;

fixed_t forwardmve;
fixed_t sidemve;
int turnspd;

extern int turnspeed;

//#define MAXPLMOVE       0x32 // Old Heretic Max move

fixed_t MaxPlayerMove[NUMCLASSES] = { 0x3C, 0x32, 0x2D, 0x31 };
extern  fixed_t forwardmove/*[NUMCLASSES][2] = {
    {0x1D, 0x3C},
    {0x19, 0x32},
    {0x16, 0x2E},
    {0x18, 0x31}
}*/;

extern fixed_t sidemove/*[NUMCLASSES][2] = {
    {0x1B, 0x3B},
    {0x18, 0x28},
    {0x15, 0x25},
    {0x17, 0x27}
}*/;

fixed_t angleturn/*[3] = { 640, 1280, 320 }*/;      // + slow turn
/*
static int *weapon_keys[] =
{
    &key_weapon1,
    &key_weapon2,
    &key_weapon3,
    &key_weapon4,
};

static int next_weapon = 0;

#define SLOWTURNTICS    6
*/
#define NUMKEYS 256
boolean gamekeydown[NUMKEYS];
/*
static int turnheld;                   // for accelerative turning
int lookheld;

boolean mousearray[4];
boolean *mousebuttons = &mousearray[1];	// allow [-1]
*/
int mousex, mousey;             // mouse values are used once
int dclicktime, dclickstate, dclicks;
//int dclicktime2, dclickstate2, dclicks2;

#define MAX_JOY_BUTTONS 20

int joyxmove, joyymove;         // joystick values are repeated
boolean joyarray[MAX_JOY_BUTTONS + 1];
boolean *joybuttons = &joyarray[1];     // allow [-1]
boolean dialog_active;

static int      joyirx;
static int      joyiry;

int savegameslot;
char savedescription[32];

int inventoryTics;

// haleyjd: removed externdriver crap

static skill_t TempSkill;
static int TempEpisode;
static int TempMap;
/*
boolean testcontrols = false;
int testcontrols_mousespeed;
*/
int joybstrafe;

int joy_a = 1;		// 0
int joy_r = 2;		// 1
int joy_plus = 4;	// 2
int joy_l = 8;		// 3
int joy_minus = 16;	// 4
int joy_b = 32;		// 5
int joy_left = 64;	// 6
int joy_down = 128;	// 7
int joy_right = 256;	// 8
int joy_up = 512;	// 9
int joy_zr = 1024;	// 10
int joy_zl = 2048;	// 11
int joy_home = 4096;	// 12
int joy_x = 8192;	// 13
int joy_y = 16384;	// 14
int joy_1 = 32768;	// 15
int joy_2 = 65536;	// 16

int     joybinvright = 0;
int     joybfire = 1;
int     joybinvuse = 2;
int     joybuse = 3;
int	joybmenu = 4;
int	joybflydown = 5;
int     joybleft = 6;
int	joybmap = 7;
int	joybright = 8;
int	joybcenter = 9;
int	joybmapzoomout = 10;
int	joybmapzoomin = 11;
int	joybjump = 12;
int	joybflyup = 13;
int     joybinvleft = 14;
int	joybspeed = 15;

extern fixed_t mtof_zoommul;    // how far the window zooms in each tic (map coords)
extern fixed_t ftom_zoommul;    // how far the window zooms in each tic (fb coords)

void AM_Start(void);
void AM_Stop(void);

extern boolean askforquit;
extern boolean askforsave;
extern  int typeofask;
extern  int typeofask2;

extern boolean automapactive;
extern boolean MenuActive;

#define KEY_1               0x02

void ChangeWeaponRight(void)
{
    static player_t*	plyrweap;
    static event_t	kbevent;

    weapontype_t	num;

    if (gamestate == GS_LEVEL && !(MenuActive || automapactive))
    {
	plyrweap = &players[consoleplayer];

	num = plyrweap->readyweapon;

	while (1)
	{
	    dont_move_forwards = true;

	    num++;

	    if (num > WP_FOURTH)
	        num = WP_FIRST;

	    if (plyrweap->weaponowned[num])
	    {
	        plyrweap->pendingweapon = num;

	        break;
	    }
        }
        kbevent.type = ev_keydown;
        kbevent.data1 = KEY_1 + num;

        D_PostEvent(&kbevent);

	dont_move_forwards = false;
    }
}

void ChangeWeaponLeft(void)
{
    static player_t*	plyrweap;
    static event_t	kbevent;

    weapontype_t	num;

    if (gamestate == GS_LEVEL && !(MenuActive || automapactive))
    {
	plyrweap = &players[consoleplayer];

	num = plyrweap->readyweapon;

	while (1)
	{
	    dont_move_forwards = true;

	    num--;

	    if (num == -1)
	        num = WP_FOURTH;

	    if (plyrweap->weaponowned[num])
	    {
	        plyrweap->pendingweapon = num;

	        break;
	    }
        }
	if(num == WP_FOURTH)
	    num = WP_FIRST;

        kbevent.type = ev_keydown;
        kbevent.data1 = KEY_1 + num;

        D_PostEvent(&kbevent);

	dont_move_forwards = false;
    }
}

#include <wiiuse/wpad.h>

void ChangeInventoryItemRight(void)
{
    event_t ev; // keyboard event

    int k_inv = 0;

//    if(WPAD_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_ZR)
    {
	k_inv = 1; // Select right object
	ev.data1 = ']';
    }

    if(k_inv)
	ev.type = ev_keydown;
    else
	ev.type = ev_keyup;

    D_PostEvent(&ev);
}

void ChangeInventoryItemLeft(void)
{
    event_t ev; // keyboard event

    int k_inv = 0;

//    if(WPAD_ButtonsDown(0) & WPAD_CLASSIC_BUTTON_ZL)
    {
	k_inv = 1; // Select right object
	ev.data1 = '[';
    }

    if(k_inv)
	ev.type = ev_keydown;
    else
	ev.type = ev_keyup;

    D_PostEvent(&ev);
}

/*
====================
=
= G_BuildTiccmd
=
= Builds a ticcmd from all of the available inputs or reads it from the
= demo buffer.
= If recording a demo, write it out
====================
*/

extern boolean inventory;
boolean usearti = true;

void G_BuildTiccmd(ticcmd_t *cmd, int maketic)
{
//    int i;
    boolean strafe/*, use*/;
//    int speed, tspeed, lspeed;
    int forward, side;
    int look, arti;
    int flyheight;
    int pClass;

//    extern boolean artiskip;

    // haleyjd: removed externdriver crap

    pClass = players[consoleplayer].class;
    memset(cmd, 0, sizeof(*cmd));

//      cmd->consistancy =
//              consistancy[consoleplayer][(maketic*ticdup)%BACKUPTICS];

    cmd->consistancy = consistancy[consoleplayer][maketic % BACKUPTICS];

//printf ("cons: %i\n",cmd->consistancy);

    strafe = gamekeydown[key_strafe]/*
          || mousebuttons[mousebstrafe]*/
          || joybuttons[joybstrafe];

    // Allow joybspeed hack.
/*
    speed = key_speed >= NUMKEYS
        || joybspeed >= MAX_JOY_BUTTONS
        || !gamekeydown[key_speed]
        || joybuttons[joybspeed];
*/
    // haleyjd: removed externdriver crap
    
    forward = side = look = arti = flyheight = 0;

//
// use two stage accelerative turning on the keyboard and joystick
//
/*
    if (joyxmove < 0 || joyxmove > 0 ||
        gamekeydown[key_right] || gamekeydown[key_left])
        turnheld += ticdup;
    else
        turnheld = 0;

    if (turnheld < SLOWTURNTICS)
        tspeed = 2;             // slow turn
    else
        tspeed = speed;

    if (gamekeydown[key_lookdown] || gamekeydown[key_lookup])
    {
        lookheld += ticdup;
    }
    else
    {
        lookheld = 0;
    }
    if (lookheld < SLOWTURNTICS)
    {
        lspeed = 1;             // 3;
    }
    else
    {
        lspeed = 2;             // 5;
    }
*/

//
// let movement keys cancel each other out
//

    if(PlayerClass[consoleplayer] == PCLASS_FIGHTER)
    {
	if (joybuttons[joybspeed]) 
	{
	    forwardmve = forwardmove * 6;
	    sidemve = sidemove * 6;
//	    turnspd = turnspeed * 6;
	}
	else if(!joybuttons[joybspeed])
	{
	    forwardmve = forwardmove;
	    sidemve = sidemove;
//	    turnspd = turnspeed;
	}
	turnspd = turnspeed;
    }
    else if(PlayerClass[consoleplayer] == PCLASS_CLERIC)
    {
	if (joybuttons[joybspeed]) 
	{
	    forwardmve = forwardmove * 4;
	    sidemve = sidemove * 4;
//	    turnspd = turnspeed * 4;
	}
	else if(!joybuttons[joybspeed])
	{
	    forwardmve = forwardmove / 1.33;
	    sidemve = sidemove / 1.33;
//	    turnspd = turnspeed / 1.33;
	}
	turnspd = turnspeed / 1.33;
    }
    else if(PlayerClass[consoleplayer] == PCLASS_MAGE)
    {
	if (joybuttons[joybspeed]) 
	{
	    forwardmve = forwardmove * 2;
	    sidemve = sidemove * 2;
//	    turnspd = turnspeed * 2;
	}
	else if(!joybuttons[joybspeed])
	{
	    forwardmve = forwardmove / 1.66;
	    sidemve = sidemove / 1.66;
//	    turnspd = turnspeed / 1.66;
	}
	turnspd = turnspeed / 1.66;
    }

//    extern int button_layout;

    if(strafe /*|| button_layout == 1*/)
    {
	if (gamekeydown[key_right])
	{
	    side += sidemve/*[pClass][speed]*/;
	}
	if (gamekeydown[key_left])
	{
	    side -= sidemve/*[pClass][speed]*/;
	}
        if (joyxmove > 0) 
            side += sidemve/*[speed]*/; 
        if (joyxmove < 0) 
            side -= sidemve/*[speed]*/; 
    }
    else
    {
	if (gamekeydown[key_right])
	    cmd->angleturn -= angleturn/*[tspeed]*/;
	if (gamekeydown[key_left])
	    cmd->angleturn += angleturn/*[tspeed]*/;

        if (joyxmove > 20) 
            side += sidemve/*[speed]*/; 
//            cmd->angleturn -= angleturn[tspeed]; 
        if (joyxmove < -20) 
            side -= sidemve/*[speed]*/; 
//            cmd->angleturn += angleturn[tspeed]; 

        if (joyirx > 0)     // calculate wii IR curve based on input
            cmd->angleturn -= turnspd * joyirx;
        if (joyirx < 0)     // calculate wii IR curve based on input
            cmd->angleturn -= turnspd * joyirx;
    }

    extern boolean dont_move_forwards;
    extern boolean dont_move_backwards;

    if (gamekeydown[key_up])
    {
	if(button_layout == 0)
	{
	    if(dont_move_forwards == true)
		forward += forwardmve/*[pClass][speed]*/;
	}
	else if(button_layout == 1 && !gamekeydown[key_use])
	    forward += forwardmve/*[pClass][speed]*/;
    }

    if (gamekeydown[key_down])
    {
	if(button_layout == 0)
	{
	    if(dont_move_backwards == true)
		forward -= forwardmve/*[pClass][speed]*/;
	}
	else if(button_layout == 1 && !gamekeydown[key_use])
	    forward -= forwardmve/*[pClass][speed]*/;
    }
    if (joyymove > 20)
        forward += forwardmve/*[speed]*/;
    if (joyymove < -20)
        forward -= forwardmve/*[speed]*/;

    if (/*gamekeydown[key_straferight] || mousebuttons[mousebstraferight]
     ||*/ joybuttons[joybstraferight])
        side += sidemve/*[speed]*/;
    if (/*gamekeydown[key_strafeleft] || mousebuttons[mousebstrafeleft]
     ||*/ joybuttons[joybstrafeleft])
        side -= sidemve/*[speed]*/;
/*
    if (joyymove < 0)
    {
        forward += forwardmove[pClass][speed];
    }
    if (joyymove > 0)
    {
        forward -= forwardmove[pClass][speed];
    }

    if (gamekeydown[key_straferight])
    {
        side += sidemove[pClass][speed];
    }
    if (gamekeydown[key_strafeleft])
    {
        side -= sidemove[pClass][speed];
    }

    // Look up/down/center keys
    if (gamekeydown[key_lookup])
    {
        look = lspeed;
    }
    if (gamekeydown[key_lookdown])
    {
        look = -lspeed;
    }
    // haleyjd: removed externdriver crap
    if (gamekeydown[key_lookcenter])
    {
        look = TOCENTER;
    }
*/
    // haleyjd: removed externdriver crap

    // Fly up/down/drop keys
    if (gamekeydown[key_flyup] || joybuttons[joybflyup])
    {
        flyheight = 5;          // note that the actual flyheight will be twice this
    }
    if (gamekeydown[key_flydown] || joybuttons[joybflydown])
    {
        flyheight = -5;
    }
/*
    if (gamekeydown[key_flycenter])
    {
        flyheight = TOCENTER;
        // haleyjd: removed externdriver crap
        look = TOCENTER;
    }

    // Use artifact key
    if (gamekeydown[key_useartifact] || joybuttons[joybinvuse])
    {
        if (gamekeydown[key_speed] && artiskip)
        {
            if (players[consoleplayer].inventory[inv_ptr].type != arti_none)
            {                   // Skip an artifact
                gamekeydown[key_useartifact] = false;
		joybuttons[joybinvuse] = false;
                P_PlayerNextArtifact(&players[consoleplayer]);
            }
        }
        else
        {
            if (inventory)
            {
                players[consoleplayer].readyArtifact =
                    players[consoleplayer].inventory[inv_ptr].type;
                inventory = false;
                cmd->arti = 0;
                usearti = false;
            }
            else if (usearti)
            {
                cmd->arti |=
                    players[consoleplayer].inventory[inv_ptr].
                    type & AFLAG_MASK;
                usearti = false;
            }
        }
    }
*/
    if (/*gamekeydown[key_jump] || mousebuttons[mousebjump]
	||*/ joybuttons[joybjump] && !(dialog_active && MenuActive))
    {
	cmd->arti |= AFLAG_JUMP;
    }

    dialog_active = false;
/*
    if (mn_SuicideConsole)
    {
        cmd->arti |= AFLAG_SUICIDE;
        mn_SuicideConsole = false;
    }

    // Artifact hot keys
    if (gamekeydown[KEY_BACKSPACE] && !cmd->arti)
    {
        gamekeydown[KEY_BACKSPACE] = false;     // Use one of each artifact
        cmd->arti = NUMARTIFACTS;
    }
    else if (gamekeydown['\\'] && !cmd->arti
             && (players[consoleplayer].mo->health < MAXHEALTH))
    {
        gamekeydown['\\'] = false;
        cmd->arti = arti_health;
    }
    else if (gamekeydown['0'] && !cmd->arti)
    {
        gamekeydown['0'] = false;
        cmd->arti = arti_poisonbag;
    }
    else if (gamekeydown['9'] && !cmd->arti)
    {
        gamekeydown['9'] = false;
        cmd->arti = arti_blastradius;
    }
    else if (gamekeydown['8'] && !cmd->arti)
    {
        gamekeydown['8'] = false;
        cmd->arti = arti_teleport;
    }
    else if (gamekeydown['7'] && !cmd->arti)
    {
        gamekeydown['7'] = false;
        cmd->arti = arti_teleportother;
    }
    else if (gamekeydown['6'] && !cmd->arti)
    {
        gamekeydown['6'] = false;
        cmd->arti = arti_egg;
    }
    else if (gamekeydown['5'] && !cmd->arti
             && !players[consoleplayer].powers[pw_invulnerability])
    {
        gamekeydown['5'] = false;
        cmd->arti = arti_invulnerability;
    }
*/
//
// buttons
//
//    cmd->chatchar = CT_dequeueChatChar();

    if (gamekeydown[key_fire] /*|| mousebuttons[mousebfire]*/
        || joybuttons[joybfire])
        cmd->buttons |= BT_ATTACK;

    WPADData *data = WPAD_Data(0);

    if(data->exp.type == WPAD_EXP_CLASSIC && !demoplayback)
    {
	if(data->btns_d)
	{
	    if(joybuttons[joybinvuse])
	    {                           // flag to denote that it's okay to use an artifact
		if (inventory)
		{
		    players[consoleplayer].readyArtifact =
			    players[consoleplayer].inventory[inv_ptr].type;
		    inventory = false;
		    cmd->arti = 0;
		    usearti = false;
		}
		else /*if (usearti)*/
		{
		    cmd->arti = players[consoleplayer].inventory[inv_ptr].type;
		    usearti = false;
		}
	    }

	    if(joybuttons[joybmenu])
	    {
		if (!MenuActive)
		    MN_ActivateMenu();
		else
		    MN_DeactivateMenu();

		if (askforquit)
		{
		    askforquit = false;
		    typeofask = 0;
		    MN_DeactivateMenu();
		}

		if (askforsave)
		{
		    askforsave = false;
		    typeofask2 = 0;
		    MN_DeactivateMenu();
		}
	    }

	    if(joybuttons[joybright])
		ChangeWeaponRight();

	    if(joybuttons[joybleft])
		ChangeWeaponLeft();

	    if(joybuttons[joybinvright])
		ChangeInventoryItemRight();

	    if (joybuttons[joybinvleft])
		ChangeInventoryItemLeft();

	    if(joybuttons[joybcenter])
		look = TOCENTER;

	    if(joybuttons[joybmap])
	    {
		if (!automapactive)
		{
		    if(!MenuActive)
			AM_Start ();
		}
		else
		{
		    if(!MenuActive)
		    {
			AM_Stop ();

			extern int screenblocks;

			R_SetViewSize (screenblocks, detailLevel);
		    }
		}
	    }

	    if(automapactive)
	    {
		if(joybuttons[joybmapzoomin])
		{
		    mtof_zoommul = M_ZOOMIN;
		    ftom_zoommul = M_ZOOMOUT;
		}

		if(joybuttons[joybmapzoomout])
		{
		    mtof_zoommul = M_ZOOMOUT;
		    ftom_zoommul = M_ZOOMIN;
		}
	    }
	}
    }

    if(automapactive)
    {
	if(!(joybuttons[joybmapzoomin] || joybuttons[joybmapzoomout]))
	{
	    mtof_zoommul = FRACUNIT;
	    ftom_zoommul = FRACUNIT;
	}
    }

    if (gamekeydown[key_use] || joybuttons[joybuse] /*|| mousebuttons[mousebuse]*/)
    {
        cmd->buttons |= BT_USE;
        dclicks = 0;            // clear double clicks if hit use button
    }

    // Weapon cycling. Switch to previous or next weapon.
    // (Disabled when player is a pig).
/*
    if (players[consoleplayer].morphTics == 0 && next_weapon != 0)
    {
        if (players[consoleplayer].pendingweapon == WP_NOCHANGE)
        {
            i = players[consoleplayer].readyweapon;
        }
        else
        {
            i = players[consoleplayer].pendingweapon;
        }

        do {
            i = (i + next_weapon) % NUMWEAPONS;
        } while (!players[consoleplayer].weaponowned[i]);

        cmd->buttons |= BT_CHANGE;
        cmd->buttons |= i << BT_WEAPONSHIFT;
    }
    else
    {
        for (i=0; i<arrlen(weapon_keys); ++i)
        {
            int key = *weapon_keys[i];

            if (gamekeydown[key])
            {
                cmd->buttons |= BT_CHANGE; 
                cmd->buttons |= i<<BT_WEAPONSHIFT; 
                break; 
            }
        }
    }

    next_weapon = 0;

//
// mouse
//
    if (mousebuttons[mousebforward])
    {
        forward += forwardmove[pClass][speed];
    }

//
// forward double click
//
    if (mousebuttons[mousebforward] != dclickstate && dclicktime > 1)
    {
        dclickstate = mousebuttons[mousebforward];
        if (dclickstate)
            dclicks++;
        if (dclicks == 2)
        {
            cmd->buttons |= BT_USE;
            dclicks = 0;
        }
        else
            dclicktime = 0;
    }
    else
    {
        dclicktime += ticdup;
        if (dclicktime > 20)
        {
            dclicks = 0;
            dclickstate = 0;
        }
    }

    use = gamekeydown[key_use] || joybuttons[joybuse];
    if (use != dclickstate2 && dclicktime2 > 1 )
    {
	dclickstate2 = use;
	if (dclickstate2)
		dclicks2++;
	if (dclicks2 == 2)
	{
		cmd->buttons |= BT_USE;
		dclicks2 = 0;
	players[consoleplayer].lookdir = 0;
	}
	else
		dclicktime2 = 0;
    }
    else
    {
	dclicktime2 += ticdup;
	if (dclicktime2 > 20)
	{
		dclicks2 = 0;
		dclickstate2 = 0;
	}
    }
*/
//
// strafe double click
//

//    if(button_layout == 0)
    {
	if (strafe)
	    side += mousex*2;
	else
	    cmd->angleturn -= mousex*0x8;
    }
/*
    else if(button_layout == 1)
    {
    	if (strafe)
    	    side += mousex*2;
    	else
    	    cmd->angleturn -= mousex*0x8;
    }

    if (mousex == 0)
    {
        testcontrols_mousespeed = 0;
    }
*/
    forward += mousey;

    // mouselook, but not when paused
    if (/*joybuttons[joybuse] &&*/ joyiry && !paused && mouselook > 0)	// FOR PSP: mouselook, but...
    {
	// We'll directly change the viewing pitch of the console player.
	float adj = ((joyiry * 0x4) << 16) / (float) 0x80000000*180*110.0/85.0;

	// initialiser added to prevent compiler warning
	float newlookdir = 0;

	extern int mspeed;

	// Speed up the X11 mlook a little.
	adj *= mspeed;

	if (mouselook == 1)
	    newlookdir = players[consoleplayer].lookdir + adj;
	else if (mouselook == 2)
	    newlookdir = players[consoleplayer].lookdir - adj;

	// vertical view angle taken from p_user.c line 249.
	if (newlookdir > 90)
	    newlookdir = 90;
	else if (newlookdir < -110)
	    newlookdir = -110;

	players[consoleplayer].lookdir = newlookdir;
/*
	cmd->buttons |= BT_USE;

	// clear double clicks if hit use button
	dclicks = 0;
						// NORMALLY THIS CAUSES NO MOVEMENT
	dont_move_forwards = false;		// UPON CHANGING VIEWING DIRECTION
*/
    }
/*
    else if (joybuttons[joybuse] && !paused && mouselook > 0)
    {
	cmd->buttons |= BT_USE;

	// clear double clicks if hit use button
	dclicks = 0;
    }
*/
    mousex = mousey = 0;

    if (forward > MaxPlayerMove[pClass])
    {
        forward = MaxPlayerMove[pClass];
    }
    else if (forward < -MaxPlayerMove[pClass])
    {
        forward = -MaxPlayerMove[pClass];
    }
    if (side > MaxPlayerMove[pClass])
    {
        side = MaxPlayerMove[pClass];
    }
    else if (side < -MaxPlayerMove[pClass])
    {
        side = -MaxPlayerMove[pClass];
    }
    if (players[consoleplayer].powers[pw_speed]
        && !players[consoleplayer].morphTics)
    {                           // Adjust for a player with a speed artifact
        forward = (3 * forward) >> 1;
        side = (3 * side) >> 1;
    }
    cmd->forwardmove += forward;
    cmd->sidemove += side;
    if (players[consoleplayer].playerstate == PST_LIVE)
    {
        if (look < 0)
        {
            look += 16;
        }
        cmd->lookfly = look;
    }
    if (flyheight < 0)
    {
        flyheight += 16;
    }
    cmd->lookfly |= flyheight << 4;

//
// special buttons
//
    if (sendpause)
    {
        sendpause = false;
        cmd->buttons = BT_SPECIAL | BTS_PAUSE;
    }

    if (sendsave)
    {
        sendsave = false;
        cmd->buttons =
            BT_SPECIAL | BTS_SAVEGAME | (savegameslot << BTS_SAVESHIFT);
    }
}


/*
==============
=
= G_DoLoadLevel
=
==============
*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"

void G_DoLoadLevel(void)
{
    int i;

    levelstarttic = gametic;    // for time calculation 
    gamestate = GS_LEVEL;
    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (playeringame[i] && players[i].playerstate == PST_DEAD)
            players[i].playerstate = PST_REBORN;
        memset(players[i].frags, 0, sizeof(players[i].frags));
    }

    SN_StopAllSequences();
    P_SetupLevel(gameepisode, gamemap, 0, gameskill);
    displayplayer = consoleplayer;      // view the guy you are playing   
    starttime = I_GetTime();
    gameaction = ga_nothing;
    Z_CheckHeap();

//
// clear cmd building stuff
// 

    memset(gamekeydown, 0, sizeof(gamekeydown));
    joyxmove = joyymove = joyirx = joyiry = 0; 
    mousex = mousey = 0;
    sendpause = sendsave = paused = false;
//    memset(mousebuttons, 0, sizeof(mousebuttons));
    memset(joybuttons, 0, sizeof(joybuttons));
/*
    if (testcontrols)
    {
        P_SetMessage(&players[consoleplayer], "PRESS ESCAPE TO QUIT.", false);
    }
*/
}


/*
===============================================================================
=
= G_Responder 
=
= get info needed to make ticcmd_ts for the players
=
===============================================================================
*/

boolean G_Responder(event_t * ev)
{
//    WPADData *data = WPAD_Data(0);

    player_t *plr;

    plr = &players[consoleplayer];
/*
    if (ev->type == ev_keyup && ev->data1 == key_useartifact)
    {                           // flag to denote that it's okay to use an artifact
        if (!inventory)
        {
            plr->readyArtifact = plr->inventory[inv_ptr].type;
        }
        usearti = true;
    }
    if(ev->type == ev_joystick && ev->data1 == 4 && data->btns_d)
    {                           // flag to denote that it's okay to use an artifact
        if (!inventory)
        {
            plr->readyArtifact = plr->inventory[inv_ptr].type;
        }
        usearti = true;
    }

    // Check for spy mode player cycle
    if (gamestate == GS_LEVEL && ev->type == ev_keydown
        && ev->data1 == key_spy && !deathmatch)
    {                           // Cycle the display player
        do
        {
            displayplayer++;
            if (displayplayer == MAXPLAYERS)
            {
                displayplayer = 0;
            }
        }
        while (!playeringame[displayplayer]
               && displayplayer != consoleplayer);
        return (true);
    }

    if (CT_Responder(ev))
    {                           // Chat ate the event
        return (true);
    }
*/
    if (gamestate == GS_LEVEL)
    {
        if (SB_Responder(ev))
        {                       // Status bar ate the event
            return (true);
        }
        if (AM_Responder(ev))
        {                       // Automap ate the event
            return (true);
        }
    }
/*
    if (ev->type == ev_mouse)
    {
        testcontrols_mousespeed = abs(ev->data2);
    }

    if (ev->type == ev_keydown && ev->data1 == key_prevweapon)
    {
        next_weapon = -1;
    }
    else if (ev->type == ev_keydown && ev->data1 == key_nextweapon)
    {
        next_weapon = 1;
    }
*/
    switch (ev->type)
    {
        case ev_keydown:

            if (joybuttons[joybinvleft])
            {
                inventoryTics = 5 * 35;
                if (!inventory)
                {
                    inventory = true;
                    break;
                }
                inv_ptr--;
                if (inv_ptr < 0)
                {
                    inv_ptr = 0;
                }
                else
                {
                    curpos--;
                    if (curpos < 0)
                    {
                        curpos = 0;
                    }
                }
                return (true);
            }
	    if(joybuttons[joybinvright])
            {
                inventoryTics = 5 * 35;
                if (!inventory)
                {
                    inventory = true;
                    break;
                }
                inv_ptr++;
                if (inv_ptr >= plr->inventorySlotNum)
                {
                    inv_ptr--;
                    if (inv_ptr < 0)
                        inv_ptr = 0;
                }
                else
                {
                    curpos++;
                    if (curpos > 6)
                    {
                        curpos = 6;
                    }
                }
                return (true);
            }
/*
            if (ev->data1 == KEY_PAUSE && !MenuActive)
            {
                sendpause = true;
                return (true);
            }
*/
            if (ev->data1 < NUMKEYS)
            {
                gamekeydown[ev->data1] = true;
            }
            return (true);      // eat key down events

        case ev_keyup:
            if (ev->data1 < NUMKEYS)
            {
                gamekeydown[ev->data1] = false;
            }
            return (false);     // always let key up events filter down

        case ev_mouse:
/*
            mousebuttons[0] = ev->data1 & 1;
            mousebuttons[1] = ev->data1 & 2;
            mousebuttons[2] = ev->data1 & 4;
*/
            mousex = ev->data2 * (mouseSensitivity + 5) / 7/*10*/;
            mousey = ev->data3 * (mouseSensitivity + 5) / 7/*10*/;
            return (true);      // eat events

        case ev_joystick:
	    joybuttons[0] = (ev->data1 & joy_a) > 0;
	    joybuttons[1] = (ev->data1 & joy_r) > 0;
	    joybuttons[2] = (ev->data1 & joy_plus) > 0;
	    joybuttons[3] = (ev->data1 & joy_l) > 0;
	    joybuttons[4] = (ev->data1 & joy_minus) > 0;
	    joybuttons[5] = (ev->data1 & joy_b) > 0;
	    joybuttons[6] = (ev->data1 & joy_left) > 0;
	    joybuttons[7] = (ev->data1 & joy_down) > 0;
	    joybuttons[8] = (ev->data1 & joy_right) > 0;
	    joybuttons[9] = (ev->data1 & joy_up) > 0;
	    joybuttons[10] = (ev->data1 & joy_zr) > 0;
	    joybuttons[11] = (ev->data1 & joy_zl) > 0;
	    joybuttons[12] = (ev->data1 & joy_home) > 0;
	    joybuttons[13] = (ev->data1 & joy_x) > 0;
	    joybuttons[14] = (ev->data1 & joy_y) > 0;
	    joybuttons[15] = (ev->data1 & joy_1) > 0;
	    joybuttons[16] = (ev->data1 & joy_2) > 0;
	    joyxmove = ev->data2; 
	    joyymove = ev->data3; 
            joyirx = ev->data4;
            joyiry = ev->data5;
            return (true);      // eat events

        default:
            break;
    }
    return (false);
}


//==========================================================================
//
// G_Ticker
//
//==========================================================================

void G_Ticker(void)
{
    int i/*, buf*/;
    ticcmd_t *cmd = NULL;

//
// do player reborns if needed
//
    for (i = 0; i < MAXPLAYERS; i++)
        if (playeringame[i] && players[i].playerstate == PST_REBORN)
            G_DoReborn(i);

//
// do things to change the game state
//
    while (gameaction != ga_nothing)
    {
        switch (gameaction)
        {
            case ga_loadlevel:
                G_DoLoadLevel();
                break;
            case ga_initnew:
                G_DoInitNew();
                break;
            case ga_newgame:
                G_DoNewGame();
                break;
            case ga_loadgame:

		dialog_active = true;

		if(!HEXEN_BETA)
		{
		    Draw_LoadIcon();
		}

                G_DoLoadGame();
                break;
            case ga_savegame:

		dialog_active = true;

		if(!HEXEN_BETA)
		{
                    Draw_SaveIcon();
		}

                G_DoSaveGame();
                break;
            case ga_singlereborn:
                G_DoSingleReborn();
                break;
            case ga_playdemo:
                G_DoPlayDemo();
                break;
            case ga_screenshot:
//                V_ScreenShot("HEXEN%02i.pcx");
                P_SetMessage(&players[consoleplayer], "SCREEN SHOT", false);
                gameaction = ga_nothing;
                break;
            case ga_leavemap:

		dialog_active = true;

		if(!HEXEN_BETA)
		{
                    Draw_TeleportIcon();
		}

                G_DoTeleportNewMap();
                break;
            case ga_completed:
                G_DoCompleted();
                break;
            case ga_worlddone:
                G_DoWorldDone();
                break;
            case ga_victory:
                F_StartFinale();
                break;
            default:
                break;
        }
    }


//
// get commands, check consistancy, and build new consistancy check
//
    //buf = gametic%BACKUPTICS;
//    buf = (gametic / ticdup) % BACKUPTICS;

    for (i = 0; i < MAXPLAYERS; i++)
        if (playeringame[i])
        {
            cmd = &players[i].cmd;

            memcpy(cmd, &netcmds[i], sizeof(ticcmd_t));

            if (demoplayback)
                G_ReadDemoTiccmd(cmd);
            if (demorecording)
                G_WriteDemoTiccmd(cmd);
/*
            if (netgame && !(gametic % ticdup))
            {
                if (gametic > BACKUPTICS
                    && consistancy[i][buf] != cmd->consistancy)
                {
                    I_Error("consistency failure (%i should be %i)",
                            cmd->consistancy, consistancy[i][buf]);
                }
                if (players[i].mo)
                    consistancy[i][buf] = players[i].mo->x;
                else
                    consistancy[i][buf] = rndindex;
            }
*/
        }

//
// check for special buttons
//
    for (i = 0; i < MAXPLAYERS; i++)
        if (playeringame[i])
        {
            if (players[i].cmd.buttons & BT_SPECIAL)
            {
                switch (players[i].cmd.buttons & BT_SPECIALMASK)
                {
                    case BTS_PAUSE:
                        paused ^= 1;
                        if (paused)
                        {
                            S_PauseSound();
                        }
                        else
                        {
                            S_ResumeSound();
                        }
                        break;

                    case BTS_SAVEGAME:
                        if (!savedescription[0])
                        {
/*
                            if (netgame)
                            {
                                strcpy(savedescription, "NET GAME");
                            }
                            else
*/
                            {
                                strcpy(savedescription, "SAVE GAME");
                            }
                        }
                        savegameslot =
                            (players[i].cmd.
                             buttons & BTS_SAVEMASK) >> BTS_SAVESHIFT;
                        gameaction = ga_savegame;
                        break;
                }
            }
        }
    // turn inventory off after a certain amount of time
    if (inventory && !(--inventoryTics))
    {
        players[consoleplayer].readyArtifact =
            players[consoleplayer].inventory[inv_ptr].type;
        inventory = false;
        cmd->arti = 0;
    }
//
// do main actions
//
//
// do main actions
//
    switch (gamestate)
    {
        case GS_LEVEL:
            P_Ticker();
            SB_Ticker();
            AM_Ticker();
//            CT_Ticker();
            break;
        case GS_INTERMISSION:
            IN_Ticker();
            break;
        case GS_FINALE:
            F_Ticker();
            break;
        case GS_DEMOSCREEN:
            H2_PageTicker();
            break;
    }
}


/*
==============================================================================

						PLAYER STRUCTURE FUNCTIONS

also see P_SpawnPlayer in P_Things
==============================================================================
*/

//==========================================================================
//
// G_PlayerExitMap
//
// Called when the player leaves a map.
//
//==========================================================================

void G_PlayerExitMap(int playerNumber)
{
    int i;
    player_t *player;
    int flightPower;

    player = &players[playerNumber];

//      if(deathmatch)
//      {
//              // Strip all but one of each type of artifact
//              for(i = 0; i < player->inventorySlotNum; i++)
//              {
//                      player->inventory[i].count = 1;
//              }
//              player->artifactCount = player->inventorySlotNum;
//      }
//      else

    // Strip all current powers (retain flight)
    flightPower = player->powers[pw_flight];
    memset(player->powers, 0, sizeof(player->powers));
    player->powers[pw_flight] = flightPower;
/*
    if (deathmatch)
    {
        player->powers[pw_flight] = 0;
    }
    else
*/
    {
        if (P_GetMapCluster(gamemap) != P_GetMapCluster(LeaveMap))
        {                       // Entering new cluster
            // Strip all keys
            player->keys = 0;

            // Strip flight artifact
            for (i = 0; i < 25; i++)
            {
                player->powers[pw_flight] = 0;
                P_PlayerUseArtifact(player, arti_fly);
            }
            player->powers[pw_flight] = 0;
        }
    }

    if (player->morphTics)
    {
        player->readyweapon = player->mo->special1;     // Restore weapon
        player->morphTics = 0;
    }
    player->messageTics = 0;
    player->lookdir = 0;
    player->mo->flags &= ~MF_SHADOW;    // Remove invisibility
    player->extralight = 0;     // Remove weapon flashes
    player->fixedcolormap = 0;  // Remove torch
    player->damagecount = 0;    // No palette changes
    player->bonuscount = 0;
    player->poisoncount = 0;
    if (player == &players[consoleplayer])
    {
        SB_state = -1;          // refresh the status bar
        viewangleoffset = 0;
    }
}

//==========================================================================
//
// G_PlayerReborn
//
// Called after a player dies.  Almost everything is cleared and
// initialized.
//
//==========================================================================

void G_PlayerReborn(int player)
{
    player_t *p;
    int frags[MAXPLAYERS];
    int killcount, itemcount, secretcount;
    unsigned int worldTimer;

    memcpy(frags, players[player].frags, sizeof(frags));
    killcount = players[player].killcount;
    itemcount = players[player].itemcount;
    secretcount = players[player].secretcount;
    worldTimer = players[player].worldTimer;

    p = &players[player];
    memset(p, 0, sizeof(*p));

    memcpy(players[player].frags, frags, sizeof(players[player].frags));
    players[player].killcount = killcount;
    players[player].itemcount = itemcount;
    players[player].secretcount = secretcount;
    players[player].worldTimer = worldTimer;
    players[player].class = PlayerClass[player];

    p->usedown = p->attackdown = true;  // don't do anything immediately
    p->playerstate = PST_LIVE;
    p->health = MAXHEALTH;
    p->readyweapon = p->pendingweapon = WP_FIRST;
    p->weaponowned[WP_FIRST] = true;
    p->messageTics = 0;
    p->lookdir = 0;
    localQuakeHappening[player] = false;
    if (p == &players[consoleplayer])
    {
        SB_state = -1;          // refresh the status bar
        inv_ptr = 0;            // reset the inventory pointer
        curpos = 0;
        viewangleoffset = 0;
    }
}

/*
====================
=
= G_CheckSpot 
=
= Returns false if the player cannot be respawned at the given mapthing_t spot 
= because something is occupying it
====================
*/

void P_SpawnPlayer(mapthing_t * mthing);

boolean G_CheckSpot(int playernum, mapthing_t * mthing)
{
    fixed_t x, y;
    subsector_t *ss;
    unsigned an;
    mobj_t *mo;

    x = mthing->x << FRACBITS;
    y = mthing->y << FRACBITS;

    players[playernum].mo->flags2 &= ~MF2_PASSMOBJ;
    if (!P_CheckPosition(players[playernum].mo, x, y))
    {
        players[playernum].mo->flags2 |= MF2_PASSMOBJ;
        return false;
    }
    players[playernum].mo->flags2 |= MF2_PASSMOBJ;

// spawn a teleport fog
    ss = R_PointInSubsector(x, y);
    an = ((unsigned) ANG45 * (mthing->angle / 45)) >> ANGLETOFINESHIFT;

    mo = P_SpawnMobj(x + 20 * finecosine[an], y + 20 * finesine[an],
                     ss->sector->floorheight + TELEFOGHEIGHT, MT_TFOG);
    if (players[consoleplayer].viewz != 1)
        S_StartSound(mo, SFX_TELEPORT); // don't start sound on first frame

    return true;
}

/*
====================
=
= G_DeathMatchSpawnPlayer
=
= Spawns a player at one of the random death match spots
= called at level load and each death
====================
*/
/*
void G_DeathMatchSpawnPlayer(int playernum)
{
    int i, j;
    int selections;

    selections = deathmatch_p - deathmatchstarts;

    // This check has been moved to p_setup.c:P_LoadThings()
    //if (selections < 8)
    //      I_Error ("Only %i deathmatch spots, 8 required", selections);

    for (j = 0; j < 20; j++)
    {
        i = P_Random() % selections;
        if (G_CheckSpot(playernum, &deathmatchstarts[i]))
        {
            deathmatchstarts[i].type = playernum + 1;
            P_SpawnPlayer(&deathmatchstarts[i]);
            return;
        }
    }

// no good spot, so the player will probably get stuck
    P_SpawnPlayer(&playerstarts[0][playernum]);
}
*/
//==========================================================================
//
// G_DoReborn
//
//==========================================================================

void G_DoReborn(int playernum)
{
/*
    int i;
    boolean oldWeaponowned[NUMWEAPONS];
    int oldKeys;
    int oldPieces;
    boolean foundSpot;
    int bestWeapon;
*/
    if (G_CheckDemoStatus())
    {
        return;
    }
//    if (!netgame)
    {
        if (SV_RebornSlotAvailable())
        {                       // Use the reborn code if the slot is available
            gameaction = ga_singlereborn;
        }
        else
        {                       // Start a new game if there's no reborn info
            gameaction = ga_newgame;
        }
    }
/*
    else
    {                           // Net-game
        players[playernum].mo->player = NULL;   // Dissassociate the corpse

        if (deathmatch)
        {                       // Spawn at random spot if in death match
            G_DeathMatchSpawnPlayer(playernum);
            return;
        }

        // Cooperative net-play, retain keys and weapons
        oldKeys = players[playernum].keys;
        oldPieces = players[playernum].pieces;
        for (i = 0; i < NUMWEAPONS; i++)
        {
            oldWeaponowned[i] = players[playernum].weaponowned[i];
        }

        foundSpot = false;
        if (G_CheckSpot(playernum, &playerstarts[RebornPosition][playernum]))
        {                       // Appropriate player start spot is open
            P_SpawnPlayer(&playerstarts[RebornPosition][playernum]);
            foundSpot = true;
        }
        else
        {
            // Try to spawn at one of the other player start spots
            for (i = 0; i < MAXPLAYERS; i++)
            {
                if (G_CheckSpot(playernum, &playerstarts[RebornPosition][i]))
                {               // Found an open start spot

                    // Fake as other player
                    playerstarts[RebornPosition][i].type = playernum + 1;
                    P_SpawnPlayer(&playerstarts[RebornPosition][i]);

                    // Restore proper player type
                    playerstarts[RebornPosition][i].type = i + 1;

                    foundSpot = true;
                    break;
                }
            }
        }

        if (foundSpot == false)
        {                       // Player's going to be inside something
            P_SpawnPlayer(&playerstarts[RebornPosition][playernum]);
        }

        // Restore keys and weapons
        players[playernum].keys = oldKeys;
        players[playernum].pieces = oldPieces;
        for (bestWeapon = 0, i = 0; i < NUMWEAPONS; i++)
        {
            if (oldWeaponowned[i])
            {
                bestWeapon = i;
                players[playernum].weaponowned[i] = true;
            }
        }
        players[playernum].mana[MANA_1] = 25;
        players[playernum].mana[MANA_2] = 25;
        if (bestWeapon)
        {                       // Bring up the best weapon
            players[playernum].pendingweapon = bestWeapon;
        }
    }
*/
}

void G_ScreenShot(void)
{
    gameaction = ga_screenshot;
}

//==========================================================================
//
// G_StartNewInit
//
//==========================================================================

void G_StartNewInit(void)
{
    SV_InitBaseSlot();
    SV_ClearRebornSlot();
    P_ACSInitNewGame();
    // Default the player start spot group to 0
    RebornPosition = 0;
}

//==========================================================================
//
// G_StartNewGame
//
//==========================================================================

void G_StartNewGame(skill_t skill)
{
    int realMap;

    G_StartNewInit();
    realMap = P_TranslateMap(1);
    if (realMap == -1)
    {
        realMap = 1;
    }
    G_InitNew(TempSkill, 1, realMap);
}

//==========================================================================
//
// G_TeleportNewMap
//
// Only called by the warp cheat code.  Works just like normal map to map
// teleporting, but doesn't do any interlude stuff.
//
//==========================================================================

void G_TeleportNewMap(int map, int position)
{
    gameaction = ga_leavemap;
    LeaveMap = map;
    LeavePosition = position;
}

//==========================================================================
//
// G_DoTeleportNewMap
//
//==========================================================================

void G_DoTeleportNewMap(void)
{
    SV_MapTeleport(LeaveMap, LeavePosition);
    gamestate = GS_LEVEL;
    gameaction = ga_nothing;
    RebornPosition = LeavePosition;
}

/*
boolean secretexit;
void G_ExitLevel (void)
{
	secretexit = false;
	gameaction = ga_completed;
}
void G_SecretExitLevel (void)
{
	secretexit = true;
	gameaction = ga_completed;
}
*/

//==========================================================================
//
// G_Completed
//
// Starts intermission routine, which is used only during hub exits,
// and DeathMatch games.
//==========================================================================

void G_Completed(int map, int position)
{
    if((HEXEN_DEMO && map > 4) || (HEXEN_MACDEMO && map > 4))
    {
	P_SetMessage(&players[consoleplayer], "ACCESS DENIED -- DEMO", true);

	S_StartSound(NULL, SFX_CHAT);

	return;
    }

    gameaction = ga_completed;
    LeaveMap = map;
    LeavePosition = position;
}

void G_DoCompleted(void)
{
    int i;

    gameaction = ga_nothing;
    if (G_CheckDemoStatus())
    {
        return;
    }
    for (i = 0; i < MAXPLAYERS; i++)
    {
        if (playeringame[i])
        {
            G_PlayerExitMap(i);
        }
    }
    if (LeaveMap == -1 && LeavePosition == -1)
    {
        gameaction = ga_victory;
        return;
    }
    else
    {
        gamestate = GS_INTERMISSION;
        IN_Start();
    }

/*
	int i;
	static int afterSecret[3] = { 7, 5, 5 };

	gameaction = ga_nothing;
	if(G_CheckDemoStatus())
	{
		return;
	}
	for(i = 0; i < MAXPLAYERS; i++)
	{
		if(playeringame[i])
		{
			G_PlayerFinishLevel(i);
		}
	}
	prevmap = gamemap;
	if(secretexit == true)
	{
		gamemap = 9;
	}
	else if(gamemap == 9)
	{ // Finished secret level
		gamemap = afterSecret[gameepisode-1];
	}
	else if(gamemap == 8)
	{
		gameaction = ga_victory;
		return;
	}
	else
	{
		gamemap++;
	}
	gamestate = GS_INTERMISSION;
	IN_Start();
*/
}

//============================================================================
//
// G_WorldDone
//
//============================================================================

void G_WorldDone(void)
{
    gameaction = ga_worlddone;
}

//============================================================================
//
// G_DoWorldDone
//
//============================================================================

void G_DoWorldDone(void)
{
    gamestate = GS_LEVEL;
    G_DoLoadLevel();
    gameaction = ga_nothing;
    viewactive = true;
}

//==========================================================================
//
// G_DoSingleReborn
//
// Called by G_Ticker based on gameaction.  Loads a game from the reborn
// save slot.
//
//==========================================================================

void G_DoSingleReborn(void)
{
    gameaction = ga_nothing;
    SV_LoadGame(SV_GetRebornSlot());
    SB_SetClassData();
}

//==========================================================================
//
// G_LoadGame
//
// Can be called by the startup code or the menu task.
//
//==========================================================================

static int GameLoadSlot;

void G_LoadGame(int slot)
{
    GameLoadSlot = slot;
    gameaction = ga_loadgame;
}

//==========================================================================
//
// G_DoLoadGame
//
// Called by G_Ticker based on gameaction.
//
//==========================================================================

void G_DoLoadGame(void)
{
    gameaction = ga_nothing;
    SV_LoadGame(GameLoadSlot);
//    if (!netgame)
    {                           // Copy the base slot to the reborn slot
        SV_UpdateRebornSlot();
    }
    SB_SetClassData();
}

//==========================================================================
//
// G_SaveGame
//
// Called by the menu task.  <description> is a 24 byte text string.
//
//==========================================================================

#include <time.h>

void G_SaveGame(int slot, char *description)
{
    time_t theTime = time(NULL);
    struct tm *aTime = localtime(&theTime);

    int day = aTime->tm_mday;
    int month = aTime->tm_mon + 1;
    int year = aTime->tm_year + 1900;
    int hour = aTime->tm_hour;
    int min = aTime->tm_min;
    int gamemapx = 0;

    if(datadisc)
	gamemapx = gamemap - 40;
    else
	gamemapx = gamemap;

    savegameslot = slot;

    if(load_extra_wad == 1)
	sprintf(savedescription, "MAP%2.2d %d/%d/%d %2.2d:%2.2d *",
		gamemapx, year, month, day, hour, min);
    else
	sprintf(savedescription, "MAP%2.2d %d/%d/%d %2.2d:%2.2d",
		gamemapx, year, month, day, hour, min);

    sendsave = true;
}

//==========================================================================
//
// G_DoSaveGame
//
// Called by G_Ticker based on gameaction.
//
//==========================================================================

void G_DoSaveGame(void)
{
    dialog_active = true;
    SV_SaveGame(savegameslot, savedescription);
    gameaction = ga_nothing;
    savedescription[0] = 0;
    P_SetMessage(&players[consoleplayer], TXT_GAMESAVED, true);
}

//==========================================================================
//
// G_DeferredNewGame
//
//==========================================================================

void G_DeferredNewGame(skill_t skill)
{
    TempSkill = skill;
    gameaction = ga_newgame;
}

//==========================================================================
//
// G_DoNewGame
//
//==========================================================================

void G_DoNewGame(void)
{
    G_StartNewGame(TempSkill);
    gameaction = ga_nothing;
}

/*
====================
=
= G_InitNew
=
= Can be called by the startup code or the menu task
= consoleplayer, displayplayer, playeringame[] should be set
====================
*/

void G_DeferedInitNew(skill_t skill, int episode, int map)
{
    TempSkill = skill;
    TempEpisode = episode;
    TempMap = map;
    gameaction = ga_initnew;
}

void G_DoInitNew(void)
{
    SV_InitBaseSlot();
    G_InitNew(TempSkill, TempEpisode, TempMap);
    gameaction = ga_nothing;
}

void G_InitNew(skill_t skill, int episode, int map)
{
    int i;

    if (paused)
    {
        paused = false;
        S_ResumeSound();
    }
    if (skill < sk_baby)
    {
        skill = sk_baby;
    }
    if (skill > sk_nightmare)
    {
        skill = sk_nightmare;
    }
    if (map < 1)
    {
        map = 1;
    }
    if (map > 99)
    {
        map = 99;
    }
    M_ClearRandom();
    // Force players to be initialized upon first level load
    for (i = 0; i < MAXPLAYERS; i++)
    {
        players[i].playerstate = PST_REBORN;
        players[i].worldTimer = 0;
    }

    // Set up a bunch of globals
    usergame = true;            // will be set false if a demo
    paused = false;
    demorecording = false;
    demoplayback = false;
    viewactive = true;
    gameepisode = episode;
    gamemap = map;
    gameskill = skill;
    BorderNeedRefresh = true;

    // Initialize the sky
    R_InitSky(map);

    // Give one null ticcmd_t
    //gametic = 0;
    //maketic = 1;
    //for (i=0 ; i<MAXPLAYERS ; i++)
    //      nettics[i] = 1; // one null event for this gametic
    //memset (localcmds,0,sizeof(localcmds));
    //memset (netcmds,0,sizeof(netcmds));

    G_DoLoadLevel();
}

/*
===============================================================================

							DEMO RECORDING

===============================================================================
*/

#define DEMOMARKER      0x80

void G_ReadDemoTiccmd(ticcmd_t * cmd)
{
    if (*demo_p == DEMOMARKER)
    {                           // end of demo data stream
        G_CheckDemoStatus();
        return;
    }
    cmd->forwardmove = ((signed char) *demo_p++);
    cmd->sidemove = ((signed char) *demo_p++);
    cmd->angleturn = ((unsigned char) *demo_p++) << 8;
    cmd->buttons = (unsigned char) *demo_p++;
    cmd->lookfly = (unsigned char) *demo_p++;
    cmd->arti = (unsigned char) *demo_p++;
}

void G_WriteDemoTiccmd(ticcmd_t * cmd)
{
/*
    if (gamekeydown['q'])       // press q to end demo recording
        G_CheckDemoStatus();
*/
    *demo_p++ = cmd->forwardmove;
    *demo_p++ = cmd->sidemove;
    *demo_p++ = cmd->angleturn >> 8;
    *demo_p++ = cmd->buttons;
    *demo_p++ = cmd->lookfly;
    *demo_p++ = cmd->arti;
    demo_p -= 6;
    G_ReadDemoTiccmd(cmd);      // make SURE it is exactly the same
}



/*
===================
=
= G_RecordDemo
=
===================
*/

void G_RecordDemo(skill_t skill, int rclass, int numplayers, int episode, int map/*,
                  char *name*/)
{
    int i;

    G_InitNew(skill, episode, map);
    usergame = false;
/*
    strcpy(demoname, name);
    strcat(demoname, ".lmp");
*/
    demobuffer = demo_p = Z_Malloc(0x20000, PU_STATIC, NULL);
    *demo_p++ = skill;
    *demo_p++ = episode;
    *demo_p++ = map;

    for (i = 0; i < MAXPLAYERS; i++)
    {
        *demo_p++ = playeringame[i];
        *demo_p++ = PlayerClass[i];
    }
    demorecording = true;
}


/*
===================
=
= G_PlayDemo
=
===================
*/

char *defdemoname;

void G_DeferedPlayDemo(char *name)
{
    defdemoname = name;
    gameaction = ga_playdemo;
}

void G_DoPlayDemo(void)
{
    skill_t skill;
    int i, episode, map;

    gameaction = ga_nothing;
    demobuffer = demo_p = W_CacheLumpName(defdemoname, PU_STATIC);
    skill = *demo_p++;
    episode = *demo_p++;
    map = *demo_p++;

    for (i = 0; i < MAXPLAYERS; i++)
    {
        playeringame[i] = *demo_p++;
        PlayerClass[i] = *demo_p++;
    }

    // Initialize world info, etc.
    G_StartNewInit();

    precache = false;           // don't spend a lot of time in loadlevel
    G_InitNew(skill, episode, map);
    precache = true;
    usergame = false;
    demoplayback = true;
}


/*
===================
=
= G_TimeDemo
=
===================
*/

void G_TimeDemo(char *name)
{
    skill_t skill;
    int episode, map;

    demobuffer = demo_p = W_CacheLumpName(name, PU_STATIC);
    skill = *demo_p++;
    episode = *demo_p++;
    map = *demo_p++;
    G_InitNew(skill, episode, map);
    usergame = false;
    demoplayback = true;
    timingdemo = true;
    singletics = true;
}


/*
===================
=
= G_CheckDemoStatus
=
= Called after a death or level completion to allow demos to be cleaned up
= Returns true if a new demo loop action will take place
===================
*/

boolean G_CheckDemoStatus(void)
{
    int     i, endtime;
    char    lbmname[10];

    FILE    *test_access;

    strcpy(lbmname,"DEMO00.lmp");

    for (i=1 ; i<=98 ; i++)
    {
	lbmname[4] = i/10 + '0';
	lbmname[5] = i%10 + '0';
/*
	if (access(lbmname,0) == -1)
	    break;  // file doesn't exist
*/
	test_access = fopen(lbmname, "rb");
  
	if(test_access)
	    break;
    }

    if (i==100)
	I_Error ("G_CheckDemoStatus: Couldn't create a DEMO");

    if (timingdemo)
    {
        endtime = I_GetTime();
        I_Error("timed %i gametics in %i realtics", gametic,
                endtime - starttime);
    }

    if (demoplayback)
    {
        if (singledemo)
            I_Quit();

        W_ReleaseLumpName(defdemoname);
        demoplayback = false;
        H2_AdvanceDemo();
        return true;
    }

    if (demorecording)
    {
        *demo_p++ = DEMOMARKER;
        M_WriteFile(lbmname, demobuffer, demo_p - demobuffer);
        Z_Free(demobuffer);
        demorecording = false;
        I_Error("Demo %s recorded", demoname);
    }

    return false;
}
