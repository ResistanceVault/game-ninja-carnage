// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL12.BIN"

// -------------------------------------------------------
#define NBZONES 7

u8 leveldata[2] = { 2 , NBZONES }; // Position 1 2 3. Nb zones 1 to 9

// -------------------------------------------------------
// A zone is top left corner, bottom right corner and id.
// Data are : 
// Byte : Number of zones
// For each zone : 5 btyes. Top left X, Top Left Y, Bottom Right X, Bottom Right Y, Id.
// start at 0. X to 12. Y to 15
u8 zonesdatas[NBZONES * 5] =
{
	4,13,5,14,1, // 
	6,10,7,11,2, // 
	10,6,11,9,3, // 
	8,13,11,15,4, //
	0,10,3,11,5, //
	6,7,7,8,6, //
	0,1,3,5,7, //
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
u8 scenario[6][SEQLINESIZE] =
{
	{ 199, 1  , 104, 104, 102, 101 , 101, 104 , 0 , 0 }, 
	{ 199, 0  , 2  , 104, 102 ,101 , 101, 104 , 0 , 0 }, 
	{ 199, 0  , 0  , 3  , 102, 101 , 101, 103 , 0 , 0 }, 
	{ 251, 50 , 4  , 104, 2  ,3   , 1   , 3   , 1 , 2}, // QTE
	{ 4  , 255, 0  , 0  , 0  ,0   , 0   , 0   , 0 , 0 },
	{ 255, 0  , 0  , 0  , 0  ,0   , 0   , 0   , 0 , 0 }, // End
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL13 BIN"; // Must be 11 characters

// -------------------------------------------------------
/*
"                                      "; // 38
*/

// Text entering.
// I want to create texte that are displaying on 4 lines.
// Some words are in color.
// there is no real interest to center.
// A message will say if message is too long.
// 251 : couleur 1
// 252 : couleur 2 (positif)
// 253 : couleur 3 (negatif)
// 254 : Encode space + number of spaces
// 255 : next line

u8 messageintro[] = {
/*
"------------------------------------" 36 chars (Line 3)
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je dois me faufiler jusqu'� la  "
 "porte d'entr�e du temple. Un Ninja"
"c'est furtif hein, donc fais en     "
 "sorte que je ne me fasse pas      "
  "rep�rer !                       "
};
u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 4
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Passer par la route principale !"
 "Je me fais rep�rer direct. Ils ont"
"envoy� les chiens et je me suis fais"
 "\2BOUFFER\1. T'en as d'autres des     "
  "bonnes id�es de merde comme �a ?"
},
{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je saute en contrebas et je me  "
 "\2FRACASSE LA TETE\1 contre les       "
"rochers. Y a 10 m�tres gros con. Je "
 "suis pas une tortue ninja !       "
  "R�fl�chis un peu merdeux !      "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je traverse le chemin pour      "
 "tenter de grimper aux arbres. Les "
"chiens m'ont senti direct et je me  "
 "suis fait \2BOUFFER\1. Pourtant       "
  "j'ai mis un bon d�o! Recommence."
},
{
/*
"------------------------------------" 36 chars (Line 3) 104
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'ai pas �t� assez discret et   "
 "les chiens m'entendent. Ils me    "
"\2BOUFFENT LE CUL\1 et les gardes me    "
 "descendent. Je finis au fond de la"
  "rivi�re. \2FUCK\1.                  "
}
};

#define NBMESSSUCESS 4 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
//-------------------------------------" 38 chars 1
/*
"------------------------------------" 36 chars (Line 3)
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. Je m'accroche sur le bord"
 "du pont. Je ne me ferai pas       "
"rep�rer comme �a. Un ninja �a grimpe"
 "partout. Ca t'en bouche un coin ? "
  "Allez continue....              "
},
{
//-------------------------------------" 38 chars 2
/*
"------------------------------------" 36 chars (Line 3)
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2SUPER\1, je longe tout le c�t� du "
 "pont sans me faire remarquer.     "
"Tiens je sais plus si j'ai �teint le"
 "gaz en partant de chez moi ? Rien "
  "� foutre en fait...             "
},
{
//-------------------------------------" 38 chars 3
/*
"------------------------------------" 36 chars (Line 3)
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je vais tenter de me glisser l�-"
 "bas. Si je suis assez discret tout"
"ira bien.                           "
 "\2PREPARE-TOI POUR LA SEQUENCE\1...   "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1, tu te faufiles � pas     "
 "feutr�s dans les fourr�s. Les     "
"chiens ne t'ont pas entendu. On va  "
 "pouvoir arriver � l'entr�e du     "
  "temple...                       "
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0x64, 0xce, 0xda, 0xda, 0x0a, 0xe4, 0xd8, 0xb0,
	0x30, 0xb0, 0xce, 0x64, 0x9a, 0xd8, 0xcc, 0xcf,
	0xe5, 0xe5, 0xd8, 0x05, 0xb0, 0xce, 0xcd, 0x98,
	0xda, 0xce, 0x65, 0x5a, 0x30, 0x98, 0x30, 0x98,
	0xa5, 0xcf, 0x9a, 0xe5, 0x98, 0xcc, 0xcf, 0xb0,
	0x0a, 0xb0, 0xd8, 0xce, 0x6e, 0xcc, 0x30, 0x98,
	0xe5, 0x4f, 0xe4, 0x98, 0xcc, 0x64, 0xcc, 0x64,
	0xda, 0x50, 0x4f, 0x20, 0x64, 0x65, 0xcf, 0xcf,
	0x8f, 0x05, 0xda, 0xcc, 0x30, 0x70, 0xcd, 0xcd,
	0xda, 0x0a, 0xa5, 0xcd, 0xce, 0xcf, 0xcf, 0x9a,
	0x65, 0xa0, 0x1a, 0x64, 0x64, 0x65, 0x98, 0xcf,
	0x70, 0x05, 0xce, 0x65, 0xcf, 0xcc, 0xce, 0x98,
	0xb0, 0x70, 0x65, 0x9a, 0xcf, 0x64, 0x64, 0xcc,
	0xcc, 0x30, 0x98, 0xcf, 0x30, 0x9a, 0x64, 0x65,
	0x9d, 0x98, 0xce, 0xce, 0xcc, 0xcf, 0xcd, 0xcf,
	0xcc, 0xcc, 0x64, 0x65, 0x65, 0x30, 0xda, 0x4f,
	0xa0, 0x5a, 0xcf, 0x9a, 0xcd, 0x65, 0x9d, 0xd8,
	0x25, 0xb0, 0xa0, 0x64, 0x9a, 0x98, 0x9a, 0x8a,
	0x8a, 0x9f, 0x30, 0x98, 0xcf, 0x65, 0xcf, 0x98,
	0x8f, 0x64, 0x6e, 0x6e, 0x25, 0x05, 0xce, 0xce,
	0xda, 0x30, 0xcc, 0x35, 0x98, 0x0a, 0x4f, 0xcd,
	0xcd, 0xe5, 0x30, 0x64, 0xce, 0x65, 0x1a, 0x45,
	0x6e, 0xcd, 0x35, 0x64, 0x98, 0x98, 0x05, 0x20,
	0xcd, 0x64, 0x98, 0x9f, 0xcc, 0x64, 0x8a, 0x0f,
	0xcc, 0x9d, 0xcd, 0xe4, 0x6e, 0x98, 0x0a, 0x0a,
	0x64, 0x6e, 0xcc, 0x35, 0x98, 0xcd, 0x00, 0x51,
	0x9d, 0x3f, 0x70, 0xcc, 0x65, 0xdc, 0x00, 0xf6,
	0xce, 0x6e, 0x98, 0x9d, 0x9d, 0x8d, 0x51, 0xbc,
	0xcc, 0xce, 0x6e, 0x6e, 0x6e, 0x20, 0xf6, 0xf4,
	0xcc, 0xce, 0x98, 0x9d, 0x9d, 0xd9, 0xbc, 0xbc,
	0x98, 0xcc, 0x30, 0x6e, 0x64, 0x20, 0xa0, 0xa0,
	0x9d, 0x3f, 0x6e, 0x20, 0xcf, 0x20, 0x05, 0x00,
	0x30, 0x9a, 0xcf, 0xcf, 0x64, 0x64, 0xcc, 0x30,
	0xce, 0x64, 0x65, 0x50, 0x9a, 0x98, 0x9d, 0x98,
	0x9a, 0x98, 0x8a, 0xa2, 0xe5, 0x64, 0x64, 0x65,
	0xcf, 0x64, 0x51, 0xf9, 0x50, 0x9a, 0x98, 0xce,
	0x9a, 0x30, 0xdc, 0x7c, 0xa2, 0xe5, 0x70, 0xcd,
	0x45, 0x64, 0x65, 0x36, 0xf9, 0x50, 0x9a, 0xce,
	0x8a, 0xb2, 0xdc, 0x7c, 0x7c, 0xa2, 0xe5, 0xcd,
	0x51, 0xed, 0x64, 0xbc, 0x36, 0xf9, 0x50, 0xda,
	0xf6, 0xf4, 0xdc, 0x74, 0xf4, 0xf4, 0xa2, 0xe5,
	0xbc, 0x36, 0xed, 0x32, 0xbc, 0x36, 0xf9, 0x50,
	0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0x7c, 0xa2,
	0x36, 0xbc, 0x36, 0xbc, 0x36, 0xbc, 0x36, 0xf9,
	0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4, 0xf4,
	0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc, 0xbc,
	0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0, 0xa0,
	0x05, 0x00, 0x05, 0x00, 0x05, 0x00, 0x05, 0x00,
	0xd8, 0x98, 0xcc, 0xda, 0xda, 0xce, 0x65, 0xcf,
	0x64, 0x64, 0xda, 0xcc, 0xcc, 0xcc, 0x65, 0x98,
	0x70, 0x9a, 0x88, 0x9d, 0x98, 0xce, 0xce, 0x98,
	0x65, 0xcd, 0x25, 0xcf, 0x9a, 0x9f, 0x30, 0x64,
	0x30, 0xb0, 0xda, 0xcf, 0x6e, 0x98, 0xce, 0xce,
	0x64, 0x6e, 0x6f, 0xce, 0x64, 0xcf, 0x8a, 0x30,
	0x3f, 0x35, 0x3a, 0x9a, 0xcf, 0xda, 0x30, 0xcc,
	0x3a, 0x6f, 0xce, 0xd8, 0x05, 0xe4, 0x64, 0x9d,
	0x64, 0x35, 0x9d, 0xcc, 0xda, 0x4f, 0x9d, 0x98,
	0xb0, 0x9d, 0x3f, 0x6f, 0xcd, 0x9a, 0xcc, 0x6e,
	0x30, 0x3f, 0xcc, 0x8f, 0xe4, 0x98, 0x9d, 0x9d,
	0xda, 0x9a, 0xcc, 0x98, 0xcf, 0x98, 0xce, 0x6e,
	0x9d, 0x3f, 0x65, 0xce, 0xcc, 0xb0, 0x65, 0xcc,
	0x98, 0x35, 0x35, 0xe5, 0xcf, 0x44, 0x20, 0x70,
	0xa5, 0xce, 0x35, 0xcd, 0x65, 0xb0, 0xcd, 0x65,
	0x0a, 0x9a, 0x64, 0x6e, 0x9a, 0x9a, 0x20, 0x9a,
	0x35, 0x3f, 0x3f, 0x3a, 0xa0, 0x65, 0x10, 0x0a,
	0x6e, 0x6e, 0x35, 0x98, 0x35, 0xcc, 0x05, 0x25,
	0x3a, 0x3f, 0x6e, 0x64, 0x9a, 0x3f, 0x10, 0xaf,
	0x65, 0x35, 0x3f, 0x98, 0x9d, 0x64, 0x05, 0x25,
	0x64, 0x9d, 0xcc, 0x3f, 0x3f, 0x6e, 0x8a, 0x0f,
	0x6e, 0x6e, 0x30, 0x6e, 0x6e, 0x9d, 0x2a, 0xa5,
	0x6e, 0x3f, 0x3f, 0x3f, 0x6e, 0x3f, 0x3a, 0x05,
	0x98, 0x98, 0x98, 0x3a, 0xcc, 0xcd, 0xcd, 0x8a,
	0x9a, 0x64, 0x98, 0xcd, 0x64, 0x64, 0xcf, 0x8a,
	0x98, 0xcc, 0x98, 0x98, 0x9a, 0x98, 0xcd, 0x45,
	0xcc, 0x3f, 0x6e, 0x6f, 0x65, 0x64, 0xce, 0x8a,
	0x98, 0x6e, 0x64, 0xcd, 0x8a, 0x9a, 0xcf, 0xcd,
	0x64, 0x6e, 0x35, 0x35, 0xcd, 0x98, 0xe5, 0xce,
	0x30, 0x6e, 0x35, 0x6e, 0x6e, 0x6e, 0x25, 0xe5,
	0x45, 0x6e, 0x98, 0x9d, 0x3f, 0x9d, 0x88, 0x00,
	0x65, 0x98, 0xcc, 0x64, 0x6e, 0x6e, 0x20, 0x1a,
	0x10, 0x0a, 0x10, 0x0a, 0x10, 0x0a, 0x10, 0x0a,
	0x05, 0x25, 0x05, 0x25, 0x05, 0x25, 0x10, 0x25,
	0x10, 0xaf, 0x10, 0xaf, 0x10, 0xaf, 0x50, 0x00,
	0x05, 0x25, 0x05, 0x25, 0x05, 0x25, 0x00, 0x65,
	0x10, 0x0f, 0x10, 0x0f, 0x50, 0x45, 0x9a, 0x98,
	0x05, 0xa5, 0x05, 0xa5, 0x00, 0xe5, 0x64, 0xcc,
	0x00, 0x0f, 0x00, 0x00, 0xe5, 0x9a, 0x9d, 0x98,
	0x05, 0x05, 0x50, 0xda, 0xce, 0x64, 0x6e, 0x64,
	0xa0, 0x0a, 0xe5, 0x65, 0x30, 0x9d, 0x98, 0x98,
	0x05, 0x00, 0x50, 0x9a, 0x64, 0x64, 0x64, 0x65,
	0xa0, 0xa0, 0xa0, 0x30, 0x98, 0xcd, 0x9a, 0x9a,
	0x05, 0x50, 0x50, 0xcf, 0x64, 0x70, 0x65, 0x15,
	0x8a, 0x8a, 0x8a, 0xcf, 0x9a, 0x88, 0xcf, 0xce,
	0xcf, 0xcf, 0x45, 0x45, 0x45, 0x70, 0x9d, 0x35,
	0x50, 0x50, 0xa0, 0xa0, 0xa0, 0xa0, 0x64, 0x30,
	0x1a, 0xba, 0x1a, 0x1a, 0x0f, 0x10, 0x3f, 0x30,
	0x00, 0xe4, 0x65, 0x3f, 0xcd, 0x64, 0x30, 0xcc,
	0x00, 0x4f, 0xda, 0x9d, 0x3f, 0x98, 0x9a, 0x8a,
	0x05, 0xe5, 0x65, 0xce, 0x9d, 0xcd, 0x45, 0x9a,
	0x30, 0xce, 0x98, 0x8a, 0xb0, 0x98, 0x30, 0x9d,
	0x30, 0xda, 0x65, 0x65, 0x35, 0x98, 0x9d, 0x6e,
	0xcd, 0x05, 0xda, 0x1a, 0x98, 0x98, 0x3f, 0x3f,
	0xb0, 0xe5, 0xa0, 0x50, 0x65, 0x98, 0x44, 0x9f,
	0xe5, 0xda, 0xda, 0x05, 0x65, 0x64, 0xa0, 0xb0,
	0x65, 0x98, 0x9a, 0xa0, 0xcf, 0x9f, 0x64, 0x6e,
	0x98, 0xda, 0xce, 0x20, 0x64, 0x35, 0x3f, 0x9d,
	0x30, 0xcc, 0xe5, 0xa0, 0x9d, 0x3f, 0x6f, 0x3f,
	0x98, 0xcc, 0xcf, 0xda, 0xcc, 0x9d, 0xcc, 0xcc,
	0x98, 0x64, 0x98, 0x98, 0xcc, 0x30, 0x6f, 0x64,
	0xe4, 0x9d, 0x70, 0xcc, 0xcc, 0x64, 0xce, 0x64,
	0xcf, 0xe5, 0x30, 0x6e, 0x9a, 0xda, 0xcf, 0xcc,
	0xe5, 0x64, 0x64, 0x3a, 0x65, 0x8a, 0xcf, 0x9a,
	0x3f, 0x1a, 0xcc, 0x70, 0xcd, 0x9d, 0x88, 0x00,
	0x64, 0x25, 0x3a, 0xf0, 0xda, 0x9a, 0x20, 0x0a,
	0x3f, 0x65, 0x70, 0xa0, 0xe5, 0xcf, 0x45, 0x00,
	0x30, 0x10, 0x9a, 0xda, 0xcf, 0x65, 0x8a, 0xc3,
	0x30, 0x8a, 0x30, 0x9d, 0xcc, 0x9a, 0xcf, 0xc3,
	0x64, 0x30, 0x64, 0x6e, 0x6e, 0x65, 0x65, 0x92,
	0x9d, 0x98, 0x1a, 0x98, 0x98, 0x98, 0xcf, 0x61,
	0x9d, 0x6e, 0xda, 0xce, 0xcf, 0x64, 0x65, 0x00,
	0x64, 0x3f, 0x25, 0xe5, 0xda, 0x9a, 0x98, 0x50,
	0x30, 0x9d, 0x88, 0x1a, 0xa5, 0xe5, 0x64, 0x20,
	0xcc, 0x3a, 0x70, 0x45, 0x50, 0x50, 0x9a, 0x98,
	0xcc, 0x70, 0x88, 0xe5, 0x0a, 0x0a, 0xe5, 0x64,
	0x9d, 0x70, 0xa5, 0x65, 0x05, 0xf0, 0x9a, 0x9a,
	0x3a, 0x3a, 0xd8, 0x65, 0x98, 0x30, 0x65, 0x30,
	0x9d, 0x10, 0xdd, 0x9d, 0xcc, 0x3f, 0x05, 0x4a,
	0x3a, 0x98, 0xee, 0xee, 0xcc, 0x64, 0x1f, 0xba,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x9a, 0x98,
	0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x65, 0xcc, 0x30,
	0x00, 0x00, 0x00, 0x00, 0x10, 0x65, 0x64, 0x64,
	0x87, 0x0f, 0xc3, 0x87, 0x1a, 0x6e, 0x30, 0x9d,
	0x25, 0x5a, 0xc3, 0x25, 0x1f, 0x98, 0x35, 0xcc,
	0x87, 0xa5, 0x92, 0x87, 0xb5, 0x20, 0xcc, 0x6f,
	0x70, 0x5a, 0x61, 0x70, 0x4e, 0x6f, 0x65, 0x9a,
	0xa0, 0x05, 0x00, 0xa0, 0x15, 0x98, 0x2a, 0x30,
	0x00, 0x0a, 0x50, 0x00, 0x44, 0x6f, 0x35, 0x10,
	0x00, 0x00, 0x00, 0xb0, 0x64, 0x3f, 0x98, 0x10,
	0x98, 0x9a, 0x9a, 0x9a, 0x9d, 0xcf, 0x64, 0x30,
	0x64, 0x64, 0x64, 0x64, 0xc8, 0x8a, 0x9a, 0x30,
	0x9a, 0x98, 0x9d, 0xc8, 0x90, 0x10, 0x00, 0x45,
	0x64, 0x35, 0xc0, 0x75, 0x30, 0x00, 0x20, 0x00,
	0x90, 0x3a, 0x30, 0x95, 0x65, 0x45, 0x30, 0x30,
	0x30, 0xba, 0xba, 0x30, 0x30, 0x45, 0x61, 0x65,
	0x30, 0x30, 0x98, 0xcd, 0xcd, 0x9a, 0xcd, 0xce,
	0x64, 0x65, 0x30, 0x64, 0x30, 0xda, 0x30, 0xcf,
	0x30, 0x64, 0x98, 0xcf, 0xce, 0xcf, 0xe4, 0xb0,
	0x64, 0x70, 0xcc, 0x30, 0xcf, 0xa5, 0x45, 0xcc,
	0x9a, 0xce, 0x98, 0xd8, 0xcf, 0x0a, 0x4f, 0xce,
	0xda, 0x64, 0x9a, 0x8a, 0xda, 0x05, 0xe5, 0xe4,
	0xa5, 0x45, 0xce, 0xa5, 0xa0, 0xe5, 0xcf, 0xe5,
	0x00, 0xb0, 0xcd, 0xcf, 0x9a, 0x98, 0xcf, 0xcf,
	0x50, 0x70, 0xcf, 0xce, 0xe5, 0x64, 0x65, 0xcd,
	0x9a, 0xcf, 0x30, 0xf0, 0x1a, 0x9d, 0x65, 0xcf,
	0xb0, 0x3a, 0x65, 0xcf, 0x64, 0x6e, 0x98, 0xda,
	0x9a, 0x64, 0x64, 0xcf, 0x9a, 0x98, 0xcc, 0x65,
	0x30, 0x98, 0x9a, 0x70, 0xcf, 0x65, 0x30, 0xe5,
	0x30, 0x30, 0x30, 0xa5, 0xe5, 0xcf, 0xda, 0x44,
	0xce, 0x64, 0x65, 0x4f, 0xcd, 0xa5, 0xe5, 0xda,
	0x65, 0x9d, 0x70, 0x1a, 0x70, 0x9a, 0x9a, 0xcf,
	0xcc, 0x98, 0x64, 0xcc, 0x98, 0x97, 0x35, 0xba,
	0xcc, 0x3a, 0x75, 0xcc, 0xcd, 0x3f, 0x89, 0x75,
	0xce, 0x6e, 0x30, 0x9d, 0xc9, 0x17, 0xdd, 0x21,
	0x30, 0x9d, 0x98, 0xee, 0x9d, 0x21, 0x3a, 0xbf,
	0x30, 0x64, 0x6e, 0x64, 0x3f, 0x12, 0x12, 0x17,
	0x65, 0x9d, 0x98, 0x9d, 0x6e, 0x3a, 0xba, 0x3f,
	0x64, 0xcc, 0x6e, 0x35, 0x95, 0x9d, 0x75, 0x3f,
	0x64, 0x9d, 0x98, 0x9d, 0x95, 0x3f, 0x21, 0x3f,
	0xb0, 0x6e, 0x6e, 0x64, 0x6e, 0x89, 0x35, 0x6a,
	0x44, 0xc6, 0x98, 0x9d, 0x95, 0xdd, 0x35, 0x95,
	0x4f, 0x6e, 0x64, 0x64, 0x3f, 0x3a, 0xbf, 0x6a,
	0xe5, 0x98, 0xcd, 0x9d, 0x6e, 0x12, 0x17, 0x95,
	0xcf, 0x64, 0xee, 0xee, 0x6e, 0xba, 0x3f, 0x6a,
	0xcd, 0x75, 0xdd, 0xcc, 0x2b, 0x75, 0x3f, 0x95,
	0x9a, 0xee, 0xee, 0xcc, 0x57, 0xbf, 0x6a, 0x6a,
	0x9a, 0xdd, 0xcc, 0x6e, 0x7f, 0x17, 0xc0, 0x95,
	0xba, 0xba, 0x3a, 0x30, 0x30, 0x00, 0x8a, 0x65,
	0x17, 0x6a, 0xc0, 0xc0, 0x3f, 0x20, 0x65, 0x45,
	0x3f, 0xc0, 0xc0, 0x3f, 0x1a, 0x20, 0xcf, 0xcf,
	0x3f, 0xc0, 0x3f, 0xcc, 0x20, 0x20, 0x8a, 0xce,
	0x6a, 0x95, 0xcd, 0xcc, 0xcc, 0xcf, 0xcf, 0xce,
	0xc0, 0x3f, 0x6e, 0x9a, 0xcf, 0xcc, 0xcf, 0x44,
	0x6a, 0x6e, 0x3a, 0xcc, 0xcc, 0xcc, 0xcf, 0xcc,
	0x95, 0x9d, 0xcc, 0x65, 0x98, 0x64, 0xcc, 0xce,
	0x6e, 0xcc, 0x9d, 0xcc, 0x8a, 0x45, 0xcc, 0xcc,
	0x95, 0x9d, 0xcc, 0xcc, 0x8a, 0x00, 0xcf, 0x45,
	0x3f, 0x6e, 0x6f, 0xcf, 0x00, 0x00, 0x98, 0xce,
	0x95, 0x3f, 0x6e, 0x45, 0x8a, 0x10, 0xcc, 0x9d,
	0x3f, 0x7f, 0x8a, 0x45, 0x8a, 0x44, 0x44, 0xcf,
	0x3f, 0x3f, 0x30, 0x65, 0x00, 0x88, 0x8a, 0x45,
	0x2b, 0x2b, 0xff, 0x65, 0xcf, 0xcf, 0x44, 0xcf,
	0x17, 0xbf, 0xff, 0xce, 0xcf, 0xcf, 0xcc, 0xce,
	0x9a, 0xce, 0x25, 0xb0, 0xcf, 0xce, 0x64, 0x65,
	0x8f, 0x00, 0x0f, 0x0a, 0x0a, 0x30, 0x98, 0x98,
	0x00, 0x1a, 0xab, 0x57, 0x25, 0x10, 0x64, 0x9a,
	0x61, 0xab, 0x17, 0x2b, 0xeb, 0x7f, 0x30, 0x30,
	0x83, 0x92, 0xb0, 0xd7, 0x57, 0x95, 0x3a, 0x44,
	0x43, 0x25, 0x30, 0x61, 0xff, 0x7f, 0xff, 0x30,
	0x92, 0x30, 0xbf, 0xba, 0x25, 0xbf, 0xff, 0xbf,
	0x30, 0x61, 0x30, 0xee, 0x30, 0x30, 0x10, 0xeb,
	0xc9, 0x92, 0xcf, 0xdf, 0x75, 0x12, 0xcd, 0x10,
	0x75, 0x43, 0x35, 0x65, 0xab, 0xee, 0x65, 0x0a,
	0x47, 0x2a, 0xba, 0xab, 0x57, 0xdd, 0xcf, 0x30,
	0x3a, 0x17, 0x17, 0x17, 0x7f, 0xee, 0x7f, 0x75,
	0x83, 0x2b, 0x2b, 0x3f, 0x43, 0xeb, 0x17, 0x17,
	0x1a, 0x03, 0x17, 0x2b, 0xba, 0x30, 0x30, 0x30,
	0x4b, 0x03, 0x03, 0xba, 0x75, 0x03, 0x17, 0x17,
	0x21, 0x57, 0x30, 0x75, 0x03, 0x2b, 0x2b, 0x2b,
	0x30, 0xee, 0x9d, 0x9d, 0x57, 0x6a, 0x6a, 0xc0,
	0xcc, 0x64, 0x6e, 0x2b, 0x7f, 0x95, 0xc0, 0x95,
	0x35, 0x9d, 0x9d, 0x17, 0xab, 0xc0, 0xc0, 0x6a,
	0xc6, 0x6e, 0x2b, 0x2b, 0xbf, 0x95, 0xc0, 0x95,
	0x95, 0x9d, 0x17, 0x57, 0x17, 0xc0, 0xc0, 0x3f,
	0x6a, 0x6e, 0x2b, 0xab, 0x6a, 0x95, 0x95, 0x95,
	0x95, 0x9d, 0x57, 0x17, 0xc0, 0x6a, 0x3f, 0x3f,
	0x6a, 0x6e, 0x2b, 0x3f, 0x95, 0x95, 0x3f, 0x2b,
	0xbf, 0x9d, 0xbf, 0x6a, 0x6a, 0x3f, 0x3f, 0x03,
	0x64, 0x2b, 0x3f, 0x3f, 0x3f, 0x2b, 0x2b, 0xba,
	0xbf, 0x9d, 0x3f, 0x3f, 0x17, 0x17, 0x57, 0x70,
	0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0x2b, 0xba, 0xb0,
	0x17, 0x17, 0x17, 0x03, 0x03, 0x57, 0x30, 0x30,
	0xba, 0xba, 0xab, 0xab, 0xab, 0xba, 0x30, 0x70,
	0x17, 0x17, 0x03, 0x03, 0x57, 0x30, 0x30, 0xf5,
	0x2b, 0x2b, 0x2b, 0x2b, 0xba, 0x30, 0x30, 0x5f,
	0x2b, 0x7f, 0x7f, 0xef, 0xcc, 0x98, 0x64, 0x9a,
	0x3f, 0x17, 0xbf, 0xbf, 0xcc, 0xcd, 0x8a, 0x44,
	0x2b, 0xeb, 0xff, 0xd7, 0x6f, 0x30, 0xcd, 0x00,
	0x7f, 0xbf, 0x17, 0x17, 0x3a, 0x30, 0x30, 0x45,
	0x17, 0xba, 0x7f, 0x7f, 0x30, 0x30, 0x35, 0x00,
	0x7f, 0x2b, 0x2b, 0x30, 0x20, 0x00, 0x45, 0x45,
	0x17, 0x17, 0x57, 0x30, 0x30, 0x30, 0x10, 0xcd,
	0x03, 0xab, 0x2b, 0x30, 0x30, 0x65, 0x8a, 0xcc,
	0xba, 0xf5, 0x57, 0x30, 0x74, 0x45, 0xcf, 0xcc,
	0xf0, 0x70, 0xba, 0x30, 0x30, 0x45, 0x9a, 0xcf,
	0x30, 0x70, 0x57, 0x30, 0x20, 0x00, 0x45, 0xce,
	0x30, 0x70, 0xba, 0x20, 0x00, 0x45, 0x44, 0xce,
	0xb0, 0x25, 0x12, 0x20, 0xe7, 0x45, 0x65, 0xcc,
	0xba, 0x70, 0xba, 0x20, 0x9a, 0x00, 0x9a, 0x3a,
	0xba, 0x25, 0x02, 0xb2, 0x00, 0x45, 0x45, 0x8a,
	0xba, 0x70, 0xba, 0x9a, 0x45, 0x20, 0x45, 0xcd,
	0x75, 0x30, 0xab, 0x17, 0x17, 0x17, 0x17, 0x17,
	0xba, 0xab, 0x2b, 0x2b, 0x3f, 0x3f, 0x3f, 0x3f,
	0x75, 0x3f, 0x3f, 0x3f, 0x3f, 0x95, 0x95, 0x95,
	0x17, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a, 0x6a,
	0x3f, 0x95, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95,
	0x6a, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x6a,
	0x95, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95,
	0x6a, 0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x6a,
	0x95, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95,
	0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x3f,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x95,
	0x6a, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x7f,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x95, 0x95, 0x3a,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xba,
	0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0x6a, 0x95, 0x30,
	0xc0, 0xc0, 0xc0, 0xc0, 0x81, 0xc0, 0x7f, 0x30,
	0x17, 0x17, 0x17, 0x57, 0x30, 0x30, 0x70, 0xff,
	0x3f, 0x2b, 0x2b, 0xba, 0x30, 0x30, 0x25, 0x03,
	0x95, 0x3f, 0x57, 0x30, 0x30, 0x30, 0xf5, 0x57,
	0x6a, 0x3f, 0xba, 0x30, 0x30, 0x70, 0x0b, 0x2b,
	0x95, 0x2b, 0x30, 0x30, 0x30, 0x25, 0xbf, 0x57,
	0x3f, 0x7f, 0x30, 0x30, 0x30, 0xa5, 0x2b, 0x3a,
	0x95, 0xba, 0x30, 0x30, 0x30, 0x5f, 0x17, 0xba,
	0x3f, 0x30, 0x30, 0x30, 0x70, 0x0b, 0x2b, 0x75,
	0x7f, 0x30, 0x30, 0x30, 0x25, 0xbf, 0x7f, 0x35,
	0xba, 0x30, 0x30, 0x30, 0x5a, 0x17, 0x12, 0x7f,
	0x30, 0x30, 0x30, 0x70, 0x0b, 0x3f, 0xba, 0x3a,
	0x30, 0x30, 0x30, 0x25, 0xbf, 0x2b, 0x75, 0x3a,
	0x30, 0x30, 0x30, 0xa5, 0x17, 0x7f, 0x35, 0xfa,
	0x30, 0x30, 0x30, 0x5f, 0x3f, 0x12, 0xbf, 0x25,
	0x30, 0x30, 0x70, 0x0b, 0x3f, 0xba, 0x7f, 0x25,
	0x30, 0x30, 0x25, 0x35, 0x3f, 0x30, 0xba, 0xa5,
	0x12, 0x25, 0x57, 0x30, 0x30, 0x8a, 0x9a, 0xce,
	0xba, 0x25, 0xbf, 0xc0, 0x12, 0x30, 0x30, 0xcd,
	0x12, 0x25, 0x35, 0x6a, 0x7f, 0x30, 0xcf, 0xce,
	0xba, 0x70, 0x0b, 0x3f, 0x2b, 0x30, 0xda, 0x30,
	0x30, 0xa5, 0x30, 0x21, 0x3f, 0x30, 0xcf, 0xe5,
	0x30, 0x5f, 0x3f, 0x3f, 0x3f, 0x65, 0xda, 0x50,
	0xba, 0x0b, 0x95, 0xc0, 0x2b, 0x30, 0x20, 0xa0,
	0x70, 0xbf, 0x6a, 0xc0, 0xd5, 0x30, 0x70, 0x50,
	0x25, 0x17, 0x95, 0xc0, 0x3a, 0x65, 0xb0, 0xa0,
	0x25, 0x17, 0x6a, 0x6a, 0x3a, 0x50, 0x9a, 0xda,
	0xe1, 0x3f, 0x95, 0x95, 0x7a, 0x9a, 0x98, 0xda,
	0x5f, 0x6a, 0x3f, 0x6a, 0x6f, 0x64, 0x65, 0xa0,
	0x0b, 0x3f, 0x95, 0x95, 0x2a, 0x9a, 0x8a, 0x00,
	0x83, 0x6a, 0x3f, 0x6a, 0x6f, 0x45, 0x45, 0xa0,
	0xbf, 0x3f, 0x3f, 0x95, 0x12, 0x8a, 0xb0, 0x50,
	0x17, 0x3f, 0x3f, 0x2b, 0x3a, 0x50, 0x9a, 0xa0,
	0xc0, 0xc0, 0xc0, 0xc0, 0x3f, 0xc0, 0xba, 0x30,
	0xc0, 0xc0, 0xc0, 0x95, 0x6a, 0x95, 0x30, 0x30,
	0xc0, 0xc0, 0xc0, 0x2b, 0xc0, 0xd5, 0x30, 0x30,
	0xc0, 0xc0, 0x95, 0x17, 0xc0, 0x3a, 0x30, 0x30,
	0xc0, 0xc0, 0x81, 0x6a, 0xc0, 0xba, 0x30, 0x30,
	0xc0, 0xc0, 0x2b, 0x6a, 0x95, 0x30, 0x30, 0x30,
	0xc0, 0xc0, 0xbf, 0x6a, 0xd5, 0x30, 0x30, 0x30,
	0xc0, 0x95, 0xbf, 0xc0, 0x3a, 0x30, 0x30, 0xb0,
	0xc0, 0x7f, 0x3f, 0xc0, 0xba, 0x30, 0x30, 0x30,
	0xc0, 0x35, 0x6a, 0x95, 0x30, 0x30, 0x30, 0xb0,
	0x95, 0xbf, 0x95, 0xd5, 0x30, 0x30, 0x70, 0x30,
	0xd5, 0x6a, 0x6a, 0x3a, 0x30, 0x30, 0x30, 0xb0,
	0x3a, 0x3f, 0xc0, 0xba, 0x30, 0x30, 0x70, 0x30,
	0x75, 0x6a, 0x3f, 0x30, 0x30, 0x30, 0xb0, 0x30,
	0xbf, 0x3f, 0xd5, 0x30, 0x30, 0x30, 0xf0, 0x70,
	0x3f, 0x6a, 0x3a, 0x30, 0x70, 0x30, 0xb0, 0x70,
	0x30, 0x30, 0xa5, 0xbf, 0x2b, 0x75, 0x3a, 0x4b,
	0x30, 0x30, 0x1a, 0xbf, 0x7f, 0x75, 0xba, 0x4b,
	0x30, 0x70, 0x5f, 0x3f, 0x12, 0xff, 0x70, 0x5f,
	0x30, 0x70, 0x0b, 0x3f, 0xba, 0xff, 0x25, 0xf5,
	0x30, 0x25, 0x35, 0x2b, 0x30, 0xba, 0x25, 0x21,
	0x30, 0xa5, 0xbf, 0x57, 0x75, 0x30, 0xa5, 0x83,
	0x30, 0x1a, 0x3f, 0x6b, 0x30, 0xba, 0x1a, 0x21,
	0x70, 0x4b, 0x17, 0x12, 0x75, 0x30, 0x1a, 0x97,
	0x70, 0x4b, 0x2b, 0xba, 0xba, 0x70, 0x70, 0x21,
	0x25, 0x75, 0x17, 0x92, 0xff, 0x25, 0xaf, 0xb5,
	0x25, 0xab, 0x2b, 0x75, 0xba, 0x25, 0xfa, 0x1f,
	0xb0, 0x57, 0x57, 0x75, 0xba, 0xb0, 0xaf, 0xb5,
	0x75, 0xab, 0x57, 0x75, 0x30, 0x5f, 0x20, 0x1f,
	0xf5, 0x57, 0x12, 0x30, 0x30, 0x1f, 0x25, 0x15,
	0xff, 0xab, 0xba, 0x75, 0x70, 0x35, 0x20, 0x1f,
	0xff, 0xff, 0xba, 0xb0, 0x25, 0xbf, 0x20, 0x15,
	0x3f, 0x3f, 0x17, 0x17, 0x12, 0xe5, 0x30, 0x50,
	0x17, 0x2b, 0x2b, 0x2b, 0x3a, 0x50, 0x9a, 0xa0,
	0x6a, 0x3f, 0x17, 0x17, 0x43, 0x65, 0x65, 0x50,
	0x3f, 0x3f, 0x3f, 0x2b, 0x2b, 0x9a, 0xda, 0xa0,
	0x6a, 0x3f, 0x17, 0x17, 0x43, 0x65, 0xf0, 0x50,
	0x3f, 0x2b, 0x83, 0x2b, 0x92, 0xda, 0xa0, 0xa0,
	0x6a, 0x3f, 0x3f, 0x57, 0xda, 0x50, 0x50, 0x54,
	0x6a, 0x92, 0x92, 0x8a, 0xa0, 0xa0, 0xa0, 0xbc,
	0x6a, 0x30, 0xda, 0x50, 0x50, 0x50, 0x54, 0x39,
	0x6a, 0xa0, 0xa0, 0xa0, 0xa0, 0x00, 0xbc, 0x36,
	0x6a, 0x00, 0x00, 0x51, 0x51, 0xf4, 0x3c, 0xf9,
	0xc0, 0xa2, 0xf2, 0xf8, 0xbc, 0x36, 0x00, 0x00,
	0x6a, 0x51, 0xbc, 0x3c, 0xa2, 0xa2, 0xa2, 0xbc,
	0xc0, 0xa2, 0xf2, 0xfc, 0xf8, 0x51, 0x54, 0x3c,
	0x6a, 0x00, 0x00, 0x00, 0x00, 0x00, 0xbc, 0x39,
	0x95, 0x00, 0x51, 0x51, 0xf2, 0xbc, 0x3c, 0xf8,
	0xbf, 0x3f, 0x12, 0x30, 0x30, 0x70, 0x30, 0xb0,
	0x6a, 0x6a, 0xba, 0x70, 0x70, 0x25, 0xb0, 0xf5,
	0x3f, 0x3f, 0x30, 0x30, 0x30, 0xa5, 0x70, 0x75,
	0x6a, 0x7f, 0x30, 0x70, 0x30, 0x50, 0xa5, 0xff,
	0x3f, 0x12, 0x30, 0x30, 0x70, 0x50, 0x70, 0xba,
	0x2b, 0x3a, 0x70, 0x70, 0x25, 0x10, 0xb0, 0xff,
	0x17, 0xba, 0x30, 0x30, 0xa0, 0x5a, 0x4b, 0xba,
	0x2b, 0x30, 0x70, 0x70, 0x0a, 0xb0, 0xf5, 0x75,
	0x17, 0x30, 0xb0, 0x30, 0x00, 0x70, 0x75, 0x83,
	0x7f, 0x70, 0x70, 0x70, 0x05, 0xa5, 0xc3, 0x75,
	0x12, 0xb0, 0xb0, 0x25, 0x10, 0xa5, 0x92, 0x83,
	0xba, 0x70, 0x70, 0x20, 0x5a, 0x5a, 0xc3, 0x43,
	0x30, 0xb0, 0xb0, 0xa0, 0xb0, 0x1a, 0x83, 0x83,
	0x70, 0x70, 0x70, 0x0a, 0x70, 0x4b, 0xc3, 0x43,
	0xb0, 0xb0, 0xb0, 0x00, 0xa5, 0xe1, 0x83, 0x92,
	0x70, 0x70, 0x70, 0x00, 0x5a, 0x61, 0xc3, 0x61,
	0xba, 0xff, 0x30, 0x30, 0x25, 0x3f, 0x20, 0x01,
	0xff, 0xd7, 0x30, 0xb0, 0xb0, 0x3f, 0xa0, 0x15,
	0xba, 0xc3, 0x61, 0x30, 0x4b, 0x6b, 0x20, 0x01,
	0x75, 0x92, 0x30, 0x30, 0x1a, 0x97, 0xa0, 0x41,
	0xc3, 0xc3, 0x61, 0x70, 0xc3, 0x6b, 0x0a, 0x10,
	0x61, 0x92, 0x92, 0x25, 0x61, 0xd2, 0x0a, 0x41,
	0x83, 0x92, 0x92, 0xa5, 0x00, 0x0f, 0x00, 0x10,
	0x43, 0x61, 0x30, 0x5a, 0xf8, 0x00, 0x00, 0x05,
	0x6b, 0x30, 0x30, 0xf4, 0xbc, 0xf8, 0xa2, 0x00,
	0x43, 0x61, 0x70, 0xb6, 0xb9, 0x28, 0x00, 0x05,
	0x92, 0x92, 0x25, 0xf4, 0x3c, 0x36, 0xf8, 0xa2,
	0x43, 0x30, 0x70, 0xb6, 0xb9, 0x78, 0x00, 0x00,
	0x92, 0x92, 0xf1, 0xf4, 0x3c, 0x39, 0x79, 0x00,
	0xc3, 0x30, 0x5a, 0xbc, 0x33, 0x78, 0x00, 0x50,
	0x92, 0x70, 0xf6, 0x3c, 0x3c, 0xbc, 0x7c, 0xa0,
	0x61, 0x25, 0xbc, 0x33, 0x39, 0x78, 0x00, 0x54,
	0x6a, 0xa2, 0xa2, 0xf2, 0xf0, 0xf0, 0x51, 0x00,
	0x95, 0x00, 0x51, 0xbc, 0x7c, 0xa2, 0xa2, 0x00,
	0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0xfc,
	0x3f, 0x00, 0xa2, 0xa2, 0xf3, 0xf3, 0xf6, 0x7c,
	0x2b, 0x51, 0x51, 0xf3, 0xf6, 0xf6, 0x3c, 0xf9,
	0x17, 0x00, 0x51, 0x51, 0xf9, 0xf9, 0xf9, 0xa2,
	0x83, 0x00, 0x00, 0x51, 0xf3, 0xa2, 0xa2, 0x00,
	0xb0, 0x00, 0xf6, 0x3c, 0xf9, 0x51, 0x00, 0x51,
	0x0a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x51, 0xf2,
	0x00, 0xa2, 0xa2, 0xa2, 0xf3, 0xf2, 0xf2, 0xf8,
	0x51, 0xa0, 0x51, 0x51, 0xf1, 0xf1, 0xf4, 0xf1,
	0xf2, 0xa8, 0x00, 0x00, 0xf2, 0xf2, 0xf3, 0xa2,
	0xf6, 0x3c, 0xf9, 0x51, 0x00, 0x00, 0x00, 0x00,
	0x39, 0x78, 0x00, 0x00, 0x51, 0x51, 0xf3, 0x51,
	0x50, 0xbc, 0x7c, 0xa2, 0xa2, 0x00, 0x00, 0x00,
	0x7c, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};