// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL83.BIN"

// -------------------------------------------------------
#define NBZONES 1

u8 leveldata[2] = { 3 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	0,0,3,3,1, // 
};

// -------------------------------------------------------
// Sequences
// -- Choice with cursor
// Time (in 10th of seconds 100=10s), for all zone, sucess or fails (fails = id + 100 )
//  { 100,101,101,101,1,0,0,0,0,0,0,0 } 
// -- Test. 250  Textid then 0
// 	{ 250,1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
// -- QTE
//    251 TIME WINTEXT ERRORTEXT sequence 1=haut, 2=bas, 3=droite, 4=gauche
// 	{ 251,25 , 8, 103, 1  , 1  , 2  ,2  , 0  , 0 },
//  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
// -- END
//  { 255,
// ----------------------------------------------------
#define SEQLINESIZE 10
u8 scenario[13][SEQLINESIZE] =
{
	{ 250, 1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 2  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 3  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 4  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 255,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL91 BIN"; // Must be 11 characters

// -------------------------------------------------------
/*
"                                      "; // 38
*/

u8 messageintro[] = {
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je marche un peu le long de la  "
 "berge et enfin la voila.          "
"\2NURE-ONNA\1 !!!! Cette conne est en   "
 "train de brosser ses cheveux au   "
  "bord de la rivi�re.             "
};
u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 1
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "none"
}

};

#define NBMESSSUCESS 4 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Ne te laisse pas avoir par son  "
 "apparence flatteuse. C'est une    "
"vraie saloperie en r�alit�. Les     "
 "pires monstres se cachent sous    "
  "l'aspect de belles personnes... "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Tiens tu sais que Nure-onna �a  " 
 "veut dire \2FEMME HUMIDE\1 ? C'est pas"
"ce que tu penses gros pervers ! Elle"
 "vit au bord de l'eau et souvent   "
  "elle noie ses victimes.         "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Bon mes armes sont pr�tes. J'ai "
 "mon fid�le sabre qui me sera bien "
"utile ici. De ton c�t�, tu es pr�t  "
 "pour ce combat ? Si tu as besoin  "
  "\2VA FAIRE TON PIPI\1 maintenant !  "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Presse \ACTION\1 pour d�marrer !   "
 "On va lui faire manger ses morts �"
"cette chiure d�moniaque !!!         "
 "                                  "
  " \2GOOOOOOOOO\1                     "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x99, 0x33, 0x99, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x66, 0x66, 0x33, 0x33, 0x99, 0x99, 0x33, 0x33,
	0x99, 0xcc, 0x99, 0x99, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x66, 0x33, 0x33, 0x33, 0x33, 0x99, 0x99,
	0x99, 0x33, 0x33, 0x33, 0x66, 0x66, 0x66, 0x66,
	0x33, 0x66, 0x66, 0x33, 0x99, 0x99, 0x99, 0x33,
	0x99, 0x99, 0x99, 0x99, 0x33, 0x33, 0x33, 0x33,
	0x66, 0xcc, 0x33, 0x33, 0x33, 0x33, 0x33, 0x66,
	0x99, 0x99, 0x99, 0x99, 0x33, 0x33, 0x33, 0x33,
	0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x33, 0x33,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x33, 0x33,
	0xcc, 0x66, 0x33, 0x66, 0x33, 0x33, 0x33, 0x33,
	0xcc, 0xcc, 0x99, 0x33, 0x33, 0x33, 0x33, 0x33,
	0xcc, 0xcc, 0xcc, 0x66, 0x33, 0x33, 0x66, 0x66,
	0x9d, 0xcc, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x91, 0x33,
	0x33, 0x66, 0x33, 0x33, 0x62, 0x62, 0x62, 0x62,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x91, 0x91, 0x91,
	0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33,
	0x33, 0x33, 0x33, 0xcc, 0x33, 0x33, 0x62, 0x33,
	0x33, 0x33, 0x66, 0x70, 0x64, 0x33, 0x91, 0x91,
	0x33, 0x33, 0x32, 0x0a, 0x1a, 0x33, 0x33, 0x33,
	0x33, 0x33, 0xd8, 0x00, 0x50, 0x99, 0x62, 0x62,
	0x66, 0x33, 0xaa, 0x00, 0x05, 0xbb, 0x91, 0x33,
	0x99, 0x66, 0x20, 0x1a, 0x0a, 0x60, 0x62, 0x62,
	0x33, 0x33, 0xa0, 0xe1, 0xa0, 0xb1, 0x91, 0xc0,
	0x33, 0x66, 0xa0, 0x09, 0xaa, 0x4a, 0x62, 0xc0,
	0x33, 0x66, 0x0a, 0x75, 0x0d, 0x11, 0xc0, 0xc0,
	0x33, 0x77, 0x0a, 0x0e, 0x92, 0x40, 0xc0, 0x91,
	0x33, 0xdd, 0x50, 0xd7, 0x06, 0x15, 0x62, 0x62,
	0x99, 0x32, 0x50, 0x49, 0xff, 0x15, 0x91, 0x91,
	0x33, 0x91, 0x33, 0x91, 0x91, 0xc0, 0x62, 0xc0,
	0x33, 0x91, 0x62, 0x33, 0x62, 0x91, 0x33, 0xc0,
	0x33, 0x33, 0x91, 0x91, 0xc0, 0xc0, 0x62, 0xc0,
	0x62, 0x62, 0x62, 0x62, 0x91, 0xc0, 0xc0, 0x91,
	0xc0, 0x91, 0x91, 0x91, 0x91, 0xc0, 0xc0, 0x62,
	0x62, 0x62, 0x33, 0x62, 0xc0, 0xc0, 0x91, 0x91,
	0x91, 0x91, 0x91, 0xc0, 0xc0, 0x62, 0x62, 0x62,
	0x62, 0x62, 0xc0, 0xc0, 0xc0, 0x91, 0x91, 0xc0,
	0x91, 0xc0, 0x91, 0xc0, 0x62, 0xc0, 0xc0, 0xc0,
	0x33, 0x62, 0x91, 0x91, 0x33, 0xc0, 0xc0, 0xc0,
	0x91, 0xc0, 0xc0, 0xc0, 0x62, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x62,
	0xc0, 0xc0, 0x62, 0x62, 0xc0, 0x62, 0xc0, 0x91,
	0xc0, 0x91, 0x91, 0x91, 0x91, 0xc0, 0xc0, 0xc0,
	0x62, 0xc0, 0xc0, 0x62, 0x62, 0xc0, 0xc0, 0xc0,
	0x91, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x91, 0xc0,
	0x6e, 0x6e, 0xcc, 0x33, 0x66, 0x33, 0x33, 0x66,
	0xcc, 0x9d, 0xcc, 0x99, 0x99, 0x99, 0x99, 0x33,
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x33,
	0x9d, 0xcc, 0x9d, 0x9d, 0xcc, 0xcc, 0x99, 0x99,
	0x6e, 0x6e, 0x6e, 0x6e, 0x6e, 0x6e, 0xcc, 0x66,
	0x9d, 0xcc, 0x9d, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc,
	0x3f, 0x6e, 0x6e, 0x6e, 0xcc, 0x9d, 0x9d, 0xcc,
	0x6e, 0x3f, 0x3f, 0x9d, 0x9d, 0xcc, 0x6e, 0xcc,
	0x9d, 0x9d, 0x9d, 0x6e, 0xcc, 0x6e, 0xcc, 0x6e,
	0x3f, 0x6e, 0x3f, 0x3f, 0x9d, 0x9d, 0x9d, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x6e, 0x6e, 0x6e,
	0x95, 0x95, 0x6a, 0x6a, 0x3f, 0x3f, 0x3f, 0x3f,
	0x6a, 0x6a, 0x3f, 0x95, 0x3f, 0x95, 0x3f, 0x6e,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x95, 0x95, 0x6a, 0x6a, 0x6a, 0x6a, 0x3f, 0x95,
	0x6a, 0x6a, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x33, 0xdd, 0x05, 0x75, 0x86, 0x44, 0xc0, 0x62,
	0x33, 0x32, 0x05, 0xa4, 0xba, 0x44, 0xc0, 0xc0,
	0x33, 0x98, 0x00, 0x1a, 0xd2, 0x55, 0xc0, 0x91,
	0x99, 0xd8, 0x00, 0x87, 0x25, 0x10, 0x6a, 0xc0,
	0xcc, 0xaf, 0x00, 0xa4, 0xa5, 0x50, 0xc0, 0xc0,
	0x99, 0xaa, 0x05, 0x25, 0x20, 0x05, 0x6a, 0xc0,
	0x66, 0x20, 0x0e, 0xba, 0x0a, 0x00, 0x6a, 0xc0,
	0x99, 0xaa, 0xa4, 0x7f, 0x0a, 0x05, 0xc8, 0x95,
	0xcc, 0x20, 0x75, 0x17, 0xa5, 0x00, 0x9d, 0xc0,
	0xcc, 0x20, 0x5d, 0x2b, 0x20, 0x00, 0x60, 0x6a,
	0xcc, 0xa0, 0xff, 0x17, 0x87, 0x00, 0x35, 0xc0,
	0xcc, 0xa0, 0xab, 0x3f, 0x52, 0x0a, 0xe4, 0x6a,
	0x6e, 0x0a, 0xff, 0x17, 0x3a, 0x00, 0xb5, 0xc0,
	0x9d, 0x0a, 0xab, 0x6a, 0x3a, 0x0a, 0x4e, 0x6a,
	0x6e, 0x00, 0xff, 0x17, 0xd5, 0xa0, 0x44, 0x6a,
	0x3f, 0x0a, 0xab, 0x6a, 0x81, 0x0a, 0x1a, 0x3f,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x62, 0xc0, 0x62,
	0xc0, 0x62, 0xc0, 0xc0, 0x91, 0x91, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x62, 0xc0, 0x91, 0x91, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0x91, 0xc0, 0x62, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x95, 0x95, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x6a, 0x6a, 0x6a, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x95, 0x95, 0x95, 0x95, 0x95, 0xc0, 0xc0, 0xc0,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x6a, 0xc0, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a,
	0xbf, 0xc0, 0x95, 0x95, 0x95, 0x3f, 0x3f, 0x3f,
	0x75, 0x6a, 0xc0, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a,
	0x1a, 0xbf, 0xc0, 0xc0, 0x95, 0x95, 0x95, 0x95,
	0x05, 0x75, 0x17, 0x95, 0xc0, 0x3f, 0x3f, 0x3f,
	0x0a, 0x1a, 0xc7, 0x83, 0x83, 0x46, 0x46, 0x46,
	0x05, 0x05, 0xcb, 0x61, 0x5a, 0x61, 0x61, 0x61,
	0x0a, 0x0a, 0x0a, 0x18, 0x92, 0x92, 0x92, 0x0a,
	0x0f, 0x0f, 0x24, 0x83, 0x2b, 0x2b, 0x83, 0xc3,
	0x30, 0x92, 0xc3, 0x17, 0x17, 0x17, 0x17, 0x17,
	0x61, 0x21, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b,
	0x92, 0x97, 0x17, 0x17, 0x17, 0x17, 0x3f, 0x3f,
	0x61, 0x2b, 0x2b, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f,
	0x97, 0x17, 0x17, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x95, 0x00, 0x5d, 0x42, 0x66, 0xa0, 0x10, 0x6a,
	0x3f, 0x00, 0x21, 0x13, 0x89, 0xa5, 0x50, 0x3f,
	0x6a, 0x00, 0x75, 0x42, 0x46, 0x20, 0x50, 0x3f,
	0x3f, 0x00, 0xf5, 0x13, 0x89, 0xaf, 0x05, 0x9d,
	0x3f, 0x00, 0xf5, 0x42, 0x66, 0x02, 0x00, 0x9d,
	0x6e, 0x00, 0x0e, 0x13, 0x91, 0x8d, 0x05, 0x35,
	0x89, 0x00, 0x5f, 0x42, 0x66, 0x72, 0x00, 0x35,
	0x12, 0x00, 0x04, 0x13, 0x91, 0x90, 0x0a, 0x4b,
	0x70, 0x00, 0x0e, 0x42, 0x62, 0x77, 0x00, 0x0e,
	0x00, 0x00, 0x10, 0xc8, 0x91, 0x81, 0x0a, 0x04,
	0xa0, 0x00, 0x5a, 0x42, 0xc0, 0x95, 0x0a, 0x5a,
	0x87, 0x00, 0x05, 0x99, 0xc0, 0x95, 0x0a, 0x41,
	0x25, 0x00, 0x0f, 0x13, 0x33, 0x66, 0x00, 0x01,
	0x82, 0x00, 0x05, 0x89, 0x99, 0x9c, 0x0a, 0x1f,
	0x25, 0x00, 0x0a, 0x46, 0x46, 0x7c, 0x00, 0x21,
	0xa0, 0x00, 0x05, 0x89, 0x89, 0x9c, 0x00, 0x9d,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95,
	0xc0, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x2b,
	0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x57,
	0x95, 0x95, 0x95, 0x95, 0x95, 0x95, 0x2b, 0xff,
	0x3f, 0x6a, 0x6a, 0x6a, 0x6a, 0x3f, 0x57, 0x61,
	0x3f, 0x95, 0x95, 0x3f, 0x17, 0x57, 0xba, 0x92,
	0x2b, 0x2b, 0x2b, 0x03, 0xab, 0xeb, 0x61, 0x61,
	0xc3, 0xc3, 0xc3, 0xc3, 0x92, 0x92, 0x92, 0x92,
	0x49, 0x49, 0x49, 0x61, 0x61, 0x61, 0x61, 0x49,
	0x8e, 0x8e, 0x0c, 0x86, 0x86, 0x86, 0x86, 0x86,
	0x58, 0x4d, 0x4d, 0x4d, 0x4d, 0x5d, 0x7f, 0x7f,
	0x17, 0xbf, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
	0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b,
	0x17, 0x3f, 0x3f, 0x3f, 0x3f, 0x17, 0x3f, 0x3f,
	0x3f, 0x95, 0x95, 0x95, 0x3f, 0x3f, 0x17, 0x17,
	0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x3f, 0x2b, 0x2b,
	0x2b, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f,
	0x17, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f,
	0x2b, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x7f,
	0x17, 0x3f, 0x3f, 0x3f, 0x3f, 0x6e, 0x6e, 0x7f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x9d, 0x9d, 0x7f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x9d, 0x6e, 0x6e, 0x7f,
	0x3f, 0x3f, 0x6e, 0x6e, 0x3f, 0x3f, 0x9d, 0x3f,
	0x3f, 0x3f, 0x9d, 0x9d, 0x3f, 0x3f, 0x3f, 0x3f,
	0x6e, 0x3f, 0x3f, 0x3f, 0x3f, 0x6e, 0x6e, 0x6e,
	0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0x9d, 0xcc, 0x9d,
	0x6e, 0xcc, 0x6e, 0xcc, 0xcc, 0x6e, 0x6e, 0x6e,
	0x9d, 0x9d, 0x9d, 0x99, 0x99, 0x99, 0xcc, 0xcc,
	0xc3, 0xcc, 0xcc, 0x66, 0x66, 0x66, 0x66, 0x66,
	0x24, 0xc3, 0xc6, 0xcc, 0x99, 0x33, 0x33, 0x33,
	0x18, 0x0f, 0x4b, 0xc6, 0xc6, 0x33, 0x66, 0x66,
	0x0a, 0x00, 0x0a, 0x66, 0x33, 0x66, 0x05, 0x3f,
	0x0a, 0x05, 0x05, 0x81, 0x99, 0x33, 0x50, 0x9d,
	0x00, 0x00, 0x5a, 0xc4, 0x62, 0x62, 0x1a, 0x3f,
	0x00, 0x05, 0x10, 0x91, 0x62, 0x33, 0xe4, 0x3f,
	0x0a, 0x0a, 0x5f, 0xc4, 0x62, 0x62, 0x35, 0x3f,
	0xa5, 0x05, 0xb5, 0x91, 0x62, 0x62, 0x9d, 0x6a,
	0x70, 0x5a, 0x75, 0x91, 0x62, 0xc0, 0x3f, 0x95,
	0xba, 0xb0, 0xbf, 0x91, 0xc0, 0x62, 0x9d, 0x6a,
	0x3f, 0xff, 0x60, 0xc0, 0x62, 0xc0, 0x3f, 0x3f,
	0x6e, 0x7f, 0xbf, 0x91, 0xc0, 0x62, 0x9d, 0x3f,
	0x9d, 0xdd, 0xea, 0xc0, 0xc0, 0xc0, 0x3f, 0x6e,
	0xcc, 0x57, 0x35, 0x91, 0xc0, 0x62, 0x3f, 0x3f,
	0xcc, 0xba, 0xea, 0xc0, 0xc0, 0xc0, 0x3f, 0x9d,
	0xcc, 0xff, 0x60, 0xc0, 0xc0, 0xc0, 0x9d, 0x3f,
	0x99, 0x57, 0x42, 0x62, 0xc0, 0x95, 0x3f, 0x6e,
	0x66, 0x23, 0x6a, 0xc0, 0xc0, 0xc0, 0x6e, 0x3f,
	0x95, 0x3f, 0xc0, 0x95, 0x95, 0x95, 0x3f, 0x3f,
	0x3f, 0x3f, 0x6a, 0x6a, 0x95, 0xc0, 0x6a, 0x3f,
	0x3f, 0x6a, 0x3f, 0x95, 0xc0, 0x95, 0x95, 0x95,
	0x3f, 0x6a, 0x3f, 0x3f, 0x6a, 0xc0, 0xc0, 0x6a,
	0x3f, 0x3f, 0x3f, 0x95, 0xc0, 0xc0, 0xc0, 0x95,
	0x6a, 0x3f, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0xc0,
	0x95, 0x95, 0x6a, 0x95, 0x95, 0x95, 0x95, 0xc0,
	0x6a, 0x6a, 0x6a, 0x6a, 0xc0, 0x6a, 0xc0, 0xc0,
	0x3f, 0x3f, 0x3f, 0x3f, 0xc0, 0xc0, 0xc0, 0xc0,
	0x3f, 0x3f, 0x3f, 0x6a, 0x6a, 0xc0, 0x6a, 0xc0,
	0x6e, 0x6e, 0x3f, 0x3f, 0x95, 0xc0, 0x95, 0xc0,
	0x9d, 0x9d, 0x6a, 0x3f, 0x3f, 0x6a, 0xc0, 0xc0,
	0x6e, 0x3f, 0x3f, 0x3f, 0x95, 0xc0, 0x95, 0x95,
	0x3f, 0x6e, 0x3f, 0x6a, 0xc0, 0x6a, 0x6a, 0xc0,
	0x3f, 0x3f, 0x3f, 0x3f, 0x95, 0x3f, 0x95, 0x95,
	0x6e, 0x3f, 0x6e, 0x3f, 0x3f, 0x6a, 0x6a, 0xc0,
	0x24, 0x0d, 0x25, 0x0f, 0x61, 0xc6, 0x33, 0x33,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0a, 0x18, 0xc6, 0x33,
	0x0f, 0x24, 0x06, 0x25, 0x0f, 0x0f, 0x4b, 0x99,
	0x1a, 0x4c, 0x4c, 0x49, 0x18, 0x1a, 0x1a, 0x93,
	0x24, 0x06, 0x8c, 0x06, 0x0b, 0x86, 0x0d, 0x64,
	0x49, 0x18, 0x49, 0x0f, 0x61, 0xc9, 0xc9, 0x49,
	0x0a, 0x0e, 0x86, 0x86, 0x83, 0xc6, 0xc6, 0xc6,
	0x4b, 0x0f, 0x18, 0x1a, 0x49, 0x61, 0x43, 0x49,
	0x0e, 0x86, 0x87, 0x86, 0x0f, 0x0e, 0xc6, 0x92,
	0x0f, 0x0f, 0x4b, 0x61, 0x0d, 0x0d, 0x49, 0x4b,
	0x24, 0x0e, 0x92, 0x1a, 0x86, 0x05, 0x92, 0x86,
	0x0f, 0x18, 0x0f, 0x0f, 0x61, 0x49, 0xc9, 0x43,
	0x24, 0x86, 0x24, 0x86, 0x86, 0xc6, 0x92, 0xc6,
	0x0f, 0x09, 0x0d, 0x4b, 0x49, 0x61, 0x49, 0x4b,
	0x86, 0x06, 0x06, 0x0f, 0x0e, 0x0a, 0x04, 0x92,
	0x0f, 0x4c, 0x49, 0x49, 0xc9, 0x43, 0x4b, 0x49,
	0x99, 0x9d, 0xc0, 0x62, 0xc0, 0x95, 0x3f, 0x9d,
	0x33, 0x62, 0xc4, 0xc0, 0xc0, 0xc0, 0x6e, 0x6e,
	0x33, 0xc8, 0xc0, 0x62, 0xc0, 0x95, 0xcc, 0x9d,
	0x99, 0x62, 0xc4, 0x62, 0xc0, 0xc4, 0xcc, 0xcc,
	0x66, 0x62, 0xc8, 0x62, 0xc0, 0x95, 0x99, 0x99,
	0x99, 0xc8, 0xc4, 0x62, 0xc0, 0xc4, 0x66, 0x66,
	0xc6, 0x62, 0x89, 0x62, 0xc0, 0xc4, 0x99, 0x33,
	0x06, 0x99, 0xc4, 0xc0, 0xc0, 0xc4, 0x66, 0x33,
	0x92, 0xc2, 0x89, 0x62, 0xc0, 0x91, 0x99, 0x66,
	0x24, 0xbb, 0x6c, 0xc0, 0xc0, 0xc4, 0xcc, 0x99,
	0x92, 0xea, 0xc8, 0x62, 0xc0, 0xc4, 0xcc, 0xcc,
	0x5d, 0x13, 0x6c, 0xc0, 0xc0, 0xc1, 0xcc, 0x99,
	0x86, 0x6a, 0xc8, 0x62, 0xc0, 0xc4, 0xc6, 0x66,
	0x57, 0xc4, 0x6c, 0xc0, 0xc0, 0x90, 0xc9, 0x99,
	0x86, 0x6a, 0x68, 0xc0, 0xc0, 0x90, 0x64, 0xc6,
	0x75, 0xc4, 0x6c, 0xc0, 0xc0, 0x90, 0x1a, 0xc9,
	0x6e, 0x6e, 0x3f, 0x9d, 0x3f, 0x95, 0x3f, 0x95,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x6a, 0x6a, 0x6a,
	0x9d, 0x9d, 0x6e, 0x3f, 0x9d, 0x3f, 0x3f, 0x95,
	0x6e, 0xcc, 0x3f, 0x6e, 0x3f, 0x3f, 0x3f, 0x6a,
	0x99, 0x9d, 0x9d, 0xcc, 0x9d, 0x9d, 0x9d, 0x9d,
	0x66, 0x66, 0x6e, 0x6e, 0x3f, 0x9d, 0x3f, 0x3f,
	0x99, 0x99, 0x9d, 0x9d, 0x9d, 0x3f, 0x9d, 0x9d,
	0x33, 0x66, 0xcc, 0x6e, 0x6e, 0x6e, 0xcc, 0x6e,
	0x33, 0x33, 0xcc, 0x9d, 0x9d, 0x9d, 0x9d, 0xcc,
	0x33, 0x66, 0x66, 0xcc, 0xcc, 0x6e, 0x3f, 0x6e,
	0x33, 0x33, 0x99, 0x99, 0xcc, 0xcc, 0x9d, 0x3f,
	0x99, 0x33, 0x66, 0x33, 0x66, 0x66, 0x6e, 0x6e,
	0x33, 0x99, 0x33, 0x33, 0xcc, 0x99, 0x9d, 0x9d,
	0x99, 0x66, 0x33, 0x66, 0x66, 0x66, 0xcc, 0x6e,
	0xcc, 0x99, 0x99, 0x33, 0x33, 0x99, 0xcc, 0xcc,
	0xc9, 0xcc, 0x66, 0x33, 0x33, 0x66, 0x66, 0x66,
	0x0f, 0x0e, 0x07, 0x83, 0xc6, 0x86, 0x25, 0x0e,
	0x0d, 0x0d, 0x49, 0x08, 0x0f, 0x0d, 0x49, 0x49,
	0x0f, 0x24, 0x86, 0x0e, 0x0f, 0x0f, 0x0f, 0x0e,
	0x0a, 0x4b, 0x18, 0x0f, 0x00, 0x4b, 0x49, 0x49,
	0x86, 0x0e, 0x0f, 0x86, 0x8c, 0x06, 0x8c, 0x86,
	0x49, 0x4b, 0x0d, 0x0f, 0x09, 0x09, 0x0d, 0x49,
	0x0e, 0x0f, 0x83, 0x05, 0x25, 0x86, 0x06, 0x0e,
	0x0d, 0x0d, 0x4b, 0x07, 0x0a, 0x0f, 0x49, 0x0e,
	0x05, 0x0e, 0xc6, 0x86, 0x87, 0x0f, 0x0f, 0x0e,
	0x4b, 0x49, 0x49, 0xc9, 0x0f, 0x49, 0x4b, 0x5f,
	0x86, 0x87, 0x0e, 0x83, 0x86, 0x06, 0x86, 0x86,
	0x49, 0x0d, 0x0f, 0x18, 0x49, 0x4c, 0x43, 0x5d,
	0x25, 0x04, 0x0f, 0x87, 0x86, 0x24, 0xd2, 0x5d,
	0x4b, 0x0d, 0x49, 0x49, 0x4b, 0x0f, 0x0d, 0x97,
	0x0f, 0x86, 0x0e, 0x87, 0x0f, 0x86, 0xa5, 0x1d,
	0x49, 0x12, 0x18, 0x18, 0x0d, 0x4b, 0xa0, 0x4e,
	0x83, 0xc4, 0x68, 0x91, 0xc0, 0x85, 0x25, 0x64,
	0x1d, 0x36, 0xb9, 0xc0, 0xc0, 0x90, 0x1a, 0x1a,
	0x97, 0xc4, 0x68, 0x91, 0xc0, 0x85, 0xc3, 0x86,
	0x48, 0x36, 0xb9, 0xc0, 0xc0, 0x90, 0x46, 0x43,
	0xbf, 0x94, 0xe8, 0x91, 0xc0, 0x84, 0x83, 0x89,
	0x48, 0xdc, 0xb9, 0x91, 0xc0, 0xc1, 0x49, 0x43,
	0xbb, 0x94, 0xe8, 0xc4, 0xc0, 0x84, 0x5a, 0xd2,
	0xea, 0xdc, 0x68, 0x91, 0xc0, 0x85, 0x18, 0xa5,
	0x3b, 0xdc, 0xe8, 0xc4, 0x91, 0x85, 0x0e, 0x0f,
	0x6a, 0x7c, 0x68, 0x62, 0xc0, 0xc1, 0x4b, 0x49,
	0x99, 0xdc, 0x68, 0xc4, 0x91, 0x84, 0x86, 0x86,
	0x6e, 0x7c, 0x68, 0x62, 0xc0, 0xc1, 0x4b, 0x0d,
	0x99, 0xdc, 0xc8, 0xc4, 0x91, 0x90, 0x0f, 0x0f,
	0xcc, 0x7c, 0x68, 0xc8, 0xc0, 0x63, 0x49, 0x49,
	0x9c, 0x78, 0x99, 0xc4, 0xc4, 0xd0, 0x5a, 0x86,
	0x6c, 0x7c, 0xc8, 0xc8, 0xc0, 0x27, 0x0f, 0x0f,
	0xc6, 0xc6, 0x99, 0x99, 0x33, 0x33, 0x33, 0x99,
	0x1a, 0xc9, 0xcc, 0x66, 0x33, 0x33, 0x33, 0x66,
	0x25, 0x64, 0xc6, 0x99, 0x99, 0x33, 0x66, 0x33,
	0x1a, 0x1a, 0xc9, 0xcc, 0x66, 0x33, 0x33, 0x33,
	0x86, 0x0f, 0x64, 0xc9, 0x99, 0x99, 0x99, 0x33,
	0xc9, 0x0d, 0x0f, 0xc6, 0xc6, 0xc6, 0x66, 0x66,
	0x83, 0x86, 0x0e, 0x4b, 0xc9, 0xc9, 0x99, 0x99,
	0x49, 0x0d, 0x0d, 0x0d, 0x61, 0xc6, 0xcc, 0xcc,
	0x83, 0x83, 0x86, 0x0e, 0x5a, 0x61, 0xc9, 0x99,
	0x49, 0x43, 0xc9, 0x0d, 0x0d, 0x1a, 0xc6, 0xc6,
	0x86, 0x83, 0xc6, 0x86, 0x24, 0x0f, 0x61, 0xc9,
	0x4b, 0x4b, 0x43, 0x07, 0x18, 0x5a, 0x1a, 0xc3,
	0x05, 0x86, 0x86, 0x0e, 0x87, 0x0f, 0x0f, 0x0f,
	0x0d, 0x4b, 0x18, 0x0d, 0x49, 0x49, 0x4b, 0x0a,
	0x86, 0x0e, 0x24, 0x5a, 0x0e, 0x87, 0xa4, 0x86,
	0x4b, 0x18, 0x4b, 0xa5, 0x0f, 0xc3, 0x49, 0x49,
	0xc6, 0x86, 0x87, 0x0f, 0x86, 0x0e, 0x0a, 0x15,
	0x43, 0xc9, 0x49, 0x18, 0x49, 0x49, 0x00, 0xe6,
	0x92, 0x87, 0x0e, 0x86, 0x04, 0x24, 0x0a, 0xf6,
	0x49, 0x0d, 0x4b, 0x43, 0x0d, 0xa4, 0x00, 0x50,
	0x0e, 0x25, 0x0e, 0xc6, 0x86, 0x86, 0x00, 0x51,
	0x49, 0x0f, 0x0f, 0x43, 0x43, 0x18, 0x0a, 0x50,
	0x0f, 0x87, 0x24, 0x83, 0x86, 0x25, 0x05, 0x50,
	0x49, 0x0a, 0x1a, 0x18, 0x1a, 0x0f, 0x0a, 0x0a,
	0x0f, 0x0f, 0x05, 0x87, 0x05, 0x0a, 0x0e, 0x05,
	0x0d, 0x49, 0x00, 0x0d, 0x49, 0x49, 0x49, 0x0a,
	0x86, 0x0e, 0x25, 0xd2, 0x0f, 0x04, 0x86, 0x86,
	0x43, 0x0f, 0x0a, 0x1a, 0xa5, 0x08, 0x08, 0x18,
	0x86, 0x0f, 0x61, 0x0e, 0x70, 0x0f, 0x0f, 0x0e,
	0xc9, 0x49, 0x49, 0x18, 0x49, 0x49, 0x49, 0xa5,
	0x86, 0x24, 0x24, 0x86, 0x0e, 0x83, 0xd2, 0x86,
	0x43, 0x0f, 0x49, 0xc9, 0x43, 0x12, 0x43, 0x49,
	0x9c, 0xf8, 0x13, 0xc4, 0xc4, 0x85, 0x05, 0x05,
	0x6c, 0x7c, 0xc8, 0xc8, 0xc0, 0x8d, 0x0a, 0x4b,
	0x9c, 0xf8, 0x46, 0x91, 0x91, 0x85, 0x05, 0x04,
	0xfc, 0xf4, 0x89, 0xc8, 0xc8, 0x88, 0x00, 0x4b,
	0xf0, 0xf8, 0x6c, 0x46, 0xc4, 0x88, 0x00, 0x05,
	0x20, 0xf4, 0x9c, 0xcc, 0xcc, 0x88, 0x00, 0x0a,
	0x75, 0x50, 0x6c, 0x6c, 0x6c, 0x28, 0x00, 0x05,
	0xb0, 0xf1, 0xbc, 0xbc, 0x9c, 0xa0, 0x00, 0x00,
	0x50, 0xf3, 0xf4, 0x7c, 0x78, 0xa2, 0x00, 0x05,
	0x0a, 0xf3, 0x75, 0x3f, 0x57, 0xa0, 0x00, 0x0a,
	0x05, 0x51, 0x75, 0x17, 0x2b, 0xaa, 0x05, 0x05,
	0x0a, 0x00, 0xb0, 0x70, 0x70, 0x0a, 0x00, 0x0f,
	0x05, 0x05, 0x00, 0x00, 0x00, 0x00, 0x05, 0x04,
	0x0a, 0x0a, 0x0a, 0x00, 0x00, 0x00, 0x0e, 0x0f,
	0x87, 0x00, 0x00, 0x05, 0x05, 0x05, 0x49, 0x87,
	0x0d, 0x05, 0x00, 0x0a, 0x0a, 0x0a, 0x05, 0x49,
	0xa4, 0xa4, 0x24, 0x8c, 0x43, 0x43, 0x87, 0x86,
	0x18, 0x18, 0x09, 0x4c, 0x09, 0xc3, 0x49, 0x0d,
	0x86, 0x24, 0x24, 0x06, 0x07, 0xd2, 0x0a, 0x86,
	0x49, 0x0d, 0x4b, 0x0a, 0x0f, 0x49, 0x0d, 0xe1,
	0x86, 0x05, 0x86, 0x86, 0x0e, 0x24, 0x24, 0x0a,
	0x4b, 0x18, 0x0d, 0xe1, 0xa5, 0x4c, 0x49, 0x49,
	0xa5, 0x05, 0x0c, 0x0e, 0x06, 0x06, 0x86, 0x87,
	0x49, 0x49, 0x4b, 0x09, 0x09, 0x49, 0x4b, 0x49,
	0x86, 0x87, 0x04, 0x86, 0xd2, 0x0e, 0x0e, 0x0e,
	0x0d, 0x08, 0x4b, 0xa5, 0x0f, 0x41, 0x49, 0x49,
	0x87, 0x0a, 0x86, 0x86, 0x86, 0x87, 0x0f, 0x86,
	0x0f, 0x49, 0x49, 0x18, 0x49, 0x49, 0x4b, 0x0f,
	0x24, 0x86, 0x87, 0x05, 0x25, 0x87, 0x86, 0x86,
	0x4b, 0x18, 0x1a, 0x05, 0x05, 0x0d, 0x4b, 0x61,
	0x86, 0x0f, 0x05, 0x0a, 0x86, 0x86, 0x92, 0xc3,
	0x43, 0x0d, 0x49, 0x49, 0x43, 0x43, 0x43, 0x4b,
	0x86, 0x0e, 0x20, 0x83, 0x83, 0x64, 0x86, 0x87,
	0x0d, 0x1a, 0x49, 0x0a, 0x49, 0xc9, 0x1a, 0x0d,
	0x0e, 0x87, 0x0e, 0x0e, 0x83, 0x87, 0x0f, 0x4b,
	0x0a, 0x4b, 0x0f, 0x49, 0x61, 0x61, 0x49, 0x58,
	0x83, 0x86, 0x86, 0x86, 0x86, 0x87, 0x83, 0x87,
	0xc9, 0x43, 0x61, 0x49, 0x4b, 0x49, 0x43, 0x58,
	0xc6, 0x92, 0x1a, 0xa5, 0xb0, 0x83, 0x89, 0x24,
	0xc9, 0xc9, 0x43, 0xc9, 0x43, 0x4b, 0x43, 0xa5,
	0x64, 0x86, 0x83, 0x83, 0x86, 0x0b, 0x86, 0x86,
	0x12, 0x49, 0x0f, 0x0f, 0x4b, 0x49, 0xe1, 0x0d,
	0x24, 0x24, 0x05, 0x05, 0x86, 0x5a, 0x86, 0x0e,
	0x18, 0x1a, 0x0f, 0x4b, 0xc9, 0x49, 0x49, 0x49,
	0x0e, 0x24, 0x64, 0xc6, 0xc6, 0x86, 0x86, 0xc6,
	0x0f, 0x0f, 0x4b, 0x43, 0x43, 0x58, 0x4b, 0xc9,
	0x04, 0x0a, 0x25, 0x87, 0x0e, 0xa5, 0x83, 0x83,
	0x1a, 0x0d, 0x0a, 0x0f, 0x41, 0x0f, 0x49, 0x43,
	0x82, 0x0e, 0x0f, 0x00, 0x05, 0x05, 0x24, 0xc6,
	0x05, 0x1a, 0x05, 0x0d, 0x1a, 0x49, 0x49, 0x43,
	0x00, 0x25, 0x0e, 0x24, 0x04, 0x70, 0x86, 0x86,
	0x0a, 0x1a, 0x0f, 0x4b, 0x43, 0x0d, 0x49, 0x05,
	0xa5, 0x24, 0x0e, 0xc6, 0x83, 0x0f, 0xa4, 0x87,
	0x5a, 0x49, 0x4b, 0xc9, 0x43, 0x49, 0x0f, 0xe1,
	0x86, 0x87, 0x86, 0x83, 0x87, 0x5a, 0x86, 0xc6,
	0x1a, 0x4b, 0x18, 0x4b, 0x49, 0x49, 0x49, 0x49,
	0x04, 0x87, 0x0e, 0x25, 0x87, 0x87, 0x0e, 0x86,
	0x4b, 0x1a, 0x4b, 0x4b, 0x49, 0xc9, 0xa5, 0x5a,
	0x86, 0x86, 0x86, 0x87, 0xc6, 0xc6, 0x86, 0x86,
	0x12, 0x0f, 0x49, 0x0d, 0x0f, 0x43, 0x43, 0x49,
	0x86, 0x87, 0x0f, 0x61, 0x24, 0x86, 0x87, 0x0f,
	0x0a, 0x4b, 0x0f, 0x0d, 0x4b, 0x18, 0x0f, 0x05,
	0x87, 0x0e, 0x86, 0x82, 0x04, 0x0f, 0x05, 0x05,
	0x49, 0x49, 0x4b, 0x4b, 0x05, 0x18, 0x18, 0x92,
	0x0d, 0x0f, 0x05, 0x04, 0x83, 0x89, 0xd2, 0x86,
	0x0f, 0x49, 0x0d, 0x08, 0x49, 0x18, 0xa5, 0x49,
	0x87, 0x0e, 0x86, 0x0f, 0x04, 0xd2, 0x87, 0x87,
	0xb0, 0x18, 0x98, 0xe1, 0x49, 0xa5, 0x50, 0x0d,
	0x0e, 0x64, 0x24, 0xd2, 0xd2, 0x86, 0x86, 0x0e,
	0x43, 0x12, 0x18, 0x49, 0x49, 0x49, 0x08, 0x4b,
	0xc6, 0x24, 0x86, 0x86, 0x24, 0x87, 0x0a, 0x0c,
	0xc9, 0x49, 0xe1, 0x49, 0xa5, 0x1a, 0x0a, 0x05,
	0xd2, 0x0e, 0x0e, 0x0e, 0x86, 0x87, 0x87, 0x05,
	0x0f, 0xe1, 0x43, 0x07, 0x49, 0x5a, 0x43, 0x08,
	0x86, 0xd2, 0x83, 0x87, 0x0e, 0x21, 0x83, 0x86,
	0x49, 0x49, 0x46, 0x49, 0x18, 0x49, 0x43, 0x43,
	0x25, 0x83, 0x89, 0x86, 0x24, 0x86, 0x86, 0x83,
	0x4b, 0x0f, 0x43, 0x18, 0x18, 0x0d, 0x41, 0x46,
	0x86, 0x86, 0x86, 0x86, 0x24, 0x86, 0x5a, 0x83,
	0x0d, 0x0f, 0x49, 0x4b, 0x49, 0x49, 0x0d, 0x41
};