// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL51.BIN"

// -------------------------------------------------------
#define NBZONES 7

u8 leveldata[2] = { 1 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	0,0,3,3,1, // 
	8,0,11,3,2, // 
	1,8,3,10,3, // 
	8,8,10,10,4, //
	2,14,3,15,5, // Fil 1
	5,11,6,15,6, // Fil 2
	7,13,8,15,7, // Fil 3
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
u8 scenario[16][SEQLINESIZE] =
{
	{ 250, 1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // 
	{ 199, 101, 102, 2  , 2  ,103, 103,103, 0 , 0 }, // 
	{ 253, 8  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Palette cycle 8
	{ 120, 101, 3  , 0  , 0  ,103, 103,103, 0 , 0 }, // 
	{ 120, 101, 0  , 0  , 0  ,104, 4  ,104, 0 , 0 }, // 
													{ 251, 100, 5,  105 , 1  ,2  , 1  , 4  ,3 , 2}, // QTE
													{ 1  , 255   , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 120, 101, 0  , 0  , 0  ,104, 0  , 6 , 0 , 0 }, // 
													{ 251, 100, 7,  105 , 4  ,3  , 2  , 1  ,4 , 3}, // QTE
													{ 2  ,1   , 255  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 120, 101, 0  , 0  , 0  ,8  , 0  , 0 , 0 , 0 }, // 
													{ 251, 100, 9,  105 , 2  ,3  , 4  , 1  ,2 , 3}, // QTE
													{ 4  ,2   , 255  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 40, 10 , 0   , 0  , 0  ,8  , 0  , 0 , 0 , 0 }, // 
	{ 253, 0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Palette normal
	{ 255,0  , 0  , 0  , 0  , 0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL52 BIN"; // Must be 11 characters

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
  "Il y a une porte blind�e ici.   "
 "C'est le seul coin que j'ai pas   "
"encore fouill�. Cette grosse catin  "
 "doit se planquer par l�. Il y a un"
  "bo�tier sur le c�t� de la porte."
};

u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 5
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je cogne sur le syst�me et tout "
 "m'\2EXPLOSE\1 � la gueule. Y a un     "
"moment donn� o� tu r�fl�chis pine   "
 "d�hu�tre ? Comment j'arrive � te  "
  "supporter dis-moi ?             "
},
{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je pense pas que ce soit le bon "
 "moment pour couper des fils.      "
"D'ailleurs j'ai fait un court-jus   "
 "avec la pince, y a tout qui p�te. "
  "J'ai la t�te � moiti� \2ARRACHEE\1. "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je tente d'arracher le fil � la "
 "main, mais les vibrations font    "
"\2EXPLOSER\1 le syst�me. Ca m'arrache la"
 "main et la moiti� de la t�te. Un  "
  "peu de subtilit� baka !         "
},
{
/*
"------------------------------------" 36 chars (Line 3) 104
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2NON\1. C'�tait pas le bon fil.    "
 "Tout explose. BOUMMMM ! Evidemment"
"c'est ma cervelle qui est sur le mur"
 "pas la tienne. T'as le bon r�le ma"
  "crotte. On �change nos places ? "
},
{
/*
"------------------------------------" 36 chars (Line 3) 105
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2AIE\1. T'as pas �t� assez rapide  "
 "�a m'a fait trembler quand j'ai   "
"coup�. Y'a tout qui explose ! J'en  "
 "ai un peu marre de crever � cause "
  "d'une boursemolle !             "
}

};

#define NBMESSSUCESS 10 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je retire le couvercle du       "
 "bo�tier. Y'a tout un merdier      "
"l�-dessous. Ca ressemble � une      "
 "serrure �lectronique. T'�tais bon "
  "en �lec au coll�ge boloss ?     "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'appuie sur les boutons et le  "
 "compteur se met � \2S'AFFOLER\1. Je   "
"sais pas si c'est une bonne ou une  "
 "mauvaise nouvelle l� ! Viteee, on "
  "fait quoi l� zigoto ?           "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1, bonne id�e, c'est le moment"
 "de couper des fils. T'as r�vis�   "
"tes classiques ? Vert, violet, Rouge"
 "??????????????????????????????????"
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. C'est le bon fil. Je dois  "
 "pas trembler en coupant !!!       "
"                                    "
 "        \2REUSSIS LA SEQUENCE\1       "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. Heureusement que tu      "
 "trembles pas comme un alcoolo !   "
"T'es un guignol mais au moins tu    "
 "sais appuyer sur des touches !    "
  "Faut se contenter de peu...     "
},
{
/*
"------------------------------------" 36 chars (Line 3) 6
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2VOILA OUI\1, c'est bien ce fil-l� "
 "qu'il faut couper. Allez...       "
"                                    "
 "        \2REUSSIS LA SEQUENCE\1       "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 7
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. \2OUI\1. J'ai coup� net, le  "
 "compteur descend toujours, alors  "
"magne-toi le cul loquedu.           "
 "                                  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 8
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Bon c'est le dernier fil,       "
 "faudrait �tre con pour se tromper."
"                                    "
 "        \2REUSSIS LA SEQUENCE\1       "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 9
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1, j'ai coup� proprement.   "
 "Mais le compteur s'affole encore  "
"plus cependant. Y a un truc qui     "
 "a pas fonctionn�. Allez, trouve   "
  "quelque chose tra�ne-savate !   "
},
{
/*
"------------------------------------" 36 chars (Line 3) 10
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. Il me reste plus que �a. Je"
 "tape comme un bourrin sur ce truc "
"et le compteur s�arr�te. J'entends  "
 "la serrure de la porte qui s'ouvre"
  "Je rentre doucement...          "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0xc0, 0xc0, 0xd4, 0xec, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xa8, 0x0a, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0x05, 0x2f, 0xec, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc4, 0x01, 0x86, 0x05, 0x40, 0xc0, 0xc0,
	0xc0, 0xd4, 0x04, 0x87, 0x1d, 0x4e, 0xc0, 0xc0,
	0xc0, 0xa8, 0x05, 0x08, 0x43, 0x5e, 0xc8, 0xc0,
	0xd4, 0x05, 0xc3, 0x0a, 0x86, 0x0a, 0x0a, 0xc8,
	0x88, 0x4b, 0x43, 0x0d, 0x58, 0x05, 0x0d, 0x44,
	0xa8, 0x83, 0x86, 0x86, 0x05, 0x0e, 0x6b, 0x5e,
	0x05, 0x06, 0x43, 0x0c, 0xa0, 0x09, 0x87, 0x0a,
	0x04, 0x83, 0xa4, 0x86, 0xa5, 0x0c, 0x0a, 0x08,
	0x4b, 0x43, 0x5a, 0x58, 0x04, 0xa5, 0x04, 0x2a,
	0x0b, 0x2b, 0x0d, 0x00, 0x5a, 0x0a, 0xa1, 0x82,
	0x4b, 0x43, 0xc3, 0xa5, 0x00, 0x50, 0x0c, 0x0a,
	0x0b, 0x2b, 0x86, 0xd2, 0xa5, 0x05, 0x0d, 0x54,
	0x04, 0x17, 0x43, 0x58, 0x0c, 0x0a, 0x0a, 0x4e,
	0xb0, 0x3c, 0xb8, 0x70, 0xf0, 0x75, 0xfe, 0x30,
	0xb0, 0x3c, 0xb8, 0x70, 0xf0, 0x75, 0xfe, 0x30,
	0xb0, 0x34, 0xb8, 0x70, 0xf0, 0x75, 0xfe, 0x30,
	0xb0, 0x3c, 0xb8, 0x30, 0xf0, 0xf5, 0xfe, 0x30,
	0xf0, 0x34, 0xb8, 0x70, 0xf0, 0x75, 0xfe, 0x30,
	0xf0, 0x34, 0x7c, 0x30, 0xf0, 0xf5, 0xfe, 0x30,
	0xf0, 0x34, 0xb8, 0x30, 0xf0, 0xb0, 0xff, 0xb8,
	0xf0, 0x34, 0x7c, 0x30, 0xf0, 0xf5, 0xfe, 0x30,
	0xf0, 0x30, 0x7c, 0x30, 0xf0, 0xb0, 0xff, 0xb8,
	0xf0, 0x34, 0x7c, 0x30, 0xf0, 0xb0, 0xff, 0xb8,
	0xf0, 0xb0, 0x3c, 0xb8, 0x70, 0xb0, 0xff, 0xb8,
	0xf0, 0x30, 0x6c, 0xb8, 0x70, 0xf0, 0xbf, 0xba,
	0xf0, 0xb0, 0x3c, 0xb8, 0x70, 0xb0, 0xff, 0xb8,
	0x70, 0xb0, 0x64, 0x7c, 0x70, 0xf0, 0x35, 0xfe,
	0x70, 0xb0, 0x34, 0xdc, 0x70, 0xf0, 0x75, 0xfe,
	0x70, 0xf0, 0x34, 0x7c, 0x30, 0xf0, 0x35, 0xfe,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x76, 0x68,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x79, 0x68,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x28, 0x42,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x91, 0xf9, 0x62,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x91, 0xf6, 0x62,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x94, 0x14, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x94, 0x01, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x76, 0xb3, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x73, 0xb9, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x28, 0xe8, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x28, 0x68, 0x91,
	0xc0, 0xc0, 0xc0, 0xc0, 0x94, 0xf9, 0x42, 0x36,
	0xc0, 0xc0, 0xc0, 0xc0, 0x7c, 0xa3, 0x33, 0x7c,
	0xc0, 0xc0, 0xc0, 0x91, 0xf9, 0x14, 0x16, 0xf6,
	0xc0, 0xc0, 0xc0, 0x36, 0xfc, 0xf6, 0xa2, 0xf1,
	0xc0, 0xc0, 0x91, 0x79, 0xfc, 0xa8, 0x00, 0xa2,
	0x05, 0x2b, 0x03, 0x86, 0xe1, 0x0d, 0x05, 0x40,
	0xa8, 0x17, 0x43, 0x0c, 0xa4, 0x86, 0xf0, 0xe8,
	0x88, 0x09, 0x83, 0x86, 0xe1, 0x49, 0x0d, 0xc8,
	0xd4, 0x4b, 0x43, 0x86, 0xa4, 0x86, 0xa0, 0xc8,
	0xc4, 0x04, 0xc3, 0xc3, 0x49, 0x0c, 0x5e, 0xc0,
	0xc0, 0x05, 0x43, 0x86, 0xa4, 0xd2, 0x4e, 0xc0,
	0xc0, 0xa8, 0x83, 0xc3, 0x49, 0x86, 0x44, 0xc0,
	0xc0, 0x88, 0x49, 0x43, 0x49, 0x58, 0xe8, 0xc0,
	0xc0, 0xd4, 0x09, 0xc3, 0xd2, 0xa0, 0xc8, 0xc0,
	0xc0, 0xd4, 0x4b, 0x43, 0x49, 0x54, 0xc0, 0xc0,
	0xc0, 0xc4, 0x0b, 0x83, 0xc3, 0x44, 0xc0, 0xc0,
	0xc0, 0xc4, 0x41, 0x43, 0x49, 0xec, 0xc0, 0xc0,
	0xc0, 0xc0, 0x01, 0x83, 0xc3, 0x40, 0xc0, 0xc0,
	0xc0, 0xc0, 0x41, 0x43, 0x49, 0xe8, 0xc0, 0xc0,
	0xc0, 0xc0, 0x01, 0x83, 0xc3, 0xe8, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x70, 0xf0, 0x30, 0x9c, 0xb8, 0xf0, 0xf5, 0x7e,
	0x30, 0xf0, 0xb0, 0x6c, 0xb8, 0xf0, 0x75, 0xfe,
	0x30, 0xf0, 0x30, 0x34, 0xb8, 0xf0, 0xf5, 0x7f,
	0x30, 0xf0, 0xb0, 0x6c, 0x38, 0xf0, 0xb0, 0xfe,
	0x30, 0x70, 0xb0, 0x34, 0x7c, 0xf0, 0xb0, 0x7f,
	0x30, 0x70, 0xf0, 0x64, 0x38, 0xf0, 0xf0, 0xff,
	0x30, 0x70, 0xf0, 0x34, 0xdc, 0x70, 0xb0, 0x7f,
	0x30, 0x30, 0xf0, 0xb0, 0x7c, 0x70, 0xf0, 0xbf,
	0x30, 0x30, 0xf0, 0x34, 0xdc, 0x70, 0xf0, 0x75,
	0x30, 0x30, 0xf0, 0xb0, 0x7c, 0x30, 0xf0, 0xbf,
	0x30, 0x30, 0xf0, 0xf0, 0xdc, 0x30, 0xf0, 0x75,
	0x30, 0x30, 0x70, 0xf0, 0xdc, 0x30, 0xf0, 0x35,
	0x30, 0x30, 0x70, 0xf0, 0xd8, 0x30, 0xf0, 0xf5,
	0x30, 0x30, 0x30, 0xf0, 0xdc, 0x30, 0x70, 0xb5,
	0x30, 0x30, 0x70, 0xf0, 0x52, 0x30, 0x70, 0xb5,
	0x30, 0x30, 0x30, 0xa5, 0x00, 0xb0, 0x70, 0xb5,
	0xc0, 0xc0, 0x36, 0xf6, 0xbc, 0xf9, 0x00, 0x51,
	0xc0, 0x91, 0x7c, 0x54, 0x62, 0x78, 0x00, 0xf6,
	0xc0, 0x94, 0xa2, 0x54, 0xb6, 0xfc, 0x54, 0x39,
	0xc0, 0x76, 0x00, 0x51, 0x54, 0xf6, 0xb6, 0x62,
	0xc0, 0xa8, 0x00, 0x00, 0xf6, 0xf3, 0xb9, 0xc0,
	0x91, 0xa2, 0x00, 0x00, 0xf4, 0xf6, 0x68, 0xc0,
	0xd4, 0x00, 0x00, 0x00, 0xf8, 0xbc, 0x62, 0xc0,
	0x73, 0x00, 0x51, 0xa0, 0xf9, 0x39, 0xc0, 0xc0,
	0x88, 0x51, 0x62, 0xa8, 0xf4, 0x62, 0xc0, 0xc0,
	0x88, 0xb9, 0xd4, 0xa2, 0xbc, 0x62, 0xc0, 0xc0,
	0x36, 0x62, 0x22, 0x51, 0xb9, 0xc0, 0xc0, 0xc0,
	0x91, 0x91, 0x00, 0xf6, 0x62, 0xc0, 0xc0, 0xc0,
	0xc0, 0x28, 0x51, 0xb9, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0x91, 0x3c, 0x62, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0x66, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x30, 0x30, 0x30, 0x20, 0xbc, 0x10, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x20, 0xbc, 0x10, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x20, 0xbc, 0x10, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x20, 0xbc, 0x10, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x20, 0xb9, 0x10, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x20, 0xbc, 0x10, 0x30, 0x30,
	0x30, 0x70, 0x0a, 0x00, 0xb9, 0x00, 0x00, 0x00,
	0x30, 0x25, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00,
	0x30, 0x20, 0xcc, 0x88, 0xb9, 0x44, 0xcc, 0xcc,
	0x30, 0x20, 0x89, 0x88, 0xb9, 0x44, 0xcc, 0xcc,
	0x30, 0x20, 0x9c, 0x88, 0xb9, 0x11, 0x99, 0x99,
	0x30, 0x20, 0x66, 0x22, 0xbc, 0x44, 0x66, 0x66,
	0x30, 0x20, 0x99, 0x88, 0xb9, 0x11, 0x99, 0x99,
	0x30, 0x20, 0x33, 0x22, 0xbc, 0x11, 0x33, 0x76,
	0x30, 0x20, 0x33, 0x73, 0x00, 0xb3, 0x33, 0x72,
	0x30, 0x20, 0x91, 0xd4, 0xf1, 0xb9, 0x91, 0xd4,
	0x30, 0x30, 0x30, 0xa0, 0xbc, 0x10, 0x70, 0xa0,
	0x30, 0x30, 0x30, 0xa0, 0xbc, 0x10, 0x30, 0xa0,
	0x30, 0x30, 0x30, 0xa0, 0xbc, 0x10, 0x30, 0xa0,
	0x30, 0x30, 0x30, 0xa0, 0xbc, 0x10, 0x30, 0xa0,
	0x30, 0x30, 0x30, 0xa0, 0xb9, 0x10, 0x30, 0xa0,
	0x30, 0x30, 0x30, 0xa0, 0xbc, 0x10, 0x30, 0xa0,
	0x00, 0x00, 0x00, 0x00, 0xb9, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00,
	0xcc, 0xcc, 0xcc, 0x88, 0xb9, 0x44, 0xcc, 0x88,
	0xcc, 0xcc, 0xcc, 0x88, 0xb9, 0x44, 0xcc, 0x88,
	0x99, 0x99, 0x99, 0x88, 0xb9, 0x11, 0x99, 0x88,
	0x66, 0x66, 0x66, 0x22, 0xbc, 0x44, 0x66, 0x22,
	0xfc, 0x99, 0x99, 0x88, 0xb9, 0x11, 0x99, 0x88,
	0xf1, 0xb9, 0x33, 0x22, 0xbc, 0x11, 0x33, 0x22,
	0x00, 0xb3, 0x33, 0x73, 0x00, 0xb3, 0x33, 0x73,
	0xf1, 0xb9, 0x91, 0xd4, 0xf1, 0xb9, 0x91, 0xd4,
	0xbc, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0xbc, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0xbc, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0xbc, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0xb9, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0xbc, 0x10, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0xb9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xb0,
	0xbc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a,
	0xb9, 0x44, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0x10,
	0xb9, 0x44, 0xcc, 0xcc, 0xcc, 0xcc, 0x46, 0x10,
	0xb9, 0x11, 0x99, 0x99, 0x99, 0x99, 0x39, 0x10,
	0xbc, 0x44, 0x66, 0x66, 0x66, 0x66, 0x66, 0x10,
	0xb9, 0x11, 0x99, 0x99, 0xfc, 0x99, 0x99, 0x10,
	0xbc, 0x11, 0x33, 0x76, 0xf1, 0xb9, 0x33, 0x10,
	0x00, 0xb3, 0x33, 0x72, 0x00, 0xb3, 0x33, 0x10,
	0xf1, 0xb9, 0x91, 0xd4, 0xf1, 0xb9, 0x91, 0x10,
	0x30, 0x20, 0x62, 0x66, 0xfc, 0x62, 0x62, 0x66,
	0x30, 0x20, 0x81, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x30, 0x20, 0x36, 0x62, 0x62, 0x62, 0x62, 0x62,
	0x30, 0x20, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x30, 0x20, 0x03, 0x57, 0x75, 0x75, 0x30, 0x30,
	0x30, 0x20, 0x2b, 0xab, 0xba, 0x30, 0x30, 0x30,
	0x30, 0x20, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	0x30, 0x20, 0xaf, 0x0f, 0xea, 0xaf, 0x0f, 0xea,
	0x30, 0x20, 0x25, 0x0e, 0xc0, 0x84, 0x0e, 0xc0,
	0x30, 0x20, 0xaf, 0x5f, 0x0d, 0x5d, 0x5f, 0x0d,
	0x70, 0x20, 0x25, 0x4a, 0x0f, 0x4a, 0x4a, 0x0f,
	0x30, 0x20, 0xaf, 0x4a, 0x0f, 0x4a, 0x4a, 0x0f,
	0x30, 0x20, 0x25, 0x4a, 0x0f, 0x4a, 0x4a, 0x0f,
	0x70, 0x20, 0xaf, 0x5f, 0x0d, 0x5d, 0x5f, 0x0d,
	0x70, 0x20, 0x25, 0x0e, 0xc0, 0x84, 0x0e, 0xc0,
	0x30, 0x20, 0x25, 0x0f, 0xc0, 0x84, 0x0f, 0xc0,
	0xfc, 0x62, 0x62, 0x66, 0xfc, 0x62, 0x62, 0x66,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x91,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x62,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	0xaf, 0x0f, 0xea, 0xaf, 0x0f, 0xea, 0xaf, 0x0f,
	0x84, 0x0e, 0xc0, 0x84, 0x0e, 0xc0, 0x84, 0x0e,
	0x5d, 0x5f, 0x0d, 0x5d, 0x5f, 0x0d, 0x5d, 0x5f,
	0x4a, 0x4a, 0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x4a,
	0x4a, 0x4a, 0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x4a,
	0x4a, 0x4a, 0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x4a,
	0x5d, 0x5f, 0x0d, 0x5d, 0x5f, 0x0f, 0x5f, 0x5f,
	0x84, 0x0e, 0xc0, 0x84, 0x0e, 0x0f, 0x0e, 0x0e,
	0x84, 0x0f, 0xc0, 0x84, 0x0e, 0x0f, 0x0e, 0x0e,
	0xfc, 0x62, 0x62, 0x66, 0xfc, 0x62, 0x62, 0x10,
	0x91, 0x91, 0x91, 0x91, 0x91, 0x91, 0x13, 0x10,
	0x62, 0x62, 0x62, 0x62, 0x62, 0x62, 0x68, 0x10,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x10,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x10,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x10,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x1a, 0x10,
	0xea, 0xaf, 0x0f, 0xea, 0xaf, 0x0f, 0x1a, 0x10,
	0xc0, 0x84, 0x0e, 0xc0, 0x84, 0x0f, 0x1a, 0x10,
	0x0d, 0x5d, 0x5f, 0x0d, 0x5d, 0x0f, 0x1a, 0x10,
	0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x0f, 0x1a, 0x10,
	0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x0f, 0x1a, 0x10,
	0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x0f, 0x1a, 0x10,
	0x0f, 0x5f, 0x5f, 0x0f, 0x5f, 0x0f, 0x1a, 0x10,
	0x0f, 0x0e, 0x0e, 0x0f, 0x0e, 0x0f, 0x1a, 0x10,
	0x0f, 0x0e, 0x0e, 0x0f, 0x0e, 0x0f, 0x1a, 0x10,
	0x30, 0xa0, 0x25, 0x0f, 0x0d, 0x5d, 0x0f, 0x0d,
	0x30, 0x20, 0x25, 0x0f, 0x0f, 0x4a, 0x0f, 0x0f,
	0x30, 0x20, 0x25, 0x0f, 0x0f, 0x4a, 0x0f, 0x0f,
	0x70, 0x20, 0x25, 0x0f, 0x0f, 0x4a, 0x0f, 0x0f,
	0xf0, 0xa0, 0x25, 0x0f, 0x0d, 0x5d, 0x0f, 0x0d,
	0x70, 0x20, 0x25, 0x0f, 0xc0, 0x84, 0x0f, 0xc0,
	0xb0, 0x20, 0x25, 0x0f, 0xea, 0xaf, 0x0f, 0xea,
	0x30, 0x20, 0x25, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	0x30, 0xa0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x70, 0xa0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x70, 0xa0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0xa0, 0x21, 0x03, 0x03, 0x03, 0x03, 0x03,
	0xb0, 0x20, 0x35, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x30, 0x20, 0x35, 0x3f, 0x2b, 0x17, 0x3f, 0x3f,
	0xb0, 0xa0, 0x35, 0x3f, 0x70, 0xa5, 0x3f, 0x3f,
	0x30, 0xa0, 0x35, 0x2b, 0xa0, 0x05, 0x17, 0x3f,
	0x5d, 0x0f, 0x0d, 0x5d, 0x5f, 0x0f, 0x5f, 0x5f,
	0x4a, 0x0f, 0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x4a,
	0x4a, 0x0f, 0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x4a,
	0x4a, 0x0f, 0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x4a,
	0x5d, 0x0f, 0x0d, 0x5d, 0x5f, 0x0d, 0x5d, 0x5f,
	0x84, 0x0f, 0xc0, 0x84, 0x0e, 0xc0, 0x84, 0x0e,
	0xaf, 0x0f, 0xea, 0xaf, 0x0f, 0xea, 0xaf, 0x0f,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x0f, 0x5f, 0x5f, 0x0f, 0x5f, 0x0f, 0x5f, 0x10,
	0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x0f, 0x1a, 0x10,
	0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x0f, 0x5f, 0x10,
	0x0f, 0x4a, 0x4a, 0x0f, 0x4a, 0x0f, 0x1a, 0x10,
	0x0d, 0x5d, 0x5f, 0x0d, 0x5d, 0x0f, 0x5f, 0x10,
	0xc0, 0x84, 0x0e, 0xc0, 0x84, 0x0f, 0x1a, 0x10,
	0xea, 0xaf, 0x0f, 0xea, 0xaf, 0x0f, 0x5f, 0x10,
	0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0b, 0x10,
	0x30, 0x30, 0x75, 0x75, 0x75, 0x75, 0x17, 0x10,
	0x30, 0x30, 0x30, 0x30, 0xba, 0xab, 0xab, 0x10,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x10,
	0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x10,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3f, 0x03, 0x3f, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3a, 0xf0, 0x1f, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x52, 0x00, 0x0b, 0x3f, 0x10,
	0xb0, 0xa0, 0x35, 0x3a, 0x4b, 0x86, 0x35, 0x3f,
	0xf0, 0xa0, 0x35, 0x7a, 0x01, 0x6b, 0xb5, 0x3f,
	0x70, 0x20, 0x35, 0x07, 0x70, 0x95, 0x0b, 0x3f,
	0x70, 0xa0, 0x35, 0x02, 0x02, 0x42, 0x01, 0x3f,
	0xb0, 0xa0, 0x35, 0x02, 0x90, 0xe0, 0x01, 0x3f,
	0x30, 0x20, 0x35, 0x07, 0x2b, 0x01, 0x0b, 0x3f,
	0x30, 0x20, 0x35, 0x7a, 0x97, 0x25, 0xb5, 0x3f,
	0xb0, 0xa0, 0x35, 0x3a, 0x49, 0x02, 0x35, 0x3f,
	0xf0, 0xa0, 0x35, 0x2b, 0xa0, 0x50, 0x17, 0x3f,
	0xf0, 0x20, 0x35, 0x3f, 0x70, 0xb0, 0x3f, 0x3f,
	0x70, 0x20, 0x35, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0xf0, 0xa0, 0x35, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0xf0, 0xa0, 0x35, 0x3f, 0x3f, 0x56, 0x17, 0x3f,
	0xa5, 0xa0, 0x35, 0x3f, 0x7e, 0xf1, 0xf4, 0x3f,
	0xf0, 0xa0, 0x35, 0x3f, 0xf9, 0x3c, 0x79, 0xbd,
	0xa5, 0xa0, 0x35, 0x3f, 0xb4, 0x7c, 0x3c, 0xb5,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3f, 0xff, 0x3f, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x3a, 0x0f, 0x35, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x20, 0x00, 0x10, 0x3f, 0x3f, 0x3f,
	0x3f, 0x7f, 0x0a, 0xc8, 0xa5, 0xbf, 0x3f, 0x3f,
	0x3f, 0x7f, 0x50, 0xce, 0x8a, 0xbf, 0x3f, 0x3f,
	0x3f, 0x3a, 0x00, 0xe5, 0xd0, 0x35, 0x3f, 0x3f,
	0x3f, 0x3a, 0x50, 0x50, 0xd8, 0x35, 0x3f, 0x3f,
	0x3f, 0x3a, 0x00, 0xa0, 0x8a, 0x35, 0x3f, 0x3f,
	0x3f, 0x3a, 0x00, 0x00, 0x00, 0x35, 0x3f, 0x3f,
	0x3f, 0x7f, 0x00, 0x45, 0x00, 0xbf, 0x3f, 0x3f,
	0x3f, 0x7f, 0x0a, 0x8a, 0x05, 0xbf, 0x3f, 0x3f,
	0x3f, 0x3f, 0x20, 0x45, 0x10, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0xaa, 0xcf, 0x55, 0x3f, 0x3f, 0x3f,
	0x3f, 0x3f, 0x2a, 0xcf, 0x11, 0x3f, 0x3f, 0xff,
	0x3f, 0x3f, 0x2a, 0xcd, 0x44, 0x3f, 0x3a, 0x0f,
	0x3f, 0x3f, 0x3f, 0x25, 0xc3, 0x18, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0xa0, 0x17, 0xd2, 0x3f, 0x10,
	0x3f, 0x3f, 0x2b, 0x1a, 0xe0, 0x2f, 0x17, 0x10,
	0x3f, 0x3f, 0x2b, 0x01, 0x01, 0x80, 0x17, 0x10,
	0x3f, 0x3f, 0x2b, 0x40, 0x70, 0x80, 0x17, 0x10,
	0x3f, 0x3f, 0x2b, 0x1f, 0x02, 0x07, 0x17, 0x10,
	0x3f, 0x3f, 0x3f, 0xe1, 0x3a, 0x5a, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x24, 0x83, 0x10, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x52, 0x00, 0xa1, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3a, 0xf0, 0x35, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3f, 0x03, 0x3f, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x10,
	0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x3f, 0x10,
	0x35, 0x3f, 0x3f, 0x9c, 0x3c, 0x6c, 0x3f, 0x10,
	0xa5, 0xa0, 0x35, 0x2b, 0xe6, 0xf8, 0xbc, 0xa3,
	0xf0, 0xa0, 0x35, 0x2b, 0x14, 0xa0, 0xb4, 0x01,
	0x5a, 0x0a, 0x60, 0x2b, 0x44, 0x00, 0x14, 0x01,
	0x5a, 0xa0, 0x35, 0x2b, 0x15, 0xa0, 0xb4, 0x01,
	0xf0, 0x0a, 0x35, 0x81, 0xe6, 0xf8, 0xbc, 0xa3,
	0xa5, 0xa0, 0x35, 0x3f, 0xb4, 0xdc, 0x3c, 0xb5,
	0x0f, 0xa0, 0x60, 0x6a, 0xf9, 0x3c, 0x79, 0xe8,
	0x5a, 0x0a, 0x35, 0x3f, 0x7e, 0xf1, 0xf4, 0x37,
	0xf0, 0xa0, 0x35, 0x95, 0x91, 0x56, 0x13, 0x95,
	0x5a, 0x0a, 0x60, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a,
	0x0f, 0x0a, 0x35, 0x95, 0x95, 0x95, 0x95, 0x95,
	0x0f, 0xa0, 0x60, 0x3f, 0xff, 0x6a, 0x6a, 0x6a,
	0xa5, 0xa0, 0x35, 0x3a, 0x0f, 0x35, 0x95, 0x95,
	0x0f, 0x0a, 0x35, 0x20, 0x00, 0x10, 0x6a, 0xc0,
	0xa5, 0x0a, 0x75, 0x0a, 0xc8, 0xa5, 0xea, 0x6a,
	0x0f, 0x0a, 0x75, 0x50, 0xce, 0x8a, 0x40, 0xc0,
	0x3f, 0x3f, 0x2a, 0xce, 0x11, 0x3f, 0x20, 0x00,
	0x3f, 0x3f, 0x2a, 0xcd, 0x44, 0x7f, 0x0a, 0xc8,
	0x6a, 0x6a, 0x2a, 0xec, 0x11, 0xd5, 0x50, 0xce,
	0x3f, 0x3f, 0x2a, 0xcd, 0x44, 0x3a, 0x00, 0xe5,
	0x95, 0x95, 0x80, 0x6e, 0x11, 0x3a, 0x50, 0x50,
	0x3f, 0x3f, 0x2a, 0xdc, 0x44, 0x3a, 0x00, 0xa0,
	0x6a, 0x6a, 0x2a, 0x6e, 0x11, 0x90, 0x00, 0x00,
	0x3f, 0x3f, 0x2a, 0xdc, 0x44, 0x7f, 0x00, 0x05,
	0x95, 0x95, 0x80, 0x6e, 0x11, 0x7f, 0x0a, 0x0a,
	0x6a, 0x6a, 0x2a, 0xc4, 0x44, 0x95, 0x20, 0x05,
	0x95, 0x95, 0x80, 0x6e, 0x11, 0x6a, 0xaa, 0x0f,
	0x6a, 0x6a, 0x2a, 0xc4, 0x44, 0x95, 0x80, 0x0f,
	0x95, 0x95, 0x80, 0x6e, 0x11, 0x6a, 0x2a, 0x0d,
	0xc0, 0xc0, 0x80, 0x6e, 0x44, 0xc0, 0x80, 0x0e,
	0x6a, 0x6a, 0x2a, 0xcc, 0x11, 0x95, 0x80, 0x0d,
	0xc0, 0xc0, 0x80, 0x6e, 0x44, 0xc0, 0x80, 0xa4,
	0x10, 0x3f, 0x6e, 0x8a, 0x00, 0x45, 0x9d, 0x10,
	0xa5, 0xbf, 0x3f, 0x9c, 0x3c, 0x6c, 0x3f, 0x10,
	0x8a, 0xea, 0x6e, 0x8a, 0x00, 0x45, 0x9d, 0x10,
	0xd0, 0x35, 0x3f, 0x9c, 0x3c, 0x6c, 0x3f, 0x10,
	0xd8, 0x35, 0xc4, 0x8a, 0x00, 0x45, 0x9d, 0x50,
	0x8a, 0x35, 0x3f, 0x9c, 0x3c, 0x6c, 0x3f, 0x10,
	0x00, 0x60, 0x6e, 0x8a, 0x00, 0x45, 0x9d, 0x50,
	0x00, 0xbf, 0x3f, 0x9c, 0x3c, 0x6c, 0x3f, 0x10,
	0x05, 0xbf, 0xc4, 0x8a, 0x00, 0x45, 0x9d, 0x50,
	0x10, 0x6a, 0x6a, 0x9c, 0x3c, 0x6c, 0x6a, 0x10,
	0x55, 0x95, 0xc4, 0x8a, 0x00, 0x45, 0xc8, 0x50,
	0x11, 0x6a, 0x6a, 0x9c, 0x3c, 0x6c, 0x6a, 0x50,
	0x44, 0xc0, 0xc4, 0x8a, 0x00, 0x45, 0xc8, 0x50,
	0x11, 0x95, 0x95, 0x9c, 0x3c, 0x6c, 0x95, 0x50,
	0x44, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x05,
	0x11, 0x6a, 0x3b, 0x56, 0x13, 0x6a, 0x6a, 0x50,
	0x0f, 0x0a, 0x30, 0x00, 0xe5, 0x00, 0x05, 0x75,
	0x0f, 0x0a, 0x30, 0x50, 0x50, 0x50, 0x52, 0x52,
	0x0f, 0x0a, 0x30, 0x00, 0xa0, 0x00, 0xa1, 0xa1,
	0x0f, 0x0a, 0x30, 0x50, 0x50, 0x50, 0x52, 0x57,
	0x05, 0x00, 0x75, 0x00, 0x00, 0x00, 0x00, 0x2b,
	0x0a, 0x00, 0x75, 0x0a, 0x00, 0x05, 0xaa, 0x57,
	0x05, 0x00, 0x35, 0x20, 0x00, 0x10, 0x2a, 0x2b,
	0x0f, 0x0a, 0x60, 0x3a, 0x0f, 0x35, 0x80, 0x57,
	0x05, 0x00, 0x60, 0x95, 0xff, 0x6a, 0x80, 0x2b,
	0x00, 0x00, 0x60, 0xc0, 0xc0, 0xc0, 0x80, 0x81,
	0x00, 0x0a, 0x60, 0xc0, 0xc0, 0xc0, 0x80, 0x2b,
	0x00, 0x0a, 0xe0, 0xc0, 0xc0, 0xc0, 0x80, 0x81,
	0x0a, 0x00, 0x4a, 0xc0, 0xc0, 0xc0, 0x80, 0x2b,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2b,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
	0x3f, 0x95, 0x80, 0xcc, 0x11, 0x6a, 0x2a, 0x0d,
	0xbb, 0xc0, 0x80, 0x6e, 0x44, 0xc0, 0x80, 0xae,
	0x64, 0x6a, 0x2a, 0xcc, 0x11, 0x95, 0x80, 0x58,
	0x1b, 0xc0, 0x80, 0x6e, 0x44, 0xc0, 0x80, 0xae,
	0x4e, 0xc0, 0x80, 0xcc, 0x11, 0xc0, 0x80, 0x58,
	0x11, 0xc0, 0x80, 0x6e, 0x44, 0xc0, 0x80, 0xae,
	0x44, 0xc0, 0x80, 0xcc, 0x11, 0xc0, 0x80, 0x84,
	0x11, 0xc0, 0x80, 0x6e, 0x44, 0xc0, 0x80, 0xae,
	0x44, 0xc0, 0x80, 0xcc, 0x11, 0xc0, 0x80, 0x84,
	0x11, 0xc0, 0x80, 0xcc, 0x44, 0xc0, 0x80, 0xae,
	0x44, 0xc0, 0x80, 0xcc, 0x11, 0xc0, 0x80, 0xae,
	0x11, 0xc0, 0x80, 0xcc, 0x44, 0xc0, 0x80, 0x0c,
	0x44, 0xc0, 0x80, 0xcc, 0x11, 0xc0, 0x80, 0xae,
	0x00, 0x00, 0x00, 0xcc, 0x44, 0xc0, 0x80, 0x0c,
	0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x00, 0xae,
	0x00, 0x00, 0x00, 0xcc, 0x00, 0x00, 0x00, 0x0c,
	0x44, 0xc0, 0x76, 0xf1, 0xf4, 0x62, 0xc0, 0x05,
	0x11, 0x95, 0xf9, 0x3c, 0x79, 0xbd, 0x95, 0x50,
	0x44, 0x91, 0xb4, 0x7c, 0x3c, 0xb1, 0xc0, 0x05,
	0x11, 0x81, 0xe6, 0xf8, 0xbc, 0xa3, 0xc0, 0x05,
	0x44, 0x81, 0x14, 0xa0, 0xb4, 0x01, 0xc0, 0x05,
	0x11, 0x81, 0x44, 0x00, 0x14, 0x01, 0xc0, 0x05,
	0x44, 0x81, 0x15, 0xa0, 0xb4, 0x01, 0xc0, 0x00,
	0x11, 0x81, 0xe6, 0xf8, 0xbc, 0xa3, 0xc0, 0x05,
	0x44, 0x91, 0xb4, 0xdc, 0x3c, 0xb1, 0xc0, 0x00,
	0x11, 0xc0, 0xf9, 0x3c, 0x79, 0xe8, 0xc0, 0x05,
	0x44, 0xc0, 0x76, 0xf1, 0xf4, 0x62, 0xc0, 0x00,
	0x11, 0xc0, 0x91, 0x56, 0x13, 0xc0, 0xc0, 0x00,
	0x44, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x00,
	0x11, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x7f, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};