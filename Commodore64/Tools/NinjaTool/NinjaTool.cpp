// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

typedef unsigned char u8;

#define SAVETEXTFILE // Enable this to export text for mistakes checks

#define C64 // Pointers start at 0 (and CPC start at 0x9c00)

// ------------------------------------------------------

//#include "Level01.h"
//#include "Level02.h"
//#include "Level11.h"
//#include "Level12.h"
//#include "Level13.h"
//#include "Level21.h"
//#include "Level22.h"
//#include "Level23.h"
//#include "Level31.h"
//#include "Level32.h"
//#include "Level33.h"
//#include "Level41.h"
//#include "Level42.h"
//#include "Level43.h"
//#include "Level51.h"
//#include "Level52.h"
//#include "Level53.h"
//#include "Level61.h"
//#include "Level62.h"
//#include "Level63.h"
//#include "Level71.h"
//#include "Level72.h"
//#include "Level73.h"
//#include "Level81.h"
//#include "Level82.h"
//#include "Level83.h"
//#include "Level91.h"
//#include "Level92.h"
//#include "Level93.h"
//#include "LevelC1.h"
//#include "LevelC2.h"
//#include "LevelC3.h"
//#include "LevelD1.h"
//#include "LevelZ1.h" // Demo
//#include "LevelZ2.h"
//#include "LevelZ3.h"

// ------------------------------------------------------
// z80 is little-endian, so first lower bytes, then higher
void writeu16(FILE* file, short pointer)
{
	u8 bigbyte = (pointer) >> 8;
	u8 lowbyte = (pointer) & 0x00FF;

	fwrite(&lowbyte, 1, 1, file);
	fwrite(&bigbyte, 1, 1, file);
}

// ------------------------------------------------------
int GetNumberOfSpaces(u8* ptext)
{
	int result = 0;
	while (*ptext == ' ')
	{
		result++;
		ptext++;
	}
	return result;
}

// ------------------------------------------------------
int GetNextWordSize(u8* ptext)
{
	int result = 0;
	while (*ptext != ' ' && *ptext != 0)
	{
		result++;
		ptext++;
	}
	return result;
}

// ------------------------------------------------------
// 1 : couleur 1 (default)
// 2 : couleur 2

// 253 : couleur 3 (negatif)
// 254 : Encode space + number of spaces
// 255 : next line

int CompressSpaces(u8* ptext, int nbspaces)
{
	int nbcaracterwritten=0;
	if (nbspaces == 0)
		return nbcaracterwritten;
	else if (nbspaces == 1)
	{
		ptext[0] = ' ';
		nbcaracterwritten++;
		return nbcaracterwritten;
	}
	else if (nbspaces == 2)
	{
		ptext[0] = ' ';
		ptext[1] = ' ';
		nbcaracterwritten+=2;
		return nbcaracterwritten;
	}
	else
	{
		ptext[0] = 254;
		ptext[1] = nbspaces;
		nbcaracterwritten += 2;
		return nbcaracterwritten;
	}
}

// ------------------------------------------------------
void ConvertUppercase(u8* pText)
{
	int totalcount = 0; // Check total size on screen. Should be 32+34+36+34+36
	// convert to UpperCase and remove special characters
	u8* pTextSave = pText;
	while (pText[0] != 0)
	{
		u8 cara = pText[0];

		if (cara >= 'a' && cara <= 'z')
			pText[0] = cara - 'a' + 'A';
		//printf("%02x %02x\n", cara, 'é');
		if (cara == (unsigned char) 'é')
			pText[0] = 'E';
		else if (cara == (unsigned char)'è')
			pText[0] = 'E';
		else if (cara == (unsigned char)'à')
			pText[0] = 'A';
		else if (cara == (unsigned char)'â')
			pText[0] = 'A';
		else if (cara == (unsigned char)'ê')
			pText[0] = 'E';
		else if (cara == (unsigned char)'È')
			pText[0] = 'E';
		else if (cara == (unsigned char)'Ê')
			pText[0] = 'E';
		else if (cara == (unsigned char)'’') // Apostrophe
			pText[0] = '\'';
		else if (cara == (unsigned char)'ô') 
			pText[0] = 'O';
		else if (cara == (unsigned char)'ç') 
			pText[0] = 'C';
		else if (cara == (unsigned char)'û') 
			pText[0] = 'U';	
		else if (cara == (unsigned char)'ù') 
			pText[0] = 'U';	
		else if (cara == (unsigned char)'ï')
			pText[0] = 'I';
		else if (cara == (unsigned char)'î')
			pText[0] = 'I';

		if (pText[0] != 1 && pText[0] != 2)
			totalcount++;

		pText++;
	}

	// Check total count
	if (totalcount != 32 + 34 + 36 + 34 + 32)
		printf("-- Warning, string do not have good number of characters : \n%s<\n", pTextSave);
}

// ------------------------------------------------------
void compresstext(u8* dest, u8* source)
{
	u8 temporarystring[512];
	u8* pSourceCurrent= source;
	u8* pDestCurrent = temporarystring;
	int i;
	for (i = 0; i < 512; i++)
		temporarystring[i] = 0;
	int currentsize = 0; // Will check that this is not too much (displayable character are 38*4)
	int currentlinesize = 0;
	// Check next word size. if size is ok to fit on line then add it, else fill line with space.
	while (currentlinesize <= 38 && pSourceCurrent[0]!=0)
	{
		// Spaces
		int space = GetNumberOfSpaces(pSourceCurrent);
		int spacetoadd = space;
		// Reduce to fit line
		if (currentlinesize + spacetoadd > 38)
			spacetoadd = 38 - currentlinesize;
		int charadded = CompressSpaces(pDestCurrent, spacetoadd);
		pDestCurrent += charadded;
		currentlinesize += spacetoadd; // Line on CPC (uncompressed)
		pSourceCurrent += space;
		// Word
		int nextwordsize = GetNextWordSize(pSourceCurrent);
		if (currentlinesize + nextwordsize <= 38)
		{
			// Copy word
			memcpy(pDestCurrent, pSourceCurrent, nextwordsize);
			pDestCurrent += nextwordsize;
			pSourceCurrent += nextwordsize;
			currentlinesize += nextwordsize;
		}
		else
		{
			// Not enough space, then add spaces and change line.
			int nbspacetoadd = 38 - currentlinesize;
			int charadded = CompressSpaces(pDestCurrent, spacetoadd);
			pDestCurrent += charadded;
			currentlinesize = 0; // Reset line size
		}
	}
	// Allocate size of string
	int size = strlen( (const char*) temporarystring);
	dest = (u8*) malloc(size + 1);
	memcpy(dest, temporarystring, size + 1);
}

// ------------------------------------------------------
int SaveCopyLine(char* dest, char* source, int size, int* processedsource)
{
	int processedbytes = 0;
	char lastcharacter = 'A';

	for (int i = 0 ; i < size; i++)
	{
		if (lastcharacter == ' ' && source[i] == ' ')
			continue;
		else if (source[i] == '\1')
		{
			size++; // These are not counted
			continue;
		}
		else if (source[i] == '\2')
		{
			size++; // These are not counted
			continue;
		}
		else
		{
			dest[processedbytes] = source[i];
			processedbytes++;
		}
		lastcharacter = source[i];
	}

	// Add space at end, if there is not
	if (lastcharacter != ' ')
	{
			dest[processedbytes] = ' ';
			processedbytes++;
	}

	*processedsource = size;

	return processedbytes;
}

// ------------------------------------------------------
void SaveTextForDebug(FILE* file, u8* message)
{
	char string[512];
	char* dest = string;
	char* source = (char*)message;
	int destbytesprocessed;
	int sourcebytesprocessed;
	// Copy characters to string
	destbytesprocessed = SaveCopyLine(dest, source, 32,&sourcebytesprocessed);
	dest += destbytesprocessed;
	source += sourcebytesprocessed;
	// -- 
	destbytesprocessed = SaveCopyLine(dest, source, 34,&sourcebytesprocessed);
	dest += destbytesprocessed;
	source += sourcebytesprocessed;
	// -- 
	destbytesprocessed = SaveCopyLine(dest, source, 36,&sourcebytesprocessed);
	dest += destbytesprocessed;
	source += sourcebytesprocessed;
	// -- 
	destbytesprocessed = SaveCopyLine(dest, source, 34,&sourcebytesprocessed);
	dest += destbytesprocessed;
	source += sourcebytesprocessed;
	// -- 
	destbytesprocessed = SaveCopyLine(dest, source, 32,&sourcebytesprocessed);
	dest += destbytesprocessed;
	source += sourcebytesprocessed;
	// -- 
	dest[0] = 0;

	fprintf(file, "%s\n", string);
}

// ------------------------------------------------------
int main()
{
	std::cout << "Starting!\n";

	FILE* file;

	// -- Save text files

#ifdef  SAVETEXTFILE // Enable this to export text for mistakes checks
	char textfilename[512];
	strcpy(textfilename, BINNAME);
	strcat(textfilename, ".txt");
	file = fopen(textfilename, "wb");
	// Save Intro
	fprintf(file, "-----------------------------------------\n");
	fprintf(file, "%s\n",BINNAME);
	SaveTextForDebug(file,messageintro);
	for (int i = 0; i < NBMESSSUCESS; i++)
		SaveTextForDebug(file,messagesucces[i]);
	fprintf(file, "morts:\n");
	for (int i = 0; i < NBMESSDEATH; i++)
		SaveTextForDebug(file,messagedeath[i]);
	fclose(file);
#endif



	file = fopen(BINNAME, "wb");

	// Compress texts
	//compresstext(messageintro_compress,messageintro);

	int emptypointer = 0x0000;
	int mypointer = 0x9C00; // Start pointer.

#ifdef C64
	mypointer = 0x0000;
#endif

	mypointer += (2 * (6 + 32 + 32));

	// Write pointers
	// 1+1+1+1+1+32+32+1 = 70 pointers = 140 bytes

	// -- Level data (1)
	// u8 leveldata[2]
	writeu16(file, mypointer);
	mypointer += sizeof(leveldata);

	// -- Next level name (1)
	// u8 nextlevelname[] = "LEVEL12 BIN";
	writeu16(file, mypointer);
	mypointer += sizeof(nextlevelname);

	// -- Pointer Zone (1)
	// u8 zonesdatas[NBZONES * 5]
	writeu16(file, mypointer);
	mypointer += sizeof(zonesdatas);

	// -- Pointer Sequences (1)
	// u8 scenario[9][10]
	writeu16(file, mypointer);
	mypointer += sizeof(scenario);

	// -- Pointer Texte Intro (1)
	// u8 messageintro[]
	writeu16(file, mypointer);
	mypointer += sizeof(messageintro);

	// -- Pointer Textes Victoires (32)
	// u8 messagesucces[4][MESSAGEMAXLENGHT]
	for (int i = 0; i < NBMESSSUCESS; i++)
	{
		writeu16(file, mypointer);
		mypointer += sizeof(messagesucces[i]);
	}
	for (int i = 0; i < (32- NBMESSSUCESS); i++)
	{
		writeu16(file, emptypointer);
	}

	// -- Pointer Textes Defaite (32)
	// u8 messagedeath[4][MESSAGEMAXLENGHT]
	for (int i = 0; i < NBMESSDEATH; i++)
	{
		writeu16(file, mypointer);
		mypointer += sizeof(messagedeath[i]);
	}
	for (int i = 0; i < (32 - NBMESSDEATH); i++)
	{
		writeu16(file, emptypointer);
	}

	// -- Pointer tiles 24 x 
	// const u8 tile_Pic11_00[24 * 8 * 16]
	writeu16(file, mypointer);

	// ------------------------------------------------------
	// Write datas

	// -- Level data (1)
	// u8 leveldata[2]
	fwrite(leveldata, 1, sizeof(leveldata), file);

	// -- Next level name (1)
	// u8 nextlevelname[] = "LEVEL12 BIN";
	fwrite(nextlevelname, 1, sizeof(nextlevelname), file);

	// -- Pointer Zone (1)
	// u8 zonesdatas[NBZONES * 5]
	fwrite(zonesdatas, 1, sizeof(zonesdatas), file);

	// -- Pointer Sequences (1)
	// u8 scenario[9][10]
	fwrite(scenario, 1, sizeof(scenario), file);

	// -- Pointer Texte Intro (1)
	// u8 messageintro[]
	ConvertUppercase(messageintro);
	fwrite(messageintro, 1, sizeof(messageintro), file);

	// -- Pointer Textes Victoires (32)
	// u8 messagesucces[4][MESSAGEMAXLENGHT]
	for (int i = 0; i < NBMESSSUCESS; i++)
	{
		ConvertUppercase(messagesucces[i]);
		fwrite(messagesucces[i], 1, sizeof(messagesucces[i]), file);
	}

	// -- Pointer Textes Defaite (32)
	// u8 messagedeath[4][MESSAGEMAXLENGHT]
	for (int i = 0; i < NBMESSDEATH; i++)
	{
		ConvertUppercase(messagedeath[i]);
		fwrite(messagedeath[i], 1, sizeof(messagedeath[i]), file);
	}

	// -- Pointer tiles 24 x 
	// const u8 tile_Pic11_00[24 * 8 * 16]
	fwrite(tile_Pic01_00, 1, sizeof(tile_Pic01_00), file);

	fclose(file);



	std::cout << "Ending!\n";
}
