// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVELZ3.BIN"

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
u8 scenario[5][SEQLINESIZE] =
{
	{ 250, 1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 2  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 3  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 250, 4  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 255,0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVELZ1 BIN"; // Must be 11 characters

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
  "                                "
 "Interfaces et retouches images par"
"                                    "
 "      \2CEDRIC QUETIER (CED)\1        "
  "                                "
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
  "None"
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
  "Image Titre par                 "
 "                                  "
"         \2YOTIS KLAPSAKIS\1            "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Routine chargement fichier par  "
 "                                  "
"       Julien Nevo (Targhan)        "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Routine voix digitalisées par   "
 "                                  "
"         Bruno (Kukulcan)           "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Routine voix digitalisées par   "
 "                                  "
"         Bruno (Kukulcan)           "
 "                                  "
  "                                "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x91,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x16,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x94, 0xfc,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x78, 0xf8,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x94, 0xf4, 0xf4,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x78, 0xf8, 0xf8,
	0xc0, 0xc0, 0xc0, 0xc0, 0x81, 0xf8, 0xf4, 0xf4,
	0xc0, 0xc0, 0xc0, 0xc0, 0x36, 0xf0, 0xf8, 0xf8,
	0xc0, 0xc0, 0xc0, 0xc0, 0x56, 0xf4, 0xf4, 0xf4,
	0xc0, 0xc0, 0xc0, 0x91, 0x78, 0xf0, 0xf8, 0xf8,
	0xc0, 0xc0, 0xc0, 0x81, 0xf8, 0xf4, 0xfc, 0xf4,
	0xc0, 0xc0, 0xc0, 0x94, 0xf0, 0xf0, 0xf8, 0xf8,
	0xc0, 0xc0, 0xc0, 0x72, 0xf0, 0xf8, 0xf4, 0xf4,
	0xc0, 0xc0, 0xc0, 0x52, 0xf8, 0xf0, 0xf8, 0xfc,
	0xc0, 0xc0, 0xc0, 0x7c, 0xfc, 0xf4, 0xf4, 0xf4,
	0xc0, 0x33, 0x03, 0x3c, 0x3c, 0x03, 0x33, 0xc0,
	0x03, 0x3c, 0xfc, 0xfc, 0xf4, 0xf4, 0xbc, 0x29,
	0xfc, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf4,
	0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4,
	0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
	0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4,
	0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8, 0xf8,
	0xf4, 0xf4, 0xf4, 0xf4, 0xfc, 0xf4, 0xf4, 0xf4,
	0xf8, 0xf8, 0xf8, 0xfc, 0xf4, 0xf8, 0xf8, 0xf8,
	0xf4, 0xfc, 0xf4, 0xf4, 0xfc, 0xf4, 0xfc, 0xf4,
	0xf8, 0xf8, 0xf8, 0xfc, 0xf4, 0xfc, 0xf8, 0xf8,
	0xf4, 0xfc, 0xfc, 0xf8, 0xfc, 0xf4, 0xfc, 0xf4,
	0xf8, 0xfc, 0xf4, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8,
	0xf4, 0xfc, 0xfc, 0xfc, 0xfc, 0xf4, 0xfc, 0xf4,
	0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xf8, 0xfc,
	0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xf4, 0xf4,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x62, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x29, 0x62, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf4, 0x39, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf8, 0xf4, 0x62, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf0, 0xf0, 0x39, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf8, 0xf0, 0xa9, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf4, 0xf0, 0xb4, 0x42, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf8, 0xf0, 0xb0, 0x68, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf4, 0xf4, 0xf4, 0xa9, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf8, 0xf0, 0xb0, 0xbc, 0xc0, 0xc0, 0xc0, 0xc0,
	0xf4, 0xf0, 0xf4, 0x74, 0x42, 0xc0, 0xc0, 0xc0,
	0xf0, 0xf8, 0xb0, 0x70, 0x39, 0xc0, 0xc0, 0xc0,
	0xf4, 0xfc, 0xb0, 0x74, 0xb9, 0xc0, 0xc0, 0xc0,
	0xf8, 0xf8, 0xf0, 0x74, 0xa9, 0xc0, 0xc0, 0xc0,
	0xf0, 0xf0, 0xb0, 0xb0, 0x21, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0x91, 0xf8, 0xf8, 0xf0, 0xf8, 0xfc,
	0xc0, 0xc0, 0x81, 0xfc, 0xf4, 0xf4, 0xf4, 0xf4,
	0xc0, 0xc0, 0x94, 0xf8, 0xfc, 0xf8, 0xfc, 0xfc,
	0xc0, 0xc0, 0x36, 0xfc, 0xfc, 0xf4, 0xf4, 0xf4,
	0xc0, 0xc0, 0x56, 0xf8, 0xf8, 0xf8, 0xfc, 0xf4,
	0xc0, 0xc0, 0x7c, 0xf0, 0xf4, 0xf4, 0xf4, 0xfc,
	0xc0, 0xc0, 0x7c, 0xf8, 0xf8, 0xf8, 0xf8, 0xfc,
	0xc0, 0x91, 0xfc, 0xf4, 0xfc, 0xf4, 0xfc, 0xfc,
	0xc0, 0x91, 0xfc, 0xfc, 0xf8, 0xf8, 0xf8, 0xbc,
	0xc0, 0x81, 0xfc, 0xf4, 0xf4, 0xf4, 0xfc, 0xfc,
	0xc0, 0x81, 0xf8, 0xf8, 0xf0, 0xfc, 0xfc, 0xbc,
	0xc0, 0x94, 0xfc, 0xf4, 0xf4, 0xfc, 0xf4, 0xfc,
	0xc0, 0x94, 0xf8, 0xf0, 0xfc, 0xf4, 0xfc, 0xfc,
	0xc0, 0x36, 0xf4, 0xf4, 0xf8, 0xfc, 0xf4, 0xfc,
	0xc0, 0xd4, 0xf8, 0xf8, 0xf8, 0xfc, 0xf8, 0xbc,
	0xc0, 0x76, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xfc,
	0xfc, 0xfc, 0xbc, 0xfc, 0x7c, 0xf8, 0xf8, 0xf8,
	0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0xf4, 0xf4,
	0xfc, 0xbc, 0xfc, 0xbc, 0xfc, 0xfc, 0xfc, 0xf8,
	0xfc, 0xbc, 0xfc, 0x3c, 0x7c, 0x7c, 0xf4, 0xf4,
	0xfc, 0xfc, 0xbc, 0xbc, 0xbc, 0xfc, 0xfc, 0xf8,
	0xfc, 0xfc, 0x7c, 0x3c, 0x7c, 0x7c, 0xf4, 0xfc,
	0xbc, 0xbc, 0xfc, 0xbc, 0xbc, 0xfc, 0xfc, 0xfc,
	0xfc, 0xfc, 0x7c, 0x3c, 0x3c, 0x7c, 0xfc, 0xf4,
	0xbc, 0xbc, 0x3c, 0x39, 0x3c, 0xbc, 0xfc, 0xfc,
	0xfc, 0x7c, 0x3c, 0xbc, 0x39, 0x7c, 0x7c, 0xf4,
	0xbc, 0x3c, 0x36, 0x3c, 0x39, 0x3c, 0x3c, 0xfc,
	0xfc, 0x3c, 0x3c, 0x39, 0x3c, 0x7c, 0x7c, 0xfc,
	0xbc, 0x3c, 0x3c, 0x33, 0x36, 0x3c, 0xfc, 0xf8,
	0x3c, 0x39, 0x39, 0x39, 0x3c, 0x7c, 0xbc, 0xfc,
	0xbc, 0x3c, 0x3c, 0x3c, 0x36, 0x36, 0xfc, 0xf8,
	0xfc, 0x39, 0x39, 0x39, 0x3c, 0x3c, 0x3c, 0xfc,
	0xf8, 0xf4, 0xf0, 0x70, 0x34, 0x62, 0xc0, 0xc0,
	0xf4, 0xf0, 0xb0, 0xb0, 0xf4, 0x62, 0xc0, 0xc0,
	0xf8, 0xf8, 0xf0, 0x30, 0xb0, 0x42, 0xc0, 0xc0,
	0xf0, 0xf4, 0xb0, 0xf0, 0x30, 0x68, 0xc0, 0xc0,
	0xf8, 0xf8, 0xf0, 0x70, 0xb8, 0x39, 0xc0, 0xc0,
	0xf4, 0xf0, 0xb0, 0xf0, 0xf0, 0xb9, 0xc0, 0xc0,
	0xf8, 0xf8, 0xf0, 0x25, 0xf0, 0xa9, 0xc0, 0xc0,
	0xf4, 0xf4, 0xb0, 0xf0, 0xf0, 0xa1, 0xc0, 0xc0,
	0xf8, 0xf8, 0xf0, 0x25, 0xf0, 0x34, 0xc0, 0xc0,
	0xf4, 0xf0, 0xb0, 0xf0, 0xb0, 0xb4, 0x62, 0xc0,
	0xf8, 0xf8, 0xf0, 0x25, 0xf0, 0x74, 0xc0, 0xc0,
	0xf4, 0xf4, 0xf4, 0xf0, 0x5a, 0xf4, 0x62, 0xc0,
	0xf8, 0xf8, 0xf8, 0xa5, 0xf0, 0xf0, 0x62, 0xc0,
	0xf4, 0xf4, 0xf0, 0xb0, 0x5a, 0xb0, 0x42, 0xc0,
	0xf0, 0xf4, 0xf0, 0x70, 0xf0, 0xf0, 0x62, 0xc0,
	0xf4, 0xf0, 0xb0, 0xf0, 0xf0, 0xb0, 0x42, 0xc0,
	0xc0, 0x76, 0xf8, 0xf0, 0xf8, 0xfc, 0xfc, 0xbc,
	0xc0, 0x56, 0xa5, 0xf4, 0xf0, 0xf4, 0xfc, 0xfc,
	0xc0, 0x72, 0x0f, 0x5a, 0xf8, 0xfc, 0xfc, 0xbc,
	0xc0, 0x56, 0xa5, 0xf4, 0xfc, 0xf4, 0xf4, 0xf4,
	0xc0, 0x52, 0xf0, 0xf0, 0xf8, 0xad, 0xf0, 0xf8,
	0xc0, 0x56, 0xa5, 0xf0, 0xf0, 0x5e, 0xfc, 0xfc,
	0x91, 0x78, 0xa5, 0xf4, 0xa5, 0xf8, 0xf8, 0xf8,
	0xc0, 0x78, 0x0f, 0xf0, 0x5a, 0xf0, 0x1a, 0xe1,
	0x91, 0x2d, 0x0f, 0xa5, 0xf0, 0xb0, 0xd7, 0x3f,
	0x91, 0xf8, 0x5a, 0x5a, 0x5a, 0x35, 0x3f, 0x3f,
	0x81, 0xad, 0xf0, 0xa5, 0xb0, 0xd7, 0x3f, 0x3f,
	0x91, 0xf8, 0x5a, 0x5a, 0x61, 0x61, 0xd7, 0x3f,
	0x81, 0xf8, 0xa5, 0xb0, 0x92, 0x1a, 0x30, 0xbf,
	0x81, 0xf0, 0x5a, 0x4b, 0x61, 0x25, 0xb0, 0x75,
	0x81, 0xad, 0xa5, 0xb0, 0x92, 0x92, 0x70, 0x30,
	0x94, 0xf0, 0x5a, 0x61, 0xeb, 0xeb, 0x92, 0x30,
	0xbc, 0x3c, 0x36, 0x33, 0x36, 0x36, 0x7c, 0x7c,
	0x3c, 0x39, 0x39, 0x36, 0x33, 0x39, 0x3c, 0xfc,
	0xbc, 0x36, 0x33, 0x36, 0x36, 0x36, 0x78, 0x87,
	0xf4, 0x39, 0x36, 0x33, 0x33, 0x39, 0x3c, 0xf4,
	0xbc, 0x3c, 0x33, 0x33, 0x36, 0x36, 0x7c, 0xf8,
	0xbc, 0x39, 0xb9, 0xb9, 0xb9, 0xb1, 0xb4, 0xe1,
	0x78, 0x78, 0x72, 0x76, 0x63, 0x6b, 0x6b, 0x92,
	0x97, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x3f,
	0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x95, 0x95, 0x3f,
	0x6a, 0x95, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x3f,
	0x3f, 0x95, 0xc0, 0xc0, 0xc0, 0x95, 0x95, 0x95,
	0x6a, 0xc0, 0x95, 0xc0, 0xc0, 0x3f, 0x6a, 0x3f,
	0x3f, 0xc0, 0x3f, 0x6a, 0xc0, 0x95, 0x95, 0x3f,
	0x3f, 0x6a, 0x95, 0xc0, 0x6a, 0xc0, 0x3f, 0x3f,
	0x3f, 0x3f, 0x6a, 0x6a, 0xc0, 0xc0, 0x6a, 0x7f,
	0x97, 0x6a, 0x3f, 0x95, 0x95, 0x6a, 0x3f, 0xff,
	0xf8, 0xf8, 0xad, 0xf1, 0xf0, 0xf0, 0x42, 0xc0,
	0xfc, 0xb0, 0xb0, 0xf0, 0xf0, 0xf0, 0x68, 0xc0,
	0x70, 0x25, 0xf0, 0x70, 0xf0, 0xf0, 0x68, 0xc0,
	0xb0, 0x5a, 0xf2, 0x5a, 0x5a, 0xf0, 0xe8, 0xc0,
	0xf8, 0xf8, 0xad, 0xa5, 0xf0, 0xf0, 0xe8, 0xc0,
	0x1a, 0x5a, 0x5a, 0xf0, 0x5a, 0xf0, 0xb1, 0xc0,
	0xd2, 0x70, 0xf1, 0xa5, 0xf0, 0xf0, 0xe0, 0xc0,
	0xbf, 0xff, 0x92, 0xf2, 0x5a, 0xf0, 0xb1, 0xc0,
	0x3f, 0x3f, 0x7f, 0x71, 0xf0, 0xf0, 0x31, 0xc0,
	0xbf, 0xbf, 0xbf, 0xfa, 0x5a, 0xf0, 0xb1, 0xc0,
	0x7f, 0xff, 0x7f, 0xba, 0xa5, 0xf0, 0x21, 0xc0,
	0xbf, 0xbf, 0x3f, 0xeb, 0xf0, 0xb0, 0xb1, 0xc0,
	0x7f, 0x7f, 0x7f, 0x6b, 0xf1, 0xf0, 0xa1, 0xc0,
	0xff, 0xd7, 0xff, 0xbf, 0x70, 0xb2, 0xa1, 0xc0,
	0xeb, 0xc3, 0xff, 0x7f, 0xfb, 0xf0, 0xb4, 0xc0,
	0x92, 0x30, 0x61, 0xbf, 0xfa, 0xf2, 0xb4, 0x62,
	0x81, 0xa5, 0xa5, 0xd7, 0xff, 0x7f, 0xeb, 0xeb,
	0x94, 0x5a, 0x5a, 0xc3, 0xbf, 0xbf, 0xbf, 0xff,
	0x94, 0xa5, 0xb0, 0xd7, 0x7f, 0x7f, 0x7f, 0x7f,
	0x94, 0x5a, 0x5a, 0xff, 0xbf, 0xbf, 0x3f, 0x3f,
	0xd4, 0xa5, 0xb0, 0xd7, 0x7f, 0x7f, 0x7f, 0x7f,
	0x94, 0x5a, 0x1a, 0xbf, 0xff, 0xbf, 0x3f, 0x3f,
	0x76, 0xa5, 0xe1, 0xff, 0x7f, 0xff, 0xeb, 0x7f,
	0xd4, 0x5a, 0x1a, 0xff, 0xeb, 0xeb, 0x30, 0x61,
	0x76, 0xa5, 0xe1, 0x7f, 0xc3, 0x92, 0x92, 0x92,
	0x72, 0x5a, 0xe3, 0xff, 0xeb, 0x61, 0x30, 0x30,
	0x56, 0xa5, 0xe1, 0x7f, 0xba, 0x30, 0xea, 0x95,
	0x72, 0x5a, 0xf7, 0xeb, 0xeb, 0x61, 0xc1, 0x60,
	0x56, 0xa5, 0xf5, 0x7f, 0xeb, 0x35, 0x82, 0x55,
	0x52, 0xf2, 0xb5, 0x3f, 0xeb, 0x35, 0x50, 0x80,
	0x78, 0xa5, 0x75, 0x7f, 0xeb, 0xd7, 0x14, 0x02,
	0x52, 0xf2, 0xb5, 0x3f, 0xff, 0x61, 0x00, 0x00,
	0xd7, 0x6a, 0x6a, 0x6a, 0x3f, 0x95, 0x3f, 0x92,
	0xbf, 0x3f, 0x3f, 0x95, 0x6a, 0xc0, 0x7f, 0x30,
	0x7f, 0x6a, 0x3f, 0x95, 0x3f, 0x95, 0xba, 0x70,
	0x3f, 0xc0, 0x95, 0x3f, 0x95, 0x3f, 0xc3, 0x30,
	0x7f, 0x6a, 0x3f, 0x3f, 0x3f, 0x7f, 0xff, 0xeb,
	0xff, 0xbf, 0x6a, 0x3f, 0x3f, 0xbf, 0xbf, 0xbf,
	0x7f, 0x7f, 0x3f, 0x3f, 0x7f, 0x7f, 0x3f, 0x7f,
	0xbf, 0x3f, 0x3f, 0x3f, 0xbf, 0xbf, 0xbf, 0xbf,
	0xff, 0x7f, 0x7f, 0x7f, 0x7f, 0xff, 0x7f, 0xba,
	0x75, 0xbf, 0xbf, 0x3f, 0xff, 0xd7, 0xff, 0x30,
	0xc3, 0x7f, 0x3f, 0x95, 0x6b, 0xc3, 0xeb, 0x97,
	0x6b, 0xeb, 0xbf, 0x95, 0xeb, 0x75, 0xd7, 0x6a,
	0xc1, 0xd7, 0x75, 0x3f, 0x6b, 0x61, 0xbf, 0xd5,
	0x6a, 0xba, 0xeb, 0xbf, 0xeb, 0x30, 0x97, 0x80,
	0x6a, 0xd7, 0x75, 0x7f, 0xeb, 0x30, 0xd7, 0x2a,
	0xd7, 0xba, 0xeb, 0xbf, 0xff, 0x92, 0x30, 0xaa,
	0x30, 0x25, 0x1a, 0xd7, 0x7a, 0xf1, 0xf4, 0xc0,
	0x25, 0xf0, 0x61, 0x61, 0xba, 0xf2, 0x5e, 0x62,
	0xf0, 0xb0, 0x92, 0x92, 0xba, 0xf1, 0xf0, 0x62,
	0xb0, 0x61, 0xc3, 0x61, 0xc3, 0xf2, 0x5a, 0x42,
	0xff, 0xeb, 0x92, 0x92, 0x92, 0xf1, 0xf0, 0x42,
	0x3f, 0xff, 0xff, 0xd7, 0x61, 0xf2, 0xf2, 0x39,
	0x7f, 0xff, 0xeb, 0x92, 0x92, 0xa5, 0xa5, 0x68,
	0x3f, 0xbf, 0xff, 0xc3, 0xc3, 0xf2, 0xf2, 0xb9,
	0x92, 0xff, 0xeb, 0xeb, 0x92, 0xa5, 0xa5, 0xb9,
	0x30, 0x61, 0xd7, 0x92, 0xc3, 0xf2, 0xf2, 0xa9,
	0xc0, 0xba, 0xc3, 0xeb, 0xc3, 0xa5, 0xf1, 0xa1,
	0x61, 0xd5, 0x61, 0x92, 0xc3, 0xf0, 0xf2, 0xbc,
	0x00, 0xc2, 0x30, 0xc3, 0x61, 0xa5, 0xf1, 0xb4,
	0xd0, 0x15, 0x30, 0x92, 0xd7, 0x5a, 0xf2, 0xf4,
	0x16, 0x55, 0x30, 0xc3, 0xeb, 0xa5, 0xf1, 0xf4,
	0x00, 0x41, 0x61, 0x61, 0x92, 0x5a, 0xe7, 0xf2,
	0x78, 0xa5, 0x35, 0xbf, 0x7f, 0x92, 0x0a, 0x50,
	0x78, 0xf2, 0xb5, 0x3f, 0xbf, 0xeb, 0x25, 0xb0,
	0x78, 0xa5, 0x35, 0x7f, 0xff, 0x7f, 0x92, 0x61,
	0xf8, 0xf2, 0xb5, 0x3f, 0xbf, 0x3f, 0xff, 0xff,
	0x7c, 0xf1, 0x75, 0x3f, 0x3f, 0xbf, 0x3f, 0xeb,
	0xf8, 0xf8, 0xb0, 0x61, 0xc3, 0x92, 0x92, 0x70,
	0x7c, 0xf4, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0xf8, 0xf8, 0xf0, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a,
	0xfc, 0xf0, 0xa5, 0xf0, 0xf0, 0xf0, 0xf0, 0xa5,
	0xf8, 0xf0, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a, 0xf2,
	0xfc, 0xf0, 0xa5, 0xf1, 0xf1, 0xf1, 0xa5, 0xa5,
	0xf8, 0xf8, 0xf0, 0x5a, 0x5a, 0xf0, 0x5a, 0x5a,
	0xfc, 0xf0, 0xa5, 0xa5, 0xf0, 0xa5, 0xa5, 0xa5,
	0xf8, 0xf8, 0xf0, 0x5a, 0xf0, 0x5a, 0x5a, 0x5a,
	0xfc, 0xf4, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5, 0xa5,
	0xf8, 0xf0, 0xf0, 0x5a, 0xf8, 0xf0, 0x5a, 0xf2,
	0x30, 0xd7, 0xd7, 0x7f, 0x7f, 0x30, 0x30, 0x70,
	0x61, 0xc3, 0xbf, 0x3f, 0xff, 0x92, 0x30, 0x30,
	0xff, 0xd7, 0x7f, 0x7f, 0xeb, 0xc3, 0x30, 0xc3,
	0xd7, 0xff, 0xc3, 0x70, 0x70, 0x30, 0x75, 0xd7,
	0xc3, 0x30, 0xf0, 0xf4, 0xa5, 0xb0, 0xb0, 0xd7,
	0x70, 0xf0, 0x5a, 0xbc, 0xf8, 0x5a, 0xf2, 0xf0,
	0xa5, 0xa5, 0xf4, 0x3c, 0xf8, 0xa5, 0xf1, 0xf1,
	0xf0, 0xf0, 0xf2, 0xbc, 0xf8, 0xf0, 0xf2, 0xf0,
	0xf1, 0xf1, 0xf4, 0xfc, 0xa5, 0xf1, 0xf1, 0xdb,
	0xf2, 0x5a, 0x5a, 0xbc, 0xf8, 0x5a, 0xf2, 0xe7,
	0xa5, 0xa5, 0xf4, 0xf4, 0xa5, 0xf1, 0xf1, 0xf1,
	0x5a, 0x5a, 0x5a, 0xf8, 0x5a, 0xf2, 0xf2, 0xf2,
	0xa5, 0xa5, 0xf0, 0xf0, 0xf1, 0xf1, 0xf1, 0xf1,
	0x5a, 0x5a, 0x5a, 0xf8, 0xf2, 0xf2, 0xf2, 0xf3,
	0xa5, 0xa5, 0xf0, 0xf1, 0xf1, 0xa5, 0xf1, 0xf1,
	0x5a, 0x5a, 0x5a, 0xf2, 0xf2, 0xf2, 0xf0, 0xf3,
	0x00, 0x1a, 0xc3, 0xc3, 0xeb, 0xa5, 0xf1, 0xda,
	0xa5, 0x61, 0xd7, 0xd7, 0x92, 0x5a, 0x4f, 0xf2,
	0x92, 0xc3, 0xff, 0xeb, 0x61, 0x25, 0xf1, 0xf0,
	0xd7, 0xd7, 0xd7, 0xd7, 0x92, 0x5a, 0xf2, 0xf0,
	0xff, 0xeb, 0xeb, 0x61, 0x70, 0x71, 0xf0, 0xf0,
	0x61, 0x61, 0x92, 0x92, 0x1a, 0x5a, 0x5a, 0xf0,
	0xa5, 0xf0, 0xf0, 0xf0, 0xf0, 0xf1, 0xf0, 0xf0,
	0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf4,
	0xa5, 0xf0, 0xf1, 0xf1, 0xf1, 0xf1, 0xf0, 0xf4,
	0xe7, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf4,
	0xf1, 0xf1, 0xdb, 0xdb, 0xdb, 0xf1, 0xf0, 0xb4,
	0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf0, 0xb4,
	0xf1, 0xa5, 0xf1, 0xf1, 0xf1, 0xf1, 0xf0, 0x34,
	0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2, 0xb0, 0xa1,
	0xf3, 0xf3, 0xa5, 0xf1, 0xf1, 0xf1, 0xf0, 0x21,
	0xa2, 0xf3, 0xf2, 0xf0, 0xf2, 0xf2, 0xb0, 0x21,
	0x7c, 0xf4, 0xf0, 0xf4, 0xf0, 0xf0, 0xf1, 0xf1,
	0xfc, 0xf8, 0xf0, 0xfc, 0xf8, 0xf0, 0x5a, 0xf2,
	0x7c, 0xf0, 0xf4, 0xf4, 0xf0, 0xa5, 0xf1, 0xa5,
	0xfc, 0xf8, 0xf8, 0xf0, 0xf0, 0x5a, 0xf2, 0xf2,
	0x7c, 0xf4, 0xfc, 0xf0, 0xf8, 0xa5, 0xf1, 0xf0,
	0xfc, 0xf8, 0xf8, 0xf8, 0xf0, 0x5a, 0xf2, 0xf0,
	0x7c, 0xfc, 0xf0, 0xf8, 0xa5, 0xf1, 0xa5, 0xf4,
	0x7c, 0xf8, 0xf8, 0xf0, 0x5a, 0x5a, 0x5a, 0xf0,
	0x56, 0x7c, 0xfc, 0xf0, 0xa5, 0xa5, 0xf0, 0xf4,
	0x7c, 0xf8, 0xf8, 0xf0, 0x5a, 0x5a, 0xf0, 0xfc,
	0x56, 0x7c, 0xf0, 0xa5, 0xa5, 0xf0, 0xf4, 0xf8,
	0x56, 0xfc, 0xf8, 0xf0, 0x5a, 0xf0, 0xf0, 0xf8,
	0x76, 0xbc, 0xf8, 0xa5, 0xf0, 0xf4, 0xf4, 0xf0,
	0x36, 0xbc, 0xf8, 0x5a, 0x5a, 0xfc, 0xf8, 0xf0,
	0xd4, 0xfc, 0xf4, 0xa5, 0xf0, 0xf4, 0xf4, 0xf1,
	0x36, 0xf8, 0xf4, 0xf0, 0xf4, 0xf0, 0xf8, 0xf2,
	0xa5, 0xf0, 0xf0, 0xf1, 0xf1, 0xf1, 0xf0, 0xf1,
	0x5a, 0xf0, 0xf0, 0xf2, 0xf2, 0xf2, 0xf2, 0xf0,
	0xf0, 0xf8, 0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf0,
	0xf0, 0xf0, 0xf0, 0xf2, 0xf2, 0xf2, 0xf2, 0xf2,
	0xf4, 0xf0, 0xf1, 0xf1, 0xf1, 0xf3, 0xf3, 0xf3,
	0xf8, 0xf0, 0xf2, 0xf2, 0xf3, 0xf3, 0xf3, 0xf2,
	0xfc, 0xf1, 0xf1, 0xf0, 0xf1, 0xf1, 0xf1, 0xf3,
	0xf8, 0xf0, 0xf2, 0xf2, 0xf3, 0xf1, 0xf3, 0xf3,
	0xf0, 0xf1, 0xf1, 0xf1, 0xf3, 0xf3, 0xf3, 0xf3,
	0xf0, 0xf2, 0xf2, 0xf3, 0xf3, 0xf3, 0x51, 0xf3,
	0xf1, 0xf1, 0xf1, 0xf3, 0xf1, 0xf3, 0xf3, 0xf3,
	0xf2, 0xf2, 0xf2, 0xf3, 0xf3, 0xf3, 0x51, 0xf3,
	0xf1, 0xf1, 0xf1, 0xf1, 0xf3, 0xa2, 0xa2, 0xf3,
	0xf2, 0xf2, 0xf2, 0xf3, 0xf3, 0xf3, 0x51, 0xf3,
	0xf1, 0xf3, 0xf1, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3,
	0xf2, 0xf2, 0xf2, 0xf2, 0xf3, 0x51, 0xa2, 0xa2,
	0xf3, 0xf3, 0xf3, 0xa5, 0xf1, 0xf1, 0x70, 0x31,
	0xa2, 0xa2, 0xf3, 0xf2, 0xf2, 0xf2, 0xb0, 0xb9,
	0xf1, 0xf3, 0xf3, 0xf3, 0xf1, 0xf1, 0x70, 0xb9,
	0xf0, 0xa2, 0xa2, 0xf2, 0x5a, 0xf2, 0xb0, 0xe8,
	0xa5, 0xf1, 0xf3, 0x51, 0xf1, 0xf1, 0xf0, 0x68,
	0xf2, 0x5a, 0xa2, 0xa2, 0xf2, 0xf2, 0xb0, 0x68,
	0xf1, 0xa5, 0xf1, 0xf3, 0x51, 0xf3, 0xf0, 0x68,
	0xf2, 0xf2, 0xf2, 0xa2, 0x00, 0xf2, 0xb0, 0x42,
	0xf0, 0xf1, 0xf1, 0xf3, 0x51, 0xa5, 0xf0, 0x42,
	0xf2, 0xf2, 0xf3, 0x51, 0xa2, 0xf2, 0x5a, 0x42,
	0xf3, 0xf3, 0xf1, 0xf3, 0xf3, 0xa7, 0xf0, 0x62,
	0xf3, 0xf1, 0xf2, 0xf2, 0xa2, 0xf2, 0x5e, 0x62,
	0xf3, 0xf1, 0xf1, 0xf1, 0xf1, 0xf3, 0xf4, 0x62,
	0xf3, 0xf3, 0xf2, 0xf2, 0xf2, 0xf2, 0xb4, 0xc0,
	0x51, 0xf3, 0xf3, 0xf1, 0xf1, 0xf2, 0xb4, 0x62,
	0x00, 0xf3, 0xf2, 0xf2, 0xf3, 0xf2, 0xa1, 0xc0,
	0x94, 0xf4, 0xf0, 0xa5, 0xf4, 0xf4, 0xf0, 0xf1,
	0x81, 0xf0, 0x5a, 0x5a, 0xf8, 0xf8, 0xf0, 0xf2,
	0x91, 0xf8, 0xa5, 0xa5, 0xf0, 0xf0, 0xf1, 0xf1,
	0x91, 0x38, 0xf0, 0x5a, 0x5a, 0xf8, 0xf2, 0xf2,
	0xc0, 0x52, 0xad, 0x0f, 0xf0, 0xf1, 0xf1, 0xf1,
	0xc0, 0x76, 0xf0, 0x0f, 0x5a, 0xf2, 0xf2, 0xf2,
	0xc0, 0x94, 0xa5, 0x0f, 0xf0, 0xf1, 0xf1, 0xf1,
	0xc0, 0x81, 0xf0, 0x0f, 0x5a, 0xf2, 0xf2, 0xf2,
	0xc0, 0x91, 0xad, 0x0f, 0xf0, 0xf1, 0xf0, 0xf0,
	0xc0, 0xc0, 0x78, 0x0f, 0x5a, 0xf2, 0xf0, 0xf0,
	0xc0, 0xc0, 0x52, 0xa5, 0xf0, 0xf0, 0xf0, 0xf0,
	0xc0, 0xc0, 0x72, 0x0f, 0x5a, 0x5a, 0xf0, 0xf0,
	0xc0, 0xc0, 0x72, 0xa5, 0x0f, 0x0f, 0xf0, 0xf0,
	0xc0, 0xc0, 0xd0, 0x0f, 0x5a, 0x5a, 0xf0, 0xf0,
	0xc0, 0xc0, 0xd0, 0xa5, 0x0f, 0xf0, 0xf0, 0xf0,
	0xc0, 0xc0, 0xd0, 0xf0, 0x0f, 0xf0, 0xf0, 0xf0,
	0xf1, 0xf1, 0xf1, 0xf0, 0xf1, 0x51, 0x51, 0x51,
	0xf2, 0xf2, 0xf3, 0xf2, 0xf3, 0xf3, 0xa2, 0x00,
	0xf1, 0xf0, 0xf1, 0xf3, 0xf3, 0xf3, 0x00, 0x00,
	0xf2, 0xf0, 0xf0, 0xf3, 0xf1, 0xf3, 0xa2, 0xa2,
	0xf1, 0xf0, 0xf1, 0xf3, 0xf3, 0x51, 0xa2, 0xa2,
	0xf2, 0xf0, 0xf0, 0xf3, 0xf3, 0xa2, 0xa2, 0x00,
	0xf0, 0xf0, 0xf1, 0xf1, 0xf3, 0x51, 0xf3, 0x00,
	0xf0, 0xf0, 0xf2, 0xf0, 0xf3, 0xa2, 0xa2, 0x00,
	0xf0, 0xf1, 0xf1, 0xf1, 0xf3, 0x51, 0x00, 0x00,
	0xf0, 0xf2, 0xf3, 0xf3, 0xf3, 0xf3, 0xa2, 0x00,
	0xf1, 0xf0, 0xf1, 0xf3, 0xf3, 0xf3, 0x00, 0x00,
	0xf0, 0xf2, 0xf3, 0xf1, 0xf1, 0xf3, 0xa2, 0x00,
	0xf0, 0xf1, 0xf1, 0xf1, 0xf3, 0xf3, 0x51, 0x00,
	0xf0, 0xf2, 0xf3, 0xf3, 0xf3, 0xf3, 0xa2, 0x00,
	0xf0, 0xf1, 0xf1, 0xf3, 0xf3, 0xf3, 0xf3, 0x00,
	0xf0, 0xf2, 0xf3, 0xf3, 0xf2, 0xa2, 0xa2, 0x00,
	0x51, 0xf3, 0xf3, 0xf1, 0xf1, 0xf0, 0xa1, 0xc0,
	0x00, 0xf3, 0xf3, 0xf2, 0xf3, 0xf2, 0xb9, 0xc0,
	0x51, 0x51, 0xf3, 0xf3, 0xf1, 0xf0, 0x39, 0xc0,
	0x00, 0xf3, 0x51, 0xf1, 0xf3, 0xf2, 0x68, 0xc0,
	0x51, 0x51, 0xf3, 0xf3, 0xf1, 0xf0, 0x42, 0xc0,
	0x00, 0xf3, 0xf3, 0xf2, 0xf3, 0xf0, 0x42, 0xc0,
	0x51, 0x51, 0xf3, 0xf3, 0xf1, 0xf4, 0x62, 0xc0,
	0x00, 0x00, 0xf2, 0xf3, 0xf2, 0xb4, 0x62, 0xc0,
	0x51, 0x51, 0xf2, 0xf3, 0xf1, 0xb4, 0xc0, 0xc0,
	0xa2, 0xf3, 0xf3, 0xf2, 0xf2, 0xa9, 0xc0, 0xc0,
	0x51, 0xf3, 0xf3, 0xf3, 0xf1, 0xa9, 0xc0, 0xc0,
	0xa2, 0xa2, 0xf3, 0xf2, 0xf2, 0x39, 0xc0, 0xc0,
	0x51, 0xa2, 0xf3, 0xf1, 0xf1, 0x39, 0xc0, 0xc0,
	0x00, 0xf3, 0xf3, 0xf2, 0xf2, 0x42, 0xc0, 0xc0,
	0x51, 0xf3, 0xf3, 0xf1, 0xf1, 0x42, 0xc0, 0xc0,
	0x00, 0xf3, 0xf3, 0xf2, 0xf2, 0x62, 0xc0, 0xc0,
	0xc0, 0xc0, 0xd0, 0xa5, 0x0f, 0x5a, 0xf0, 0xf0,
	0xc0, 0xc0, 0x72, 0x5a, 0x0f, 0x5a, 0x5a, 0xf0,
	0xc0, 0xc0, 0xd0, 0xa5, 0xa5, 0xf0, 0x5a, 0xf0,
	0xc0, 0xc0, 0x72, 0xf0, 0xf0, 0xf0, 0xf0, 0xf0,
	0xc0, 0xc0, 0x72, 0xa5, 0xa5, 0xf0, 0xa5, 0xf0,
	0xc0, 0xc0, 0x52, 0xa5, 0xf0, 0x5a, 0xf0, 0xf2,
	0xc0, 0xc0, 0x72, 0xf0, 0xf0, 0x5a, 0x5a, 0xf1,
	0xc0, 0xc0, 0x52, 0xa5, 0xf0, 0xf1, 0xf0, 0xf2,
	0xc0, 0xc0, 0x52, 0xf0, 0xf0, 0xf0, 0xf1, 0xf1,
	0xc0, 0xc0, 0x78, 0xf0, 0xf1, 0xf1, 0xf0, 0xf2,
	0xc0, 0xc0, 0x52, 0xf1, 0xf1, 0xf0, 0xf1, 0xf1,
	0xc0, 0xc0, 0x78, 0xf0, 0xf0, 0xf0, 0xf2, 0xf2,
	0xc0, 0xc0, 0x78, 0xf1, 0xf1, 0xf0, 0xf1, 0xf1,
	0xc0, 0xc0, 0xf8, 0xf0, 0xf2, 0xf0, 0xf3, 0xf2,
	0xc0, 0xc0, 0x78, 0xf1, 0xf3, 0xf1, 0xf1, 0xf3,
	0xc0, 0xc0, 0xf8, 0xf2, 0xf2, 0xf0, 0xf3, 0xf2,
	0xf1, 0xf3, 0xf1, 0xf2, 0xf3, 0xa2, 0xf3, 0x00,
	0xf0, 0xf2, 0xf3, 0xf2, 0xf3, 0xf3, 0xa2, 0x51,
	0xf0, 0xf1, 0xf1, 0xf3, 0xf3, 0xf3, 0xf3, 0x00,
	0xf2, 0xf2, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0xa2,
	0xf0, 0xf1, 0xf2, 0xf1, 0xf3, 0xf3, 0xf3, 0x00,
	0xf2, 0xf0, 0xf2, 0xf1, 0xf3, 0xf3, 0x51, 0xa2,
	0xf0, 0xf1, 0xf2, 0xf1, 0xf3, 0xf3, 0xf3, 0x00,
	0xf2, 0xf0, 0xf3, 0xf3, 0xf3, 0xf3, 0x51, 0xa2,
	0xf0, 0xf1, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3, 0x00,
	0xf2, 0xf1, 0xf1, 0xf3, 0xf3, 0xf3, 0xf3, 0xa2,
	0xf0, 0xf1, 0xf3, 0xf2, 0xf3, 0xf3, 0xf3, 0xf3,
	0xf0, 0xf3, 0xf3, 0xf2, 0xf3, 0xf3, 0xf3, 0xa2,
	0xf0, 0xf1, 0xf3, 0xf3, 0xf3, 0xf1, 0xf3, 0x51,
	0xf0, 0xf2, 0xf3, 0xf1, 0xf3, 0xf3, 0xf3, 0xa2,
	0xf0, 0xf1, 0xf1, 0xf3, 0xf3, 0xf3, 0xf3, 0xf3,
	0xf0, 0xf2, 0xf3, 0xf2, 0xf3, 0xf3, 0xf3, 0xf3,
	0x51, 0xf3, 0xf3, 0xf3, 0xf1, 0x42, 0xc0, 0xc0,
	0x00, 0xf3, 0xf3, 0xf2, 0xf2, 0x62, 0xc0, 0xc0,
	0x51, 0xf3, 0xf3, 0xf3, 0xf1, 0x62, 0xc0, 0xc0,
	0x00, 0xf3, 0xf2, 0xf2, 0xf3, 0x62, 0xc0, 0xc0,
	0x51, 0xf3, 0xf3, 0xf1, 0xf1, 0xc0, 0xc0, 0xc0,
	0x00, 0xa2, 0xf2, 0xf3, 0xf2, 0x62, 0xc0, 0xc0,
	0x51, 0xf3, 0xf3, 0xf1, 0xf3, 0xc0, 0xc0, 0xc0,
	0xa2, 0xf3, 0xf3, 0xf2, 0xf2, 0x62, 0xc0, 0xc0,
	0x51, 0xf3, 0xf3, 0xf1, 0xf3, 0xc0, 0xc0, 0xc0,
	0xf3, 0xf3, 0xf1, 0xf2, 0xf2, 0x62, 0xc0, 0xc0,
	0xf3, 0x51, 0xf1, 0xf1, 0xf3, 0x62, 0xc0, 0xc0,
	0xf3, 0xf3, 0xf3, 0xf2, 0xf2, 0x42, 0xc0, 0xc0,
	0xf3, 0xf3, 0xf3, 0xf1, 0xf3, 0x42, 0xc0, 0xc0,
	0xf3, 0xf2, 0xf3, 0xf2, 0xf2, 0x68, 0xc0, 0xc0,
	0xf3, 0xf3, 0xf3, 0xf1, 0xf3, 0x68, 0xc0, 0xc0,
	0xf3, 0xf3, 0xf3, 0xf2, 0xf3, 0xb9, 0xc0, 0xc0
};