// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define BINNAME "LEVEL91.BIN"

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
	2,2,5,4,1, // 
	0,9,3,11,2, // 
	8,11,11,15,3, // 
	10,3,11,7,4, // 
	6,7,9,9,5, // 
	2,13,5,15,6, // 
	0,0,11,1,7, // 
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
u8 scenario[18][SEQLINESIZE] =
{
	{ 253, 7  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Palette cycle
	{ 252, 9  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Cursor mode

	{ 250, 1  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 199, 2  , 104, 104, 104,104, 110,109, 0 , 0 }, // Hit head
	{ 199, 0  , 3  , 105, 105,105, 110,109, 0 , 0 }, // dodge hand
													{251, 45, 4,  101 , 3  ,2  , 1  , 3  ,2 , 255}, //
													{ 0  ,0   , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 },
	{ 199, 0  , 0  , 5  , 106,106, 110,109, 0 , 0 }, // dodge tail
													{251, 45, 6,  102 , 1  ,2  , 4  , 2  ,3 , 4}, //
													{ 3  ,1   , 255  , 4  , 0  ,0  , 0  , 0 , 0 , 0 },	
	{ 90 , 0  , 0  , 0  , 7  ,107, 110,109, 0 , 0 }, // dodge attack
	{ 90 , 0  , 0  , 0  , 0  ,8  , 110,109, 0 , 0 },  // dodge second hand
													{251, 45, 9,  103 , 2  ,4  , 1  , 3  ,2 , 4}, //
													{ 3  ,1   , 2  , 1  , 255  ,0  , 0  , 0 , 0 , 0 },	
	{ 90 , 0  , 0  , 0  , 0  ,0  , 110, 10, 0 , 0 },  // Jump

	{ 250, 11 , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Text
	{ 253, 0  , 0  , 0  , 0  ,0  , 0  , 0 , 0 , 0 }, // Palette normal
	{ 255,0  , 0  , 0  , 0  , 0  , 0  , 0 , 0 , 0 }, // END
};

// -------------------------------------------------------
u8 nextlevelname[] = "LEVEL92 BIN"; // Must be 11 characters

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
  "Je m'approche de la morue, elle "
 "me demande si je la trouve belle. "
"Je lui r�ponds que j'voudrais m�me  "
 "pas de sa gueule pour me torcher  "
  "le cul ! Quoi pas classe ????   "


};
u8* messageintro_compress;


#define MESSAGEMAXLENGHT (32+34+36+34+32+4+1)

#define NBMESSDEATH 10
u8 messagedeath[NBMESSDEATH][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 101
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2NON\1. Je n'ai pas �t� assez      "
 "rapide pour �viter sa main droite."
"Elle me choppe et sa langue vient   "
 "rentrer dans ma poitrine pour me  "
  "sucer le sang...                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 102
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2AIE\1. J'ai pas �t� assez rapide  "
 "et sa queue me frappe violemment. "
"Je suis projet� quelques m�tres en  "
 "arri�re. Elle me suit pour venir  "
  "m'achever.                      "
},
{
/*
"------------------------------------" 36 chars (Line 3) 103
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2NON\1. Je n'ai pas �t� assez      "
 "rapide pour �viter sa main gauche."
"Elle me choppe et sa langue vient   "
 "rentrer dans ma poitrine pour me  "
  "sucer le sang...                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 104
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "J'ai � peine boug� qu'elle me   "
 "chope avec sa langue et me \2SUCE\1   "
"d'un coup la moiti� de mon sang.    "
 "J'aime bien me faire sucer d'habi-"
  "-tude mais l� c'est rude !      "
},
{
/*
"------------------------------------" 36 chars (Line 3) 105
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je tente un truc mais elle me   "
 "chope direct avec sa main droite. "
"Elle me \2BROIE\1 la nuque, et me jette "
 "� l'eau. Je coule et je me fais   "
  "bouffer par les Kappas...       "
},
{
/*
"------------------------------------" 36 chars (Line 3) 106
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2ET NON\1. Sa queue me frappe tr�s "
 "fort en me projetant au sol. J'ai "
"� peine le temps de reprendre mes   "
 "esprits qu'elle vient m'achever.  "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 107
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2AIE\1. Elle lance une attaque     "
 "puissante pendant que je tentais  "
"un truc. C'�tait plut�t le moment d'"
 "esquiver, l�, souviens-t-en !     "
  "                                "
},
{
/*
"------------------------------------" 36 chars (Line 3) 108
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je tente un truc mais elle me   "
 "choppe direct avec sa main gauche."
"Elle me \2BROIE\1 la nuque, et me jette "
 "� l'eau. Je coule et je me fais   "
  "bouffer par les Kappas...       "
},
{
/*
"------------------------------------" 36 chars (Line 3) 109
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je saute en l'air pour lui faire"
 "un coup de pied dont j'ai le      "
"secret, mais elle me choppe et me   "
 "broie la nuque. Elle jette mon    "
  "\2CADAVRE\1 dans la rivi�re.        "
},
{
/*
"------------------------------------" 36 chars (Line 3) 110
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Je tente une roulade pour me    "
 "rapprocher d'elle. MAIS elle me   "
"marave avec la queue puissante.     "
 "J'ai litt�ralement tout les os en "
  "\2BOUILLIE\1 !                      "
}

};

#define NBMESSSUCESS 11 
u8 messagesucces[NBMESSSUCESS][MESSAGEMAXLENGHT] =
{
{
/*
"------------------------------------" 36 chars (Line 3) 1
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Ca l'a v�n�r grave !! Elle se   "
 "transforme en serpent d�mon !!    "
"J'arrive plus tr�s bien � voir les  "
 "zones des frappes, c'est � toi de "
  "te d�brouiller la !             "
},
{
/*
"------------------------------------" 36 chars (Line 3) 2
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. Qui dit serpent dit       "
 "langue et la frapper direct dans  "
"la t�te est une excellente id�e. Ca "
 "la surprend et me laisse quelques "
  "secondes pour r�fl�chir.        "
},
{
/*
"------------------------------------" 36 chars (Line 3) 3
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. Bien vu, elle va tenter de "
 "me chopper avec sa main droite.   "
"                                    "
 "       \2FAIS LA SEQUENCE\1           "
  " pour que je puisse y �chapper. "
},
{
/*
"------------------------------------" 36 chars (Line 3) 4
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. J'esquive sa main.       "
 "Ca l'�nerve !!! Tu savais que les "
"d�mons bouffaient d'abord ton cul   "
 "pour atteindre tes intestins qui  "
  "sont ta conscience ?            "
},
{
/*
"------------------------------------" 36 chars (Line 3) 5
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. Elle va s�rement me       "
 "frapper avec sa queue.            "
"                                    "
 "       \2FAIS LA SEQUENCE\1           "
  " pour que je puisse y �chapper. "
},
{
/*
"------------------------------------" 36 chars (Line 3) 6
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. Je fais un petit saut et   "
 "j'�vite sa queue puissante.       "
"                                    "
 "D'habitude c'est moi qu'on        "
  "qualifie de queue puissante.... "
},
{
/*
"------------------------------------" 36 chars (Line 3) 7
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2TRES BIEN\1. J'�vite de justesse  "
 "son attaque.                      "
"On m'avait dit qu'elle pouvait      "
 "mesurer jusqu'� 300m, comme quoi  "
  "m�me les d�mons se vantent !!!!!"
},
{
/*
"------------------------------------" 36 chars (Line 3) 8
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2OUI\1. Bien vu, elle va tenter de "
 "me chopper avec sa main gauche.   "
"                                    "
 "       \2FAIS LA SEQUENCE\1           "
  " pour que je puisse y �chapper. "
},
{
/*
"------------------------------------" 36 chars (Line 3) 8
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BRAVO\1. J'�vite sa main gauche   "
 "qui tente de m'attraper. Elle est "
"de plus en plus en col�re !!        "
 "Vas-y, continue, on va l'user un  "
  "peu avant de lui rentrer dedans."
},
{
/*
"------------------------------------" 36 chars (Line 3) 8
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "\2BIEN\1. Un dernier saut pour      "
 "�viter une nouvelle attaque de sa "
"queue. Jusque-l� tout va bien. On va"
 "passer � la vitesse sup�rieure    "
  "vu qu'elle est bien furax !     "
},
{
/*
"------------------------------------" 36 chars (Line 3) 8
/"----------------------------------" 34 chars (Line 2 & 4)
//"--------------------------------" 32 chars (Line 1 & 5)
*/
  "Fini de rigoler maintenant. Je  "
 "l'ai d�stabilis�e. Je sors mes    "
"armes et je lui rentre dans le lard "
 "� la grosse truie !!!!            "
  "M�me de dos elle moche !!!      " 
}

};

// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
// Tile tile_Pic02_00: 16x16 pixels, 8x16 bytes.
const u8 tile_Pic01_00[8 * 16 * 24] = {
	0x8a, 0x0f, 0x0f, 0x0f, 0x4b, 0xc3, 0x6b, 0x86,
	0x00, 0x0a, 0x4b, 0x0f, 0x4b, 0x97, 0xc3, 0x49,
	0x25, 0x0d, 0x0d, 0x0f, 0xc3, 0x6b, 0x6b, 0x0c,
	0x49, 0x0d, 0x4b, 0x0f, 0x97, 0x97, 0xc3, 0x49,
	0x86, 0x0e, 0x87, 0x4b, 0x3f, 0x6b, 0x86, 0xc3,
	0x49, 0xc3, 0x49, 0x86, 0x97, 0x97, 0xc3, 0x49,
	0x86, 0xc3, 0x49, 0x0c, 0x49, 0x6b, 0x86, 0xc3,
	0x49, 0x86, 0x0c, 0x0c, 0x97, 0xc3, 0x49, 0x49,
	0xc3, 0x49, 0x0c, 0x49, 0x6b, 0x6b, 0xc3, 0xc3,
	0x49, 0x86, 0x0c, 0x97, 0xc3, 0x97, 0xc3, 0xc3,
	0xc3, 0x0c, 0x49, 0x49, 0x6b, 0xc3, 0x6b, 0xf0,
	0xc3, 0x86, 0x97, 0x97, 0x86, 0x97, 0xd2, 0xa2,
	0xc3, 0x49, 0xc3, 0x6b, 0x49, 0xc3, 0xa2, 0x00,
	0xc3, 0x86, 0x97, 0x86, 0x49, 0xd2, 0x00, 0xf2,
	0xc3, 0x49, 0x6b, 0x0c, 0xc3, 0x71, 0x51, 0xf4,
	0xc3, 0x86, 0x97, 0x86, 0xc3, 0xa0, 0xf2, 0xf9,
	0x86, 0xc3, 0xc3, 0x0f, 0x86, 0xc3, 0xc3, 0x87,
	0x49, 0x6b, 0xc3, 0x0f, 0x49, 0x49, 0xc3, 0x0d,
	0x97, 0xc3, 0xc3, 0x0d, 0x86, 0x86, 0x86, 0x0f,
	0x6b, 0x49, 0x49, 0x49, 0x49, 0x0f, 0x0d, 0x0e,
	0x97, 0x86, 0xc3, 0x86, 0x87, 0x05, 0x86, 0x0e,
	0x6b, 0x0c, 0x49, 0x0c, 0x0f, 0x05, 0x0d, 0x0e,
	0x3f, 0x86, 0x86, 0x86, 0x87, 0x00, 0x0e, 0x0f,
	0x6b, 0x0d, 0x49, 0x49, 0x0d, 0x00, 0x0e, 0x0d,
	0x6b, 0x0c, 0x0e, 0x86, 0x0d, 0x00, 0x0e, 0x0f,
	0xc3, 0x0d, 0x49, 0x49, 0x0f, 0x00, 0x05, 0x05,
	0xa4, 0x0c, 0x0c, 0x0d, 0x0f, 0x0a, 0x00, 0x00,
	0xf2, 0x49, 0x49, 0x08, 0x0f, 0x0a, 0x00, 0x00,
	0x51, 0x24, 0xc3, 0x08, 0x05, 0x0f, 0x00, 0x05,
	0xa2, 0xe1, 0xc3, 0x0d, 0x05, 0x0d, 0x0a, 0x0f,
	0xa0, 0xb2, 0xc3, 0x87, 0x05, 0x0d, 0x0f, 0x0d,
	0x50, 0x50, 0xc3, 0x0d, 0x0f, 0x0f, 0x0f, 0x0f,
	0x0e, 0x86, 0x0c, 0x0f, 0x00, 0x0f, 0xc3, 0xc3,
	0x4b, 0x0c, 0x49, 0x0d, 0x00, 0x4b, 0xc3, 0xc3,
	0xc3, 0x86, 0x0c, 0x0d, 0x0f, 0x0c, 0xc3, 0xc3,
	0xc3, 0x0c, 0x49, 0x0d, 0x0f, 0x49, 0xc3, 0xc3,
	0xc3, 0x86, 0xc3, 0x87, 0x0f, 0xc3, 0xc3, 0xc3,
	0x49, 0x0c, 0x49, 0xc3, 0x49, 0x97, 0xc3, 0x97,
	0xc3, 0x86, 0x4b, 0x86, 0xc3, 0x6b, 0xc3, 0x6b,
	0x49, 0x0d, 0x49, 0xc3, 0x97, 0xc3, 0xc3, 0x97,
	0xc3, 0x86, 0x0e, 0xc3, 0x6b, 0x6b, 0xc3, 0x3f,
	0x49, 0x0c, 0x0d, 0x49, 0x3f, 0xc3, 0xc3, 0x97,
	0x0c, 0x86, 0x0e, 0x86, 0x6b, 0xc3, 0x6b, 0x97,
	0x4b, 0x49, 0x0d, 0x49, 0x3f, 0x97, 0x97, 0x97,
	0x0e, 0x86, 0x0f, 0x49, 0x6b, 0xc3, 0x3f, 0x3f,
	0x4b, 0x49, 0x0d, 0x4b, 0x3f, 0x97, 0x97, 0x3f,
	0x0e, 0x86, 0x0f, 0x49, 0x3f, 0xc3, 0x49, 0x3f,
	0x4b, 0x49, 0x0f, 0x4b, 0x3f, 0x86, 0xc3, 0x97,
	0x49, 0xc3, 0x6b, 0x0c, 0x92, 0xa2, 0xf6, 0x7c,
	0xc3, 0x97, 0xc3, 0x49, 0xd2, 0x51, 0xf4, 0xb4,
	0x49, 0x3f, 0x2e, 0x86, 0xd2, 0x50, 0xf9, 0x50,
	0xc3, 0x97, 0x86, 0x4b, 0x71, 0x51, 0xf8, 0xa2,
	0x49, 0x6b, 0x0d, 0x86, 0xa0, 0xf2, 0xfc, 0xf1,
	0x0c, 0xc3, 0x0c, 0x49, 0xa0, 0xf3, 0xa0, 0xf6,
	0x0c, 0x86, 0x0d, 0x92, 0xa2, 0xf2, 0xf9, 0x51,
	0x0e, 0x49, 0x0e, 0xc3, 0xa2, 0xf1, 0xa0, 0xa2,
	0x0f, 0x0c, 0x0c, 0x92, 0x51, 0xf2, 0xf9, 0x51,
	0x0e, 0x0e, 0x0e, 0x92, 0x50, 0xe7, 0xb4, 0xf1,
	0x0f, 0x0d, 0x0e, 0xd2, 0x50, 0x11, 0xf8, 0x72,
	0x0f, 0x0f, 0x49, 0x92, 0x54, 0x55, 0x67, 0xb4,
	0x0f, 0x0e, 0x0e, 0xd2, 0x50, 0x11, 0xbb, 0xda,
	0x0f, 0x0f, 0x49, 0x58, 0x54, 0x55, 0x66, 0xcd,
	0x0f, 0x0e, 0x86, 0xd2, 0x54, 0x00, 0xbb, 0x68,
	0x0f, 0x0d, 0x0c, 0x58, 0x54, 0x40, 0x50, 0xda,
	0xf6, 0x51, 0xc3, 0x86, 0x0f, 0x0d, 0x0f, 0x0d,
	0xa8, 0xa0, 0x61, 0xc3, 0x0f, 0x0d, 0x0f, 0x0d,
	0xf9, 0x51, 0xe1, 0x86, 0x0d, 0x86, 0x0e, 0x86,
	0xf4, 0xa2, 0xe3, 0x49, 0x0d, 0x49, 0x0c, 0x0d,
	0x14, 0xf1, 0x10, 0x86, 0x86, 0xc3, 0x86, 0x0c,
	0xa2, 0xf9, 0x50, 0x49, 0xc3, 0xc3, 0xc3, 0x0c,
	0xa8, 0x54, 0x51, 0xa4, 0x86, 0xc3, 0xc3, 0x86,
	0xf1, 0x50, 0xa2, 0xf2, 0xc3, 0x49, 0x49, 0x87,
	0x50, 0xa2, 0xf1, 0x51, 0xe1, 0xc3, 0x86, 0x86,
	0x51, 0xa0, 0xa8, 0x00, 0x51, 0xe1, 0x49, 0x0d,
	0xa2, 0xf9, 0xf1, 0x00, 0x00, 0x05, 0x86, 0x0d,
	0xf1, 0xa0, 0xf9, 0x50, 0xa2, 0x00, 0x0e, 0x0d,
	0xf8, 0xf9, 0xf9, 0xf6, 0xf1, 0x00, 0x50, 0x0d,
	0x7c, 0xf1, 0xf9, 0x55, 0xf8, 0xa2, 0x05, 0xa5,
	0x14, 0xf9, 0xa0, 0xf6, 0xfe, 0xf1, 0x00, 0x0f,
	0x6c, 0xf1, 0xf9, 0x50, 0xfd, 0xf8, 0xa2, 0x05,
	0x0e, 0x86, 0x0f, 0x4b, 0x3f, 0xc3, 0x49, 0xc3,
	0x0e, 0xc3, 0x0d, 0x49, 0x3f, 0x86, 0x0c, 0xc3,
	0x0e, 0xc3, 0x86, 0x4b, 0x6b, 0xc3, 0x0c, 0x49,
	0x0f, 0x49, 0x49, 0x1d, 0x3f, 0x86, 0x0c, 0x49,
	0x0e, 0xc3, 0x86, 0x49, 0x6b, 0xc3, 0x0c, 0x49,
	0x0f, 0x49, 0xc3, 0x1d, 0x3f, 0xc3, 0x86, 0x49,
	0x0f, 0x0e, 0x86, 0xc3, 0x6b, 0xc3, 0x86, 0xc3,
	0x0f, 0x0f, 0x0c, 0x97, 0x3f, 0xc3, 0xc3, 0x49,
	0x0f, 0x05, 0x86, 0x97, 0x6b, 0xc3, 0x86, 0xc3,
	0x0a, 0x0a, 0x0c, 0x97, 0xc3, 0xc3, 0xc3, 0x49,
	0x05, 0x00, 0x0c, 0x97, 0x6b, 0xc3, 0xc3, 0xc3,
	0x0a, 0x00, 0x86, 0x97, 0xc3, 0xc3, 0xc3, 0x49,
	0x05, 0x00, 0xc3, 0x6b, 0x6b, 0x6b, 0x86, 0x86,
	0x0a, 0x00, 0x86, 0x97, 0xc3, 0xc3, 0xc3, 0x4b,
	0x0f, 0x00, 0x49, 0x6b, 0xc3, 0x6b, 0x86, 0x86,
	0x0a, 0x00, 0x49, 0x3f, 0x97, 0x97, 0xc3, 0x4b,
	0x0f, 0x0e, 0x86, 0xd2, 0x50, 0x14, 0xb4, 0x99,
	0x0f, 0x49, 0xc3, 0x58, 0x50, 0x44, 0x6c, 0x66,
	0x0f, 0x0e, 0x86, 0x1a, 0x51, 0x55, 0xb1, 0xdd,
	0x0f, 0x0d, 0x0d, 0x58, 0x00, 0x44, 0x6c, 0x66,
	0x87, 0x0e, 0x0e, 0x1a, 0x00, 0x55, 0x7d, 0xdd,
	0x0f, 0x0f, 0x0f, 0x0f, 0x51, 0x44, 0xbe, 0xee,
	0x87, 0x0f, 0x05, 0x1a, 0x50, 0x54, 0xdd, 0xdd,
	0x49, 0x0a, 0x05, 0x05, 0x54, 0x50, 0xee, 0xee,
	0x87, 0x0a, 0x05, 0x00, 0x5f, 0x51, 0xe8, 0xd5,
	0x0d, 0x00, 0x00, 0x00, 0x54, 0x00, 0x00, 0xe4,
	0x87, 0x05, 0x00, 0x00, 0x5f, 0x51, 0xdd, 0xdd,
	0x0d, 0x00, 0x00, 0x00, 0x5e, 0x51, 0xec, 0xfe,
	0x0f, 0x05, 0x00, 0x00, 0x5f, 0xa8, 0xb4, 0xf8,
	0x0a, 0x0a, 0x00, 0x00, 0x5e, 0xaa, 0x50, 0xf1,
	0x0a, 0x05, 0x00, 0x05, 0x4f, 0xa8, 0xa2, 0x00,
	0x0a, 0x0a, 0x0a, 0x05, 0x5a, 0xa8, 0x00, 0x45,
	0xdc, 0xa2, 0xa0, 0xf6, 0xee, 0xfe, 0xf1, 0x05,
	0x6c, 0xf1, 0x00, 0xf2, 0xfd, 0xdc, 0xf1, 0xa2,
	0xdc, 0xa2, 0xa0, 0xf4, 0xee, 0xf8, 0xa2, 0xa2,
	0x6c, 0x50, 0x00, 0xf7, 0xf8, 0xa2, 0x00, 0x51,
	0xdc, 0xa2, 0xa0, 0xf4, 0xfb, 0xf4, 0xa0, 0x00,
	0xee, 0x50, 0x00, 0xf5, 0xa8, 0xfd, 0xf8, 0x51,
	0x9c, 0x50, 0xa0, 0xe4, 0xa8, 0xfe, 0xba, 0x0a,
	0x7c, 0x50, 0xa8, 0xf5, 0xa0, 0xfd, 0xfd, 0xa0,
	0xdc, 0x50, 0xa0, 0xe4, 0xa8, 0xee, 0xfe, 0xaf,
	0x78, 0x50, 0xa8, 0xf5, 0xa0, 0xfd, 0xdd, 0xd8,
	0xf9, 0x50, 0x28, 0xe6, 0xa0, 0xce, 0xee, 0xba,
	0xf1, 0x05, 0xa8, 0xf5, 0x0a, 0xf5, 0xdd, 0xdd,
	0xa2, 0x8a, 0x28, 0xe6, 0xa0, 0x4f, 0xee, 0xcc,
	0x45, 0x88, 0x28, 0x54, 0x0f, 0x50, 0xff, 0xdd,
	0xce, 0xaf, 0xa8, 0xe6, 0x5a, 0x20, 0xf5, 0xcc,
	0xdd, 0x8d, 0xa0, 0x54, 0x10, 0xaf, 0x54, 0xee,
	0x0f, 0x00, 0x4b, 0x6b, 0xc3, 0x6b, 0x86, 0x0c,
	0x0a, 0x00, 0x49, 0xc3, 0x4b, 0x97, 0xc3, 0x4b,
	0x0f, 0x00, 0x4b, 0x6b, 0x05, 0xc3, 0x87, 0xc3,
	0x05, 0x0a, 0x4b, 0xc3, 0x05, 0x97, 0xc3, 0x4b,
	0x05, 0x0a, 0x41, 0x6b, 0x0f, 0xc3, 0x87, 0x86,
	0x05, 0x08, 0x4b, 0x86, 0x05, 0x4b, 0x87, 0x4b,
	0xa7, 0x0a, 0x41, 0x6b, 0x0f, 0x4b, 0x87, 0x86,
	0x05, 0x0d, 0x41, 0x86, 0x05, 0x4b, 0x87, 0x4b,
	0x00, 0x0e, 0x41, 0x6b, 0x00, 0x0f, 0x87, 0x86,
	0x00, 0x0d, 0x41, 0x86, 0x05, 0x05, 0x87, 0x4b,
	0x0a, 0x0e, 0x4b, 0x2e, 0x0a, 0x0f, 0x87, 0x87,
	0x0a, 0x04, 0x49, 0x86, 0x0a, 0x05, 0xc3, 0x0d,
	0xa0, 0x04, 0x49, 0x86, 0x82, 0x05, 0x86, 0x87,
	0x8a, 0x05, 0x49, 0xc3, 0x0d, 0x05, 0xc3, 0x0d,
	0xaf, 0x05, 0x0e, 0x86, 0x87, 0x05, 0x86, 0x87,
	0xaa, 0xa2, 0x49, 0xc3, 0x0d, 0x05, 0xc3, 0x0d,
	0x0a, 0x0f, 0x00, 0x05, 0xa0, 0x8a, 0xa2, 0xf5,
	0x0f, 0x05, 0x0a, 0x05, 0xa0, 0xa0, 0x51, 0xf4,
	0x0a, 0x05, 0x0f, 0x05, 0xda, 0x00, 0xa2, 0xf5,
	0x0f, 0x00, 0x0a, 0x05, 0xda, 0x00, 0x51, 0x54,
	0x0a, 0x00, 0x0a, 0x05, 0xcf, 0x00, 0x00, 0xe7,
	0x0f, 0x00, 0x0a, 0x41, 0xcf, 0xa0, 0x5b, 0x00,
	0x0a, 0x00, 0x0a, 0x4b, 0xcf, 0xa0, 0x20, 0xa2,
	0x0d, 0x00, 0x05, 0x49, 0xe5, 0x8a, 0x08, 0x51,
	0x0f, 0x0a, 0x05, 0xc3, 0xe5, 0x8a, 0x08, 0xa2,
	0x0d, 0x05, 0x41, 0xc3, 0xe5, 0xad, 0x0d, 0x51,
	0x0e, 0x0a, 0x0e, 0xc3, 0xf4, 0xa8, 0x0d, 0xa2,
	0x0d, 0x0d, 0x4b, 0x0d, 0xe5, 0xad, 0x18, 0x51,
	0x0c, 0x0e, 0x4b, 0x0a, 0xf4, 0x8f, 0x18, 0x00,
	0x49, 0x0c, 0x49, 0x00, 0xf4, 0x8f, 0x0c, 0x51,
	0x86, 0x87, 0x87, 0x00, 0xf5, 0xad, 0x49, 0x0a,
	0x0c, 0x1d, 0x87, 0x00, 0xf4, 0xad, 0x0c, 0x0a,
	0xec, 0xaf, 0xa8, 0xf6, 0x5f, 0x20, 0x5a, 0xee,
	0xdd, 0x88, 0xa0, 0x45, 0x10, 0x10, 0xa5, 0x64,
	0xec, 0xaf, 0x00, 0xe7, 0x5a, 0x5f, 0xa8, 0x31,
	0xdd, 0x8a, 0xa0, 0x50, 0x05, 0x30, 0xaf, 0xce,
	0xef, 0xa0, 0x00, 0x05, 0x10, 0x75, 0x70, 0x9b,
	0xa5, 0x00, 0xa0, 0x00, 0x1a, 0x98, 0x5e, 0x64,
	0x00, 0x00, 0x00, 0x0a, 0xf5, 0x64, 0xf5, 0x31,
	0x50, 0x00, 0x0a, 0xa0, 0xb0, 0x8d, 0xfd, 0x64,
	0x10, 0xa0, 0xaa, 0x20, 0x5f, 0x70, 0xba, 0xbb,
	0x04, 0x70, 0x20, 0x25, 0x10, 0x10, 0xdd, 0x64,
	0x04, 0x18, 0xa0, 0x25, 0x00, 0x64, 0x98, 0xbb,
	0x04, 0x86, 0x0a, 0x70, 0x1a, 0xcc, 0x98, 0xee,
	0x04, 0x49, 0xd2, 0xb0, 0xf5, 0x64, 0x70, 0x99,
	0x04, 0x86, 0x92, 0x1a, 0xb0, 0xba, 0x10, 0xee,
	0x0e, 0x49, 0x6b, 0x25, 0x50, 0x20, 0xb0, 0x99,
	0x04, 0xc3, 0x97, 0xc7, 0x00, 0x5a, 0x30, 0x66,
	0x8d, 0x00, 0x86, 0x87, 0x87, 0x00, 0x86, 0x87,
	0xfa, 0xa2, 0x49, 0x0d, 0x0f, 0x05, 0xc3, 0x0d,
	0xd8, 0xa2, 0x0c, 0x87, 0x0f, 0x00, 0xc3, 0x0d,
	0x67, 0xa2, 0x0e, 0x0d, 0x0f, 0x05, 0xc3, 0x0d,
	0xcd, 0xf3, 0x05, 0x0d, 0x0f, 0x00, 0x86, 0x0f,
	0x67, 0xf2, 0x00, 0x0f, 0x4b, 0x05, 0xc3, 0x0d,
	0xcd, 0xf6, 0xa2, 0x00, 0x74, 0x00, 0x86, 0x0f,
	0xfa, 0xf6, 0xf1, 0x00, 0x00, 0x05, 0xc3, 0x0d,
	0xfa, 0xf6, 0x00, 0xa2, 0x00, 0x0f, 0x86, 0x0f,
	0x25, 0xf2, 0x00, 0x10, 0x4b, 0x4b, 0xc3, 0x0f,
	0x25, 0xf2, 0xa2, 0x05, 0x0f, 0x4b, 0x87, 0x0a,
	0x8a, 0xf3, 0xa8, 0x05, 0x4b, 0x4b, 0x86, 0x0a,
	0x8a, 0x51, 0xf1, 0x05, 0x0e, 0xc3, 0x87, 0x0a,
	0xa0, 0x00, 0xf2, 0x00, 0x4b, 0x97, 0x87, 0x00,
	0xa5, 0x51, 0x51, 0xa2, 0x10, 0xc3, 0x87, 0x00,
	0x5a, 0x0a, 0xa2, 0xf3, 0x00, 0x1d, 0x0d, 0x00,
	0xc3, 0x6b, 0x0a, 0x00, 0xf5, 0xad, 0x49, 0x20,
	0x49, 0x86, 0x00, 0x0a, 0xde, 0xaf, 0xc3, 0x20,
	0xc3, 0x86, 0x00, 0x00, 0xf5, 0xad, 0x49, 0x08,
	0xc3, 0x0a, 0x00, 0x0a, 0xde, 0xaf, 0x97, 0x08,
	0x87, 0x00, 0x00, 0x00, 0xf5, 0xf8, 0xc3, 0x82,
	0x87, 0x00, 0x00, 0x0a, 0xdf, 0xaf, 0x97, 0x87,
	0x87, 0x00, 0x00, 0x0a, 0xfd, 0xda, 0x6b, 0x2f,
	0x87, 0x00, 0x00, 0x0a, 0xee, 0xda, 0x3f, 0x87,
	0x87, 0x00, 0x05, 0xa0, 0xfd, 0xa4, 0x6b, 0x2e,
	0x87, 0x00, 0x4b, 0x0a, 0xee, 0xa4, 0x97, 0x86,
	0x87, 0x00, 0x0e, 0xa0, 0xfd, 0x41, 0x3f, 0x2e,
	0xc3, 0x00, 0x0e, 0xa0, 0xee, 0xa4, 0x3f, 0x86,
	0x86, 0x05, 0x0f, 0x05, 0xdd, 0xe1, 0x3f, 0x6b,
	0xc3, 0x0f, 0x05, 0x05, 0xfe, 0xcb, 0x3f, 0x86,
	0x86, 0x0f, 0x05, 0x50, 0xd8, 0xcb, 0x3f, 0x6b,
	0x49, 0x0d, 0x05, 0x50, 0xaf, 0x1d, 0x3f, 0xc3,
	0x4b, 0x49, 0x3f, 0x6b, 0x1a, 0x98, 0x98, 0xdd,
	0x04, 0x97, 0x97, 0x97, 0x5a, 0x64, 0xdd, 0x77,
	0x4b, 0xc3, 0x3f, 0x6b, 0xa0, 0x98, 0x98, 0x98,
	0x0c, 0x97, 0x97, 0x3f, 0xa0, 0xe4, 0x75, 0x67,
	0x49, 0xc3, 0x3f, 0x6b, 0x87, 0x05, 0xf5, 0xd8,
	0x0c, 0x97, 0x3f, 0x97, 0x87, 0xe4, 0x64, 0xaf,
	0x49, 0x6b, 0x6b, 0xc3, 0x87, 0xcc, 0x75, 0xda,
	0xc3, 0x97, 0x97, 0x97, 0x82, 0xe4, 0x64, 0xe5,
	0x49, 0x6b, 0x49, 0xc3, 0xa0, 0x1a, 0xdd, 0x4e,
	0xc3, 0xc3, 0x0e, 0x97, 0x82, 0x50, 0xef, 0x10,
	0x49, 0x86, 0x0e, 0xc3, 0xa0, 0x1a, 0xd8, 0x44,
	0xc3, 0x0c, 0x0f, 0x49, 0x0a, 0x64, 0xaf, 0x10,
	0x86, 0x87, 0x0f, 0x87, 0x05, 0xcc, 0xaa, 0x45,
	0x86, 0x0d, 0x05, 0x08, 0x5a, 0x99, 0xa8, 0x50,
	0x87, 0x0d, 0x05, 0x87, 0xe4, 0x66, 0xda, 0x05,
	0x0d, 0x86, 0x05, 0x1a, 0x99, 0xcd, 0xe5, 0x05,
	0x4f, 0x05, 0x51, 0x00, 0x3f, 0x6b, 0x87, 0x00,
	0x10, 0x0a, 0x00, 0x10, 0x97, 0x6b, 0x0a, 0x05,
	0x4e, 0x50, 0x51, 0x10, 0x3f, 0x6b, 0x0a, 0x05,
	0xb0, 0x4f, 0x00, 0x4b, 0x97, 0xc3, 0x0a, 0x0f,
	0xce, 0xe4, 0x00, 0xe1, 0xc3, 0x87, 0x0f, 0x0f,
	0x98, 0xdf, 0x00, 0x61, 0x30, 0xc3, 0x0f, 0x0e,
	0x64, 0x64, 0x00, 0x70, 0x0a, 0x0e, 0x0f, 0x0e,
	0xdd, 0xdd, 0x00, 0xa5, 0x00, 0x0e, 0x0f, 0x4b,
	0xcc, 0xba, 0x00, 0x5a, 0xe5, 0x05, 0x0f, 0x0e,
	0xdd, 0xdd, 0x05, 0xbb, 0x88, 0x05, 0x0f, 0x4b,
	0xcc, 0xba, 0x45, 0xcc, 0x00, 0x00, 0x00, 0x0e,
	0xdd, 0x65, 0x55, 0x66, 0xdf, 0x00, 0x00, 0x4b,
	0xee, 0xfa, 0x1b, 0xee, 0xaa, 0x45, 0x00, 0x0f,
	0xdd, 0x25, 0xce, 0xcf, 0x00, 0x00, 0x00, 0x0f,
	0xba, 0xaa, 0xee, 0xa0, 0xf5, 0x77, 0xa0, 0x87,
	0x75, 0x8a, 0x77, 0x45, 0xbb, 0x99, 0xaa, 0x0f,
	0x0c, 0x87, 0x05, 0x54, 0xd8, 0x49, 0x6b, 0x2e,
	0x0c, 0x0d, 0x0f, 0x54, 0xaf, 0x97, 0x97, 0xc3,
	0x86, 0x86, 0x0f, 0x55, 0x8d, 0x49, 0x6b, 0x2e,
	0x0c, 0x0d, 0x0f, 0x44, 0xaf, 0x97, 0xc3, 0x6b,
	0x86, 0x86, 0x0f, 0x55, 0x88, 0xc3, 0x6b, 0x6b,
	0x0c, 0x0d, 0x0f, 0x44, 0x8d, 0xc3, 0x97, 0xc3,
	0x86, 0x86, 0x0f, 0x44, 0x88, 0x6b, 0x6b, 0xc3,
	0x49, 0x0c, 0x0d, 0x44, 0x88, 0xc3, 0x97, 0x86,
	0x86, 0x0c, 0x0e, 0x44, 0x88, 0x6b, 0x6b, 0x58,
	0x49, 0x49, 0x0d, 0x44, 0xaa, 0xc3, 0x86, 0x87,
	0x86, 0x86, 0x0c, 0x55, 0x88, 0x6b, 0x6b, 0x0d,
	0x6b, 0x49, 0x0d, 0x44, 0xfa, 0xc3, 0x86, 0xa5,
	0xc3, 0x86, 0x86, 0x55, 0xa8, 0xc3, 0x6b, 0x0b,
	0x6b, 0x49, 0x49, 0x44, 0xfa, 0x82, 0x86, 0x01,
	0x97, 0x86, 0x86, 0x55, 0xf8, 0xd2, 0xa4, 0x11,
	0x6b, 0x49, 0x49, 0x55, 0xfe, 0xf4, 0xf0, 0x11,
	0x0f, 0x0d, 0x05, 0x64, 0x33, 0xda, 0x00, 0x00,
	0x0f, 0x0c, 0x0a, 0x99, 0x33, 0xb9, 0x8d, 0x00,
	0x0f, 0x0c, 0x05, 0x64, 0x66, 0x66, 0xdd, 0x5a,
	0x0f, 0x0f, 0x00, 0x1a, 0x99, 0x99, 0xd8, 0x76,
	0x0d, 0x0a, 0x64, 0x8d, 0x64, 0x66, 0xcc, 0xd0,
	0x0d, 0x10, 0x99, 0xcc, 0x4e, 0x99, 0x98, 0xa8,
	0xa5, 0x64, 0x62, 0x66, 0x98, 0x66, 0x64, 0x8a,
	0x1a, 0x99, 0xc0, 0x91, 0x98, 0x98, 0x98, 0x0a,
	0x1a, 0xc8, 0xc0, 0x66, 0x66, 0xce, 0x65, 0x00,
	0x50, 0x39, 0x91, 0x91, 0x98, 0xcc, 0x25, 0x00,
	0xa1, 0x36, 0x62, 0x66, 0x32, 0x98, 0x8a, 0x0a,
	0x13, 0x91, 0x39, 0x99, 0x64, 0x65, 0x05, 0x00,
	0x62, 0x33, 0x36, 0x66, 0x64, 0x8f, 0x00, 0x0a,
	0xc0, 0x91, 0xdd, 0xb0, 0x98, 0x0a, 0x05, 0x00,
	0x62, 0x33, 0x66, 0xb0, 0xcd, 0x00, 0x0a, 0x0a,
	0xc0, 0x91, 0x99, 0xaf, 0x20, 0x05, 0x05, 0x54,
	0x9a, 0xa0, 0xcd, 0xf5, 0x67, 0xe5, 0x22, 0x25,
	0x5a, 0x4e, 0xda, 0xb9, 0xda, 0xcf, 0x8a, 0x05,
	0x8f, 0x45, 0x45, 0xee, 0xa5, 0xe5, 0xa0, 0x05,
	0xa0, 0xcf, 0xf5, 0x67, 0x0a, 0x00, 0x00, 0x05,
	0x45, 0xfa, 0xb9, 0x72, 0x05, 0xe5, 0xa0, 0x05,
	0xf5, 0xd8, 0xbb, 0x8d, 0x50, 0xef, 0xda, 0x05,
	0xce, 0xee, 0x33, 0x8a, 0x1a, 0xdd, 0xcf, 0x05,
	0xdd, 0x99, 0x66, 0xa0, 0xe4, 0xff, 0xcf, 0x05,
	0xee, 0x66, 0x77, 0x5a, 0x75, 0xdd, 0xcf, 0x00,
	0xdd, 0x99, 0x67, 0x10, 0xee, 0xfa, 0x45, 0x00,
	0xee, 0x66, 0xcd, 0x55, 0xff, 0xa0, 0x70, 0x00,
	0xcd, 0x99, 0xd8, 0x50, 0xfa, 0x10, 0xd8, 0x10,
	0xfc, 0x66, 0xfa, 0x00, 0x05, 0x64, 0xaf, 0x10,
	0xb9, 0x99, 0xaf, 0x00, 0x75, 0xdd, 0xd8, 0x10,
	0xce, 0x66, 0x8f, 0x05, 0xee, 0xee, 0xaf, 0x41,
	0x9b, 0x99, 0x8a, 0x45, 0x99, 0x99, 0x88, 0x41,
	0x97, 0xc3, 0x87, 0x55, 0xfd, 0xfc, 0xa5, 0x40,
	0x6b, 0x6b, 0x0d, 0x54, 0xfe, 0xf8, 0xa5, 0x11,
	0x3f, 0x86, 0x0d, 0x50, 0xfc, 0xa0, 0xa5, 0xa1,
	0x6b, 0xc3, 0x0d, 0x0a, 0xf8, 0x50, 0xd2, 0x27,
	0x97, 0x86, 0x0a, 0x5a, 0x00, 0xe1, 0xb0, 0x33,
	0x6b, 0x0c, 0x0a, 0x1f, 0x6b, 0xc3, 0xe4, 0x91,
	0x86, 0x86, 0x00, 0x35, 0x6b, 0xc3, 0xe4, 0x62,
	0x49, 0x0c, 0x00, 0x1d, 0xc3, 0xc3, 0x4e, 0x91,
	0x86, 0x87, 0x0a, 0x97, 0x86, 0xc3, 0xe4, 0x62,
	0x49, 0x0d, 0x00, 0x97, 0xc3, 0xc3, 0x0a, 0x99,
	0x86, 0x0a, 0x05, 0x6b, 0x86, 0xc3, 0x5a, 0xee,
	0x0f, 0x00, 0x41, 0x3f, 0xc3, 0xc3, 0x5f, 0x55,
	0x0a, 0x00, 0x41, 0x6b, 0x86, 0xc3, 0x4e, 0xaf,
	0x00, 0x00, 0x15, 0x3f, 0xc3, 0xc3, 0x4e, 0x77,
	0x00, 0x00, 0x97, 0x6b, 0x86, 0xc3, 0x4e, 0x91,
	0x00, 0x05, 0x97, 0x97, 0xc3, 0x49, 0x4e, 0x62,
	0x62, 0x66, 0x66, 0x20, 0x8a, 0x0f, 0x0e, 0x45,
	0x91, 0x91, 0xcc, 0x98, 0x05, 0x0f, 0x0d, 0x50,
	0x62, 0x66, 0x6c, 0x30, 0x0f, 0x0e, 0x0e, 0x45,
	0xbb, 0x9c, 0xba, 0x65, 0x0f, 0x0f, 0x0d, 0x50,
	0x1e, 0x7d, 0xcc, 0xa4, 0x0e, 0x0e, 0x0d, 0xf5,
	0xd5, 0xbb, 0x98, 0x04, 0x0d, 0x0f, 0x08, 0xf0,
	0xc0, 0x66, 0x65, 0x0e, 0x0e, 0x0e, 0x0a, 0xf5,
	0x91, 0x91, 0xd8, 0x0e, 0x0d, 0x0f, 0x0a, 0xef,
	0xc0, 0x66, 0x25, 0x0e, 0x0e, 0x0e, 0x0a, 0xdf,
	0x91, 0xcc, 0xa0, 0x0e, 0x0d, 0x0f, 0x0a, 0xef,
	0x66, 0x98, 0xa5, 0x0e, 0x86, 0x0d, 0x50, 0xda,
	0xcc, 0xfa, 0x20, 0x0e, 0x49, 0x0d, 0x54, 0xfe,
	0xf5, 0x98, 0x88, 0x0e, 0x86, 0x0d, 0x55, 0xed,
	0xee, 0x70, 0x25, 0x04, 0x49, 0x0d, 0x54, 0xaa,
	0x33, 0x98, 0x88, 0x0e, 0x0c, 0x08, 0xf5, 0xa8,
	0x66, 0x70, 0x25, 0x04, 0x0c, 0x0a, 0xf4, 0x8f,
	0x44, 0x66, 0xa0, 0x54, 0xee, 0x66, 0x25, 0x41,
	0x9b, 0xdd, 0xa0, 0x50, 0xdd, 0x98, 0xa0, 0x41,
	0xee, 0x77, 0x0a, 0x00, 0xf0, 0xa0, 0x25, 0x41,
	0xee, 0xcd, 0x0a, 0x05, 0xee, 0xee, 0xfa, 0x41,
	0xce, 0xef, 0x00, 0x05, 0xdd, 0xdd, 0xd8, 0x41,
	0xee, 0xfa, 0x00, 0x05, 0xee, 0x66, 0xfa, 0x41,
	0xce, 0x8f, 0x00, 0x0f, 0x99, 0xc4, 0xd8, 0x0f,
	0xe4, 0x8f, 0x00, 0x05, 0xee, 0x66, 0xaa, 0x0e,
	0x44, 0x8f, 0x00, 0x5f, 0x5f, 0xdd, 0x0a, 0x0f,
	0x44, 0xfa, 0x00, 0x4e, 0xaf, 0x00, 0x00, 0x0e,
	0x4e, 0xfa, 0x00, 0xf5, 0x77, 0x5f, 0x0a, 0x0f,
	0xe4, 0xef, 0x00, 0x4e, 0x91, 0xcc, 0xa0, 0x4b,
	0x31, 0xcd, 0x05, 0xf5, 0x91, 0xdd, 0xa0, 0x0e,
	0xee, 0x77, 0x05, 0xf5, 0x33, 0xcc, 0xa0, 0x49,
	0x99, 0xdd, 0x50, 0xfa, 0x99, 0xdd, 0x0a, 0x86,
	0xcc, 0x77, 0x50, 0xd8, 0xee, 0xfa, 0x05, 0xc3,
	0x00, 0x0f, 0x6b, 0xc3, 0x86, 0xc3, 0x4e, 0x91,
	0x00, 0x4b, 0x6b, 0x97, 0xc3, 0x4b, 0xf0, 0x99,
	0x05, 0x97, 0x6b, 0xc3, 0x86, 0x49, 0xa5, 0xe4,
	0x0e, 0x97, 0xc3, 0x97, 0xc3, 0x4b, 0xa0, 0x5a,
	0x0e, 0x3f, 0xc3, 0xc3, 0xc3, 0x49, 0xa0, 0xa5,
	0x49, 0x97, 0xc3, 0xc3, 0xc3, 0xd2, 0x00, 0x64,
	0x86, 0xc3, 0xc3, 0xa5, 0x00, 0x00, 0x00, 0x64,
	0x4b, 0xc3, 0xa5, 0x00, 0x51, 0xf3, 0xa2, 0xb0,
	0x0e, 0xd2, 0x00, 0x00, 0xf2, 0xf2, 0xa2, 0x5a,
	0x5a, 0x0a, 0x00, 0xf2, 0xf4, 0xf1, 0xf3, 0x00,
	0x0f, 0x00, 0x51, 0xbc, 0xf8, 0xf2, 0xa2, 0x05,
	0x00, 0x51, 0xf4, 0xf1, 0xf0, 0xf1, 0x51, 0x00,
	0x00, 0xf2, 0xf2, 0xf3, 0xf2, 0xa2, 0xa2, 0xa2,
	0x51, 0xf3, 0xf1, 0x51, 0xf3, 0x51, 0x51, 0xf3,
	0x00, 0xf3, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2, 0xa2,
	0x51, 0xf3, 0x51, 0x00, 0x51, 0x51, 0x00, 0x00,
	0x99, 0x98, 0xd8, 0x04, 0x0c, 0x05, 0xcf, 0x50,
	0x66, 0x30, 0xcd, 0x50, 0x0d, 0x45, 0xda, 0x45,
	0x98, 0x98, 0xdd, 0x05, 0xa0, 0xda, 0x0a, 0xf5,
	0x64, 0x30, 0x66, 0xa0, 0x45, 0xa5, 0x50, 0xce,
	0x05, 0xe4, 0x99, 0xda, 0x00, 0x00, 0xe5, 0x99,
	0xff, 0x98, 0xcc, 0x67, 0xa0, 0x50, 0xde, 0x66,
	0x66, 0x64, 0x99, 0x99, 0xed, 0xe5, 0xec, 0x33,
	0x99, 0xcd, 0x66, 0x33, 0xdd, 0xee, 0x62, 0x77,
	0x64, 0xfa, 0xb9, 0x99, 0x91, 0x91, 0x91, 0xbb,
	0x05, 0x05, 0xf5, 0x62, 0x62, 0x62, 0x77, 0x77,
	0xf5, 0xcd, 0x44, 0xbb, 0x33, 0x91, 0xbb, 0xee,
	0x5a, 0xbb, 0x4f, 0xdd, 0x77, 0x77, 0x77, 0xcd,
	0x05, 0xf5, 0xa5, 0xe4, 0xbb, 0xbb, 0xee, 0x8f,
	0x00, 0x4f, 0x0a, 0x4f, 0xdd, 0x77, 0xda, 0x0a,
	0xa2, 0x00, 0x00, 0x00, 0x4f, 0xcf, 0xa5, 0x51,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x99, 0xdd, 0x10, 0x66, 0xa0, 0x00, 0x05, 0x86,
	0x66, 0x67, 0x55, 0x33, 0xdd, 0xff, 0x05, 0xc3,
	0x99, 0xef, 0x55, 0x62, 0x66, 0xc9, 0x05, 0xc3,
	0x77, 0xd8, 0x10, 0x62, 0x66, 0xba, 0x05, 0xc3,
	0x99, 0xfa, 0x50, 0x99, 0xcc, 0xd8, 0x05, 0x86,
	0x77, 0x8f, 0x50, 0xee, 0xcc, 0xaf, 0x10, 0x0f,
	0xee, 0x8f, 0x05, 0xf5, 0xdd, 0xa0, 0x05, 0x0f,
	0x77, 0xa0, 0x00, 0x05, 0x0f, 0x00, 0x05, 0x0f,
	0xef, 0x0a, 0x51, 0xf1, 0xf4, 0xf1, 0x05, 0x0f,
	0xd8, 0x00, 0xa2, 0xf3, 0xf2, 0x78, 0xa2, 0x0f,
	0x8f, 0x51, 0x51, 0x51, 0xf1, 0xf4, 0xa0, 0x0f,
	0x0a, 0xf3, 0xa2, 0x00, 0xf2, 0xf2, 0xa2, 0x0f,
	0x51, 0x51, 0x51, 0x51, 0xf0, 0xf1, 0xa0, 0x0f,
	0xf3, 0xa2, 0x00, 0xf3, 0xf2, 0xf2, 0xa2, 0x4b,
	0x51, 0x51, 0x51, 0x51, 0xf3, 0xf3, 0xf1, 0x04,
	0x00, 0x00, 0x00, 0xa2, 0xf2, 0xf2, 0xf3, 0x00
};