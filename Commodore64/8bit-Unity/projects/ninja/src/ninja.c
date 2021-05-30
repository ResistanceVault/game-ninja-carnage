//-------------------------------------------------------------------------------
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
// April 2021
//-----------------------------LICENSE NOTICE------------------------------------
// License Creative Commons CC-BY-NC-SA
// Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)
// This is a human-readable summary of (and not a substitute for) the license. Disclaimer.
// You are free to:
// Share — copy and redistribute the material in any medium or format
// Adapt — remix, transform, and build upon the material
// The licensor cannot revoke these freedoms as long as you follow the license terms.
// Under the following terms:
// Attribution — You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
// NonCommercial — You may not use the material for commercial purposes.
// ShareAlike — If you remix, transform, or build upon the material, you must distribute your contributions under the same license as the original.
// No additional restrictions — You may not apply legal terms or technological measures that legally restrict others from doing anything the license permits.
// Notices:
// You do not have to comply with the license for elements of the material in the public domain or where your use is permitted by an applicable exception or limitation.
// No warranties are given. The license may not give you all of the permissions necessary for your intended use. For example, other rights such as publicity, privacy, or moral rights may limit how you use the material.
//-------------------------------------------------------------------------------
//
// Warning : On C64, ASCII code are not interpretted the same way.
// So when encoding ASCII in C, they'll be compiled in PETSCII charaters.
//------------------------------------------------------------------------------

// -- Important defines
//#define ISSIDEB

// -- Debug
//#define NOINTRO
//#define STARTLEVELH '3'
//#define STARTLEVELL '1' // Keep one , for music load, else crash will happend
//#define DEBUGBORDERCOLOR // Debug loading using border colors
//#define ALLOWSKIPLEVEL // We can skip level in all cases
// ---------------------

//------------------------------------------------------------------------------

// Side B
#ifdef ISSIDEB
#define STARTLEVELH '6'
#define STARTLEVELL '1'
#define NOINTRO
#endif

#ifdef DEBUGBORDERCOLOR			
 #define DEBUGCOLOR(i) POKE(53280,i) 
#else
 #define DEBUGCOLOR(i)
#endif	

// ----------------------------------------------------------------------------
// utils\cc65\bin\cl65 -o build/c64/cktest.bin 
// -m build/c64/map64.txt -v    -Cl -O -t c64 -C unity/C64/c64.cfg -I unity D:/Kristof/C64_Dev/8bit-Unity/projects/cktest/src/cktest.c build/c64/unity.lib unity/IP65/ip65.lib unity/IP65/ip65_c64.lib
// -m build/c64/map64.txt
// -l build/c64/ninja.s                     (generate assembly listing)
// -Ln build/c64/ViceLabels.txt              Create a VICE label file

#define WITHMUSIC
#define CUTMUSICDURINGLOADING // This is not running fine else

// Max song is 4539 bytes. 
// Starting song at 600 will be better.

// Mapping final :
// SID        800 - 17ff (4096 bytes max) 
// CODESTART 1840 end 6310 (with data) miss 1536 texts
// free 
// Some functions in a000 and 8000 (to check ?)
// Texts     9400 Time up. Size 1536 bytes
// Level     9c00 
// SCREENRAM C000 - C3FF
// SPRITERAM C700 - CFFF
// COLORRAM  D800
// BITMAPRAM E000 - FFFF

// -- Text colors
// Schema 1 ... 0 / 4 / 2
#define TEXTCOLORNORMAL 0x07
#define TEXTCOLORGOOD   0x47
#define TEXTCOLORBAD    0x27
//1)  9 / 8 / 2
//#define TEXTCOLORNORMAL 0x97
//#define TEXTCOLORGOOD   0x87
//#define TEXTCOLORBAD    0x27
//2)  9 / 8 / 10
//#define TEXTCOLORNORMAL 0x97
//#define TEXTCOLORGOOD   0x87
//#define TEXTCOLORBAD    0xA7
////3)  0 / 8 / 2
//#define TEXTCOLORNORMAL 0x07
//#define TEXTCOLORGOOD   0x87
//#define TEXTCOLORBAD    0x27 
////4)  0 / 8 / 10
//#define TEXTCOLORNORMAL 0x07
//#define TEXTCOLORGOOD   0x87
//#define TEXTCOLORBAD    0xA7
////5)  0 / 9 / 2
//#define TEXTCOLORNORMAL 0x07
//#define TEXTCOLORGOOD   0x97
//#define TEXTCOLORBAD    0x27
////6)  9 / 5 / 2
//#define TEXTCOLORNORMAL 0x97
//#define TEXTCOLORGOOD   0x57
//#define TEXTCOLORBAD    0x27
////7)  9 / 6 / 2
//#define TEXTCOLORNORMAL 0x97
//#define TEXTCOLORGOOD   0x67
//#define TEXTCOLORBAD    0x27

#include "unity.h"
#include "ninja_additionnal.h"
#include <cbm.h>
#include <device.h>

typedef unsigned char u8;
typedef unsigned short u16;

// ---------------------------------------------
// -- Const

const char keyNext = KEY_SP;

// ---------------------------------------------
// Prototypes
void InitGauge(u8 totaltime);
int  MainLoop(void);
void EraseAndStopGauge(u8 fillvalue);
void IRQMultiColourLowResOn(void);
void IRQMultiColourHiResOn(void);
void PlaySoundWarning(void);
int  MainLoopIntro(void);
void RedisplayLastIconColors(void);
void PlaySoundGood(void);
// -- Asm functions
extern void playsample(void); // Asm. Adress of sound must be in $57 (and $58)
extern void detectSID(void); // result in $59
//extern unsigned char getkey(void);

// ---------------------------------------------
// -- Variables

static char sc_rasterLineMulti  = 30;    //50-2;  // Low Res
static char sc_rasterLineMulti2 = 194-0; // 186 Hi Res

static char gInterruptReached=0;
static char gInterruptInactive=0;
static char gInterruptMusicInactive=0;

#define spriteFrames 2
#define spriteCols   12
#define spriteRows   21
unsigned char spriteColors[] = { BLACK, RED, GREEN, YELLOW, 0, 0, 0, 0, GREEN, RED };  // 0-8: Sprite colors, 9-10: Shared colors

// d016 / 53270 ... control Register 2
#define MultiModeOn()		VIC.ctrl1 |= (1<<4); VIC.ctrl2 |= (1<<4)
#define MultiModeOff()		VIC.ctrl2 &= ~(1<<4)
#define MultiHiModeOn()		VIC.ctrl1 |= (1<<4); VIC.ctrl2 &= ~ (1<<4)
#define MultiHiModeOff()	VIC.ctrl2 &= ~(1<<4)

// -- Intro vars

u8 gTextScrollAnimationCounter=255; // 0 to 49 = active, >49 = inactive
u8 gTextScrollString[41]; // One line of text
u8 gTextScrollY; // Y position

// -- Game Variables
#define gOffsetPicY 8 // In line (this is a constant)
u8  gOffsetPicX=2; // Number of char , this can vary (3 positions)

// -- Unique memory bloc, shared by intro and game
unsigned char gAllDataBloc[18800]; // end approx 0x0AD69 (not up to date)

// offset          0 FonteIntro
// offset        594 Texts
// offset        922 FlagLogo
// Total size 1246
// offset          0 LogoRSE2_C64
// offset       2404 sndResistance
// Total size 5189
// offset          0 ninja_title_025_Final160
// offset      10004 sndNinja
// Total size 17515
// offset          0 pixelshi
// offset       2240 colors1and2hi
// offset       2520 font
// offset       3688 IconDeath
// offset       3852 IconSucess
// offset       4016 IconQTE1
// offset       4180 IconQTE2
// offset       4344 IconQTE3
// offset       4508 IconQTE4
// offset       4672 IconQTEAction
// offset       4836 back1
// offset       4880 back2
// offset       4924 IconFloppy
// Total size data 5088
// Total size space 14624

// -- Intro bloc usage
// Intro1.bin
#define FonteIntro  (gAllDataBloc+0)
#define IntroTexts  (gAllDataBloc+594)
#define FlagLogo    (gAllDataBloc+922)
// Intro2.bin
#define LogoRSE2_C64  (gAllDataBloc+1246+0)
#define sndResistance (gAllDataBloc+1246+2404)
// Intro3.bin
#define ninja_title_025_Final160 (gAllDataBloc+1246+0)
#define sndNinja                 (gAllDataBloc+1246+10004)

// -- Game bloc usage
#define pixelshi gAllDataBloc
#define colors1and2hi (pixelshi+(320*7))
#define font (colors1and2hi+(40*7))
#define IconDeath (font+(146*8))
#define IconSucess (IconDeath+164) 
#define IconQTE1 (IconSucess+164) 
#define IconQTE2 (IconQTE1+164) 
#define IconQTE3 (IconQTE2+164) 
#define IconQTE4 (IconQTE3+164) 
#define IconQTEAction (IconQTE4+164) 
#define back1 (IconQTEAction+164) 
#define back2 (back1+44)
#define IconFloppy  (back2+44)
#define gTimeUpMessages  (IconFloppy+164) // 512*3. This file is loaded by langage
#define gLevelBuffer  (gTimeUpMessages+(512*3)) // 8000 // This is loaded for each level

// -- SFX

unsigned char sfx_good[]={		
			0x0A,0x00,0x08,0xB2,0x41,0xB2,0xB2,0xB5,0xB5,0xB5,0xB7,0xB7,0xB7,0xB8,0xB8,0xB8,
			0xBC,0xBC,0xBC,0xBE,0x21,0xBE,0xC1,0xC1,0xC3,0xC3,0xC4,0xC4,0xC8,0xC8,0xCA,0xCA,
			0xCA,0xCA,0x00
		};
unsigned char sfx_goodx2[]={		
			0x0A,0x00,0x08,0xB2,0x41,0xB2,0xB2,0xB2,0xB2,0xB2,0xB5,0xB5,0xB5,0xB5,0xB5,0xB5,
			0xB7,0xB7,0xB7,0xB7,0xB7,0xB7,0xB8,0xB8,0xB8,0xB8,0xB8,0xB8,0xBC,0xBC,0xBC,0xBC,
			0xBC,0xBC,0xBE,0x21,0xBE,0xBE,0xBE,0xC1,0xC1,0xC1,0xC1,0xC3,0xC3,0xC3,0xC3,0xC4,
			0xC4,0xC4,0xC4,0xC8,0xC8,0xC8,0xC8,0xCA,0xCA,0xCA,0xCA,0x00
		};
		
unsigned char sfx_bad[]={		
			0x0A,0x00,0x08,0xD8,0x81,0xA6,0x41,0x98,0x40,0xDF,0x81,0xD1,0x80,0xC0,0xDE,0xAC,
			0x31,0xA0,0xA6,0x9A,0x9A,0x9E,0x98,0x98,0x98,0x9C,0x96,0x96,0x96,0x9A,0xA0,0x94,
			0x94,0x98,0x9E,0x92,0x92,0x96,0x41,0x9C,0x90,0x90,0x94,0x8E,0x8E,0x8E,0x92,0x8C,
			0x8C,0x8C,0x90,0x8A,0x8A,0x8A,0x8E,0x88,0x88,0x88,0x00
		};
unsigned char sfx_badx2[]={		
			0x0A,0x00,0x08,0xD8,0x81,0xD8,0xA6,0x41,0xA6,0x98,0x40,0x98,0xDF,0x81,0xDF,0xD1,
			0x80,0xD1,0xC0,0xC0,0xDE,0xDE,0xAC,0x31,0xAC,0x9A,0x9A,0x9A,0x9A,0x9A,0x9A,0x9E,
			0x9E,0xA4,0xA4,0x98,0x98,0x98,0x98,0x9C,0x9C,0xA2,0xA2,0x96,0x96,0x96,0x96,0x9A,
			0x9A,0xA0,0x94,0x94,0x94,0x94,0x98,0x98,0x9E,0x9E,0x92,0x92,0x92,0x92,0x96,0x41,
			0x96,0x9C,0x90,0x90,0x90,0x90,0x90,0x94,0x94,0x9A,0x8E,0x8E,0x8E,0x8E,0x8E,0x92,
			0x8C,0x8C,0x8C,0x8C,0x8C,0x8C,0x8C,0x90,0x90,0x96,0x8A,0x8A,0x8A,0x8A,0x8A,0x8E,
			0x8E,0x88,0x88,0x88,0x88,0x88,0x88,0x00	
		};	
		
unsigned char sfx_warning[]={		
			0x0A,0x00,0x08,0xD5,0x21,0xC9,0xC9,0xC9,0xCE,0xC2,0xC2,0xC2,0xD0,0xC4,0xC4,0xC4,
			0xCA,0xBE,0xBE,0xBE,0x00
		};	
unsigned char sfx_warningx2[]={		
			0x0A,0x00,0x08,0xD5,0x21,0xC9,0xC9,0xC9,0xC9,0xC9,0xCE,0xCE,0xC2,0xC2,0xC2,0xC2,
			0xD0,0xD0,0xC4,0xC4,0xC4,0xC4,0xCA,0xCA,0xBE,0xBE,0xBE,0xBE,0x00
		};			

// -- Globals ---------------------------------------------

// -- Structure for a level binary
struct sleveldata
{
	u8 position; // 1 2 3 (image 1 (left), middle 2 or right 3)
	u8 nbzones; // 1 to 9
} sleveldata;

// -- Data zone
// The begining of the zone are the pointer to the data. All is absolute encoded
// The binary data is one block and read at once from 0x9C00. The pointer are automatically mapped and point to the data.
struct sleveldata* gLevelData;
u8* gNextLevelName;
u8* gZonesData;
u8* gScenario;
u8* gMessageIntro;
u8* gMessageSucces[32];
u8* gMessageDeath[32];
u8* gTilesData;

// ---------------------------------------------
u8  myjoy; // Bug always send back $FF
u8  gNbZones;
u8* gCurrentScenarioLine;  // Pointer to current scenario line
u8* gCurrentScenarioLine_Save;  // Pointer to current scenario line
u8* gSfxTextDeathSave; // Save pointer to text to display after SFX
//u8  gSfxDeathCount; // Each xx death play sound
u8  gamestate; // 0=playing 1=dead & wait space
u8  QTEstate; // For managing the QTE sequence.
u8  QTEcurrentstep; // Step of the rythmic sequence.
u16 gGauge; // Current value of gauge. max is 128*256. 0 is not activated
u16 gGaugeSpeed; // Decrease speed (each frame)
u8  gGaugeEnded; // Switch to 1 when time is over.
u8  gGaudeInUse; // 0=not used, 1=used
u8  gIconLastDisplayedId; // Id of last icon displayed (0=none)
u8  gPaletteMode; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall
u8  gPaletteNextSwap; // >0 in frame
u8  gNextPalette; // 0=Palette 1=PaletteAlt
u8  gCursorMode; // Special mode for cursor. 0=normal. 1=Slow and random (hit), 2=reversed (all) 3=reversed Up, 4=reversed down 5=always falling
u8  gQTEForceNoWait; // False. For level 92 force no wait
u8  gCheatEnable;
u8  gMusicDoubleSpeed; // 0=1x speed, 1=2x Speed
u8  gSkippingLevel; // For level skip
//u8  gCheatSequence[3]; // SOS is the sequence.
u8  gCheatSequenceCorrectLetters; 
u16 gNbDeath; // Stats on number of death
// Sprite size (in bytes)
#define SP_W   6
#define SP_H   8
// Screen size (in bytes)
#define SCR_W   80
#define SCR_H  200
u8  gTime; // Seconds
u8  gTimeFrag; // 10th of seconds
u8  gTimePrecise; // Frame precise (0 to 255)
u8  gQte_auto_mode; // 0 = off 1=on
u8  gQte_next; // Time before next QTE
u8  gScenario_qte_live[2][10]; // Random QTE for last level
#define NBMESFAILTIME 6
u8  gMessageFailTimeCurrent;
u8* gTimeUpDeathsPtr[7]; // 14 bytes
//u8  gInterrptcount;
u8  textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)
u8  gPicAltCurrentIndex; // For animation, store current alt picture 0 or more
u8  gIsOldSID;  // 1 if old sid.
u8  gSoundGoodHaveBeenPlayedThisFrame; // To avoid playing twice

#define NBMAXZONES 9
u8 zones[NBMAXZONES*5]; // start with no values

u8 firstlevelload[12]={0x4c,0x45,0x56,0x45,0x4c,0x30,0x31,0x2e,0x42,0x49,0x4e,0x00}; //"level01.bin";
u8 musicload[12]     ={0x4D,0x55,0x53,0x49,0x43,0x30,0x4e,0x2e,0x42,0x49,0x4e,0x00};      //"music0n.bin"; // Will be encoded to uppercases (petscii) N=New SID O=OLDSID

// ---------------------------------------------
// Play a digitzed sample (blocking function)
// Level Technic
// Parameters are passed using memory adress : $5b et $57
// Will turn screen off and on again
// 30  is normal speed to play 5Khz samples. 22 to 38 for faster and slower
void playsampleaudio(u8* audiodata, u8 speed)
{
	//unsigned char a;
	u16 data;
	data = &(audiodata[2]); // Header is 2 bytes
	// Header
	POKE(0x5b,audiodata[0]);
	POKE(0x5c,audiodata[1]);
	// Audio data
	POKE(0x57,(data&0xff));
	POKE(0x58,(data>>8));
	// Speed
	POKE(0x5D,speed); // Normal speed
	playsample();
	//EnterBitmapMode();
	//MultiModeOn();
}
				
// -------------------------------------------------------
void InstallRaster()
{
	asm("sei");
	
	asm("lda #$7f");
	asm("sta $dc0d"); // Turn off CIAs1 Timer interrupts
	asm("sta $dd0d"); // Turn off CIAs2 Timer interrupts
	asm("lda $dc0d"); // ;by reading this two registers we negate any pending CIA irqs.
	asm("lda $dd0d");
	asm("and $d011");
	asm("sta $d011");
	
	asm("lda %v", sc_rasterLineMulti);
	asm("sta $d012");
	// Set new IRQ adress
	asm("lda #<%v",IRQMultiColourLowResOn);
	asm("sta $0314");
	asm("lda #>%v", IRQMultiColourLowResOn);
	asm("sta $0315");
	// --
	asm("lda #1");    // Set Interrupt Request Mask...
	asm("sta $d01a"); // we want IRQ by Rasterbeam. Tell vic2 to generate raster IRQ
	asm("cli");
}

// -------------------------------------------------------
// IRQ for LOW RES. First part of screen
void IRQMultiColourLowResOn()
{
	//asm("lda #$ff");  // Clear the interrupt request register, to clear the interrupt condition.
	//asm("sta $D019"); // Otherwise, the VIC will trigger another IRQ immediately regardless of the rasterline.
  
	//asm("lda #$01");  // DEBUG Change Back color (white)
	//asm("sta $d020"); // DEBUG 
	
	if (!gInterruptInactive)
		MultiModeOn(); // Low res bitmap mode
#ifdef WITHMUSIC
	if (!gInterruptMusicInactive)
	{
		asm("jsr $0603"); // GoatTracker2 SID play
	}	
	else
	{			// Volume to zero
		unsigned char filterandvolume = PEEK(0xd418); // filter mode and main volume control
		filterandvolume = filterandvolume & 0xF8; // volume to zero
		POKE(0xd418,filterandvolume);	
	}
#endif		
	if (!gInterruptInactive)
	{		
		// Set next interrupt (hi res)
		asm("lda #<%v", IRQMultiColourHiResOn);
		asm("sta $0314");
		asm("lda #>%v", IRQMultiColourHiResOn);
		asm("sta $0315");
		asm("lda %v", sc_rasterLineMulti2);
		asm("sta $d012");
	}	
	else
		gInterruptReached=1; // Intro, we only have one IRQ
	
	asm("asl $d019"); // acknowledge IRQ
	
	//asm("lda #$00");  // DEBUG Change Back color (black)
	//asm("sta $d020"); // DEBUG 	
	
	asm("jmp $ea31"); // Scan keys. If you jump to $EA31, you should do it only once per frame.
}

// -------------------------------------------------------
void IRQMultiColourHiResOn()
{
	//asm("lda #$05");  // DEBUG Change back color (green)
	//asm("sta $d020"); // DEBUG	
	
	if (!gInterruptInactive)
	{
		MultiHiModeOn(); // Hi Res mode
	}
#ifdef WITHMUSIC	
	if (!gInterruptMusicInactive) // Double speed music gMusicDoubleSpeed
	{
		if (gMusicDoubleSpeed) // For playing in x2 also setup this
		  asm("jsr $0603"); // GoatTracker2 SID play
	}
#endif		
	// Set next interrupt (low res)
	asm("lda #<%v", IRQMultiColourLowResOn);
	asm("sta $0314");
	asm("lda #>%v", IRQMultiColourLowResOn);
	asm("sta $0315");
	asm("lda %v", sc_rasterLineMulti);
	asm("sta $d012");
	asm("asl $d019"); // acknowledge IRQ
	gInterruptReached=1;	
	//asm("jmp $ea81"); // return to kernel interrupt routine
	asm("jmp $ea81"); // Scan keys. If you jump to $EA31, you should do it only once per frame.
	// $EA81 doesn't scan the keyboard automatically, and so saves some time.
	// $EA81 just gets the A, X and Y registers from the stack. If you preserve them at the beginning of the interrupt (self-modifying code), you 
	// RTI
}

// -------------------------------------------------------	
int main (void)
{
	// Set text mode colors
    textcolor(COLOR_GRAY1); // Important
    bordercolor(COLOR_BLACK);
    bgcolor(COLOR_BLACK);
	
	detectSID(); // If old SID, then 1 is written in 0x59
	gIsOldSID = PEEK(0x59); // Goet value of 0x59
	
	MainLoop();
	
    // Done
    return EXIT_SUCCESS;
}

// -------------------------------------------------------	
// Display Picture (optim), Each character at one
// Display a picture at my own format (header got size)
// u8 column id
// u8 line (should be multiple of 8)
// u8 data... picture data (4 bytes header).
// Sample header :
// 16,32,0,1, // x,y,back color, nb screenram buffers
// Then bitmap data
// Color data
// Screen data
void DisplayPicture(u8* data, u8 x, u8 y)
{
	u8 i=0;
	u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8* colorramptr=0;
	u8* dataptr1 = 0;
	u8* dataptr2 = 0;
	u8* dataptr3 = 0;
	u8* increment=0;
	u8 column;
	u8 line;
	u8 bytewidth;
	u16 bytewidthx8;
	u8 byteheight;
	column=x;
	line=y>>3;
	bitmapramptr = BITMAPRAM+column*8+y*40;
	increment=column+line*40;
	screenramptr = SCREENRAM+increment;
	colorramptr  = COLORRAM+increment;	
	// Point on data ptr (loaded data)
	dataptr1=data; // Start of bitmap bin file (my format)
	bytewidth = dataptr1[0]/4;
	bytewidthx8 = (bytewidth<<3);
	byteheight = dataptr1[1]/8;
	// Start by color data
	dataptr1 +=4;
	dataptr2 = dataptr1 + (bytewidthx8 * byteheight);
	dataptr3 = dataptr2 + (bytewidth * byteheight);
	for (i=0; i<byteheight; i++)
	{
		// Color ram
		memcpy(colorramptr,dataptr2,bytewidth);
		dataptr2 += bytewidth; // Next line in data
		colorramptr += 40; // Next line on screen.
		// Screen ram
		memcpy(screenramptr,dataptr3,bytewidth);
		dataptr3 += bytewidth; // Next line in data
		screenramptr += 40; // Next line on screen.	
		// Pixels
		memcpy(bitmapramptr,dataptr1,bytewidthx8);
		dataptr1 += bytewidthx8; // Next line in data
		bitmapramptr += 8*40; // Next line on screen.		
	}	
}

// -------------------------------------------------------	
// Display Picture (optim), Each character at one
// Display a picture at my own format (header got size)
// u8 column id
// u8 line (should be multiple of 8)
// u8 data... picture data (4 bytes header).
// Sample header :
// 16,32,0,1, // x,y,back color, nb screenram buffers
// Then bitmap data
// Color data
// Screen data
void DisplayPictureColorOnly(u8* data, u8 x, u8 y)
{
	u8 i=0;
	//u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8* colorramptr=0;
	u8* dataptr1 = 0;
	u8* dataptr2 = 0;
	u8* dataptr3 = 0;
	u8* increment=0;
	u8 column;
	u8 line;
	u8 bytewidth;
	u16 bytewidthx8;
	u8 byteheight;
	column=x;
	line=y>>3;
	//bitmapramptr = BITMAPRAM+column*8+y*40;
	increment=column+line*40;
	screenramptr = SCREENRAM+increment;
	colorramptr  = COLORRAM+increment;	
	// Point on data ptr (loaded data)
	dataptr1=data; // Start of bitmap bin file (my format)
	bytewidth = dataptr1[0]/4;
	bytewidthx8 = (bytewidth<<3);
	byteheight = dataptr1[1]/8;
	// Start by color data
	dataptr1 +=4;
	dataptr2 = dataptr1 + (bytewidthx8 * byteheight);
	dataptr3 = dataptr2 + (bytewidth * byteheight);
	for (i=0; i<byteheight; i++)
	{
		// Color ram
		memcpy(colorramptr,dataptr2,bytewidth);
		dataptr2 += bytewidth; // Next line in data
		colorramptr += 40; // Next line on screen.
		// Screen ram
		memcpy(screenramptr,dataptr3,bytewidth);
		dataptr3 += bytewidth; // Next line in data
		screenramptr += 40; // Next line on screen.	
		// Pixels
		//memcpy(bitmapramptr,dataptr1,bytewidthx8);
		//dataptr1 += bytewidthx8; // Next line in data
		//bitmapramptr += 8*40; // Next line on screen.		
	}	
}

// -------------------------------------------------------
// Commun code with CPC
// -------------------------------------------------------

// -------------------------------------------------------
// Display Background with pattern
void DisplayBackground()
{
	u8 x,y;
	// Display Background
	for (y=0;y<9;y++)
	{	
		for (x=0;x<20;x++)
		{
			if( (x+y) % 2 )
				DisplayPicture(back1,x*2,y*16);
			else
				DisplayPicture(back2,x*2,y*16);
		}
	}
}

// -------------------------------------------------------
// Displaypic (one game picture 48*128 piwels low res)
// Redisplay all picture
// -------------------------------------------------------
void DisplayPic()
{
	u8 i=0;
	u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8* colorramptr=0;
	u8* dataptr = 0;
	u8* dataptr2 = 0;
	u8* dataptr3 = 0;
	u8* ofssetscreentemp;
	u8 column = gOffsetPicX;
	u8 line=gOffsetPicY>>3; // divide by 8
	bitmapramptr = BITMAPRAM+(column<<3)+gOffsetPicY*40;
	ofssetscreentemp = column+line*40;
	screenramptr = SCREENRAM+ofssetscreentemp;
	colorramptr  = COLORRAM+ofssetscreentemp;
	// Point on data ptr (loaded data)
	dataptr=gTilesData; // Start of bitmap bin file (my format)
	dataptr+=4; // Skip header
	dataptr2=dataptr+16*8*12;
	dataptr3=dataptr2+16*12;
	// Data picture
	// 4 bytes header (we can skip them).
	// Bitmap data 8*12 on 16 lines = 1536 bytes
	// Color data = 1*12*16 = 192
	// Screen color data = 1*12*16 = 192
	for (i=0; i<16; i++)
	{
		memcpy(bitmapramptr,dataptr,8*12);
		dataptr += 8*12; // Next line in data
		bitmapramptr += 8*40; // Next line on screen.
		
		memcpy(colorramptr,dataptr2,12);
		dataptr2 += 12; // Next line in data
		colorramptr += 40; // Next line on screen.
		
		memcpy(screenramptr,dataptr3,12);
		dataptr3 += 12; // Next line in data
		screenramptr += 40; // Next line on screen.		
	}
	gPicAltCurrentIndex=0;
	gIconLastDisplayedId=0; // No icons displayed at start
}

// -------------------------------------------------------
// DisplayPicAlt
// altindex = 1 2 3 4 5 6
// Will only change the colors, not the bitmap
// -------------------------------------------------------
void DisplayPicAlt(int altindex)
{
	u8 i=0;
	u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8* colorramptr=0;
	u8* dataptr = 0;
	u8* dataptr1 = 0;
	u8* ofssetscreentemp;
	u8 column = gOffsetPicX;
	u8 line=gOffsetPicY>>3;
	if (gPicAltCurrentIndex==altindex) // If this alt version is already display then do nothing
		return;
	//bitmapramptr = BITMAPRAM+column*8+gOffsetPicY*40;
	ofssetscreentemp = column+line*40;
	screenramptr = SCREENRAM+ofssetscreentemp;
	colorramptr  = COLORRAM+ofssetscreentemp;
	// Point on data ptr (loaded data)
	dataptr=gTilesData; // Start of bitmap bin file (my format)
	// Check that there are enough alt pictures
	if (dataptr[3]<altindex)
		return;
	dataptr+=4; // Skip header
	dataptr+=1536; // Skip bitmap data
	dataptr+= ((192+192)*altindex); // Skip color data. 12*16=192.
	dataptr1=dataptr+16*12;
	// Data picture
	// 4 bytes header (we can skip them).
	// Bitmap data 8*12 on 16 lines = 1536 bytes
	// Color data = 1*12*16 = 192
	// Screen color data = 1*12*16 = 192
	//for (i=0; i<16; i++)
	//{
	//	memcpy(bitmapramptr,dataptr,8*12);
	//	dataptr += 8*12; // Next line in data
	//	bitmapramptr += 8*40; // Next line on screen.
	//}
	for (i=0; i<16; i++)
	{
		memcpy(colorramptr,dataptr,12);
		dataptr += 12; // Next line in data
		colorramptr += 40; // Next line on screen.
		
		memcpy(screenramptr,dataptr1,12);
		dataptr1 += 12; // Next line in data
		screenramptr += 40; // Next line on screen.		
	}	
	gPicAltCurrentIndex=altindex; // Store last used index
}

// -------------------------------------------------------
// Redisplay a 32x32 bytes
// x in bytes, y in lines
// consider the altindex also
void redisplaytilefromindex(u8 x, u8 y)
{
	// Copy from picture to screen.
	// Size is 4 bytes width and 4 bytes heights
	// Picture is 12bytes width and 16 bytes height
	u8 i=0;
	u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8* colorramptr=0;
	u8* dataptrcurrent = 0;
	u8* dataptrcurrent2 = 0;
	u8* dataptrcurrent3 = 0;	
	u8* dataptrstartbloc = 0;
	//u8* spriteadress;
	u8 columnonscreen = gOffsetPicX+x;
	u8 lineonscreen=(gOffsetPicY+y)>>3;
	u8 skipblocofcharbefore = x+((y*3)/2); // (y/8*12)+x;
	//DisableRom();
	bitmapramptr = BITMAPRAM+columnonscreen*8+lineonscreen*8*40;
	screenramptr = SCREENRAM+columnonscreen+lineonscreen*40;
	colorramptr  = COLORRAM+columnonscreen+lineonscreen*40;
	// Point on data ptr (loaded data)
	dataptrstartbloc=gTilesData; // Start of bitmap bin file (my format)
	dataptrstartbloc+=4; // Skip header
	// Data picture
	// 4 bytes header (we can skip them).
	// Bitmap data 8*12 on 16 lines = 1536 bytes
	// Color data = 1*12*16 = 192
	// Screen color data = 1*12*16 = 192
	dataptrcurrent = dataptrstartbloc + (skipblocofcharbefore*8);
	// Compute ram color ptr
	dataptrstartbloc+=8*12*16; // next bloc
	dataptrstartbloc += ((192+192)*gPicAltCurrentIndex); // Go to current used alt picture
	dataptrcurrent2 = dataptrstartbloc + skipblocofcharbefore;	
	// Compute screen color ptr
	dataptrstartbloc+=12*16; // next bloc
	dataptrcurrent3 = dataptrstartbloc + skipblocofcharbefore;	
	
	for (i=0; i<4; i++)
	{
		memcpy(bitmapramptr,dataptrcurrent,8*4);
		dataptrcurrent += 8*12; // Next line in data
		bitmapramptr += 8*40; // Next line on screen.
		
		memcpy(colorramptr,dataptrcurrent2,4);
		dataptrcurrent2 += 12; // Next line in data
		colorramptr += 40; // Next line on screen.

		memcpy(screenramptr,dataptrcurrent3,4);
		dataptrcurrent3 += 12; // Next line in data
		screenramptr += 40; // Next line on screen.		
	}
}

// -------------------------------------------------------
// drawStringwithSize
// 2 colors.
// Background color is yellow 7 
// Front colors is black 0 or xx
// Schema 1 ... 0 / 4 / 2
//#define TEXTCOLORNORMAL 0x07
//#define TEXTCOLORGOOD   0x47
//#define TEXTCOLORBAD    0x27
u8 drawStringwithSize(u8* ptext, u8 x, u8 y, u8 totalsize)
{
	u16 ymul40;
	u8 i=0;
	u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8 currentsize=0;
	u8* ptrfont=0;
	u8 nbspecialbytes=0;
	u8 currentcolor = TEXTCOLORNORMAL; // Text color + background color

	ymul40 = 40*y;
	bitmapramptr = BITMAPRAM+(ymul40<<3)+(x<<3);
	screenramptr = SCREENRAM+(ymul40)+x;
	while (currentsize!=totalsize)
	{
		u8 cara = ptext[0];
		if (cara==1)
		{
			currentcolor=TEXTCOLORNORMAL; // Text color + background color
			ptext++;
			nbspecialbytes++;
		}
		else if (cara==2)
		{
			currentcolor=textboldcolor; // Text color + background color
			ptext++;
			nbspecialbytes++;
		}
		else
		{			
			// Display one character
			POKE(screenramptr,0x77); // First set to back color (to hide changes)
			ptrfont = (u8*) font + ( ( ptext[0] - 32 ) << 3 ) ;
			memcpy(bitmapramptr,ptrfont, 8);
			POKE(screenramptr,currentcolor);			
			bitmapramptr += 8;
			screenramptr++;
			ptext++;
			x++;
			currentsize++;
		}
	}
	return currentsize+nbspecialbytes;
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
void DisplayBanner()
{
	// Display HiRes picture
	// 320*8 bytes
	// source : lda pixelshi
	// Dest   :	sta $2000+(320*LINEYHIRES)
	//DisableRom();
	u8 i;
	u8* dataptr1 = pixelshi;
	u8* dataptr2 = colors1and2hi;
	u8* bitmapptr = (BITMAPRAM+320*18);
	u8* screenramptr = (SCREENRAM+(40*18));	
	// Display line by line to avoir color problems
	for (i=0; i<7; i++)
	{
		memcpy(bitmapptr    , dataptr1,       320);
		memcpy(screenramptr , dataptr2,        40);
		dataptr1+=320;
		dataptr2+=40;
		bitmapptr+=320;
		screenramptr+=40;
	}
	//EnableRom();
}

// -------------------------------------------------------
// Display Text
void displayline2(u8* ptext)
{
	u8 nbbytesprocessed;
	//DisableRom();

	nbbytesprocessed = drawStringwithSize(ptext, 4,19, 40-8); 
	ptext += nbbytesprocessed;
   
	nbbytesprocessed = drawStringwithSize(ptext, 3, 20 , 40-6); 
	ptext += nbbytesprocessed;
	
	nbbytesprocessed = drawStringwithSize(ptext, 2 , 21 , 40-4); 
	ptext += nbbytesprocessed;	

	nbbytesprocessed = drawStringwithSize(ptext, 3, 22 , 40-6); 
	ptext += nbbytesprocessed;	

	drawStringwithSize(ptext, 4 , 23, 40-8); 

	//EnableRom();	
}

// -------------------------------------------------------
void DisplayDeadStats()
{
	// Display variable gNbDeath
	// Can go up to 9999
	int loop;
	u8* string;
	// Search string 0000 in death 
	int offsetfound=0; // Should be different than 0
	string = gTimeUpDeathsPtr[NBMESFAILTIME];
	for (loop=0; loop<178; loop++)
	{
		if (string[loop]=='0' && string[loop+1]=='0' )
		{
			offsetfound=loop;
			break;
		}
	}
	// Change number of deaths
	if (offsetfound!=0)
	{
		string[offsetfound+0]= ( (gNbDeath/ 1000 ) % 10) + '0';
		string[offsetfound+1]= ( (gNbDeath/ 100  ) % 10) + '0';
		string[offsetfound+2]= ( (gNbDeath/ 10   ) % 10) + '0';
		string[offsetfound+3]= ( (gNbDeath       ) % 10) + '0';
	}
	displayline2(string); // Display stat message
}

// -------------------------------------------------------
// Erase an icon of 32x32 pixels (wich is 4 bytes width, on 4 bytes height)
// Depend on value in gIconLastDisplayedId
void EraseLastIcon() // 1=right, 2=up, 3=left 4=down, all others are in center
{    
	u8 x=6-2; // Default center. 5=death 6=Sucess 7=Start
	u8 y=64-16;
	if (gIconLastDisplayedId==2) // Up
	{
		y=64-16-32;
	}
	else if (gIconLastDisplayedId==1) // Right
	{
		x=6-2+4;
	}
	else if (gIconLastDisplayedId==3) // Left
	{
		x=6-2-4;
	}	
	else if (gIconLastDisplayedId==4) // Down
	{
		y=64-16+32;
	}	
	redisplaytilefromindex(x,y); // x in bytes, y in lines
	gIconLastDisplayedId=0; // Nothing on screen
}

// -------------------------------------------------------
void DisplayCentralIconDeath()
{ 	// x =6-2 y=64-16
	DisplayPicture(IconDeath , 4+gOffsetPicX, 48+gOffsetPicY);
	gIconLastDisplayedId=5;
}
// -------------------------------------------------------
void DisplayCentralIconSucess()
{
	DisplayPicture(IconSucess , 4+gOffsetPicX, 48+gOffsetPicY);
	gIconLastDisplayedId=6;
}
// -------------------------------------------------------
void DisplayCentralIconQTEStart()
{
	DisplayPicture(IconQTEAction , 4+gOffsetPicX, 48+gOffsetPicY);
	gIconLastDisplayedId=7;
}
// -------------------------------------------------------
void DisplayCentralIconQTELeft()
{
	DisplayPicture(IconQTE3 , 0+gOffsetPicX, 48+gOffsetPicY);
	gIconLastDisplayedId=3;	
}
// -------------------------------------------------------
void DisplayCentralIconQTERight()
{
	DisplayPicture(IconQTE1 , 8+gOffsetPicX, 48+gOffsetPicY);
	gIconLastDisplayedId=1;	
}
// -------------------------------------------------------
void DisplayCentralIconQTEUp()
{
	DisplayPicture(IconQTE2 , 4+gOffsetPicX, (48-32)+gOffsetPicY);
	gIconLastDisplayedId=2;
}
// -------------------------------------------------------
void DisplayCentralIconQTEDown()
{
	DisplayPicture(IconQTE4 , 4+gOffsetPicX, (48+32)+gOffsetPicY);
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
// On C64, when picture is animated, the icon of QTE get scrambled
// So redisplay each frame the icon if the alternate picture is changing
void RedisplayLastIconColors()
{
	switch(gIconLastDisplayedId)
	{
		case 1: DisplayPictureColorOnly(IconQTE1 , 8+gOffsetPicX, 48+gOffsetPicY); break; // Right
		case 2: DisplayPictureColorOnly(IconQTE2 , 4+gOffsetPicX, (48-32)+gOffsetPicY); break; // Up
		case 3: DisplayPictureColorOnly(IconQTE3 , 0+gOffsetPicX, 48+gOffsetPicY); break; // Left
		case 4: DisplayPictureColorOnly(IconQTE4 , 4+gOffsetPicX, (48+32)+gOffsetPicY); break; //Down
		case 5: DisplayPictureColorOnly(IconDeath , 4+gOffsetPicX, 48+gOffsetPicY); break; //Death
		case 6: DisplayPictureColorOnly(IconSucess , 4+gOffsetPicX, 48+gOffsetPicY); break; // Success
		case 7: DisplayPictureColorOnly(IconQTEAction , 4+gOffsetPicX, 48+gOffsetPicY); break; // Start
		default: break;
	}
}

// -------------------------------------------------------
void SetOffsetImage(u8 imageid)
{
	if (imageid==1)
	{
		gOffsetPicX=1; // 1 , 14 , 27 ... picture column for image 1 2 3.
	}
	else if (imageid==2)
	{
		gOffsetPicX=14;

	}	
	else if (imageid==3)
	{
		gOffsetPicX=27;
	}
	else
		gOffsetPicX = 0; // Bad value
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
		gTime++;
		gTimeFrag=0;
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
		else if ( nextaction == 253 ) // Change Palette mode 0 1 2 3 4 5 6 7
		{
			gPaletteMode=gCurrentScenarioLine[1]; // 0=normal 1=blink 2=smoke 3=smoke&blood 
			//4=anim palette fall 5=river 6=diablo 7=fire
			gPaletteNextSwap=10; // >0 in frame
			gNextPalette=1; // 0=Palette 1=PaletteAlt		
			gCurrentScenarioLine+=10;
			nextaction = gCurrentScenarioLine[0];
			// Specific modes, only one palette
			if (!gSkippingLevel)
			{
				if (gPaletteMode==0)
					DisplayPicAlt(0); // Level 42 normal picture
				else if (gPaletteMode==2)
					DisplayPicAlt(1); // Level 42 smoke			
				else if (gPaletteMode==3)
					DisplayPicAlt(2); // Level 42 blood	
			}
		}
		else if ( nextaction == 249 ) // Change QTE mode 
		{
			gQte_auto_mode=gCurrentScenarioLine[1];
			gQte_next = 142 + (rand() % 64); // 142 + 64... difficulty medium
			gCurrentScenarioLine+=10;
			nextaction = gCurrentScenarioLine[0];
		}	
		else if ( nextaction == 254 ) // end, display background
		{
			//DisplayBackgroundForOnePicture();
			//gNoNeedToEraseCursor=1;
			nextaction = 255; // Force end
		}		
	}
	
	// -- Do we have a game command ?
	if ( nextaction < 240 ) // Command
	{
		gamestate=0; // Default is using cursor	
		if (!gSkippingLevel)		
			InitGauge(nextaction); // Playing. Nextaction is time
		return;
	}

	// -- Do I need to erase cursor ?	
	//if (gNoNeedToEraseCursor==0)
	//	erasecursor(xold,yold); // Erase cursor, we do not need it	
	
	// -- Process commands
	if (  nextaction == 250 ) // Text
	{
		gamestate = 2; // Text
	}
	else if ( nextaction == 251 ) // QTE
	{
		if (gQTEForceNoWait)
			gamestate = 3; // Start QTE gamestate. Level 92 only
		else
			gamestate = 7 ; // Wait Key before starting QTE
		QTEstate=1; // Start the QTE sequence (here from beginning, with two Warning icon)
		if ( gCurrentScenarioLine[1]<50) // If QTE time is quick, then only display one warning icon.
			QTEstate=3;
		gTimePrecise=0; // Reset precise time
		if (!gSkippingLevel)
		{
			DisplayCentralIconQTEStart(); // Icone for saying it is starting (to be done after)		
			PlaySoundWarning();
		}
	}
	else if ( nextaction == 248 ) // Display stats
	{
		gamestate=5; // Display stats.
	}	
	else if ( nextaction == 255 ) // END
	{
		gamestate = 4;	
		DisplayCentralIconSucess();
		PlaySoundGood();
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

		EraseAndStopGauge(0x3C); // graphic init (colors) and fill full gauge
		
		gGaudeInUse=1;		
}

// -------------------------------------------------------
void EraseAndStopGauge(u8 fillvalue) // Display black gauge or full gauge
{
	// Fill all 16 characters with 8 bytes of 0x00 (Black)
	// Set up color in color RAM
	u8 i;
	u8 lineonscreen = gOffsetPicY>>3;
	u8* ptrvideo = 	BITMAPRAM+(gOffsetPicX+12)*8+gOffsetPicY*40;
	for (i=0; i<16; i++)
	{
		ptrvideo[0]=fillvalue; // 0x00 is pixel black. 0x3C is two center pixel to COLOR3 (colorram)
		ptrvideo[1]=fillvalue;
		ptrvideo[2]=fillvalue;
		ptrvideo[3]=fillvalue;
		ptrvideo[4]=fillvalue;
		ptrvideo[5]=fillvalue;
		ptrvideo[6]=fillvalue;
		ptrvideo[7]=fillvalue;
		ptrvideo += 8*40;
	}
	ptrvideo = COLORRAM+(gOffsetPicX+12)+lineonscreen*40;
	for (i=0; i<8; i++)
	{
		ptrvideo[0]=0x05; // Green
		ptrvideo += 40;
	}
	ptrvideo[0]=0x0A; // Orange
	ptrvideo += 40;
	ptrvideo[0]=0x0A; // Orange
	ptrvideo += 40;
	ptrvideo[0]=0x0A; // Orange
	ptrvideo += 40;
	ptrvideo[0]=0x0A; // Orange
	ptrvideo += 40;
	
	ptrvideo[0]=0x02; // Red
	ptrvideo += 40;	
	ptrvideo[0]=0x02; // Red
	ptrvideo += 40;	
	ptrvideo[0]=0x02; // Red
	ptrvideo += 40;	
	ptrvideo[0]=0x02; // Red
	ptrvideo += 40;		
	
	gGaudeInUse=0;
}

// -------------------------------------------------------
// Gauge
void DisplayGauge()
{
	u8 switchpoint; // 128 to 8. Top part is black (128 down to switchpoint. Bottom part color depend of value. Green, yellow, red).
	u8 i;
	u8 currentchar; // Divide 128 in 16 chars. 0 to 15
	u8 nbblackline; // 0 to 7 (on current char how much black line there are)
	u8* ptrvideocurrent;
	u8* ptrvideo = 	BITMAPRAM+(gOffsetPicX+12)*8+gOffsetPicY*40; // Start of bitmap Gauge
	
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

	switchpoint=(gGauge>>8); // Switch point is 128 to 0

	currentchar = 15 - (switchpoint / 8 ); // 0 to 15
	nbblackline = 7 - (switchpoint % 8); // 0 to 7
	
	// Display current char
	ptrvideocurrent = ptrvideo + 8*40*currentchar;
	for (i=0; i < nbblackline ; i++)
		ptrvideocurrent[i]=0x00;
	for (i=nbblackline; i < 8 ; i++)
		ptrvideocurrent[i]=0x3c;
	
	// Erase previous char (to be sure)
	if (currentchar>0)
	{
		ptrvideocurrent = ptrvideo + 8*40*(currentchar-1);
		for (i=0; i < 8 ; i++)
			ptrvideocurrent[i]=0x00;
	}
}

// -------------------------------------------------------
void EraseTopScreen()
{
	u8 i;
	u8* ptr1 = (BITMAPRAM);
	u8* ptr2 = (SCREENRAM);
	u8* ptr3 = (SCREENRAM);
	for (i=0; i<18; i++)
	{
		memset(ptr1,0,  320);
		memset(ptr2,0,   40);
		memset(ptr3,0,   40);
		ptr1+=320;
		ptr2+=40;
		ptr3+=40;		
	}
}

// -------------------------------------------------------
void EraseBanner()
{
	// Display HiRes picture
	// 320*8 bytes
	// source : lda pixelshi
	// Dest   :	sta $2000+(320*LINEYHIRES)
	//DisableRom();
	u8 i;
	u8* ptr1 = (BITMAPRAM+320*18);
	u8* ptr2 = (SCREENRAM+(40*18));
	u8* ptr3 = (SCREENRAM+(40*18));
	for (i=0; i<7; i++)
	{
		memset(ptr1,0,  320);
		memset(ptr2,0,   40);
		memset(ptr3,0,   40);
		ptr1+=320;
		ptr2+=40;
		ptr3+=40;		
	}
	//EnableRom();
}

// -------------------------------------------------------
void EraseScreen()
{
	memset((char*)(BITMAPRAM) ,0,       320*25);
	memset((char*)(SCREENRAM) ,0,        40*25);
	memset((char*)(SCREENRAM) ,0,        40*25);
}

// -------------------------------------------------------	
void PlaySoundGood()
{
	if (gSkippingLevel)
		return;
	if (gSoundGoodHaveBeenPlayedThisFrame)
		return;
	#ifdef WITHMUSIC
	if (!gMusicDoubleSpeed)
	{
		asm("lda #<%v", sfx_good);
		asm("ldy #>%v", sfx_good);
		asm("ldx #14");	// Voice 3 (0 , 7 or 14)
		asm("jsr $606");
	}
	else
	{
		asm("lda #<%v", sfx_goodx2);
		asm("ldy #>%v", sfx_goodx2);
		asm("ldx #14");	// Voice 3 (else 0 , 7 or 14)
		asm("jsr $606");
	}
	#endif
	gSoundGoodHaveBeenPlayedThisFrame=1;
}
// -------------------------------------------------------	
void PlaySoundBad()
{
	if (gSkippingLevel)
		return;	
	#ifdef WITHMUSIC
	if (!gMusicDoubleSpeed)
	{	
		asm("lda #<%v", sfx_bad);
		asm("ldy #>%v", sfx_bad);
		asm("ldx #14");	// Voice 3 (else 7 or 14)
		asm("jsr $606");
	}
	else
	{	
		asm("lda #<%v", sfx_badx2);
		asm("ldy #>%v", sfx_badx2);
		asm("ldx #14");	// Voice 3 (else 7 or 14)
		asm("jsr $606");
	}
	#endif
}
// -------------------------------------------------------	
void PlaySoundWarning()
{
	if (gSkippingLevel)
		return;	
	#ifdef WITHMUSIC
	if (!gMusicDoubleSpeed)
	{	
		asm("lda #<%v", sfx_warning);
		asm("ldy #>%v", sfx_warning);
		asm("ldx #14");	// Voice 3 (else 7 or 14)
		asm("jsr $606");
	}
	else
	{	
		asm("lda #<%v", sfx_warningx2);
		asm("ldy #>%v", sfx_warningx2);
		asm("ldx #14");	// Voice 3 (else 7 or 14)
		asm("jsr $606");
	}
	#endif
}

// -------------------------------------------------------
// Digit voice (not used on C64)	 		
void PlayRandomSound()
{
	PlaySoundBad();
}

// -------------------------------------------------------
// UpdatePaletteMode
// Called each frame, at start of interrupt.
// This will replace the color page (so can have problem with icons)
void UpdatePaletteMode()
{
	u8 prevaltpic = gPicAltCurrentIndex; // 0 to xxx
	
	if (gPaletteMode==1) // Blink (Level 52)
	{
		gPaletteNextSwap--;
		
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap=5 + (rand()%16) ; // (cpct_getRandom_lcg_u8(0) >> 4); // 0 to 16
			gNextPalette = !gNextPalette;
		}
		if (gNextPalette==1)
			DisplayPicAlt(1); // cpct_setPalette(palettedark, 16); 
		else
			DisplayPicAlt(0); // cpct_setPalette(palette, 16); 
	}
	else if (gPaletteMode==4) // Fall
	{
		gPaletteNextSwap--;
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap=7;
			gNextPalette--;
			gNextPalette = gNextPalette % 4;
		}		
		if (gNextPalette==0)
			DisplayPicAlt(0); // cpct_setPalette(paletteFall1, 16); 
		else if (gNextPalette==1)
			DisplayPicAlt(1); // cpct_setPalette(paletteFall2, 16); 
		else if (gNextPalette==2)
			DisplayPicAlt(2); // cpct_setPalette(paletteFall3, 16);
		else if (gNextPalette==3)
			DisplayPicAlt(3); // cpct_setPalette(paletteFall4, 16); 
	}	
	else if (gPaletteMode==5 || gPaletteMode==8 ) // River, bomb numbers
	{
		gPaletteNextSwap--;
		if (gPaletteNextSwap==0)
		{
			gPaletteNextSwap=7;
			gNextPalette++;
			gNextPalette = gNextPalette % 3;
		}		
		if (gNextPalette==0)
			DisplayPicAlt(0); // cpct_setPalette(paletteRiver3, 16); 
		else if (gNextPalette==1)
			DisplayPicAlt(1); // cpct_setPalette(paletteRiver1, 16); 
		else if (gNextPalette==2)
			DisplayPicAlt(2); // cpct_setPalette(paletteRiver2, 16);
	} 
	else if (gPaletteMode==6) // Diablo
	{
		gPaletteNextSwap--;
		
		if (gPaletteNextSwap==0)
		{
			//gPaletteNextSwap= 20 + (cpct_getRandom_lcg_u8(0) >> 3);
			gPaletteNextSwap= 20 + (rand()%32);
			gNextPalette = !gNextPalette;
		}
		if (gNextPalette==1)
			DisplayPicAlt(1); // cpct_setPalette(paletteDiablo, 16); 
		else
			DisplayPicAlt(0); // cpct_setPalette(palette, 16); 
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
			DisplayPicAlt(0); //cpct_setPalette(palette, 16); 
		else if (gNextPalette==1)
			DisplayPicAlt(1); //cpct_setPalette(paletteFire2, 16); 
		else if (gNextPalette==2)
			DisplayPicAlt(2); //cpct_setPalette(paletteFire3, 16);
	} 

	// If we changed the alt pic then redrawn any on screen icon
	if ( prevaltpic != gPicAltCurrentIndex )
	  RedisplayLastIconColors();
}

//#define DEBUGSTRINGSIZE (32+34+36+34+32+1)
//u8 stringdebug[DEBUGSTRINGSIZE];

// -------------------------------------------------------
//
// MAIN LOOP
//
// -------------------------------------------------------
int MainLoop(void)
{
	// Local vars 
	u8 x, y;   // Sprite coordinates
	u8 i,j;
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
	
	// Init vars
	//gSfxDeathCount=0;
	gNbDeath=0; // Reset total number of death
	gMessageFailTimeCurrent=0;
	gGauge=(128<<8);
	gGaugeSpeed=(1<<8);
	gTime=0; // Seconds
	gTimeFrag=0; // 50th of seconds
	gTimePrecise=0; // Frame precise (0 to 255) 
	gNextLevelName=firstlevelload;
    gQTEForceNoWait=0; // For level 92
	gSkippingLevel=0;
	gMusicDoubleSpeed=0;	
#ifndef ALLOWSKIPLEVEL	
	gCheatEnable=0; // Allow user to skip level
#else
	gCheatEnable=1; // Force cheat code to be enable at start
#endif
	gCheatSequenceCorrectLetters=0;

	// -- Init C64
	InitJoy();		
	InitBitmap();
	InitSprites(spriteFrames, spriteCols, spriteRows, spriteColors);

	EraseScreen();	
	EnterBitmapMode();

	DisableSprite(0);
	LocateSprite(20, 20);
	SetSprite(0, 0);
	
	// Wait until 'SPACE' is pressed
	// Play music until keyboard is pressed

	gInterruptInactive=1; 
	gInterruptMusicInactive=1;
	waitRaster(100); // Wait a line at position xxx.
	InstallRaster();

	// -- Intro
#ifndef NOINTRO	
	MainLoopIntro();
#endif
	//DEBUGCOLOR(3); // Debug. Change Background color

	// -- Load main game
	musicload[6]=0x4e; // New SID by default
#ifndef ISSIDEB
	musicload[5]='0'; // Tutorial
#else
	musicload[5]='6'; // Level 6
#endif
	
	// Debug. Start specific level
#ifdef STARTLEVELH
	firstlevelload[5]=STARTLEVELH;
	firstlevelload[6]=STARTLEVELL;
	musicload[5]=STARTLEVELH;
#endif	

	// Load game data
	if (cbm_load ("game.bin", getcurrentdevice(), gAllDataBloc) == 0) 
	{
		POKE(53280,4); // Debug. Change Background color. Purple=4. Use for ERROR
		return 0;
	}

	// Debug
	//memset(stringdebug,0x20,DEBUGSTRINGSIZE); // Space
	//stringdebug[DEBUGSTRINGSIZE-1]=0;
 
 	// -- Load TimeUp texts
	{
		FILE* fp;
		u16* ptr;
		u16 ptrvalue;
		DEBUGCOLOR(2); // Debug. Change Background color
		
		if (cbm_load ("timeup.bin", getcurrentdevice(), gTimeUpMessages) == 0) 
		{
			POKE(53280,4); // Debug. Change Background color. Purple=4. Use for ERROR
			return 0;
		}		
		
		DEBUGCOLOR(0); // Debug. Change Background color
		// Init ptr
		ptr = (u16*) gTimeUpMessages; // 7 pointer at beginning of buffer
		for (x=0; x<7; x++)
		{
		  ptrvalue = ptr[x];		
		  gTimeUpDeathsPtr[x] =gTimeUpMessages+ptrvalue+(7*2); // Add base adress to offset, and 14 bytes for pointer zone (first offset is 0)
		}
	}
	
	while(1)  // General loop, including level load and change.
	{
		u8 loop;
		u16* ptr;
		u16 ptrvalue;
		u8* ptr8;
		
		// -- Load Level (c64) ----------------------
		#ifdef CUTMUSICDURINGLOADING
		// Fix : Sometime sound can be heard a bit while loading
				unsigned char filterandvolume = PEEK(0xd418); // filter mode and main volume control
				filterandvolume = filterandvolume & 0xF8; // volume to zero
				POKE(0xd418,filterandvolume);
		#endif
			
		if (cbm_load (gNextLevelName, getcurrentdevice(), gLevelBuffer) == 0) 
		{
			POKE(53280,4); // Debug. Change Background color. Purple=4. Use for ERROR
			return 0;
		}		
		DEBUGCOLOR(0); // Debug. Change Background color
		// Init pointers (offset are inside buffer)
		ptr = (u16*) gLevelBuffer;
		ptrvalue = ptr[0];
		ptr8 = gLevelBuffer + ptrvalue; //struct sleveldata* gLevelData;
		memcpy(&gLevelData,&ptr8,2);
		ptrvalue = ptr[1];
		gNextLevelName = gLevelBuffer + ptrvalue; //u8* gNextLevelName;
		ptrvalue = ptr[2];
		gZonesData = gLevelBuffer + ptrvalue; //u8* gZonesData;
		ptrvalue = ptr[3];
		gScenario = gLevelBuffer + ptrvalue; //u8* gScenario;
		ptrvalue = ptr[4];
		gMessageIntro = gLevelBuffer + ptrvalue;
		for (i=0;i<32;i++)
		{
			ptrvalue = ptr[5+i];
			gMessageSucces[i]=0;
			if (ptrvalue!=0)
				gMessageSucces[i] = gLevelBuffer + ptrvalue;	 //u8* gMessageSucces[32];	
		}
		for (i=0;i<32;i++)
		{
			ptrvalue = ptr[32+5+i];
			gMessageDeath[i]=0;
			if (ptrvalue!=0)
				gMessageDeath[i] = gLevelBuffer + ptrvalue;	 //u8* gMessageDeath[32];
		}
		ptrvalue = ptr[32+5+32];
		gTilesData = gLevelBuffer + ptrvalue;
		
#ifdef WITHMUSIC	
		// -- Load new music 
		if (gLevelData->position==1) // If first image, then redisplay background
		{
			DEBUGCOLOR(2); // Debug. Change Background color
			
			if (cbm_load (musicload, getcurrentdevice(), 0x600) == 0) 
			{
				POKE(53280,5); // Debug. Change Background color. Purple=4. Use for ERROR
				return 0;
			}			
			
			DEBUGCOLOR(0); // Debug. Change Background color
		}
#endif		
		EraseBanner(); // Erase loading icon

		x=8;
		y=8;
		gCursorMode=0; // 0 = Default mode
		gCurrentScenarioLine = gScenario;  
		firepressedlast=0;
		SetOffsetImage(gLevelData->position); // Init offset of level (1=Left 2=center 3=right)
		if (gLevelData->position==1) // If first image, then redisplay background
		 DisplayBackground(); 
		
		DisplayPic();
		resetzones();
		
		// Set Default palette effect = No effect
		gPaletteMode=0; // 0=normal 1=blink 2=smoke 3=smoke&blood 4=anim palette fall(82) 5=river(82) 6=Diablo(81) 7=fire 8=bomb numbers
		gPaletteNextSwap=10; // >0 in frame
		gNextPalette=1; // 0=Palette 1=PaletteAlt
		gCursorMode=0;	
		gQte_auto_mode=0;		
		
		InitNextAction(); // Init variable gamestate // 0=Playing (cursor) 1=State=dead 2=Display text 3=QTE  
	
		// Initialize the song to be played
#ifdef WITHMUSIC		
		if (gLevelData->position==1)
		{
			asm("lda #0"); // SubTune 1
			asm("jsr $0600"); // GoatTracker2 SID Init. (Dojo #3 sub tunes)
		}
		else
		{
	#ifdef CUTMUSICDURINGLOADING
			// Setback volume
			unsigned char filterandvolume = PEEK(0xd418); // filter mode and main volume control
			filterandvolume = filterandvolume | 0x07; // volume to 15
			POKE(0xd418,filterandvolume);
	#endif
		}
#endif 		
		shouldloopingame = 1; // We want to loop for game sequence
		gInterruptInactive=0; // Inside interrupts things are processed.
		gInterruptMusicInactive=0;

		// Banner (After interrupts are set, so that colors will be ok and no glitch)
		DisplayBanner();
		// -- Intro text
		displayline2(gMessageIntro);

		gIconLastDisplayedId=0; // No icons displayed at start.
		
		//028B	Repeat Key: Speed Counter ($04)
		//028C	Repeat Key: First repeat delay Counter ($10)
		//POKE(0x028B,1); // Temp counter ? no effect
		//POKE(0x028C,1); // Temp counter ? no effect
		//POKE(650,64); // Disable repeat if all keys(work fine)
		
		while(shouldloopingame) // This is false when we change level
		{
			u8 c64keypressed=0;
			gSoundGoodHaveBeenPlayedThisFrame=0;
			//u8 getkeyresult=0;
			
			// Get joy value. All to 1, and pressed buttons are 0
			myjoy = PEEK(0xdc01) & PEEK(0xdc00); // GetJoy(0); // BUG: This always send back 255. (lda 0xdc01)
			//getkeyresult=PEEK(197); // getkey();
			//getkeyresult=PEEK(203); // (linked to 197) 0xcb // getkey(); // 7 up and down, 2 right and left
			//getkeyresult=PEEK(0xdc01); 
			// myjoy can be mixed with keyboard
			/*
			{
				u8 stringdebug[4];
				u8 centaines = getkeyresult/100;
				u8 dizaines = ( getkeyresult - (centaines*100) ) / 10;
				stringdebug[0]=0x30+centaines;
				stringdebug[1]=0x30+dizaines;
				stringdebug[2]=0x30+(getkeyresult%10);
				stringdebug[3]=0;
				displayline2(stringdebug);
			} 
			*/
			
			// Manage Time
			UpdateTime();
			
			UpdatePaletteMode(); // Update palette effects
			
			firepressed=0;
			//if (cpct_isKeyPressed(Key_Space) || cpct_isKeyPressed(Joy1_Fire1) || cpct_isKeyPressed(Joy0_Fire1)) 
			//	firepressed=1;
			if ((myjoy&0x10)==0)	
				firepressed=1;
			
			//c64keypressed = getkeyresult;
			
			if ( kbhit() )
				c64keypressed = cgetc();
			else 
				c64keypressed = 0;
			
			
			/*
			{
				u8 stringdebug[40];
				u8 centaines = c64keypressed/100;
				u8 dizaines = ( c64keypressed - (centaines*100) ) / 10;
				stringdebug[0]=0x30+centaines;
				stringdebug[1]=0x30+dizaines;
				stringdebug[2]=0x30+(c64keypressed%10);
				displayline2(stringdebug);
			} 
			*/
			
			if ( c64keypressed == ' ' )
				firepressed=1;
			
			#ifdef WITHMUSIC
			// Test SFX
			//if ( c64keypressed == 's' )
			//{
			//	PlaySoundGood();
			//}
			#endif

			// Skip level ?
			if (gCheatEnable) // Cheat. Skip level
			{
				if (c64keypressed=='n') 
				{
					gSkippingLevel=1; // Do not play SFX while forcing all actions
					while ( gCurrentScenarioLine[0] != 255 )
					{
						if ( gGaudeInUse == 1 )
							EraseAndStopGauge(0x00);						
						if (gCurrentScenarioLine[0]==251) // QTE, skip 10 more bytes
						 gCurrentScenarioLine+=10;
						gCurrentScenarioLine+=10;
						InitNextAction();
					}
					gamestate=4; // End, go to next level
					firepressed=1; // Simulate fire press
					firepressedlast=0;
					gSkippingLevel=0;	
					//gInterruptMusicInactive=1;// When jumping to next level music is inactive			
				}
			}
			else if (c64keypressed!=0)
			{
				// is user activating the cheat code ?
				// Cheat code activation is "SOS"
				// gCheatSequenceCorrectLetters
				if ( gCheatSequenceCorrectLetters == 0 && (c64keypressed=='s' ) )
					gCheatSequenceCorrectLetters = 1;
				else if ( gCheatSequenceCorrectLetters == 1 && (c64keypressed=='s' ) )
				{
					// do nothing, wait that S stop beeing pressed
				}				
				else if ( gCheatSequenceCorrectLetters == 1 && (c64keypressed=='o' ) )
					gCheatSequenceCorrectLetters = 2;
				else if ( gCheatSequenceCorrectLetters == 2 && (c64keypressed=='o' ) )
				{
					// do nothing, wait that S stop beeing pressed
				}				
				else if ( gCheatSequenceCorrectLetters == 2 && (c64keypressed=='s' ) )
				{
					bordercolor(COLOR_WHITE);
					gCheatEnable = 1;
					bordercolor(COLOR_BLACK);
				}
				else
					gCheatSequenceCorrectLetters=0;
			}
			// -- End cheat code skip level
			
			if (gamestate!=0) // Disable sprites if mode is not "playing" else keep it enabled
			  DisableSprite(0); 
			 
			if (gamestate==0) // Playing
			{
				u8 iskeyup,iskeydown,iskeyleft,iskeyright;
				iskeyup=0;
				iskeydown=0;
				iskeyleft=0;
				iskeyright=0;
				// If cursor have moved, then erase the tiles where he was (4 tiles)
				//erasecursor(xold,yold); // Coords in bytes
				// Check if user has pressed a Cursor Key and, if so, move the sprite if
				// it will still be inside screen boundaries
			
				if      ((myjoy&0x01)==0)        iskeyup = 1; // cpct_isKeyPressed(Key_CursorUp) || cpct_isKeyPressed(Joy1_Up) || cpct_isKeyPressed(Joy0_Up) ;
				else if ((myjoy&0x02)==0)        iskeydown= 1; //cpct_isKeyPressed(Key_CursorDown) || cpct_isKeyPressed(Joy1_Down) || cpct_isKeyPressed(Joy0_Down);
				else if ( c64keypressed == 145 ) iskeyup = 1;
				else if ( c64keypressed == 17 )  iskeydown = 1;				
				
				if      ((myjoy&0x04)==0)        iskeyleft= 1; //cpct_isKeyPressed(Key_CursorLeft) || cpct_isKeyPressed(Joy1_Left) || cpct_isKeyPressed(Joy0_Left);
				else if ((myjoy&0x08)==0)        iskeyright= 1; //cpct_isKeyPressed(Key_CursorRight) || cpct_isKeyPressed(Joy1_Right) || cpct_isKeyPressed(Joy0_Right);
				else if ( c64keypressed == 157 ) iskeyleft = 1;
				else if ( c64keypressed == 29 )  iskeyright = 1;
				
				// Special mode for cursor. 0=normal. 1=Slow and random (hit), 
				// 2=reversed (all) 3=reversed Up, 4=reversed horizontal 5=always falling
				if (gCursorMode==0 || gCursorMode>=8) // Normal mode & "do not show zone" mode & 9 show only sometimes
				{
					if      ( iskeyright ) x+=4; 
					else if ( iskeyleft ) x-=4; 
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
				}
				else if (gCursorMode==1) // 1=Slow and random (hit)
				{
					u8 xrand,yrand;
					xrand = rand() % 4 ; // 0 1 2 3
					if ( xrand < 2 ) x += xrand;
					else x -= (xrand-2);
					yrand = rand() % 4 ; // 0 1 2 3
					if ( yrand < 2 ) y += xrand;
					else y -= (yrand-2);
					if      ( iskeyright ) x+=2; 
					else if ( iskeyleft ) x-=2; 
					if      ( iskeyup  ) y-=2;
					else if ( iskeydown ) y+=2;
				}
				else if (gCursorMode==2)  // 2=reversed (all)
				{
					if      ( iskeyright ) x-=4; 
					else if ( iskeyleft ) x+=4; 
					if      ( iskeyup  ) y+=4;
					else if ( iskeydown ) y-=4;
				}
				else if (gCursorMode==3) // 3=reversed Up
				{
					if      ( iskeyright ) x+=4; 
					else if ( iskeyleft ) x-=4; 
					if      ( iskeyup  ) y+=4;
					else if ( iskeydown ) y-=4;
				}
				else if (gCursorMode==4) // 4=reversed horizontal
				{
					if      ( iskeyright ) x-=4; 
					else if ( iskeyleft ) x+=4; 
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
				}
				else if (gCursorMode==5) // 5=always falling
				{
					if      ( iskeyright ) x+=4; 
					else if ( iskeyleft ) x-=4; 
					y+=2;
					if (y==(128 - SP_H) )
						y=0;
				}
				else if (gCursorMode==6) // 6=always going right
				{
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
					x++;
					if ( x >= (96-SP_W) ) x = 0;
				}	
				else if (gCursorMode==7) // 7=always going left
				{
					if      ( iskeyup  ) y-=4;
					else if ( iskeydown ) y+=4;
					x--;
					if (x > 200 ) x=(96-SP_W); // Negative
				}	
				// gCursorMode == 8 // Do not show color
				// gCursorMode == 9 // show only sometimes
				// Limit Cursor
				if (x > 200 ) x=0; // If x become negative (<0)
				else if ( x >= (96-SP_W) ) x = (96-SP_W);
				if (y > 200 ) y=0; // If x become negative (<0)
				else if ( y >= (128 - SP_H) ) y = (128 - SP_H);	
				// Save position and get pointed zone
				pointedzone = GetPointedZone(x/8,y/8);
				// Get video memory byte for coordinates x, y of the sprite (in bytes)
				// Draw the sprite in the video memory location got from coordinates x, y
				
				if (gCursorMode!=9 || ( ( rand() % 4 ) == 1 ) )
				{
					EnableSprite(0);
					LocateSprite((gOffsetPicX*8)+x+2, gOffsetPicY+y+5);
					if ( pointedzone == 0 && (gCursorMode!=8) ) // Cursor mode 8 , all is red
						SetSprite(0, 1); // Sprite 0, Frame 0 or 1 (green, red)
					else
						SetSprite(0, 0); // Sprite 0, Frame 0 or 1 (green, red) 
				}
				else
					DisableSprite(0);
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
							EraseAndStopGauge(0x00);						
						gCurrentScenarioLine+=10;
						InitNextAction();
						PlaySoundGood(); // Sound GOOD
					}
					else if (actiontodo < 200) // 101 to 200 are deaths
					{
						u8 message = actiontodo-100; // first message is 101
						// Death
						u8 indexoftext = (message-1);
						//gSfxDeathCount++; // Increase nb deaths for SFX play
//#define SFXDEATHNUMBERBEFOREPLAYSOUND 4 // Each 4 death a digitalised voice is played
						//if (gSfxDeathCount<SFXDEATHNUMBERBEFOREPLAYSOUND)
						//{
							textboldcolor = TEXTCOLORBAD; // 0x27 (red) 0x47 (purple)
							displayline2(gMessageDeath[indexoftext]);
							textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)
						//}
						//else
						//	gSfxTextDeathSave=gMessageDeath[indexoftext];
						// Erase gauge is it was used
						if ( gGaudeInUse == 1)
						{
							EraseAndStopGauge(0x00);	
						}							
						// Go to state = 1 (cursor will not be displayed again)
						//if (gSfxDeathCount<SFXDEATHNUMBERBEFOREPLAYSOUND)
						gamestate=1;
						//else
						//{
						//	gamestate=6;
						//	gSfxDeathCount=0;
						//}
						// Display dead cursor
						//erasecursor(xold,yold); // Coords in bytes
						//gPaletteMode=0; // Reset death
						gCursorMode=0;
						DisplayCentralIconDeath();	
						PlaySoundBad(); //Sound BAD
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
						textboldcolor = TEXTCOLORBAD; // 0x27 (red) 0x47 (purple)
						displayline2(gTimeUpDeathsPtr[gMessageFailTimeCurrent]);
						textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)
						gMessageFailTimeCurrent++;
						if (gMessageFailTimeCurrent==NBMESFAILTIME)
							gMessageFailTimeCurrent=0;
						// Go to state = 1 (cursor will not be displayed again)
						gamestate=1;
						// Display dead cursor
						//erasecursor(xold,yold); // Coords in bytes
						gPaletteMode=0; // Reset death
						gCursorMode=0;
						DisplayCentralIconDeath();	
						PlaySoundBad(); //Sound BAD
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
						pScript[1]=35; // Short time.
						pScript[2]=1;
						pScript[3]=101;
						// -- 
						pScript[4]=1 + (rand() % 4);
						pScript[5]=1 + (rand() % 4);
						pScript[6]=1 + (rand() % 4);
						pScript[7]=1 + (rand() % 4);
						pScript[8]=255;
						gQTEForceNoWait=1; // QTE will not wait for a key
						InitNextAction();
						gQTEForceNoWait=0;
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
			else if (gamestate==7) // -- BEFORE QTE. WAIT FOR KEY
			{
				if (firepressed!=0 && firepressedlast==0) // Wait space to continue
				{
					gamestate=3; // Start QTE
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
				
				if      ((myjoy&0x01)==0) uppressed = 1; // cpct_isKeyPressed(Key_CursorUp) || cpct_isKeyPressed(Joy1_Up) || cpct_isKeyPressed(Joy0_Up) ;
				else if ((myjoy&0x02)==0) downpressed= 1; //cpct_isKeyPressed(Key_CursorDown) || cpct_isKeyPressed(Joy1_Down) || cpct_isKeyPressed(Joy0_Down);
				else if ( c64keypressed == 145 )     uppressed = 1;
				else if ( c64keypressed == 17 ) downpressed = 1;

				if      ((myjoy&0x04)==0) leftpressed= 1; //cpct_isKeyPressed(Key_CursorLeft) || cpct_isKeyPressed(Joy1_Left) || cpct_isKeyPressed(Joy0_Left);
				else if ((myjoy&0x08)==0) rightpressed= 1; //cpct_isKeyPressed(Key_CursorRight) || cpct_isKeyPressed(Joy1_Right) || cpct_isKeyPressed(Joy0_Right);
				else if ( c64keypressed == 157 )     leftpressed = 1;
				else if ( c64keypressed == 29 ) rightpressed = 1;
				
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
						PlaySoundWarning();
						gTimePrecise=0;
						QTEstate=3;
					}
				}				
				else if (QTEstate==3) // Display START icon for 1s
				{
					if (gTimePrecise>=50) // 1s
					{
						EraseLastIcon(); // Erase center icon
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
							PlaySoundGood(); // Sound GOOD							
							
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
								gQte_next = 100 + (rand() % 64); // 100 + 64
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
							PlaySoundGood(); // Sound GOOD
							
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
						textboldcolor = TEXTCOLORBAD; // 0x27 (red) 0x47 (purple)
						displayline2(gMessageDeath[(message-1)]);
						textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)						
						// Go to state = 1 (cursor will not be displayed again)
						gamestate=1;
						gPaletteMode=0; // Reset death
						gCursorMode=0;						
						// Display death icon
						DisplayCentralIconDeath();	
						PlaySoundBad(); //Sound BAD
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
					u8 levelnumber=gNextLevelName[5]; // Ascii 
					// Init next level
					// Init next music to load

					if (gLevelData->position==3 || (levelnumber=='1' && gLevelData->position==2) || (levelnumber=='6' && gLevelData->position==1) ) // Tuto and disk 
					{	
						musicload[5]=levelnumber; // Music name
						// 0x4e = N ... Index 6
						// 0x4f = O ... Index 6	
						musicload[6]=0x4e; // Defaut is new SID
						gMusicDoubleSpeed=0;
						if (levelnumber=='9' || levelnumber=='3' || levelnumber=='1')
							gMusicDoubleSpeed=1;
						if (gIsOldSID)
						{
							if (levelnumber=='1' || levelnumber=='2' || levelnumber=='3' || levelnumber==0x43 || levelnumber==0x5A ) // 43 = C = Credit , 5A = Z = demo end
								musicload[6]=0x4f; // Explicit Old SID	
						}	
					}
					gamestate=8;
					// In all case stop music (asynchronous play not working in all cases
					// We stop it now, so that next step, volume will be set to zero. (to fix bug ; sometime audio is heard during loading)
					#ifdef CUTMUSICDURINGLOADING
					gInterruptMusicInactive=1;
					#endif					
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
			/*
			else if (gamestate==6) // -- Death with sound (play sound)
			{
				EraseBanner(); // Erase bottom of screen (we are going to stay in mode 0)	
				//cpct_removeInterruptHandler();
				//cpct_akp_stop(); // Stop music
				//cpct_setPalette(palette, 16);  					
				//textzonereached=1; // to passe the end of this while loop
				gInterruptReached=1; // to pass the end of this while loop
				//cpct_setVideoMode(0);				
				PlayRandomSound(); // This is blocking !!
				gamestate=1; // Go death wait spacebar.
				//interrptcount=0;
				//setInterruptAtVSYNCStart(myInterruptHandler);	
				DisplayBanner();
				textboldcolor = TEXTCOLORBAD; // 0x27 (red) 0x47 (purple)
				displayline2(gSfxTextDeathSave);
				textboldcolor = TEXTCOLORGOOD; // 0x27 (red) 0x47 (purple)				
			}	
			*/		
			// Go to next level
			else if (gamestate==8) // -- DO THE LEVEL CHANGE (so always start at beginning of a frame)
			{
#ifdef WITHMUSIC
				// Only set volume to 0 should be enough.
				{
			#ifdef CUTMUSICDURINGLOADING
					unsigned char filterandvolume = PEEK(0xd418); // filter mode and main volume control
					filterandvolume = filterandvolume & 0xF8; // volume to zero
					POKE(0xd418,filterandvolume);
			#endif
				}
#endif						
				shouldloopingame=0; // Exit main loop and do level load.
				EraseBanner(); // Erase bottom of screen (we are going to stay in mode 0)	
				gInterruptInactive=1; // Will not change resolution anymore
				gInterruptReached=1; // to pass the end of this while loop
				MultiModeOn(); // Low res bitmap mode
				DisplayPicture(IconFloppy , 18, 128+24); // Display floppy
				
				// Test side change
				if (gNextLevelName[5]=='6')	// Test side change.
				{
					if (cbm_load ("side2.bin", getcurrentdevice (), gLevelBuffer+8000) == 0) 
					{
						// Side is not changed, so restart level
						gCurrentScenarioLine = gScenario;
						#ifdef WITHMUSIC		
						if (gLevelData->position==1)
						{
							asm("lda #0"); // SubTune 1
							asm("jsr $0600"); // GoatTracker2 SID Init.
						}
						else
						{
							// Setback volume
							unsigned char filterandvolume = PEEK(0xd418); // filter mode and main volume control
							filterandvolume = filterandvolume | 0x07; // volume to 15
							POKE(0xd418,filterandvolume);
						}
						#endif
						shouldloopingame = 1; // We want to loop for game sequence
						gInterruptInactive=0; // Inside interrupts things are processed.
						gInterruptMusicInactive=0;
						DisplayBanner();
						// -- Intro text
						displayline2(gMessageIntro);
						InitNextAction(); // Init variable gamestate // 0=Playing (cursor) 1=State=dead 2=Display text 3=QTE  
					}					
				} // if side b test
			}	
			
			firepressedlast = firepressed;

			while (!gInterruptReached) // Wait interrupt flag
			{
			}
			gInterruptReached=0;
			
		} // While loop in game
		
	
	} // While 1 (all levels)
	
//#ifdef WITHMUSIC	
//	// Stop music
//	StopMusic();
//#endif

    // Done
    return EXIT_SUCCESS;	
}

// -------------------------------------------------------	
// DisplayCharacter
// One character from font
void DisplayCharacter(unsigned char chartodisplay, u8 x, u8 y)
{
	u8 charoffset=0;
	u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8* colorramptr=0;
	u8* dataptr = 0;
	u8* increment=0;
	u8 column;
	u8 line;
	column=x;
	line=y>>3;
	bitmapramptr = BITMAPRAM+column*8+y*40;
	increment=column+line*40;
	screenramptr = SCREENRAM+increment;
	colorramptr  = COLORRAM+increment;	
	// Point on data ptr (loaded data)
	dataptr=FonteIntro; // Start of bitmap bin file (my format)
	dataptr+=4; // Skip header
	charoffset=(chartodisplay-0x20);
	dataptr+= (charoffset*8);
	memcpy(bitmapramptr,dataptr,8); // Copy 8 bytes
	dataptr = FonteIntro + 4 + (8*59) + charoffset;
	memcpy(colorramptr,dataptr,1); // Copy 1 byte
	dataptr += 59;
	memcpy(screenramptr,dataptr,1); // Copy 1 byte
}

// -------------------------------------------------------	
// DisplayCharacterProgressive
// One character from font
// progressive from bottom 0=nothing, 1=one line at bottom, 8=all line displayed
void DisplayCharacterProgressive(unsigned char chartodisplay, u8 x, u8 y, u8 step)
{
	u8 i;
	u8 blanklines;
	u8 charoffset=0;
	u8* bitmapramptr=0;
	u8* screenramptr=0;
	u8* colorramptr=0;
	u8* dataptr = 0;
	u8* increment=0;
	u8 column;
	u8 line;
	blanklines=(8-step)%8;
	column=x;
	line=y>>3;
	bitmapramptr = BITMAPRAM+column*8+y*40;
	increment=column+line*40;
	screenramptr = SCREENRAM+increment;
	colorramptr  = COLORRAM+increment;	
	// Point on data ptr (loaded data)
	dataptr=FonteIntro; // Start of bitmap bin file (my format)
	dataptr+=4; // Skip header
	charoffset=(chartodisplay-0x20);
	dataptr+= (charoffset*8);
	for (i=0;i<blanklines;i++)
		bitmapramptr[i]=0;
	bitmapramptr += blanklines;
	memcpy(bitmapramptr,dataptr,step); // Copy "step" bytes (lines)
	dataptr = FonteIntro + 4 + (8*59) + charoffset;
	memcpy(colorramptr,dataptr,1); // Copy 1 byte
	dataptr += 59;
	memcpy(screenramptr,dataptr,1); // Copy 1 byte
}

// -------------------------------------------------------	
// X in column
// Y in lines
void drawString(u8* ptext, u8 x, u8 y)
{
	while (ptext[0]!=0)
	{
		DisplayCharacter(ptext[0],x,y);
		ptext++;
		x++;
	}
}

// -------------------------------------------------------
// u8 gTextScrollAnimationCounter=255; // 0 to 49 = active, >49 = inactive
// u8 gTextScrollString[41]; // One line
// gTextScrollY
void UpdateScrollString()
{
	if (gTextScrollAnimationCounter>49)
		return;
	
	// Test display char progressive
	{
		signed char start = gTextScrollAnimationCounter;
		signed char end = start - 8;
		u8 step = 1;
		signed char i;
		for (i=start; i>end; i--)
		{
			if (i>=0 && i<=39)
				DisplayCharacterProgressive(gTextScrollString[i],i,gTextScrollY,step);
			step++;
		}
	}
	gTextScrollAnimationCounter++;
}


unsigned char BlocRed[14]={
    4,8,0,0, // x,y,back color, nb colors buffers
    0x55,0x55,0x55,0x55,0x55,0x55,0x55,0x55,
    0x00,
    0x20,
};

unsigned char BlocBlack[14]={
    4,8,0,0, // x,y,back color, nb colors buffers
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,
    0x00,
};

#define BLOODYRAINDURATION 127

u8  bloodspeed[40] = { 135,67,57,147,117,87,95,57,62,113,112,54,115,112,64,87,67,134,51,147,137,157,127,177,62,113,102,146,97,145,128,100,97,136,128,54,139,147,137,157 };
u16 bloodfract[40];
u8  bloody[40];
u8  bloodyraincounter; // 0 means off
u8  bloodyraincolor; // 0 = black 1=red

// ------------------------------
// 0 = black 1=red
void BloodyRainInit(u8 color)
{
	u8 i;
	bloodyraincounter=1;
	bloodyraincolor=color;
	for (i=0 ; i< 40 ; i++)
	{
		bloodfract[i]=0;
		bloody[i]=0;
	}
}



// ------------------------------
// Name : BloodyRainUpdate
void BloodyRainUpdate()
{
	u8 x;
	if (bloodyraincounter==0)
		return;
	if (bloodyraincounter>BLOODYRAINDURATION)
	{
		// If was red then switch to black
		if (bloodyraincolor==1)
			BloodyRainInit(0); // Restart the count
		else
			bloodyraincounter=0;
		return;
	}
	bloodyraincounter++;
	
	for(x=0;x<40;x++)
	{
		bloodfract[x]+=bloodspeed[x];
		if(bloodfract[x]>255)
		{
			bloodfract[x]-=256;
			if(bloody[x]<25) 
			{
				bloody[x]++;
				if (bloodyraincolor==1)
					DisplayPicture(BlocRed,x,(bloody[x]-1)*8);
				else
					DisplayPicture(BlocBlack,x,(bloody[x]-1)*8);
			}
		}
	}
}

// -------------------------------------------------------
//
// MAIN LOOP INTRO
//
// -------------------------------------------------------
int MainLoopIntro(void)
{
	// Local vars 
	u8 x, y;   // Sprite coordinates
	u8 i,j;
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
	u16 globalcount=0;
	
	// Init vars

	// -- Load part 1
	if (cbm_load ("intro1.bin", getcurrentdevice (), gAllDataBloc) == 0) 
	{
		POKE(53280,4); // Debug. Change Background color. Purple=4. Use for ERROR
		return 0;
	}

	// Display Loading text and flag
	drawString(IntroTexts,0,96);
	DisplayPicture(FlagLogo,2,200-40);

	// -- Load part 2 (Resistance logo + Resistance audio)
	if (cbm_load ("intro2.bin", getcurrentdevice (), gAllDataBloc+1246) == 0) 
	{
		POKE(53280,4); // Debug. Change Background color. Purple=4. Use for ERROR
		return 0;
	}
	
	EraseScreen();	
	playsampleaudio(sndResistance,30);
	DisplayPicture(LogoRSE2_C64,5,48);
	drawString(IntroTexts+41*1,0,48+64); // "Present"
	drawString(IntroTexts+41*0,0,200-16); // "Loading"	
	
	// -- Load part 3 (Image title + Ninja sound + music)
	if (cbm_load ("intro3.bin", getcurrentdevice (), gAllDataBloc+1246) == 0) 
	{
		POKE(53280,4); // Debug. Change Background color. Purple=4. Use for ERROR
		return 0;
	}

	{
		//u8 color=1; //Debug
#ifdef WITHMUSIC	
		if (cbm_load ("musicin.bin", getcurrentdevice (), 0x600) == 0) 
		{
			POKE(53280,4); // Debug. Change Background color. Purple=4. Use for ERROR
			return 0;
		}
#endif		

		//POKE(53280,1); // Debug. Change Background color

#ifdef WITHMUSIC		
		asm("lda #0"); // SubTune 1		
		asm("jsr $0600"); // GoatTracker2 SID Init. (Dojo #3 sub tunes)
#endif 	
	
		shouldloopingame = 1; // We want to loop for game sequence
		gInterruptInactive=1; // Inside interrupts things are processed.
		gInterruptMusicInactive=0; // Music should be played
		
		//POKE(53280,1); // Debug. Change Background color
		
		// Wait until 'SPACE' is pressed
		// Play music until keyboard is pressed
		//waitRaster(100);
		//InstallRaster();

		gTextScrollString[40]=0; // End string always 0
		
#define LOGOAPPEAR              (50*3)
#define LOGODISAPPEAR           (LOGOAPPEAR+50*3)
#define TITLEAPPEAR             (LOGODISAPPEAR+3*50)
#define SCROLLSTART             (TITLEAPPEAR+8*50)
#define SCROLLSPACEBETWEENTEXTS (50*3)
#define ENDSCROLL               (SCROLLSTART+SCROLLSPACEBETWEENTEXTS*6)		
		
		// Debug
		globalcount=LOGODISAPPEAR-1;
		//BloodyRainInit(1); // Red

		while(shouldloopingame)
		{
			u8 c64keypressed;
			u8 firepressed=0;
			u8 myjoy;
			//u8 string[41];
			//string[40]=0;
			
			//POKE(53280,color++); // Debug. Change Background color
			
			//DisableSprite(0);
			
			// Get joy value. All to 1, and pressed buttons are 0
			myjoy = PEEK(0xdc01) & PEEK(0xdc00);
			firepressed=0;
			if ((myjoy&0x10)==0)	
				firepressed=1;
			// Testkeyboard
			if ( kbhit() )
				c64keypressed = cgetc();
			else 
				c64keypressed = 0;
			if ( c64keypressed == ' ' )
				firepressed=1;
	
			// If fire or keyboard, then end scroll
			if (firepressed)
				globalcount=ENDSCROLL;
			
			BloodyRainUpdate(); // Always update bloody rain. Counter=0 means innactive
		
			// Steps 
			//if (globalcount==LOGOAPPEAR) // 3s display logo
			//{
			//	DisplayPicture(LogoRSE2_C64,5,48);
			//	strncpy(gTextScrollString,"                presente                ",40);
			//	gTextScrollAnimationCounter=0;
			//	gTextScrollY=48+64;
			//	//LoadGame();
			//}
			if (globalcount==LOGODISAPPEAR) // 3s display logo
				EraseScreen();
			else if (globalcount==TITLEAPPEAR) // 3s display logo
			{			
				DisplayPicture(ninja_title_025_Final160,0,0);
			}
			else if (globalcount==SCROLLSTART)
				drawString("                                        ",0,200-8);
			else if (globalcount==SCROLLSTART+10)
			{
				strncpy(gTextScrollString,IntroTexts+41*2,40);
				gTextScrollAnimationCounter=0;
				gTextScrollY=200-8;				
				//drawString("   histoire et programmation : oriens   ",0,200-8);	
			}				
			else if (globalcount==SCROLLSTART+SCROLLSPACEBETWEENTEXTS*1)
			{
				strncpy(gTextScrollString,IntroTexts+41*3,40);
				gTextScrollAnimationCounter=0;
				gTextScrollY=200-8;				
				//drawString("        graphismes : grass & ced        ",0,200-8);		
			}				
			else if (globalcount==SCROLLSTART+SCROLLSPACEBETWEENTEXTS*2)
			{
				strncpy(gTextScrollString,IntroTexts+41*4,40);
				gTextScrollAnimationCounter=0;
				gTextScrollY=200-8;				
				//drawString("             musique : xxxxx            ",0,200-8);		
			}				
			else if (globalcount==SCROLLSTART+SCROLLSPACEBETWEENTEXTS*3)
			{
				strncpy(gTextScrollString,IntroTexts+41*5,40);
				gTextScrollAnimationCounter=0;
				gTextScrollY=200-8;				
				//drawString("                logo : fra              ",0,200-8);		
			}				
			else if (globalcount==SCROLLSTART+SCROLLSPACEBETWEENTEXTS*4)
			{
				strncpy(gTextScrollString,IntroTexts+41*6,40);
				gTextScrollAnimationCounter=0;
				gTextScrollY=200-8;				
				//drawString("         copyright 2020 resistance      ",0,200-8);	
			}				
			else if (globalcount==SCROLLSTART+SCROLLSPACEBETWEENTEXTS*5)
			{
				strncpy(gTextScrollString,IntroTexts+41*7,40);
				gTextScrollAnimationCounter=0;
				gTextScrollY=200-8;				
				//drawString("    appuyez sur espace ou bouton feu    ",0,200-8);	
			}				
			else if (globalcount==ENDSCROLL-1)
			{
				globalcount=SCROLLSTART-1; // Restart text
			}
			else if (globalcount==ENDSCROLL)
			{
				//EraseScreen();
				BloodyRainInit(1); // Red and black rain				
			}
			else if (globalcount==ENDSCROLL+BLOODYRAINDURATION*2)
			{
				#ifdef WITHMUSIC	
				// Stop music
				gInterruptMusicInactive=1;
				ResetSID(); // Stop music = Not working !!
				waitRaster(1);
				//StopMusic(); // This is not working. (but no need to stop SID)
				#endif
				playsampleaudio(sndNinja,30);
				gInterruptReached=1;
				InstallRaster();
	
				strncpy(gTextScrollString,IntroTexts+41*0,40); // "Please wait Loading"
				gTextScrollAnimationCounter=0;
				gTextScrollY=96;				
				//drawString(IntroTexts+41*0,0,96);	
				DisplayPicture(FlagLogo,2,200-40);
			}
			else if (globalcount==ENDSCROLL+(BLOODYRAINDURATION*2)+(40*2))
			{
				shouldloopingame=0; // quit main loop
			}			

			//-- end steps
			
			UpdateScrollString(); // Display text progressive
			
			while (!gInterruptReached) // Wait interrupt flag
			{
			}
			gInterruptReached=0;
			
			globalcount++;
			
		} // While loop in game
		
	
	} // While 1 (all levels)
	
	//DEBUGCOLOR(1); // Debug. Change Background color

    // Done
    return EXIT_SUCCESS;	
}

// ----------------------------------------------------------------------------
// History of main tasks:
// DONE : With "True drive emulation" data are not read. Using Fread is not ok by default, need to use cbm_load. All binary data must have 2 extra bytes which hold adress(not used here) 
// DONE : Do replay can be outside ? Check Miniplayer == Useless
// DONE : Find routine for playing samples (digitized voices)
// DONE : Find some SFX : Good / BAD / Warning
// DONE : Interrupts are sometimes not setup (and the whole game loop is frozen) ==> I've done a fix, to test more longuer.
// DONE : Redo font (R is bad)
// DONE : Font : White is not readable as color
// DONE : Check random code ==> ok with Joystick, so nothing to check
// DONE : Recenter Cursors
// DONE : Manage cursor on/off
// DONE : Read correct joystick value
// DONE : Check background display (slow ?) ==> optimised once
// DONE : Check display picture (slow). memcpy is optimized ? => Yes optimised
// DONE : How to disable interrupt (during loading) and enable them again ==> Keep the irq but not doing anything/ TODO : Joystick is not working fine. IT is because I setup interrupts ?
// DONE : find 2 musics goattracker 2 : dojo.sng / hyperspace.sng
// DONE : Make test with goat tracker. 2 musics (about 4Ko). 
// DONE : GoatTracker2 is choosen as tracker
// ----------------------------------------------------------------------------
