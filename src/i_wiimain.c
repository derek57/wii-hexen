#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include <sdcard/wiisd_io.h>
#include <fat.h>

bool sd = false;
bool usb = false;

int wii_main()
{
    FILE * fp2;

    PAD_Init();

    // Init the wiimotes
    WPAD_Init();

    WPAD_SetIdleTimeout(60*5); // 5 minutes 

    WPAD_SetDataFormat(WPAD_CHAN_ALL, WPAD_FMT_BTNS_ACC_IR);

    WPAD_SetVRes(WPAD_CHAN_ALL, 320, 200);

    // Init the file system
    fatInitDefault();

    //Determine SD or USB
    fp2 = fopen("sd:/apps/wiihexen/psphexen.wad", "rb");

    if(fp2)
	sd = true;

    if(!fp2)
	fp2 = fopen("usb:/apps/wiihexen/psphexen.wad", "rb");

    if(fp2 && !sd)
	usb = true;

    return 0;
}
