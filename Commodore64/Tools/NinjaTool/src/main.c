//-----------------------------LICENSE NOTICE------------------------------------
//  _______  .___ _______       ____.  _____                             
//  \      \ |   |\      \     |    | /  _  \                            
//  /   |   \|   |/   |   \    |    |/  /_\  \                           
// /    |    |   /    |    /\__|    /    |    \                          
// \____|__  |___\____|__  \________\____|__  /                          
// ________\/   _____ ___\/_____ _______    \/____    ___________________
// \_   ___ \  /  _  \\______   \\      \   /  _  \  /  _____/\_   _____/
// /    \  \/ /  /_\  \|       _//   |   \ /  /_\  \/   \  ___ |    __)_ 
// \     \___/    |    |    |   /    |    /    |    \    \_\  \|        \
//  \______  \____|__  |____|_  \____|__  \____|__  /\______  /_______  /
//         \/        \/       \/        \/        \/        \/        \/ 
// By Christophe Kohler 
// January 2020
//
// Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
//
//------------------------------------------------------------------------------

// -- Important defines
#define ALLOWSKIPLEVEL // We can skip level in all cases
//#define ISSIDEB
// --------------------

// -- Debug
//#define NOINTRO
//#define STARTLEVELH '8'
//#define STARTLEVELL '1'
// ---------------------

//------------------------------------------------------------------------------

// Side B
#ifdef ISSIDEB
#define STARTLEVELH '6'
#define STARTLEVELL '1'
#endif

// Mapping final :
// 0x0150 - 0x080E Music
// 0x0810 - 0x08b8 Sfx
// 0x0900 - 0x9C00 Code + data gfx + Digit voice (should not go more than 0x9A00)
// 0x9A00 - 0x9C00 Read buffer (512 bytes)
// 0x9C00 - 0xBB2A Level (one at a time)
// 0XBC00 - 0XC000 Stack
// 0XC000 - 0xFFFF Screen 

//------------------------------------------------------------------------------

#include <cpctelera.h>
#include <types.h>
#include "sprites.h"
#include <stdio.h>
#include <string.h>
#include "sfx.h" // Audio sfx (resident before 0x4000)
#include "font.c.h"
#include "fontalt.c.h"
#include "digitvoices.h"

// -- Extern Functions for loading data
extern void loadlevel(void); // Load in 0x8000
extern u8 levelname[11]; // "LEVELXX BIN"
extern void loadmusic(void); // Load in 0x0150
extern u8 musicname[11]; // "NINJAXX BIN"
extern void loadtitle(void); // Load in 0xC000
extern void asm_FdcOn(void);  // Start Drive motor
extern void asm_FdcOff(void); // End Drive motor

extern void LECTURE_SON(u8* adresse_son , u16 longueur,u8 volume,u8 vitesse);

// ---------------------------------------------
// Prototypes
void InitGauge(u8 totaltime);

// ---------------------------------------------
struct sleveldata
{
	u8 position; // 1 2 3 (image 1 (left), middle 2 or right 3)
	u8 nbzones; // 1 to 9
};

// ---------------------------------------------
// -- Data zone is from 0x9C00 to 0xBC00 (need to take care of the stack also)
// The begining of the zone are the pointer to the data. All is absolute encoded
// The binary data is one block and read at once from 0x9C00. The pointer are automatically mapped and point to the data.
__at(0x9C00) struct sleveldata* gLevelData;
__at(0x9C02) u8* gNextLevelName;
__at(0x9C04) u8* gZonesData;
__at(0x9C06) u8* gScenario;
__at(0x9C08) u8* gMessageIntro;
__at(0x9C0A) u8* gMessageSucces[32];
__at(0x9C4A) u8* gMessageDeath[32];
__at(0x9C8A) u8* gTilesData;
// ---------------------------------------------

// Level 1 palette
const u8 palette[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x4D, 0x4E, 0x5C, 0x56, 0x5B, 0x44, 0x43, 0x47 };

const u8 palettedark[16] = { 0x54, 0x4B, 0x4C, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54 };

const u8 palettesmoke[16] =      { 0x54, 0x4B, 0x4C, 0x52, 0x40, 0x54, 0x40, 0x54, 0x40, 0x40, 0x40, 0x54, 0x40, 0x54, 0x40, 0x40 };
const u8 palettesmokeblood[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x4C, 0x54, 0x40, 0x54, 0x40, 0x40, 0x40, 0x54, 0x40, 0x54, 0x40, 0x40 };

const u8 palettebas[4] = { 0x54, 0x5E, 0x58, 0x4A }; // Black, Dark yellow, Dark purple, Yellow

#ifndef ISSIDEB
// Title A
const u8 palettetitle[16] = { 0x54,0x4c,0x4a,0x5c,0x4e,0x56,0x4b,0x57,0x44,0x43,0x40,0x05e,0x58,0x055,0x47,0x4f };
#else
// Title B
const u8 palettetitle[16] = { 0x54,0x5c,0x4c,0x4b,0x40,0x4e,0x44,0x53,0x47,0x4a,0x46,0x056,0x45,0x043,0x5e,0x5b };
#endif

// Anim fall
const u8 paletteFall1[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x54, 0x4E, 0x5C, 0x56, 0x5E, 0x44, 0x5E, 0x5E };
const u8 paletteFall2[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x5E, 0x4E, 0x5C, 0x56, 0x54, 0x44, 0x5E, 0x5E };
const u8 paletteFall3[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x5E, 0x4E, 0x5C, 0x56, 0x5E, 0x44, 0x54, 0x5E };
const u8 paletteFall4[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x5E, 0x4E, 0x5C, 0x56, 0x5E, 0x44, 0x5E, 0x54 };
// River
//                              0      1     2     3     4     5    *6*    7     8     9     10   11   *12*   *13*   14   15
const u8 paletteRiver1[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x44, 0x46, 0x4D, 0x4E, 0x5C, 0x56, 0x55, 0x5D, 0x43, 0x47 };
const u8 paletteRiver2[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x4D, 0x4E, 0x5C, 0x56, 0x44, 0x55, 0x43, 0x47 };
const u8 paletteRiver3[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x55, 0x46, 0x4D, 0x4E, 0x5C, 0x56, 0x5D, 0x44, 0x43, 0x47 };
// Diablo Blink                                                                                                    *14*
const u8 paletteDiablo[16] = { 0x54, 0x4B, 0x4C, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x4D, 0x4E, 0x5C, 0x56, 0x5B, 0x44, 0x4B, 0x47 };
// Fire                                    *2*                                        *9*                           *14*
const u8 paletteFire2[16] =  { 0x54, 0x4B, 0x43, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x4D, 0x4C, 0x5C, 0x56, 0x5B, 0x44, 0x4E, 0x47 };
const u8 paletteFire3[16] =  { 0x54, 0x4B, 0x4E, 0x52, 0x5E, 0x58, 0x5D, 0x46, 0x4D, 0x43, 0x5C, 0x56, 0x5B, 0x44, 0x4C, 0x47 };

#define gOffsetPicY 4 // In line (this is a constant)

// Offset to manage all 3 pictures, the same way
u8  gOffsetPicX; // In bytes
u8  gNbZones;
u8* gCurrentScenarioLine;  // Pointer to current scenario line
u8* gCurrentScenarioLine_Save;  // Pointer to current scenario line
u8* gSfxTextDeathSave; // Save pointer to text to display after SFX
u8  gSfxDeathCount; // Each xx death play sound
u8  gamestate; // 0=playing 1=dead & wait space
u8  QTEstate; // For managing the QTE sequence.
u8  QTEcurrentstep; // Step of the rythmic sequence.
u8  xold, yold;

u16 gGauge; // Current value of gauge. max is 128*256. 0 is not activated
u16 gGaugeSpeed; // Decrease speed (each frame)
u8  gGaugeEnded; // Switch to 1 when time is over.
u8  gGaudeInUse; // 0=not used, 1=used
u8  gIconLastDisplayedId; // Id of last icon displayed

u8 gPaletteMode; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall
u8 gPaletteNextSwap; // >0 in frame
u8 gNextPalette; // 0=Palette 1=PaletteAlt

u8 gCursorMode; // Special mode for cursor. 0=normal. 1=Slow and random (hit), 2=reversed (all) 3=reversed Up, 4=reversed down 5=always falling

u8 gNoNeedToEraseCursor; // For special case, backround is displayed, so no need to erase cursor.

u16 gNbDeath; // Stats on number of death
   
// Sprite size (in bytes)
#define SP_W   2
#define SP_H   8

// Screen size (in bytes)
#define SCR_W   80
#define SCR_H  200

u8 gTime; // Seconds
u8 gTimeFrag; // 10th of seconds
u8 gTimePrecise; // Frame precise (0 to 255)

/*
u8 scenario[9][10] =
{
	{ 50 ,101,102 ,102 ,102,1   ,102 , 0 , 0 , 0 },
	{ 50 ,101, 2  ,104 ,103 ,0  ,104 , 0 , 0 , 0 },
	{ 250,4  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 40 ,101, 0  ,3   ,103 ,0  ,104 , 0 , 0 , 0 },
	{ 250,6  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 251,25 , 1  , 1  , 2  ,2  , 0  , 0 , 0 , 0 },
	{ 0  ,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 40 ,101, 0  ,0   ,103 ,0  ,4   , 0 , 0 , 0 },
	{ 255,0  , 0  ,0   ,0   ,0  ,0   , 0 , 0 , 0 },
};
*/

//{251, 45, 5,  103 , 1  ,3  , 4  , 2  ,1 , 4}, //
//{ 3  ,2   , 1  , 4  , 255  ,0  , 0  , 0 , 0 , 0 },

u8 gQte_auto_mode; // 0 = off 1=on
u8 gQte_next; // Time before next QTE
u8 gScenario_qte_live[2][10]; // Random QTE for last level


#define MESSAGEMAXLENGHT (40*5+8)
#define NBMESFAILTIME 6
u8 gMessageFailTimeCurrent;
const u8 gMessageFailTime[NBMESFAILTIME][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 CHARS (LINE 3) 1
/"----------------------------------" 34 CHARS (LINE 2 & 4)
//"--------------------------------" 32 CHARS (LINE 1 & 5)
*/
  "EH \2PIGNOUF\1, C'EST QUOI QUE T'AS "
 "PAS COMPRIS QUAND JE T'AI DIT     "
"QU'IL FALLAIT ETRE RAPIDE SUR CE    "
 "COUP LA ? TU DORS OU QUOI ?       "
  "REVEILLES TOI LAMER !!!         "
},
{
/*
"------------------------------------" 36 CHARS (LINE 3) 2
/"----------------------------------" 34 CHARS (LINE 2 & 4)
//"--------------------------------" 32 CHARS (LINE 1 & 5)
*/
  "OH FACE DE PET, TU DORS ENCORE ?"
 "JE TE RAPPELLE QUE JE ME FAIS     "
"\2DEFONCER LE CUL\1 QUAND T'ES PAS ASSEZ"
 "RAPIDE. ALORS METS EN MARCHE TES  "
  "NEURONES ET PLUS VITE, BEUBEU ! "
},
{
/*
"------------------------------------" 36 CHARS (LINE 3) 3
/"----------------------------------" 34 CHARS (LINE 2 & 4)
//"--------------------------------" 32 CHARS (LINE 1 & 5)
*/
  "T'AS DE LA COMPOTE DANS LA TETE?"
 "TOUT CE QUE TU AS A FAIRE C'EST   "
"APPUYER SUR DES BOUTONS ET T'EN ES  "
 "MEME PAS CAPABLE !!! BOUGE-TOI    "
  "\2TRONCHE DE CAKE\1 !               "
},
{
/*
"------------------------------------" 36 CHARS (LINE 3) 4
/"----------------------------------" 34 CHARS (LINE 2 & 4)
//"--------------------------------" 32 CHARS (LINE 1 & 5)
*/
  "ENCORE RATE. MON POULPE A PLUS  "
 "DE VERTEBRES ET DE REFLEXES QUE   "
"TOI! J'ETAIS TRANQUILLE AVANT DE TE "
 "CONNAITRE, J'TE JURE! ALORS \2SORS\1  "
  "\2TOI LES DOIGTS DU CUL\1 MAINTENANT"
},
{
/*
"------------------------------------" 36 CHARS (LINE 3) 5
/"----------------------------------" 34 CHARS (LINE 2 & 4)
//"--------------------------------" 32 CHARS (LINE 1 & 5)
*/
  "T'ES SUREMENT QUELQU'UN DE TRES "
 "INTELLIGENT... MAIS J'AIMERAIS    "
"JUSTE SAVOIR QUAND !? VA FALLOIR    "
 "REAGIR PLUS VITE MON GARS HEIN. ON"
  "VA \2PAS Y ARRIVER\1 COMME CA!      "
},
{
/*
"------------------------------------" 36 CHARS (LINE 3) 6
/"----------------------------------" 34 CHARS (LINE 2 & 4)
//"--------------------------------" 32 CHARS (LINE 1 & 5)
*/
  "T'AS PAS SEULEMENT L'AIR D'UN   "
 "CON, TU EN AS TOUTE LA GAMME ! Y A"
"UN MOMENT OU TU VAS DECIDER A BIEN  "
 "JOUER OU CA TE FAIT MARRER QUE JE "
  "ME FASSE \2CRAMER LE CUL\1 ?        "
}

};

const u8 gMessageStats[MESSAGEMAXLENGHT] =
{
/*
"------------------------------------" 36 CHARS (LINE 3) 1
/"----------------------------------" 34 CHARS (LINE 2 & 4)
//"--------------------------------" 32 CHARS (LINE 1 & 5)
*/
  "VOICI LES STATS :               "
 "JE SUIS MORT 0000 FOIS !!!        "
"ET BEN LA VACHE !!!!                "
 "                                  "
  "                                "
};

u8 interrptcount;
u8 textzonereached=0;

// -------------------------------------------------------
#define NBMAXZONES 9
u8 zones[NBMAXZONES*5]; // start with no values

// -------------------------------------------------------
// Display Background with pattern
void DisplayBackground()
{
	u8* pmemscreen;
	u8 x;
	u8 y;
	for (y=0;y<9;y++)
	{	
		pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, 0, y*16);
		for (x=0;x<20;x++)
		{
			if( (x+y) % 2 )
				cpct_drawSprite(tile_BackgroundTile1, pmemscreen , 8/2, 16);
			else
				cpct_drawSprite(tile_BackgroundTile2, pmemscreen , 8/2, 16);
			pmemscreen += 4;
		}
	}
	// Erase last line (because interruption is happening here)
	pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, 0, (9*16)-4);
	for (x=0; x<80; x++)
		pmemscreen[x]=0;
	pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, 0, (9*16)-3);
	for (x=0; x<80; x++)
		pmemscreen[x]=0;
	pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, 0, (9*16)-2);
	for (x=0; x<80; x++)
		pmemscreen[x]=0;
}

// -------------------------------------------------------
// Display Background with pattern for only one picture
void DisplayBackgroundForOnePicture()
{
	u8* pmemscreen;
	u8 x;
	u8 y;
	for (y=0;y<9;y++)
	{	
		pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, gOffsetPicX, y*16); // Use gOffsetPicX as start here
		for (x=0;x<8;x++)
		{
			if( (x+y) % 2 )
				cpct_drawSprite(tile_BackgroundTile1, pmemscreen , 8/2, 16);
			else
				cpct_drawSprite(tile_BackgroundTile2, pmemscreen , 8/2, 16);
			pmemscreen += 4;
		}
	}
	// Erase last line (because interruption is happening here)
	pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, 0, (9*16)-4);
	for (x=0; x<80; x++)
		pmemscreen[x]=0;
	pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, 0, (9*16)-3);
	for (x=0; x<80; x++)
		pmemscreen[x]=0;
	pmemscreen = cpct_getScreenPtr(CPCT_VMEM_START, 0, (9*16)-2);
	for (x=0; x<80; x++)
		pmemscreen[x]=0;
}

// -------------------------------------------------------
// displaypic
// Redisplay all picture
void DisplayPic()
{
	u8 x1;
	u8 x2;
	u8 x3;
	u8* pmem;
	u8 y;
	int loop;
	x1 = 0+gOffsetPicX;
	x2 = (16/2)+gOffsetPicX;
	x3 = (32/2)+gOffsetPicX;
	pmem=gTilesData;
	y=0+gOffsetPicY;
	for (loop=0; loop<8; loop++)
	{
		cpct_drawSprite(pmem, cpctm_screenPtr(CPCT_VMEM_START, x1, y), 16/2, 16);
		pmem += 8*16;
		cpct_drawSprite(pmem, cpctm_screenPtr(CPCT_VMEM_START, x2, y), 16/2, 16);
		pmem += 8*16;
		cpct_drawSprite(pmem, cpctm_screenPtr(CPCT_VMEM_START, x3, y), 16/2, 16);
		pmem += 8*16;
		y += 16;
	}
}

// -------------------------------------------------------
// Redisplay only one tile, depending on index 0 to 24
void redisplaytilefromindex(u8 index)
{
	u8 x=0;
	u8* pmem;
	pmem=gTilesData;
	if (index%3==0) x=0+gOffsetPicX;
	else if (index%3==1) x=(16/2)+gOffsetPicX;
	else if (index%3==2) x=(32/2)+gOffsetPicX;
	
	cpct_drawSprite(pmem+(8*16*index), cpctm_screenPtr(CPCT_VMEM_START, x, 16*(index/3)+gOffsetPicY), 16/2, 16);
	
	/*
	if (index==0) cpct_drawSprite(pmem+(8*16*0), cpctm_screenPtr(CPCT_VMEM_START, x1, 0+gOffsetPicY), 16/2, 16);
	if (index==1) cpct_drawSprite(pmem+(8*16*1), cpctm_screenPtr(CPCT_VMEM_START, x2, 0+gOffsetPicY), 16/2, 16);
	if (index==2) cpct_drawSprite(pmem+(8*16*2), cpctm_screenPtr(CPCT_VMEM_START, x3, 0+gOffsetPicY), 16/2, 16);	
                                              
	if (index==3) cpct_drawSprite(pmem+(8*16*3), cpctm_screenPtr(CPCT_VMEM_START, x1, 16+gOffsetPicY), 16/2, 16);
	if (index==4) cpct_drawSprite(pmem+(8*16*4), cpctm_screenPtr(CPCT_VMEM_START, x2, 16+gOffsetPicY), 16/2, 16);
	if (index==5) cpct_drawSprite(pmem+(8*16*5), cpctm_screenPtr(CPCT_VMEM_START, x3, 16+gOffsetPicY), 16/2, 16); 
                                              
	if (index==6) cpct_drawSprite(pmem+(8*16*6), cpctm_screenPtr(CPCT_VMEM_START, x1, 16*2+gOffsetPicY), 16/2, 16);
	if (index==7) cpct_drawSprite(pmem+(8*16*7), cpctm_screenPtr(CPCT_VMEM_START, x2, 16*2+gOffsetPicY), 16/2, 16);
	if (index==8) cpct_drawSprite(pmem+(8*16*8), cpctm_screenPtr(CPCT_VMEM_START, x3, 16*2+gOffsetPicY), 16/2, 16); 		

	if (index==9) cpct_drawSprite(pmem+(8*16*9), cpctm_screenPtr(CPCT_VMEM_START,  x1, 16*3+gOffsetPicY), 16/2, 16);
	if (index==10) cpct_drawSprite(pmem+(8*16*10), cpctm_screenPtr(CPCT_VMEM_START, x2, 16*3+gOffsetPicY), 16/2, 16);
	if (index==11) cpct_drawSprite(pmem+(8*16*11), cpctm_screenPtr(CPCT_VMEM_START, x3, 16*3+gOffsetPicY), 16/2, 16); 	
                                                
	if (index==12) cpct_drawSprite(pmem+(8*16*12), cpctm_screenPtr(CPCT_VMEM_START, x1, 16*4+gOffsetPicY), 16/2, 16);
	if (index==13) cpct_drawSprite(pmem+(8*16*13), cpctm_screenPtr(CPCT_VMEM_START, x2, 16*4+gOffsetPicY), 16/2, 16);
	if (index==14) cpct_drawSprite(pmem+(8*16*14), cpctm_screenPtr(CPCT_VMEM_START, x3, 16*4+gOffsetPicY), 16/2, 16); 
                                                
	if (index==15) cpct_drawSprite(pmem+(8*16*15), cpctm_screenPtr(CPCT_VMEM_START, x1, 16*5+gOffsetPicY), 16/2, 16);
	if (index==16) cpct_drawSprite(pmem+(8*16*16), cpctm_screenPtr(CPCT_VMEM_START, x2, 16*5+gOffsetPicY), 16/2, 16);
	if (index==17) cpct_drawSprite(pmem+(8*16*17), cpctm_screenPtr(CPCT_VMEM_START, x3, 16*5+gOffsetPicY), 16/2, 16);  
                                                
	if (index==18) cpct_drawSprite(pmem+(8*16*18), cpctm_screenPtr(CPCT_VMEM_START, x1, 16*6+gOffsetPicY), 16/2, 16);
	if (index==19) cpct_drawSprite(pmem+(8*16*19), cpctm_screenPtr(CPCT_VMEM_START, x2, 16*6+gOffsetPicY), 16/2, 16);
	if (index==20) cpct_drawSprite(pmem+(8*16*20), cpctm_screenPtr(CPCT_VMEM_START, x3, 16*6+gOffsetPicY), 16/2, 16); 	
                                               
	if (index==21) cpct_drawSprite(pmem+(8*16*21), cpctm_screenPtr(CPCT_VMEM_START, x1, 16*7+gOffsetPicY), 16/2, 16);
	if (index==22) cpct_drawSprite(pmem+(8*16*22), cpctm_screenPtr(CPCT_VMEM_START, x2, 16*7+gOffsetPicY), 16/2, 16);
	if (index==23) cpct_drawSprite(pmem+(8*16*23), cpctm_screenPtr(CPCT_VMEM_START, x3, 16*7+gOffsetPicY), 16/2, 16);
*/ 	
}

// -------------------------------------------------------
// ptext should be null terminated
// color is 1 2 3 (white, GReen, Red)
// For multi color string use ck_drawStringM1Colored
void ck_drawStringM1(u8* ptext, u8* pvideomem)
{
	u16 offset;
	u8* spriteadress;
	while (ptext[0]!=0)
	{
		offset = ( ptext[0] - 32 ) * 16 ; // Offset of sprite ( ascii code - 32 , * size of one sprite (16 bytes) )
		spriteadress = (u8*) (tile_Fonte8x8_000) + offset;
		cpct_drawSprite(spriteadress, pvideomem, 8/4, 8);
		pvideomem+=2;
		ptext++;
	}
}

// -------------------------------------------------------
// ptext should be null terminated
// color is controlled with ascii caractere 1 and 2 (1=black 2=yellow)
u8 ck_drawStringM1withSize(u8* ptext, u8* pvideomem, u8 length)
{
	u16 offset;
	u8* spriteadress;
	u8 currentsize=0;
	u8* spritebaseaddress=tile_Fonte8x8_000; // Black default font
	u8 nbspecialbytes=0;
	while (currentsize!=length)
	{
		u8 cara = ptext[0];
		if (cara==1)
		{
			spritebaseaddress=tile_Fonte8x8_000;
			ptext++;
			nbspecialbytes++;
		}
		else if (cara==2)
		{
			spritebaseaddress=tile_Fonte8x8_ALT_000;
			ptext++;
			nbspecialbytes++;
		}
		else
		{			
			offset = ( ptext[0] - 32 ) * 16 ; // Offset of sprite ( ascii code - 32 , * size of one sprite (16 bytes) )
			spriteadress =  spritebaseaddress+ offset;
			cpct_drawSprite(spriteadress, pvideomem, 8/4, 8);
			pvideomem+=2;
			ptext++;
			currentsize++;
		}
	}
	return currentsize+nbspecialbytes;
}

// -------------------------------------------------------
// Find the tile impacted under cursor position.
// X to X +1
// Y to Y + 8
void erasecursor(u8 x, u8 y)
{
	// We compute tiles for x-1 x+1 y-4 y+4
	u8 index1; // index of tile 0 to 23
	u8 index2;
	u8 index3;
	u8 index4;
	
	u8 xmin;
	u8 xmax;
	u8 ymin;
	u8 ymax;
	
	xmax=x;
	ymax=y;

	xmin=x;
	ymin=y;	
	if (x<24-3) xmax=x+2;
	if (y<(128-9)) ymax=y+8;	
	
	index1 = (xmin/8)+(3*(ymin/16));
	index2 = (xmin/8)+(3*(ymax/16));
	index3 = (xmax/8)+(3*(ymin/16));
	index4 = (xmax/8)+(3*(ymax/16));	
	
	// Display values
	//cpct_drawStringM0("$ $", CPCT_VMEM_START, index1, index2);
	redisplaytilefromindex(index1);
	if ( index2 != index1 ) redisplaytilefromindex(index2);
	if ( (index3 != index2) && (index3 != index1) ) redisplaytilefromindex(index3);
	if ( (index4 != index3) && (index4 != index2) && (index4 != index1) ) redisplaytilefromindex(index4);
}

// -------------------------------------------------------
u8 GetPointedZone(u8 x, u8 y)
{
	u8 i;
	u8 total;
	total = 5*gNbZones;
	for (i=0; i<total; i+=5)
	{
		if ( zones[i+4] != 0 && x >= zones[i+0] && y >= zones[i+1] && x <= zones[i+2] && y <= zones[i+3] )
			return zones[i+4];
	}
	return 0;
}

// -------------------------------------------------------
// Desactivate one zone
void MarkZoneAsUsed(u8 zoneid)
{
	u8 i;
	u8 total;
	total = 5*gNbZones;
	for (i=0; i<total; i+=5)
	{
		if ( zones[i+4] == zoneid )
			zones[i+4]=0;
	}
}

// -------------------------------------------------------
// Reset all zones
void resetzones()
{
   u8 i;
   gNbZones=gLevelData->nbzones;
   // Copie zones to dynamic array
   for (i=0; i<5*gNbZones;i++)
	   zones[i]=gZonesData[i];
}

// -------------------------------------------------------
void myInterruptHandler() 
{
   // Set the color of the border differently for each interrupt
   if (interrptcount==0)
   {
	//cpct_setBorder(2);
	cpct_setVideoMode(0);
	
	if(gPaletteMode==0)
		cpct_setPalette(palette, 16); // set again 4 first colors
	else if(gPaletteMode==2)
		cpct_setPalette(palettesmoke, 16); // smoke	
	else if(gPaletteMode==3)
		cpct_setPalette(palettesmokeblood, 16); // smoke and blood		
	else if (gPaletteMode==1) // Blink
	{
		gPaletteNextSwap--;
		
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap=5 + (cpct_getRandom_lcg_u8(0) >> 4);
			gNextPalette = !gNextPalette;
		}
		if (gNextPalette==1)
			cpct_setPalette(palettedark, 16); 
		else
			cpct_setPalette(palette, 16); 
	}
	else if (gPaletteMode==4) // Fall
	{
		gPaletteNextSwap--;
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap=7;
			gNextPalette++;
			gNextPalette = gNextPalette % 4;
		}		
		if (gNextPalette==0)
			cpct_setPalette(paletteFall1, 16); 
		else if (gNextPalette==1)
			cpct_setPalette(paletteFall2, 16); 
		else if (gNextPalette==2)
			cpct_setPalette(paletteFall3, 16);
		else if (gNextPalette==3)
			cpct_setPalette(paletteFall4, 16); 
	} // 	
	else if (gPaletteMode==5) // River
	{
		gPaletteNextSwap--;
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap=7;
			gNextPalette++;
			gNextPalette = gNextPalette % 3;
		}		
		if (gNextPalette==0)
			cpct_setPalette(paletteRiver3, 16); 
		else if (gNextPalette==1)
			cpct_setPalette(paletteRiver1, 16); 
		else if (gNextPalette==2)
			cpct_setPalette(paletteRiver2, 16);
	} 
	else if (gPaletteMode==6) // Diablo
	{
		gPaletteNextSwap--;
		
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap= 20 + (cpct_getRandom_lcg_u8(0) >> 3);
			gNextPalette = !gNextPalette;
		}
		if (gNextPalette==1)
			cpct_setPalette(paletteDiablo, 16); 
		else
			cpct_setPalette(palette, 16); 
	}
	else if (gPaletteMode==7) // Fire
	{
		gPaletteNextSwap--;
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap=7;
			gNextPalette++;
			gNextPalette = gNextPalette % 3;
		}		
		if (gNextPalette==0)
			cpct_setPalette(palette, 16); 
		else if (gNextPalette==1)
			cpct_setPalette(paletteFire2, 16); 
		else if (gNextPalette==2)
			cpct_setPalette(paletteFire3, 16);
	} 
   }
	
   if (interrptcount==4) // Bottom of screen
   {
	//cpct_setBorder(1);
	cpct_setVideoMode(1);
	textzonereached=1; // This will unlock main loop
	cpct_setPalette(palettebas, 4);  
	cpct_akp_musicPlay();	
   }
   
   // Count one more interrupt. There are 6 interrupts in total (0-5)
   interrptcount++;
   if (interrptcount>5)
	   interrptcount=0;
}

// -------------------------------------------------------
/*
void myInterruptHandler2() 
{
   if (interrptcount==4) // Bottom of screen
   {
	cpct_akp_musicPlay();	
   }
   
   // Count one more interrupt. There are 6 interrupts in total (0-5)
   interrptcount++;
   if (interrptcount>5)
	   interrptcount=0;
}
*/

// -------------------------------------------------------
void setInterruptAtVSYNCStart(void *intHdl)
{
	//Asegurarnos que cambiamos la gestión de interrupciones es al terminar VSync
	//pues el primero puede saltar si estamos a mitad de VSync. Por eso se pasan 2 interrupciones más
	//y entra en espera. Ahora es seguro que saltará de la espera al llegar el CRTC al VSync
	cpct_waitVSYNC();
	__asm
        ei
		halt
        ei
		halt
	__endasm;
	cpct_waitVSYNC();//Nos aseguramos que CRTC estaba a mitad de pantalla
	cpct_setInterruptHandler(intHdl);
}

// -------------------------------------------------------
void DisplayBanner()
{
		// -- Display Banner
		u8* pvideomem;    
#define BANNERSTARY (128+8+8)
		// Draw banner Background. Function cpct_drawSolidBox have max width 64
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 4,BANNERSTARY ); // Corner top left
		cpct_drawSolidBox (pvideomem,cpct_px2byteM1 (1, 1, 1 , 1), 64, 7*8); // 7 lines. Not 2 and last colomns
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 2+64,BANNERSTARY ); // Corner top left
		cpct_drawSolidBox (pvideomem,cpct_px2byteM1 (1, 1, 1 , 1), 80-8-64+2, 7*8); // 7 lines. Not 2 and last colomns		
		// Draw corners and center
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 0,BANNERSTARY ); // Corner top left
		cpct_drawSprite(BannerCorner, pvideomem , 32/4, 24);
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 0,BANNERSTARY+24 ); //Center left
		cpct_drawSprite(BannerCenter, pvideomem , 16/4, 8);	
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 0,BANNERSTARY+24+8+24-1 ); // Corner bottom left
		cpct_drawSpriteVFlip_f(BannerCorner, pvideomem , 32/4, 24);		
		//pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, (32/4),BANNERSTARY ); // Top
		//cpct_drawSprite(BannerTop, pvideomem , 256/4, 2);	
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 80-(32/4),BANNERSTARY ); // Corner top right
		cpct_drawSpriteHFlipM1(BannerCorner, pvideomem , 32/4, 24);
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 80-(16/4),BANNERSTARY+24 ); //Center right
		cpct_drawSpriteHFlipM1(BannerCenter, pvideomem , 16/4, 8);	
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 80-(32/4),BANNERSTARY+24+8+24-1 ); // Corner Bottom right
		cpct_hflipSpriteM1(32/4, 24,BannerCorner);
		cpct_drawSpriteVFlip_f(BannerCorner, pvideomem , 32/4, 24);
		cpct_hflipSpriteM1(32/4, 24,BannerCorner);
		
		// Display details on top and bottom
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 59,BANNERSTARY ); // Corner top left
		cpct_drawSprite(BannerBorderDetailTop, pvideomem , 8/4, 2);
		pvideomem-=10;
		cpct_drawSprite(BannerBorderDetail1, pvideomem , 16/4, 1);
		pvideomem-=10;
		cpct_drawSprite(BannerBorderDetail2, pvideomem , 16/4, 1);
		pvideomem-=10;
		cpct_drawSprite(BannerBorderDetail3, pvideomem , 16/4, 1);
		// Bottom
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 20, BANNERSTARY + 7*8 - 1 ); // Corner top left
		cpct_drawSprite(BannerBorderDetail1, pvideomem , 16/4, 1);
		pvideomem+=10;
		cpct_drawSprite(BannerBorderDetail2, pvideomem , 16/4, 1);
		pvideomem+=10;
		cpct_drawSprite(BannerBorderDetail3, pvideomem , 16/4, 1);		
		pvideomem+=10;
		cpct_drawSprite(BannerBorderDetail1, pvideomem , 16/4, 1);		
}

#define TEXTLINE1Y (BANNERSTARY+4)
#define TEXTLINE2Y (BANNERSTARY+4+10)
#define TEXTLINE3Y (BANNERSTARY+4+10+10)
#define TEXTLINE4Y (BANNERSTARY+4+10+10+10)
#define TEXTLINE5Y (BANNERSTARY+4+10+10+10+10)

// -------------------------------------------------------
// 
void displayline2(u8* ptext)
{
	u8* pvideomem; // Pointer to video memory	
	u8 nbbytesprocessed;
   
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 4*2, TEXTLINE1Y);
	nbbytesprocessed = ck_drawStringM1withSize(ptext, pvideomem, 40-8); 
	ptext += nbbytesprocessed;
   
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 3*2, TEXTLINE2Y);
	nbbytesprocessed = ck_drawStringM1withSize(ptext, pvideomem, 40-6); 
	ptext += nbbytesprocessed;
	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 2*2, TEXTLINE3Y);
	nbbytesprocessed = ck_drawStringM1withSize(ptext, pvideomem, 40-4); 
	ptext += nbbytesprocessed;	

	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 3*2, TEXTLINE4Y);
	nbbytesprocessed = ck_drawStringM1withSize(ptext, pvideomem, 40-6); 
	ptext += nbbytesprocessed;	

	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 4*2, TEXTLINE5Y);
	ck_drawStringM1withSize(ptext, pvideomem, 40-8); 	
}

// -------------------------------------------------------
void DisplayDeadStats()
{
	// Display variable gNbDeath
	// Can go up to 9999
	u8* pvideomem; // Pointer to video memory
	u8 string[5];
	displayline2(gMessageStats);
	string[0]= ( (gNbDeath/ 1000 ) % 10) + '0';
	string[1]= ( (gNbDeath/ 100  ) % 10) + '0';
	string[2]= ( (gNbDeath/ 10   ) % 10) + '0';
	string[3]= ( (gNbDeath       ) % 10) + '0';
	string[4]=0;
	// Ligne 2 character 13
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, (3+13)*2, TEXTLINE2Y);
	ck_drawStringM1(string, pvideomem); 	
}

// -------------------------------------------------------
void EraseLastIcon() // 0=center 1=right, 2=up, 3=left 4=down
{
	u8 index1=10; // Default center
	u8 index2=13;
	if (gIconLastDisplayedId==2) // Up
	{
		index1=4;
		index2=7;
	}
	else if (gIconLastDisplayedId==1) // Right
	{
		index1=11;
		index2=14;
	}
	else if (gIconLastDisplayedId==3) // Left
	{
		index1=9;
		index2=12;
	}	
	else if (gIconLastDisplayedId==4) // Down
	{
		index1=16;
		index2=19;
	}	
	redisplaytilefromindex(index1);
	redisplaytilefromindex(index2);
}

// -------------------------------------------------------
void DisplayCentralIconDeath()
{
	u8* pvideomem;    // Pointer to video memory	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 8+gOffsetPicX, 48+gOffsetPicY);
	cpct_drawSprite(tile_IconDeath, pvideomem, 8, 32);	
	gIconLastDisplayedId=0;
}
// -------------------------------------------------------
void DisplayCentralIconSucess()
{
	u8* pvideomem;    // Pointer to video memory	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 8+gOffsetPicX, 48+gOffsetPicY);
	cpct_drawSprite(tile_IconSucess, pvideomem, 8, 32);	
	gIconLastDisplayedId=0;
}
// -------------------------------------------------------
void DisplayCentralIconQTEStart()
{
	u8* pvideomem;    // Pointer to video memory	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 8+gOffsetPicX, 48+gOffsetPicY);
	cpct_drawSprite(tile_IconQTEAction, pvideomem, 8, 32);	
	gIconLastDisplayedId=0;
}
// -------------------------------------------------------
void DisplayCentralIconQTELeft()
{
	u8* pvideomem;    // Pointer to video memory	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 0+gOffsetPicX, 48+gOffsetPicY);
	cpct_drawSprite(IconQTE3, pvideomem, 8, 32);
	gIconLastDisplayedId=3;	
}
// -------------------------------------------------------
void DisplayCentralIconQTERight()
{
	u8* pvideomem;    // Pointer to video memory	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 16+gOffsetPicX, 48+gOffsetPicY);
	cpct_drawSprite(IconQTE1, pvideomem, 8, 32);
	gIconLastDisplayedId=1;	
}
// -------------------------------------------------------
void DisplayCentralIconQTEUp()
{
	u8* pvideomem;    // Pointer to video memory	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 8+gOffsetPicX, (48-32)+gOffsetPicY);
	cpct_drawSprite(IconQTE2, pvideomem, 8, 32);	
	gIconLastDisplayedId=2;
}
// -------------------------------------------------------
void DisplayCentralIconQTEDown()
{
	u8* pvideomem;    // Pointer to video memory	
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 8+gOffsetPicX, (48+32)+gOffsetPicY);
	cpct_drawSprite(IconQTE4, pvideomem, 8, 32);	
	gIconLastDisplayedId=4;
}
// -------------------------------------------------------
// 1=up, 2=down, 3=right, 4=left
void DisplayQTEIcon(u8 QTEid)
{
	if (QTEid==1)
			DisplayCentralIconQTEUp();
	else if (QTEid==2)
			DisplayCentralIconQTEDown();
	else if (QTEid==3)
			DisplayCentralIconQTERight();	
	else if (QTEid==4)
			DisplayCentralIconQTELeft();			
}

// -------------------------------------------------------
void SetOffsetImage(u8 imageid)
{
	
	if (imageid==1)
	{
		gOffsetPicX=2; // 2 , 28 , 54 ... In bytes, for image 1 2 3.
	}
	else if (imageid==2)
	{
		gOffsetPicX=28;

	}	
	else if (imageid==3)
	{
		gOffsetPicX=54;
	}
}
// -------------------------------------------------------
// Update time
void UpdateTime()
{
	// Count time	
	gTimeFrag++;
	gTimePrecise++;

	if (gTimeFrag==50)
	{
		//u8 text[32]="TIME : x\0";
		//u8* pvideomem; 
		gTime++;
		gTimeFrag=0;
		//text[7]=gTime+'0';
		//pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 2, TEXTLINE1Y);
		//ck_drawStringM1withSize(text, pvideomem, 8); 	
	}	
}

// -------------------------------------------------------
// Init variable gamestate // 0=Playing (cursor) 1=State=dead 2=Display text 3=QTE 4=END LEVEL (go to next level)
// 249 Change QTE mode 0 1
// 250 Display text
// 251 QTE
// 252 Change cursor mode. 0 to 5
// 253 Change palette mode. 0 1 2 3 4 
// 254 Display Background
// 255 End
void InitNextAction()
{
	u8 nextaction = gCurrentScenarioLine[0]; // Time or next action
	
	// -- Test special function for changing flags. They graphically do not do nothing.
	// We skip these lines as they do not require main loop to run.
	while ( nextaction == 249 || ( nextaction >= 252 && nextaction <= 254 ) )
	{
		if ( nextaction == 252 ) // Change cursor mode 0 to 5
		{
			gCursorMode=gCurrentScenarioLine[1];
			gCurrentScenarioLine+=10;
			nextaction = gCurrentScenarioLine[0];
		}
		else if ( nextaction == 253 ) // Change Palette mode 0 1 2 3 4 
		{
			gPaletteMode=gCurrentScenarioLine[1]; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall
			gPaletteNextSwap=10; // >0 in frame
			gNextPalette=1; // 0=Palette 1=PaletteAlt		
			gCurrentScenarioLine+=10;
			nextaction = gCurrentScenarioLine[0];
		}
		else if ( nextaction == 249 ) // Change QTE mode 
		{
			gQte_auto_mode=gCurrentScenarioLine[1]; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall
			gQte_next = 100 + ( cpct_getRandom_lcg_u8(0) >> 3 ); // 100 + 64
			gCurrentScenarioLine+=10;
			nextaction = gCurrentScenarioLine[0];
		}	
		else if ( nextaction == 254 ) // end, display background
		{
			DisplayBackgroundForOnePicture();
			gNoNeedToEraseCursor=1;
			nextaction = 255; // Force end
		}		
	}
	
	// -- Do we have a game command ?
	if ( nextaction < 240 ) // Command
	{
		gamestate=0; // Default is using cursor		
		InitGauge(nextaction); // Playing. Nextaction is time
		return;
	}

	// -- Do I need to erase cursor ?	
	if (gNoNeedToEraseCursor==0)
		erasecursor(xold,yold); // Erase cursor, we do not need it	
	
	// -- Process commands
	if (  nextaction == 250 ) // Text
	{
		gamestate = 2; // Text
	}
	else if ( nextaction == 251 ) // QTE
	{
		gamestate = 3;
		QTEstate=1; // Start the QTE sequence (here from beginning, with two Warning icon)
		if ( gCurrentScenarioLine[1]<50) // If QTE time is quick, then only display one warning icon.
			QTEstate=3;
		gTimePrecise=0; // Reset precise time
		DisplayCentralIconQTEStart(); // Icone for saying it is starting (to be done after)		
	}
	else if ( nextaction == 248 ) // Display stats
	{
		gamestate=5; // Display stats.
	}	
	else if ( nextaction == 255 ) // END
	{
		gamestate = 4;	
		DisplayCentralIconSucess();
	}

}

// -------------------------------------------------------
// Init Gauge
void InitGauge(u8 totaltime)
{
		//totaltime=30; // Debug
		if (totaltime==0 || totaltime > 128)
		{
			gGaudeInUse=0;
			return;
		}
		// Init Gauge.
		// Start value is u8, in 10th of second. Means 10=1 seconds. 200 = 20 secondes.
		// Gauge is 128 is 128 pixels. Stored in u16 with 8 bits for seconds and 8 bits for fragments of seconds.
		// Gauge is updated 50 time a frame. Means one update is 0.02s.
		// Total time in s is time/10. Divided by 0.02 we get number of step (so same as x50).
		// 128*256/nbsteps = speed. 
		// Speed = 6553 / time. (approximated to have integer)
		gGaugeSpeed = 6553 / totaltime;
		gGauge = 128 * 256; // Gauge start full.
		gGaugeEnded=0;
		gGaudeInUse=1;
}

// -------------------------------------------------------
void EraseAndStopGauge() // Display black gauge
{
	u8* pvideomem;
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, gOffsetPicX+24,gOffsetPicY); 
	cpct_drawSolidBox(pvideomem,cpct_px2byteM0(0, 0), 1, 128);
	gGaudeInUse=0;
}

// -------------------------------------------------------
// Gauge
void DisplayGauge()
{
	u8* pvideomem;
	u8 switchpoint; // 128 to 8. Top part is black (128 down to switchpoint. Bottom part color depend of value. Green, yellow, red).
	u8 color;
	
	if (gGaugeEnded==1)
		return;
	
	if (gGauge>=gGaugeSpeed)
	{
		gGauge -= gGaugeSpeed;
	}
	else
	{
		// End if timer.
		gGaugeEnded=1;
	}

	switchpoint=(gGauge>>8);
	
	// -- Select display color of Gauge
	if (switchpoint>64)
		color = 6; // Color "ok"
	else if (switchpoint>32)
		color = 9; // Color "beware"
	else
		color = 2; // Very low color URGENT
	
	if (switchpoint<128)
	{
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, gOffsetPicX+24,gOffsetPicY); 
		cpct_drawSolidBox(pvideomem,cpct_px2byteM0(0, 0), 1, 128-switchpoint);
	}
	if (switchpoint>0)
	{
		pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, gOffsetPicX+24,gOffsetPicY+(128-switchpoint) ); 
		cpct_drawSolidBox(pvideomem,cpct_px2byteM0(color, color), 1, switchpoint);		
	}
	
}

// -------------------------------------------------------
void EraseBanner()
{
	u8* pvideomem;    // Pointer to video memory
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 0,BANNERSTARY ); // Corner top left
	cpct_drawSolidBox (pvideomem,cpct_px2byteM1 (0, 0, 0 , 0), 64, 7*8); // 7 lines. Not 2 and last colomns
	pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, 64,BANNERSTARY ); // Corner top left
	cpct_drawSolidBox (pvideomem,cpct_px2byteM1 (0, 0, 0 , 0), 80-64, 7*8); // 7 lines. Not 2 and last colomns	
}

// -------------------------------------------------------			
void PlayRandomSound()
{
	u8 xrand;
	u8 speed;
	speed = 3 + ( cpct_getRandom_lcg_u8(0) >> 6 ); // Speed 3 to 7
	xrand = ( cpct_getRandom_lcg_u8(0) % 6 ); // 0 1 2 3 4 5
	if (xrand==0)
		LECTURE_SON(SHOUT1_VOX,1456,15,speed);
	else if (xrand==1)
		LECTURE_SON(SHOUT2_VOX,6240,15,3); // This one should never been too slow
	else if (xrand==2)	
		LECTURE_SON(SHOUT3_VOX,1216,15,speed);
	else if (xrand==3)	
		LECTURE_SON(SHOUT4_VOX,1856,15,speed);
	else if (xrand==4)	
		LECTURE_SON(SHOUT5_VOX,1696,15,speed);
	else
		LECTURE_SON(SHOUT8_VOX,2768,15,speed);
}

// -------------------------------------------------------
//
// MAIN LOOP
//
// -------------------------------------------------------
void main(void) 
{
	u8  x, y;   // Sprite coordinates
	u8* pvideomem;    // Pointer to video memory
	u8 pointedzone=0; // 0 means none
	u8 firepressed;
	u8 leftpressed;
	u8 rightpressed;
	u8 uppressed;
	u8 downpressed;	
	u8 firepressedlast=0; // To be able to check triggering "just pressed"
	u8 leftpressedlast=0; // To be able to check triggering "just pressed"
	u8 rightpressedlast=0; // To be able to check triggering "just pressed"
	u8 uppressedlast=0; // To be able to check triggering "just pressed"
	u8 downpressedlast=0; // To be able to check triggering "just pressed"	
	u8 shouldloopingame=1; // Main loop. False to allow level change.
	
	gSfxDeathCount=0;
	
	gNbDeath=0; // Reset total number of death
	gMessageFailTimeCurrent=0;
	gGauge=(128<<8);
	gGaugeSpeed=(1<<8);
	gTime=0; // Seconds
	gTimeFrag=0; // 50th of seconds
	gTimePrecise=0; // Frame precise (0 to 255) 

	cpct_disableFirmware(); // Disable Firmware, we will never use it.

	cpct_setVideoMode(0);
	cpct_setBorder(HW_BLACK); // Black	
	
#ifndef NOINTRO	
	cpct_clearScreen(0);
	// -- Load Title Screen
	asm_FdcOn();
	cpct_setPalette(palettetitle, 16); 
	loadtitle(); // Load title screen in 0xc000
	// Load music 00
	musicname[5]='0';
	musicname[6]='T';	
	loadmusic(); // Load a music
	asm_FdcOff();
	cpct_akp_musicInit(0x0150);    // Initialize the music
	// Wait Space
	firepressed=0;
	while(firepressed==0)
	{
		cpct_akp_musicPlay();	
		cpct_scanKeyboard_f();	 
		if (cpct_isKeyPressed(Key_Space) || cpct_isKeyPressed(Joy1_Fire1) || cpct_isKeyPressed(Joy0_Fire1) )
			firepressed=1;
		cpct_waitVSYNC();
		//LECTURE_SON(SHOUT1_VOX,1456,15,10);
	}
	cpct_akp_stop();
#endif
	
	// -- Load game
	cpct_clearScreen(0);	
	cpct_setPalette(palette, 16);  

	// Display All background
	DisplayBackground();  
	
	levelname[5]='0';
	levelname[6]='1';
	
	musicname[5]='0';
	musicname[6]='0';	

	// Debug. Start specific level
#ifdef STARTLEVELH
	levelname[5]=STARTLEVELH;
	levelname[6]=STARTLEVELL;
	musicname[6]=STARTLEVELH;
#endif	
 
	while(1)  // General loop, including level load and change.
	{
		u8 loop;
		
 		// -- Load level
		asm_FdcOn(); // Start motor
		loadlevel(); // Load a full level
		// -- Load new music 
		if (gLevelData->position==1) // Load music when loading first image 
			loadmusic(); // Load a music
		asm_FdcOff(); // Stop motor

		// -- Init Level
		x=8;
		y=8;
		xold=8;
		yold=8;
		gCursorMode=0; // 0 = Default mode
		gCurrentScenarioLine = gScenario;  
		firepressedlast=0;
		SetOffsetImage(gLevelData->position); // Init offset of level (1=Left 2=center 3=right)
		if (gLevelData->position==1) // If first image, then redisplay background
			DisplayBackground();  
		DisplayPic();
		resetzones();
		
		// Set Default palette effect = No effect
		gPaletteMode=0; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall(82) 5=river(82) 6=Diablo(81) 7=fire
		gPaletteNextSwap=10; // >0 in frame
		gNextPalette=1; // 0=Palette 1=PaletteAlt
		gCursorMode=0;	
		gQte_auto_mode=0;		
		
		InitNextAction(); // Init variable gamestate // 0=Playing (cursor) 1=State=dead 2=Display text 3=QTE  
	
		// Initialize the song to be played
		if (gLevelData->position==1)
		{
			cpct_akp_musicInit(0x0150);    // Initialize the music
			cpct_akp_SFXInit(g_sfx);      // Initialize instruments to be used for SFX (Same as music song) 
		}
 		
		interrptcount=0;
		setInterruptAtVSYNCStart(myInterruptHandler);

		shouldloopingame = 1; // We want to loop for game sequence
		gNoNeedToEraseCursor=0;
		
		// Banner (After interrupts are set, so that colors will be ok and no glitch)
		DisplayBanner();
		// -- Intro text
		displayline2(gMessageIntro);		
		
		while(shouldloopingame) // This is false when we change level
		{
			u8 flipflop; // Used for animation
			
			// Scan Keyboard (fastest routine) 
			cpct_scanKeyboard_f();	 

			// Manage Time
			UpdateTime();
			
			firepressed=0;
			if (cpct_isKeyPressed(Key_Space) || cpct_isKeyPressed(Joy1_Fire1) || cpct_isKeyPressed(Joy0_Fire1)) 
				firepressed=1;

#ifdef ALLOWSKIPLEVEL
			// Debug. Skip level
			if (cpct_isKeyPressed(Key_N)) 
			{
				while ( gCurrentScenarioLine[0] != 255 )
				{
					if (gCurrentScenarioLine[0]==251) // QTE, skip 10 more bytes
					 gCurrentScenarioLine+=10;
					gCurrentScenarioLine+=10;
					InitNextAction();
				}
				gamestate=4;
				firepressed=1;
				firepressedlast=0;
			}
#endif
			   
			if (gamestate==0) // Playing
			{
				u8 iskeyup,iskeydown,iskeyleft,iskeyright;
				// If cursor have moved, then erase the tiles where he was (4 tiles)
				erasecursor(xold,yold); // Coords in bytes
				// Check if user has pressed a Cursor Key and, if so, move the sprite if
				// it will still be inside screen boundaries
				iskeyup = cpct_isKeyPressed(Key_CursorUp) || cpct_isKeyPressed(Joy1_Up) || cpct_isKeyPressed(Joy0_Up) ;
				iskeydown = cpct_isKeyPressed(Key_CursorDown) || cpct_isKeyPressed(Joy1_Down) || cpct_isKeyPressed(Joy0_Down);
				iskeyleft=cpct_isKeyPressed(Key_CursorLeft) || cpct_isKeyPressed(Joy1_Left) || cpct_isKeyPressed(Joy0_Left);
				iskeyright=cpct_isKeyPressed(Key_CursorRight) || cpct_isKeyPressed(Joy1_Right) || cpct_isKeyPressed(Joy0_Right);
				// Special mode for cursor. 0=normal. 1=Slow and random (hit), 
				// 2=reversed (all) 3=reversed Up, 4=reversed horizontal 5=always falling
				if (gCursorMode==0 || gCursorMode>=8) // Normal mode & "do not show zone" mode & 9 show only sometimes
				{
					if      ( iskeyright ) ++x; 
					else if ( iskeyleft ) --x; 
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
				}
				else if (gCursorMode==1) // 1=Slow and random (hit)
				{
					u8 xrand,yrand;
					xrand = cpct_getRandom_lcg_u8(0) >> 6 ; // 0 1 2 3
					if ( xrand < 2 ) x += xrand;
					else x -= (xrand-2);
					yrand = cpct_getRandom_lcg_u8(0) >> 6 ; // 0 1 2 3
					if ( yrand < 2 ) y += xrand;
					else y -= (yrand-2);
					if      ( iskeyright ) ++x; 
					else if ( iskeyleft ) --x; 
					if      ( iskeyup  ) y-=2;
					else if ( iskeydown ) y+=2;
				}
				else if (gCursorMode==2)  // 2=reversed (all)
				{
					if      ( iskeyright ) --x; 
					else if ( iskeyleft ) ++x; 
					if      ( iskeyup  ) y+=4;
					else if ( iskeydown ) y-=4;
				}
				else if (gCursorMode==3) // 3=reversed Up
				{
					if      ( iskeyright ) ++x; 
					else if ( iskeyleft ) --x; 
					if      ( iskeyup  ) y+=4;
					else if ( iskeydown ) y-=4;
				}
				else if (gCursorMode==4) // 4=reversed horizontal
				{
					if      ( iskeyright ) --x; 
					else if ( iskeyleft ) ++x; 
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
				}
				else if (gCursorMode==5) // 5=always falling
				{
					if      ( iskeyright ) ++x; 
					else if ( iskeyleft ) --x; 
					y+=2;
					if (y==(128 - SP_H) )
						y=0;
				}
				else if (gCursorMode==6) // 6=always going right
				{
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
					if (flipflop==0) { x++; };
					flipflop++;
					flipflop=flipflop%4;
					if ( x >= (24-SP_W) ) x = 0;
				}	
				else if (gCursorMode==7) // 7=always going left
				{
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
					if (flipflop==0) { x--;  };
					flipflop ++;
					flipflop=flipflop%4;
					if (x > 200 ) x=(24-SP_W); // Negative
				}	
				// gCursorMode == 8 // Do not show color
				// gCursorMode == 9 // show only sometimes
				
				// Limit Cursor
				if (x > 200 ) x=0; // Negative
				else if ( x >= (24-SP_W) ) x = (24-SP_W);
				if (y > 200 ) y=0; // Negative
				else if ( y >= (128 - SP_H) ) y = (128 - SP_H);	
				// Save position and get pointed zone
				xold=x;
				yold=y;						
				pointedzone = GetPointedZone(x/2,y/8);
				// Get video memory byte for coordinates x, y of the sprite (in bytes)
				pvideomem = cpct_getScreenPtr(CPCT_VMEM_START, x+gOffsetPicX, y+gOffsetPicY);
				// Draw the sprite in the video memory location got from coordinates x, y
				
				if (gCursorMode!=9 || ( ( cpct_getRandom_lcg_u8(0) >> 6 ) == 1 ) )
				{
					if ( pointedzone == 0 && (gCursorMode!=8) ) // Cursor mode 8 , all is green
						cpct_drawSprite(tile_pointerred, pvideomem, SP_W, SP_H);
					else
						cpct_drawSprite(tile_pointergreen, pvideomem, SP_W, SP_H); 
				}
				// Do action key is pressed ?
				if (firepressed!=0 && firepressedlast==0 && pointedzone > 0) // Zone start at 1
				{
					u8 actiontodo=0;
					actiontodo = gCurrentScenarioLine[pointedzone]; // 0 is time
					
					// Check diablo mode
					if (gPaletteMode==6) // Diablo
					{
						if (actiontodo<100 && gNextPalette == 0 )
						{
							actiontodo = 101; // Force death if diablo is displayed
						}
					}
			
					if (actiontodo<100) // Win! // 1 to 100 are sucess text id
					{
						u8 loopzone;
						u8 indexoftext = (actiontodo-1);
						displayline2(gMessageSucces[indexoftext]);
						// Disable all possible choices on the scenario line. Means values >0 and < 99.
						// This allow multiple choices
						for (loopzone=1; loopzone<10; loopzone++)
						{
							u8 value = gCurrentScenarioLine[loopzone];
							if (value>0 && value<100)
								MarkZoneAsUsed(loopzone);
						}
						if ( gGaudeInUse == 1 )
							EraseAndStopGauge();						
						gCurrentScenarioLine+=10;
						InitNextAction();
						cpct_akp_SFXPlay(1,15, 100, 0, 0, 4); // Sound GOOD
					}
					else if (actiontodo < 200) // 101 to 200 are deaths
					{
						u8 message = actiontodo-100; // first message is 101
						// Death
						u8 indexoftext = (message-1);
						gSfxDeathCount++; // Increase nb deaths for SFX play
#define SFXDEATHNUMBERBEFOREPLAYSOUND 4 // Each 4 death a digitalised voice is played
						if (gSfxDeathCount<SFXDEATHNUMBERBEFOREPLAYSOUND)
							displayline2(gMessageDeath[indexoftext]);
						else
							gSfxTextDeathSave=gMessageDeath[indexoftext];
						// Erase gauge is it was used
						if ( gGaudeInUse == 1)
						{
							EraseAndStopGauge();	
						}							
						// Go to state = 1 (cursor will not be displayed again)
						if (gSfxDeathCount<SFXDEATHNUMBERBEFOREPLAYSOUND)
							gamestate=1;
						else
						{
							gamestate=6;
							gSfxDeathCount=0;
						}
						// Display dead cursor
						erasecursor(xold,yold); // Coords in bytes
						//gPaletteMode=0; // Reset death
						gCursorMode=0;
						DisplayCentralIconDeath();	
						cpct_akp_SFXPlay(2,15, 50, 0, 0, 4); //Sound BAD
						gNbDeath++; // stats
					}
				} // Action Key pressed
				// Manage Gauge
				if ( gGaudeInUse == 1 )
				{
					DisplayGauge(); 
					if (gGaugeEnded==1) // Death because time have ended
					{
						// Death Message
						displayline2(gMessageFailTime[gMessageFailTimeCurrent]);
						gMessageFailTimeCurrent++;
						if (gMessageFailTimeCurrent==NBMESFAILTIME)
							gMessageFailTimeCurrent=0;
						// Go to state = 1 (cursor will not be displayed again)
						gamestate=1;
						// Display dead cursor
						erasecursor(xold,yold); // Coords in bytes
						gPaletteMode=0; // Reset death
						gCursorMode=0;
						DisplayCentralIconDeath();	
						cpct_akp_SFXPlay(2,15, 50, 0, 0, 4); //Sound BAD
						gNbDeath++; // stats
					}
				}
				// QTE auto mode
				if (gQte_auto_mode==1)
				{
					gQte_next--;
					if (gQte_next==0)
					{
						u8* pScript;
						u8 k;
						// start random QTE
						// Save Pointer
						gCurrentScenarioLine_Save=gCurrentScenarioLine;
						// Init false cookie
						gCurrentScenarioLine=(u8*)gScenario_qte_live;
						pScript=gCurrentScenarioLine;
						pScript[0]=251;
						pScript[1]=25; // Short time.
						pScript[2]=1;
						pScript[3]=101;
						// -- 
						pScript[4]=1 + (cpct_getRandom_lcg_u8(0) & 0x03 );
						pScript[5]=1 + (cpct_getRandom_lcg_u8(0) & 0x03 );
						pScript[6]=1 + (cpct_getRandom_lcg_u8(0) & 0x03 );
						pScript[7]=1 + (cpct_getRandom_lcg_u8(0) & 0x03 );
						pScript[8]=255;
						
						// Strangly that code do not work.
						//pScript+=4;
						//for (k=4; k<4+4;k++)
						//	pScript[k]= 1; // + (cpct_getRandom_lcg_u8(0) % 3);
						//pScript[4+4]=255;
						
						InitNextAction();
					}
				}
				
			
				
			} // -- END of gamestate playing
			else if (gamestate==1) // -- Game state DEAD, waiting key to restart
			{
				if (firepressed!=0 && firepressedlast==0) // Restart when SPACE is pressed
				{
					DisplayPic();				
					// -- Intro text
					displayline2(gMessageIntro);	
					gamestate=0;	
					gCurrentScenarioLine=gScenario;
					resetzones();
					InitNextAction();				
				}
			}
			else if (gamestate==2) // -- DISPLAY TEXT
			{
				if (firepressed!=0 && firepressedlast==0) // Display text when SPACE is pressed
				{
					// Display text
					int texid = gCurrentScenarioLine[1];
					displayline2(gMessageSucces[texid-1]);
					gCurrentScenarioLine+=10;
					InitNextAction();
				}
			}
			else if (gamestate==3) // -- QTE ----------------------------------------------------
			{
				// gCurrentScenarioLine[] .. 20 values
				// QTEID(251) / (1) Time / (2) Text if win / (3) Text if lost / (4) sequence / 255
				// Start with QTEstate=1; // Start the QTE sequence
				// gTimePrecise
				// 
				uppressed=0;
				downpressed=0;
				leftpressed=0;
				rightpressed=0;				
				if      (cpct_isKeyPressed(Key_CursorRight) || cpct_isKeyPressed(Joy1_Right) || cpct_isKeyPressed(Joy0_Right)) rightpressed=3; 
				else if (cpct_isKeyPressed(Key_CursorLeft) || cpct_isKeyPressed(Joy1_Left) || cpct_isKeyPressed(Joy0_Left))  leftpressed=4; 
				if      (cpct_isKeyPressed(Key_CursorUp) || cpct_isKeyPressed(Joy1_Up) || cpct_isKeyPressed(Joy0_Up))    uppressed=1; 
				else if (cpct_isKeyPressed(Key_CursorDown) || cpct_isKeyPressed(Joy1_Down) || cpct_isKeyPressed(Joy0_Down))  downpressed=2; 	
				
				if (QTEstate==1) // Display START icon for 1s
				{
					if (gTimePrecise>=50) // 1s
					{
						EraseLastIcon(); // Erase center icon
						gTimePrecise=0;
						QTEstate=2;
					}
				}
				else if (QTEstate==2) // Wait 1s
				{
					if (gTimePrecise>=50) // 1s
					{
						DisplayCentralIconQTEStart(); // Icone for saying it is starting
						gTimePrecise=0;
						QTEstate=3;
					}
				}				
				else if (QTEstate==3) // Display START icon for 1s
				{
					if (gTimePrecise>=50) // 1s
					{
						EraseLastIcon(); // Erase center icone
						gTimePrecise=0;
						QTEstate=4;
					}
				}
				else if (QTEstate==4) // Wait 1s
				{
					if (gTimePrecise>=50) // 1s
					{
						EraseLastIcon(); // Erase center icon
						gTimePrecise=0;
						QTEstate=5; // Start sequence
						// Display first icon
						QTEcurrentstep = 0;
						DisplayQTEIcon(gCurrentScenarioLine[4+QTEcurrentstep]); // 1=up, 2=down, 3=right, 4=left
					}
				}
				else if (QTEstate==5) // Sequence
				{
					u8 win=0;
					u8 lost=0;
					u8 wantedaction = gCurrentScenarioLine[4+QTEcurrentstep]; // 1=up, 2=sown, 3=right, 4=left
					// Test Pressed key
					u8 keypressed=0; 
					if      (rightpressed && !rightpressedlast) keypressed=3; 
					else if (leftpressed && !leftpressedlast)  keypressed=4; 
					else if (uppressed && !uppressedlast)    keypressed=1; 
					else if (downpressed && !downpressedlast)  keypressed=2; 
					
					if (keypressed!=0) // If something have been pressed
					{
						if (wantedaction==keypressed) win=1;
						else lost =1;
					}
					
					// Test win.
					if (win==1)
					{
						// Go to next step 
						QTEcurrentstep++;
						// Test end
						if ( gCurrentScenarioLine[4+QTEcurrentstep] == 255 )
						{
							// QTE is won !
							// Erase last icon
							EraseLastIcon();
							cpct_akp_SFXPlay(1,15, 100, 0, 0, 4); // Sound GOOD							
							
							if (gQte_auto_mode==0)
							{
								// Display win text
								displayline2(gMessageSucces[(gCurrentScenarioLine[2]-1)]);	
								// And continue sequence
								gCurrentScenarioLine+=20; // Jump two lines of sequence.
								InitNextAction();
							}	
							else // Random QTE mode
							{
								gCurrentScenarioLine=gCurrentScenarioLine_Save;
								gQte_next = 100 + ( cpct_getRandom_lcg_u8(0) >> 3 ); // 100 + 64
								InitNextAction();
							}
							
						}
						else
						{
							// Go to next step of sequence.
							// We add a small pause here.
							// Erase last icon
							EraseLastIcon();
							QTEstate=6;
							gTimePrecise=0;
							cpct_akp_SFXPlay(1,15, 100, 0, 0, 4); // Sound GOOD
							
						}
					}
					else if (gTimePrecise>=gCurrentScenarioLine[1] || lost==1) // Death // If player have taken too much time
					{
						u8 message;
						// Erase last icons
						EraseLastIcon();
						// Display death text
						message = gCurrentScenarioLine[3]-100; // first message is 101
						// Death
						displayline2(gMessageDeath[(message-1)]);				
						// Go to state = 1 (cursor will not be displayed again)
						gamestate=1;
						gPaletteMode=0; // Reset death
						gCursorMode=0;						
						// Display death icon
						DisplayCentralIconDeath();	
						cpct_akp_SFXPlay(2,15, 50, 0, 0, 4); //Sound BAD
						gNbDeath++; // stats
					}
				}
				else if (QTEstate==6) // pause after good answer. Very brief to have blank between two arrows
				{
					if (gTimePrecise>=8) // Very short pause.
					{			  
						DisplayQTEIcon(gCurrentScenarioLine[4+QTEcurrentstep]); // 1=up, 2=down, 3=right, 4=left	
						gTimePrecise=0;	
						QTEstate=5; // Back to QTE play state
					}							
				
				}					
				// Save key states
				uppressedlast = uppressed;
				downpressedlast = downpressed;
				leftpressedlast = leftpressed;
				rightpressedlast = rightpressed;	
			// ------- END QTE -----------------------------------------------
			}
			else if (gamestate==4) // -- END, load next level
			{
				// Display sucess message
				if (firepressed!=0 && firepressedlast==0) // Display text when SPACE is pressed
				{			
					// Load and init next level
					// -- Load level
					// gNextLevelName // LEVEL01 BIN
					levelname[5]=gNextLevelName[5];
					levelname[6]=gNextLevelName[6];
					// Check if music need new name.
					//if (levelname[5]=='0') // Restart
					//	musicname[6]='1'; // First music
					//else if (levelname[6]=='1')
					musicname[6]=levelname[5]; // 10 musics
					shouldloopingame=0; // Exit main loop and do level load.
					// Stop interruptions during loading 
					EraseBanner(); // Erase bottom of screen (we are going to stay in mode 0)	
					cpct_removeInterruptHandler();
					cpct_akp_stop(); // Stop music
					cpct_akp_SFXStopAll();
					//cpct_setInterruptHandler(myInterruptHandler2);					
					cpct_setPalette(palette, 16);  					
					textzonereached=1; // to passe the end of this while loop
					cpct_setVideoMode(0);

				}
			}
			else if (gamestate==5) // -- DISPLAY STATS
			{
				if (firepressed!=0 && firepressedlast==0) // Wait space to continue
				{
					// Display text
					DisplayDeadStats();
					gCurrentScenarioLine+=10;
					InitNextAction();
				}
			}
			else if (gamestate==6) // -- Death with sound (play sound)
			{
				EraseBanner(); // Erase bottom of screen (we are going to stay in mode 0)	
				cpct_removeInterruptHandler();
				cpct_akp_stop(); // Stop music
				cpct_setPalette(palette, 16);  					
				textzonereached=1; // to passe the end of this while loop
				cpct_setVideoMode(0);				
				PlayRandomSound(); // This is blocking !!
				gamestate=1; // Go death wait spacebar.
				interrptcount=0;
				setInterruptAtVSYNCStart(myInterruptHandler);	
				DisplayBanner();
				displayline2(gSfxTextDeathSave);	
			}				
			
			firepressedlast = firepressed;

			//cpct_waitVSYNC();
			//interrptcount=0;
			while (textzonereached==0) // Wait interruption signal
			{
			};
			textzonereached=0;
			
		} // While loop in game
		
	
	} // While 1 (all levels)
}
