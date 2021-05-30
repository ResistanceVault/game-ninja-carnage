// NinjaTool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Double dash to add word cuts.

#include <iostream>
#include <fstream>      // std::ifstream
#include <string>
#include <Windows.h>
#include <cstdio>
#include <sstream>
#include <vector>

// -- Plateform (default is CPC)
//#define CPC
#define C64
//#define SPECCY

// -- Langages
//#define LANGAGE 3  // French
//#define LANGAGE 4  // English
//#define LANGAGE 7  // Spanish
//#define LANGAGE 10 // Italian
//#define LANGAGE 13 // Greek
//#define LANGAGE 16 // German
#define LANGAGE 19 // Polish // Disable cut words

//#define DEMO // Level 33 will chain to level z1 insteade of 41

// -- Options (now mandatory)
#if LANGAGE!=19
#define CUTWORDALLOWED
#endif
#define AUTOLINEFEEDALLOWED
#define CENTERALL
#define DEBUGONLYDISPLAYERRORSTEXT

// -- Debug flags
//#define GREEKTEXTCHECK // Use this to check length and display strings
//#define POLISHTEXTCHECK
//#define DOONLYFIRSTLEVEL
//#define DISPLAYALLTEXTS

//#define DUMPSPECTRUMTEXTS
#define SPECCY_NUMTEXTSPERPARTS 96 // For each part
#define SPECCY_LEVELSIDEB 51 // This is from this level that side B start

// -------------------------------------------------

typedef unsigned char u8;

#define SAVETEXTFILE // Enable this to export text for mistakes checks

using namespace std;

// ------------------------------------------------------

string gStringIntro;

vector<string> gStringTexts;

vector<string> gStringDeaths;

vector<string> gStringTimeUpDeaths;

#define NBPOINTER 	(1+1+1+1+1+32+32+1) // = 70 pointers = 140 bytes
#define STARTMEMORYCPC 0x9c00

#define BANNERSIZE (32+34+36+34+32)

struct levelbinary
{
	unsigned short pointers[NBPOINTER]; // 140 bytes
	unsigned char data[10240];
};

levelbinary gLevelInput;
int         gLevelInputSize;

levelbinary gLevelOutput;
int         gLevelOutputSize;

bool gError; // True if any error occured

int gGlobalLineNumber; // For debug keep current line in input text
int gGlobalLevelStartNumber;
int gGlobalLevelLine;

int gSpeccyCurrentTextNumber=0; // up to SPECCY_NUMTEXTSPERPARTS text in one part
int gSpeccyCurrentGlobalTextNumber=0; // 0 to 400 (in bas text, not in source file)
int gSpecctCurrentPart=0; // 0 1 
int gSpeccyCurrentSide=0; // 0 1
FILE* gSpeccyIntroFile = NULL;
FILE* gSpeccyTextFile = NULL;
FILE* gSpeccyTimeUpFile = NULL;

// Prototypes
void SpeccycheckOpenOrChangeFile();
#ifdef  DUMPSPECTRUMTEXTS
FILE* SpectrumFile;
#endif
bool ReplaceControlCharacters(string& inputstring, int indexstart, int indexend);

// ------------------------------------------------------
// Name : FindPreviousSentenceEnd
// Index 0 if nothing found.
int FindPreviousSentenceEnd(string& inputstring, int startoffset)
{
	int currentposition = startoffset;
	// French rule.
	// End of sentence. 
	// .   ...
	// !  !!!!!
	// ?  ?????
	// Rule : If starting character is any .?! then skip.

	// -- Skip first characters if there are ponctuation mark (previous find)
	while (inputstring[currentposition] == '.' || inputstring[currentposition] == '?' || inputstring[currentposition] == '!')
		currentposition--;

	// -- Now find next ponctuation
	while ( currentposition>0 && inputstring[currentposition] != '.' && inputstring[currentposition] != '?' && inputstring[currentposition] != '!')
		currentposition--;

	return currentposition;
}

// ------------------------------------------------------
// Name : InjectSpaces
// startoffset index of starting character.
// lineendpos is index of next line character (so exluded)
// allowwordcut if true, then words can be cut, minimum size if 4 letter AAA|A converted into AA-|AA
void InjectSpaces(string& inputstring, int startoffset, int lineendpos, bool allowwordcut=false, bool addlinefeedafterponctuation=false)
{
	// -- If only space, then do nothing
	bool onlyspace = true;
	for (int i = 0 ; i<lineendpos-startoffset ; i++)
		if (inputstring[startoffset + i] != ' ') { onlyspace = false; break; }
	if (onlyspace)
		return;

	// -- Remove space characters at start
	// add them at end if there are some space also
	// Else keep the word in one piece so that it will be cut
	while (inputstring[startoffset] == ' ')
	{
		inputstring.erase(startoffset, 1);
		//if (lineendpos==168)
		//	inputstring.insert(lineendpos-1, 1,' ');
		//if (inputstring[lineendpos-1]==' ' || inputstring[lineendpos-2]==' ')
		//	inputstring.insert(lineendpos-1, 1,' ');
	}
	// -- If string is too short, then add spaces at end
	if (inputstring.size() < BANNERSIZE )
	{
		inputstring.insert(inputstring.size(), (BANNERSIZE) - inputstring.size(), ' ');
	}

	// -- Remove double dash if any
	int cutlinepos = 0;
	bool shouldcontinue = true;
	do
	{
		int temppos;
		temppos = inputstring.find("--", startoffset);
		if ( temppos != string::npos && temppos<lineendpos)
		{
			inputstring.erase(temppos, 2);
			cutlinepos = temppos;
		}
		else
		{
			shouldcontinue = false; // Nothing found, then stop
		}
	} while ( shouldcontinue );

	// -- If any line feed then apply it.
	bool linefeeddone = ReplaceControlCharacters(inputstring, startoffset, lineendpos);
	if (linefeeddone)
		return;

	// -- If Auto line feed, then search any ponctuation.
	if (addlinefeedafterponctuation)
	{
		int indexlastponctuation = FindPreviousSentenceEnd(inputstring, lineendpos);
		if (indexlastponctuation>startoffset)
		{
			// Insert line feed
			// Add space until end
			inputstring.insert(indexlastponctuation+1, lineendpos-(indexlastponctuation+1), ' ');

			// Try a second time
			indexlastponctuation = FindPreviousSentenceEnd(inputstring, indexlastponctuation);
			if (indexlastponctuation>startoffset)
			{
				// Insert line feed
				// Add space until end
				inputstring.insert(indexlastponctuation+1, lineendpos-(indexlastponctuation+1), ' ');
			}

			return;
		}

	}

	// -- Check end if line. 
	// If we are inside a word, then the word will go to next line.
	// Check final character and next one.
	bool  problematend = false;
	//bool ponctuationwithspace = false;
	if (inputstring[lineendpos - 1] != ' ' && inputstring[lineendpos] != ' ')
		problematend = true;
	/*
	if (inputstring[lineendpos - 1] == ' ' && inputstring[lineendpos] == '!')
	{
		problematend = true;
		ponctuationwithspace = true;
	}
	if (inputstring[lineendpos - 1] == ' ' && inputstring[lineendpos] == '?')
	{
		problematend = true;
		ponctuationwithspace = true;
	}
	*/

	if (problematend)
	{
		int poslastspace;
		//if (!ponctuationwithspace)
			poslastspace = inputstring.rfind(" ", lineendpos - 1);
		//else
		//	poslastspace = inputstring.rfind(" ", lineendpos - 2);

		// -- If we are asked to cut word, then check that minimum is there.
		// 3 characters before and 1 character after.
		if (allowwordcut)
		{
			int posnextspace = inputstring.find(" ", lineendpos-1);

			int nbcharsbeforeendofline = lineendpos - poslastspace -1;
			int nbcharsafterendofline = posnextspace - lineendpos;

			if (nbcharsbeforeendofline >= 3 && nbcharsafterendofline >= 1)
			{
				// Check if we had found a cutting place
				if (cutlinepos != 0 && cutlinepos > (poslastspace + 2) && cutlinepos < lineendpos - 1)
				{
					inputstring.insert(cutlinepos, "-");
					// Add spaces until end
					if (lineendpos-cutlinepos>0)
					{
						for (int j=0; j<(lineendpos-cutlinepos)-1; j++)
							inputstring.insert(cutlinepos+1, " ",1);
					}
				}
				else // No cut word
					inputstring.insert(lineendpos - 1, "-");
			}
			else
			{
				// We can not cut word, so unactivate word cut. Next word will be on next line
				allowwordcut = false;
			}
		}
		else
		{
			// No cut words is allowed but we can check if manual word cut have been added
			int posnextspace = inputstring.find(" ", lineendpos-1);
			int nbcharsbeforeendofline = lineendpos - poslastspace -1;
			int nbcharsafterendofline = posnextspace - lineendpos;

			if (nbcharsbeforeendofline >= 3 && nbcharsafterendofline >= 1)
			{
				// Check if we had found a cutting place
				if (cutlinepos != 0 && cutlinepos > (poslastspace + 2) && cutlinepos <= lineendpos - 1)
				{
					inputstring.insert(cutlinepos, "-");
					// Add spaces until end
					if (lineendpos-cutlinepos>0)
					{
						for (int j=0; j<(lineendpos-cutlinepos)-1; j++)
							inputstring.insert(cutlinepos+1, " ",1);
						allowwordcut = true; // To not inject space at end.
					}
				}
			}
		}

		// -- If we are not allowed to cut words, then insert space, so next word will start on next line
		if (!allowwordcut)
		{
			// Inject spaces.
			if ( poslastspace != string::npos )
				inputstring.insert(poslastspace, (lineendpos-1) - poslastspace, ' ');
		}
	}
}

// ------------------------------------------------------
// isuppercase
// ------------------------------------------------------
bool isuppercase(unsigned char charcode)
{
	if (charcode >= 'A' && charcode <= 'Z')
		return true;
	//if (charcode >= '0' && charcode <= '9')
	//	return true;
#ifndef SPECCY
	if (charcode >= 128 && charcode <= 138 ) // greek
		return true;
	if (charcode >= 160 && charcode <= 168 ) // polish
		return true;
#else
	// Polish
	#if LANGAGE==19
	if (charcode >= 194 && charcode <= 201 ) //polish
		return true;
	#else
	if (charcode >= 188 && charcode <= 197 ) //greek
		return true;
	#endif
#endif
	if (charcode == '-' ) // For name ORE-UNA
		return true;
	if (charcode == '\'' ) // For french "C'EST"
		return true;
	if (charcode == '`' ) // For ITalian for all À È Ì Ò
		return true;

	return false;
}

// ------------------------------------------------------
// islowercase
// ------------------------------------------------------
bool islowercase(unsigned char charcode)
{
	if (charcode >= 'a' && charcode <= 'z')
		return true;
	if (charcode >= '0' && charcode <= '9')
		return true;
#ifndef SPECCY
	if (charcode >= 128+100 && charcode <= 138+100 ) // greek
		return true;
	if (charcode >= 169 && charcode <= 177 ) // polish
		return true;
#else
	// Polish
	#if LANGAGE==19
	if (charcode >= 202 && charcode <= 209 ) //polish
		return true;
	#else
	if (charcode >= 198 && charcode <= 207 ) //greek
		return true;
	#endif
#endif

	return false;
}

// ------------------------------------------------------
// ASCII to uppercase
// ------------------------------------------------------
unsigned char touppercase(unsigned char charcode)
{
	if (charcode >= 'a' && charcode <= 'z')
		charcode = charcode - 'a' + 'A'; // To uppercases

#ifndef SPECCY
	// so UPPERCASES : 128 to 138 
	// Lowercases    : same +50
	if (charcode >= 128+100 && charcode <= 138+100) // Greek lowercases
		charcode -= 100;
	if (charcode >= 169 && charcode <= 177) // polish lowercases
		charcode -= 9;
#else
	#if LANGAGE==19
	// so UPPERCASES : 194 to 201
	// Lowercases    : 202 to 209
	if (charcode >= 202 && charcode <= 209) // polishF lowercases
		charcode -= 8;
	#else
	// so UPPERCASES : 188 to 197
	// Lowercases    : 198 to 207
	if (charcode >= 198 && charcode <= 207) // Greek lowercases
		charcode -= 10;
	#endif
#endif

	return charcode;
}

// ------------------------------------------------------
// ASCII to lowercase
unsigned char tolowercase(unsigned char charcode)
{
	if (charcode >= 'A' && charcode <= 'Z')
		charcode = charcode - 'A' + 'a'; // To uppercases

#ifndef SPECCY
	// so UPPERCASES : 128 to 144 (96+32)
	// Lowercases    : 228 to 244
	if (charcode >= 128 && charcode <= 144) // Greek lowercases
		charcode += 100;
	if (charcode >= 160 && charcode <= 169) // Greek lowercases
		charcode += 9;
#else
	#if LANGAGE==19
	// so UPPERCASES : 194 to 201
	// Lowercases    : 202 to 209
	if (charcode >= 194 && charcode <= 201) // polish uppercase
		charcode += 8;
	#else
	// so UPPERCASES : 188 to 197
	// Lowercases    : 198 to 207
	if (charcode >= 188 && charcode <= 197) // Greek lowercases
		charcode += 10;
	#endif
#endif

	return charcode;
}


// ------------------------------------------------------
// DetectUppercasesWords
// ------------------------------------------------------
void DetectUppercasesWords(string& inputstring)
{
	// A line can have multiple words displayed with alternate fonts. (usually max 2).
	// We add \2 and \1 to switch font
	// 3 consecutives uppercases can be considered as the start of a sequence.
	// (because in french "J'ai" will fail with only 2
	// Space is not stopping the sequence. Any non uppercase will stop it.

	// Word mixing Uppercases and lowercases or number should not be detected

	bool sequenceisstarted = false;
	int  sequencestartindex = -1;
	for (int i = 0; i < inputstring.size(); i++)
	{
		// Handle here special words
		if (inputstring.compare(i, strlen("dESiRE"), "dESiRE") == 0)
		{ inputstring.insert(i, "\2");i += strlen("dESiRE")+1; inputstring.insert(i, "\1"); }
		if (inputstring.compare(i, strlen("8BITS-UNITY"), "8BITS-UNITY") == 0)
			{ inputstring.insert(i, "\2");i += strlen("8BITS-UNITY")+1; inputstring.insert(i, "\1"); }
		if (inputstring.compare(i, strlen("SIM1"), "SIM1") == 0)
			{ inputstring.insert(i, "\2");i += strlen("SIM1")+1; inputstring.insert(i, "\1"); }
		if (inputstring.compare(i, strlen("4PLAY"), "4PLAY") == 0)
			{ inputstring.insert(i, "\2");i += strlen("4PLAY")+1; inputstring.insert(i, "\1"); }
		if (inputstring.compare(i, strlen("PHOENIX0870"), "PHOENIX0870") == 0)
			{ inputstring.insert(i, "\2");i += strlen("PHOENIX0870")+1; inputstring.insert(i, "\1"); }
		if (inputstring.compare(i, strlen("ZZAP! 64"), "ZZAP! 64") == 0)
			{ inputstring.insert(i, "\2");i += strlen("ZZAP! 64")+1; inputstring.insert(i, "\1"); }
		

		// Handle all special cases
		bool thirdcharacterisok = false;
		if (i<=inputstring.size()-3 ) 
			thirdcharacterisok = isuppercase(inputstring[i + 2]) || inputstring[i + 2] == ',' || inputstring[i + 2] == ' ' || inputstring[i + 2] == '.' || inputstring[i + 2] == '!' || inputstring[i + 2] == '?';
		
		if (i<=inputstring.size()-1 )
		{
			//bool previousisok; // Previous character should not be a character (lowercase number or letter)
			//if (i == 0)
			//	previousisok = true;
			//else
			//	previousisok = !islowercase(inputstring[i - 1]);

			if (isuppercase(inputstring[i]) && isuppercase(inputstring[i + 1]) && thirdcharacterisok && !sequenceisstarted)
			{
				// Start of sequence is detected.
				sequenceisstarted = true;
				inputstring.insert(i, "\2");
				i++;
			}
		}
		// Detect end of sequence
		// Space is not considered as end of sequence.
		
		if (sequenceisstarted)
		{
			bool isend = false;
			isend = (inputstring[i] != ' ' && !isuppercase(inputstring[i]));
			// Test space // consider that case "OK Einstein".
			if (inputstring[i]==' ' && i<=inputstring.size()-2 )
			{
				if (!isuppercase(inputstring[i + 1]) || !isuppercase(inputstring[i + 2]))
					isend = true;
			}

			if (isend)
			{
				sequenceisstarted = false;
				inputstring.insert(i, "\1");
				i++;
			}
		}
	}
}

// ------------------------------------------------------
void SetAlternateFont(string& inputstring)
{
	string string1;
	string string2;
	string string3;
	string string4;
	string string5;
	string1 = inputstring.substr(0, 32);
	string2 = inputstring.substr(32, 34);
	string3 = inputstring.substr(32+34, 36);
	string4 = inputstring.substr(32+34+36, 34);
	string5 = inputstring.substr(32+34+36+34, inputstring.size() - 32+34+36+34);
	DetectUppercasesWords(string1);
	DetectUppercasesWords(string2);
	DetectUppercasesWords(string3);
	DetectUppercasesWords(string4);
	DetectUppercasesWords(string5);
	inputstring = string1 + string2 + string3 + string4 + string5;
}

// ------------------------------------------------------
#ifndef SPECCY
// All chars in font, verify what is available
// Greek uppercase are from 128 to 138
// Spanish ñ 139
// German ß 127 ä ü ö 140 141 142
// then other accents
// polish 160 to 168 (and lowercase 169 to 177)
const unsigned char gFontCharacters[]=" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz[\\]^_`{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaA\xaB\xaC\xaD\xaE\xaF\xb0\xb1\0";
#else
// the font go until code 209 (0xd1)
const unsigned char gFontCharacters[]=" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz[\\]^_`{|}~\x7f\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8d\x8e\x8f\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F\xa0\xa1\xa2\xa3\xa4\xa5\xa6\xa7\xa8\xa9\xaA\xaB\xaC\xaD\xaE\xaF\xb0\xb1\xb2\xb3\xb4\xb5\xb6\xb7\xb8\xb9\xbA\xbB\xbC\xbD\xbE\xbF\xc0\xc1\xc2\xc3\xc4\xc5\xc6\xc7\xc8\xc9\xcA\xcB\xcC\xcD\xcE\xcF\xd0\xd1\0";
#endif
// ------------------------------------------------------
bool isexistinginfont(unsigned char charcode)
{
	int index = 0;
	while (gFontCharacters[index] != 0)
	{
		if (gFontCharacters[index] == charcode)
			return true;
		index++;
	}
	return false;
}

// ------------------------------------------------------
void ConvertToExistingCharacters(string& inputstring)
{
	for (int i = 0; i < inputstring.size(); i++)
	{
		unsigned char currentchar = inputstring[i];
		if (currentchar == '\1' || currentchar == '\2' || currentchar == ' ')
			continue;

		// For greek, force all uppecase
		#if LANGAGE==13
		currentchar = touppercase(currentchar); // To uppercases
		inputstring[i] = currentchar;
		#endif

		if (!isexistinginfont(currentchar)) // Not found
		{
			currentchar = touppercase(currentchar); // To uppercases

			if (!isexistinginfont(currentchar)) // Not found
			{
				printf("character not found in font : %c (%08x) \n",currentchar,currentchar);
			}

			inputstring[i] = currentchar;
		}
	}
}

// ------------------------------------------------------
// Code UTF-8 2 to 4. Then converted ascii code.
// Each line is 6 char
// NbBytesForUTFCode (2 3 4) then 4 bytes UTF-8 then replace char.
#ifndef SPECCY
#define REPLACEMENTCHARS (109+16)
unsigned char gConversionTable[REPLACEMENTCHARS][6] =
{
	// CPC and C64 version, same layout in font
	{ 2,0xc2,0xb0,0,0,'\'' }, //	° 
	// French
	{ 2,0xc3,0xa9,0,0, 114+32 }, //	é
	{ 2,0xc3,0xa8,0,0, 113+32 }, //	è
	{ 2,0xc3,0xa0,0,0, 111+32 }, //	à
	{ 2,0xc3,0xa2,0,0, 122+32 }, //	â
	{ 2,0xc3,0xaa,0,0, 123+32 }, //	ê
	{ 2,0xc3,0x88,0,0,'E' }, //	È
	{ 2,0xc3,0x8a,0,0,'E' }, //	Ê
	{ 3,0xe2,0x80,0x99,0,'\'' }, //	’
	{ 2,0xc3,0xb4,0,0, 124+32 }, //	ô 
	{ 2,0xc3,0xa7,0,0, 119+32 }, //	ç
	{ 2,0xc3,0xbb,0,0, 125+32 }, //	û
	{ 2,0xc3,0xb9,0,0, 118+32 }, //	ù
	{ 2,0xc3,0xaf,0,0, 127+32 }, //	ï
	{ 2,0xc3,0xae,0,0, 126+32 }, //	î
	{ 2,0xc5,0x8d,0,0,'o' }, //	ō
	{ 2,0xc3,0x87,0,0,'C' }, //	Ç
	// German
	{ 2,0xc3,0xa4,0,0, 108+32 }, //	ä (MUST HAVE)
	{ 2,0xc3,0xbc,0,0, 109+32 }, //	ü (MUST HAVE)
	{ 2,0xc3,0xb6,0,0, 110+32 }, // ö (MUST HAVE)
	{ 2,0xc3,0x9f,0,0, 95+32 }, // ß (MUST HAVE)
	{ 2,0xc3,0x96,0,0,'O' }, // Ö
	{ 2,0xc3,0x9c,0,0,'U' }, // Ü
	{ 2,0xc3,0x84,0,0,'A' }, // Ä
	// Italian
	{ 2,0xc3,0xb2,0,0, 121+32 }, //	ò (MUST HAVE)
	{ 2,0xc3,0xac,0,0, 120+32 }, //	ì (MUST HAVE)
	{ 2,0xc3,0x8c,0,0,'I' }, //	Ì 
	{ 2,0xc3,0x92,0,0,'O' }, // Ò 
	{ 2,0xc3,0x80,0,0,'A' }, // À 
	// Spanish
#if LANGAGE!=19 // Polish have it
	{ 2,0xc3,0x93,0,0, 'O' }, // Ó
	{ 2,0xc3,0xb3,0,0, 116+32 }, // ó (MUST HAVE)
#endif
	{ 2,0xc2,0xbf,0,0, '%' },    // ¿ // % is not used in texts (MUST HAVE)
	{ 2,0xc3,0xad,0,0, 115+32 }, // í (MUST HAVE)
	{ 2,0xc3,0x8d,0,0,'I' }, // Í
	{ 2,0xc2,0xa1,0,0,'$' }, // ¡ // $ is not used in texts (MUST HAVE)
	{ 2,0xc3,0xb1,0,0, 107+32 }, // ñ (MUST HAVE)
	{ 2,0xc3,0xa1,0,0, 112+32 }, // á (MUST HAVE)
	{ 2,0xc3,0x9a,0,0,'U' }, // Ú
	{ 2,0xc3,0xba,0,0, 117+32 }, // ú
	{ 2,0xc3,0x89,0,0, 'E' }, // É
	{ 2,0xc3,0x81,0,0, 'A' }, // Á
	{ 2,0xc3,0x91,0,0, 'N' }, // Ñ
	{ 3,0xe2,0x80,0x8b,0,' ' }, // Zero width space (can happend)
	// Greek
 { 2 , 0xce, 0x91, 0 , 0 , 'A' }, // Α 
 { 2, 0xce, 0x86,0 , 0 , 'A' }, //  Ά
 { 2 , 0xce, 0xb1, 0 , 0 , 'a' }, // α
 { 2, 0xce , 0xac , 0, 0 , 'a' }, // ά
 { 2 , 0xce, 0x92, 0 , 0 , 'B' }, // Β
 { 2 , 0xce, 0xb2, 0 , 0 , 'b' }, // β
  { 2 , 0xce, 0x95, 0 , 0 , 'E' }, // Ε
 { 2, 0xce, 0x88,0 , 0 , 'E' }, //  Έ
 { 2 , 0xce, 0xb5, 0 , 0 , 'e' }, // ε
 { 2, 0xce, 0xad,0 , 0 , 'e' }, //  έ 
 { 2 , 0xce, 0x97, 0 , 0 , 'H' }, // Η
 { 2, 0xce, 0x89,0 , 0 , 'H' }, //  Ή
 { 2 , 0xce, 0xb7, 0 , 0 , 'h' }, // η
 { 2, 0xce , 0xae , 0, 0 , 'h' }, // ή
  { 2 , 0xce, 0x96, 0 , 0 , 'Z' }, // Ζ
 { 2 , 0xce, 0xb6, 0 , 0 , 'z' }, // ζ
 { 2 , 0xce, 0x99, 0 , 0 , 'I' }, // Ι
 { 2, 0xce, 0x8a,0 , 0 , 'I' }, //  Ί
 { 2, 0xcf, 0x8a,0 , 0 , 'i' }, //  ϊ
 { 2, 0xce, 0x90,0 , 0 , 'i' }, //  ΐ
 { 2, 0xce, 0xaf,0 , 0 , 'i' }, //  ί
 { 2 , 0xc3, 0xaa, 0 , 0 , 'e' }, // ê
 { 2 , 0xce, 0x9a, 0 , 0 , 'K' }, // Κ
 { 2 , 0xce, 0xba, 0 , 0 , 'k' }, // κ
 { 2 , 0xce, 0x9c, 0 , 0 , 'M' }, // Μ
 { 2 , 0xce, 0xbc, 0 , 0 , 'm' }, // μ
 { 2 , 0xce, 0x9d, 0 , 0 , 'N' }, // Ν
 { 2 , 0xce, 0xbd, 0 , 0 , 'n' }, // ν
 { 2 , 0xce, 0xb9, 0 , 0 , 'i' }, // ι
 { 2 , 0xce, 0x9f, 0 , 0 , 'O' }, // Ο
 { 2, 0xce, 0x8c,0 , 0 , 'O' }, //  Ό
 { 2, 0xcf, 0x8c,0 , 0 , 'o' }, //  ό
 { 2 , 0xce, 0xbf, 0 , 0 , 'o' }, // ο
 { 2 , 0xce, 0xa1, 0 , 0 , 'P' }, // Ρ
 { 2 , 0xcf, 0x81, 0 , 0 , 'p' }, // ρ
 { 2 , 0xce, 0xa4, 0 , 0 , 'T' }, // Τ
 { 2 , 0xcf, 0x84, 0 , 0 , 't' }, // τ
 { 2 , 0xce, 0xa5, 0 , 0 , 'Y' }, // Υ
 { 2 , 0xcf, 0x85, 0 , 0 , 'y' }, // υ
 { 2 , 0xce, 0xa7, 0 , 0 , 'X' }, // Χ
 { 2 , 0xcf, 0x87, 0 , 0 , 'x' }, // χ
#ifdef GREEKTEXTCHECK
 // Greek special characters. In font start in 12x8 = 96
 { 2 , 0xce, 0x93, 0 , 0 , 'g' }, // Γ // 96
 { 2 , 0xce, 0xb3, 0 , 0 , 'v' }, // γ // 96
 { 2 , 0xce, 0x94, 0 , 0 , 'd' }, // Δ // 97
 { 2 , 0xce, 0xb4, 0 , 0 , 'd' }, // δ // 97
 { 2 , 0xce, 0x98, 0 , 0 , 'O' }, // Θ // 98
 { 2 , 0xce, 0xb8, 0 , 0 , 'O' }, // θ // 98
 { 2 , 0xce, 0x9b, 0 , 0 , 'A' }, // Λ // 99
 { 2 , 0xce, 0xbb, 0 , 0 , 'a' }, // λ // 99
 { 2 , 0xce, 0x9e, 0 , 0 , 'E' }, // Ξ // 100
 { 2 , 0xce, 0xbe, 0 , 0 , 'e' }, // ξ // 100
  { 2 , 0xce, 0xa0, 0 , 0 , 'M' }, // Π // 102
 { 2 , 0xcf, 0x80, 0 , 0 , 'p' }, // π // 102
 { 2 , 0xce, 0xa3, 0 , 0 , 'E' }, // Σ // 103
 { 2 , 0xcf, 0x83, 0 , 0 , 'o' }, // σ
 { 2 , 0xce, 0xa6, 0 , 0 , 'O' }, // Φ // 104
 { 2 , 0xcf, 0x86, 0 , 0 , 'O' }, // φ // 104
 { 2 , 0xce, 0xa8, 0 , 0 , 'W' }, // Ψ // 106
 { 2 , 0xcf, 0x88, 0 , 0 , 'w' }, // ψ // 106
 { 2 , 0xce, 0xa9, 0 , 0 , 'O' }, // Ω // 107
 { 2, 0xce, 0x8f,0 , 0 , 'O' }, //  Ώ // 107
 { 2, 0xcf , 0x82 , 0, 0 , 'c' }, // ς
 { 2, 0xcf, 0x8d,0 , 0 , 'u' }, //  ύ	
 { 2, 0xcf, 0x8b,0 , 0 , 'u' }, //  ϋ
 { 2 , 0xcf, 0x89, 0 , 0 , 'w' }, // ω
 { 2,  0xcf,  0x8e, 0, 0 , 'w' }, // ώ
#else
 // Greek special characters. In font start in 12x8 = 96 (and ascii code is +32)
 // There are 16 characters in font, I need ti manage the lower case.
 // Lower cases will be +100 and then back to 
 // so UPPERCASES : 128 to 144 (96+32)
 // Lowercases    : 228 to 244
 { 2 , 0xce, 0x93, 0 , 0 , 96+32 },     // Γ // 96 uppercase
 { 2 , 0xce, 0xb3, 0 , 0 , 96+32+100 }, // γ // 96 lowercase of Γ
 { 2 , 0xce, 0x94, 0 , 0 , 97+32 },     // Δ // 97 uppercase
 { 2 , 0xce, 0xb4, 0 , 0 , 97+32+100 }, // δ // 97 lowercase of Δ
 { 2 , 0xce, 0x98, 0 , 0 , 98+32 },     // Θ // 98 uppercase
 { 2 , 0xce, 0xb8, 0 , 0 , 98+32+100 }, // θ // 98 lowercase of Θ
 { 2 , 0xce, 0x9b, 0 , 0 , 99+32 },     // Λ // 99 uppercase
 { 2 , 0xce, 0xbb, 0 , 0 , 99+32+100 }, // λ // 99 lowercase of Λ
 { 2 , 0xce, 0x9e, 0 , 0 , 100+32 },    // Ξ // 100 uppercase
 { 2 , 0xce, 0xbe, 0 , 0 , 100+32+100 }, // ξ // 100 lowercase of Ξ
  { 2 , 0xce, 0xa0, 0 , 0, 102+32 },    // Π // 102 uppercase
 { 2 , 0xcf, 0x80, 0 , 0 , 102+32+100 }, // π // 102 lowercase of Π
 { 2 , 0xce, 0xa3, 0 , 0 , 103+32 },     // Σ // 103 uppercase
 { 2 , 0xcf, 0x83, 0 , 0 , 103+32+100 }, // σ // 103 lowercase of Σ 
 { 2 , 0xcf , 0x82 , 0, 0, 103+32+100 },// ς uppercase Σ
 { 2 , 0xce, 0xa6, 0 , 0 , 104+32 },     // Φ // 104 uppercase
 { 2 , 0xcf, 0x86, 0 , 0 , 104+32+100 }, // φ // 104 lowercase of Φ
 { 2 , 0xce, 0xa8, 0 , 0 , 106+32 },     // Ψ // 106 uppercase
 { 2 , 0xcf, 0x88, 0 , 0 , 106+32+100 }, // ψ // 106 lowercase of Ψ
 { 2 , 0xce, 0xa9, 0 , 0 , 101+32 },     // Ω // 107
 { 2 , 0xce, 0x8f,0 , 0 ,  101+32 },      // Ώ // 107
 { 2 , 0xcf, 0x89, 0 , 0 , 101+32+100 }, // ω lowercase of Ω
 { 2,  0xcf,  0x8e, 0, 0 , 101+32+100 }, // ώ lowercase of Ω
 { 2 , 0xcf, 0x8d,0 , 0 , 'y' },         //  ύ uppercase Υ	
 { 2 , 0xcf, 0x8b,0 , 0 , 'y' },         //  ϋ uppercase Υ


#ifndef POLISHTEXTCHECK
 // Polish

 // C3 93 Ó 136
 { 2 , 0xC4 ,0x84,0 , 0 , 128+32 },    //  Ą
 { 2 , 0xC4 ,0x86,0 , 0 , 129+32 },    //  Ć
 { 2 , 0xC4 ,0x98,0 , 0 , 130+32 },    //  Ę
 { 2 , 0xC5 ,0x81,0 , 0 , 131+32 },    //  Ł
 { 2 , 0xC5 ,0x83,0 , 0 , 132+32 },    //  Ń
#if LANGAGE==19 // Polish have it
 { 2 , 0xc3, 0x93,0,0, 133+32 }, // Ó
#endif
 { 2 , 0xC5 ,0x9A,0 , 0 , 134+32 },    //  Ś
 { 2 , 0xC5 ,0xB9,0 , 0 , 135+32 },    //  Ź
 { 2 , 0xC5 ,0xBB,0 , 0 , 136+32 },    //  Ż

 // C3 B3 ó 168
 { 2 , 0xC4 ,0x85,0 , 0 , 137+32 },    //  ą
 { 2 , 0xC4 ,0x87,0 , 0 , 138+32 },    //  ć
 { 2 , 0xC4 ,0x99,0 , 0 , 139+32 },    //  ę
 { 2 , 0xC5 ,0x82,0 , 0 , 140+32 },    //  ł
 { 2 , 0xC5 ,0x84,0 , 0 , 141+32 },    //  ń
#if LANGAGE==19 // Polish have it
 { 2,0xc3,0xb3,0,0, 142+32 }, // ó (MUST HAVE)
#endif
 { 2 , 0xC5 ,0x9B,0 , 0 , 143+32 },    //  ś
 { 2 , 0xC5 ,0xBA,0 , 0 , 144+32 },    //  ź
 { 2 , 0xC5 ,0xBC,0 , 0 , 145+32 },    //  ż
#else
 // C3 93 Ó 136
 { 2 , 0xC4 ,0x84,0 , 0 , 'A' },    //  Ą
 { 2 , 0xC4 ,0x86,0 , 0 , 'C' },    //  Ć
 { 2 , 0xC4 ,0x98,0 , 0 , 'E' },    //  Ę
 { 2 , 0xC5 ,0x81,0 , 0 , 'L' },    //  Ł
 { 2 , 0xC5 ,0x83,0 , 0 , 'N' },    //  Ń
#if LANGAGE==19 // Polish have it
 { 2 , 0xc3, 0x93,0,0, 'O' }, // Ó
#endif
 { 2 , 0xC5 ,0x9A,0 , 0 , 'S' },    //  Ś
 { 2 , 0xC5 ,0xB9,0 , 0 , 'Z' },    //  Ź
 { 2 , 0xC5 ,0xBB,0 , 0 , 'Z' },    //  Ż

 // C3 B3 ó 168
 { 2 , 0xC4 ,0x85,0 , 0 , 'a' },    //  ą
 { 2 , 0xC4 ,0x87,0 , 0 , 'c' },    //  ć
 { 2 , 0xC4 ,0x99,0 , 0 , 'e' },    //  ę
 { 2 , 0xC5 ,0x82,0 , 0 , 'l' },    //  ł
 { 2 , 0xC5 ,0x84,0 , 0 , 'n' },    //  ń
#if LANGAGE==19 // Polish have it
 { 2,0xc3,0xb3,0,0, 'o' }, // ó (MUST HAVE)
#endif
 { 2 , 0xC5 ,0x9B,0 , 0 , 's' },    //  ś
 { 2 , 0xC5 ,0xBA,0 , 0 , 'z' },    //  ź
 { 2 , 0xC5 ,0xBC,0 , 0 , 'z' },    //  ż
#endif

};
#endif

#else 
#define REPLACEMENTCHARS (109+16)
unsigned char gConversionTable[REPLACEMENTCHARS][6] =
{
     // SPECCY Version (different font layout, BOLD letters, no uppercases)
	// Spectrum font can handle all caracters, accents also (upperased accents)
	{ 2,0xc2,0xb0,0,0,'\'' }, //	° 
	// French
	{ 2,0xc3,0xa9,0,0, 166 }, //	é
	{ 2,0xc3,0xa8,0,0, 161 }, //	è
	{ 2,0xc3,0xa0,0,0, 160 }, //	à
	{ 2,0xc3,0xa2,0,0, 171 }, //	â
	{ 2,0xc3,0xaa,0,0, 172 }, //	ê
	{ 2,0xc3,0x88,0,0,'E' }, //	È
	{ 2,0xc3,0x8a,0,0,'E' }, //	Ê
	{ 3,0xe2,0x80,0x99,0,'\'' }, //	’
	{ 2,0xc3,0xb4,0,0, 174 }, //	ô 
	{ 2,0xc3,0xa7,0,0, 187 }, //	ç
	{ 2,0xc3,0xbb,0,0, 175 }, //	û
	{ 2,0xc3,0xb9,0,0, 164 }, //	ù
	{ 2,0xc3,0xaf,0,0, 181 }, //	ï
	{ 2,0xc3,0xae,0,0, 173 }, //	î
	{ 2,0xc5,0x8d,0,0,'o' }, //	ō
	{ 2,0xc3,0x87,0,0,'C' }, //	Ç
	// German
	{ 2,0xc3,0xa4,0,0, 179 }, //	ä (MUST HAVE)
	{ 2,0xc3,0xbc,0,0, 183 }, //	ü (MUST HAVE)
	{ 2,0xc3,0xb6,0,0, 182 }, // ö (MUST HAVE)
	{ 2,0xc3,0x9f,0,0, 159 }, // ß (MUST HAVE)
	{ 2,0xc3,0x96,0,0,'O' }, // Ö
	{ 2,0xc3,0x9c,0,0,'U' }, // Ü
	{ 2,0xc3,0x84,0,0,'A' }, // Ä
	// Italian
	{ 2,0xc3,0xb2,0,0, 163 }, //	ò (MUST HAVE)
	{ 2,0xc3,0xac,0,0, 162 }, //	ì (MUST HAVE)
	{ 2,0xc3,0x8c,0,0,'I' }, //	Ì 
	{ 2,0xc3,0x92,0,0,'O' }, // Ò 
	{ 2,0xc3,0x80,0,0,'A' }, // À 
	// Spanish
	{ 2,0xc3,0x93,0,0, 'O' }, // Ó
	{ 2,0xc3,0xb3,0,0, 168 }, // ó (MUST HAVE)
	{ 2,0xc2,0xbf,0,0, 156 },    // ¿ (MUST HAVE)
	{ 2,0xc3,0xad,0,0, 167 }, // í (MUST HAVE)
	{ 2,0xc3,0x8d,0,0, 'I' }, // Í
	{ 2,0xc2,0xa1,0,0, 157 }, // ¡ (MUST HAVE)
	{ 2,0xc3,0xb1,0,0, 177 }, // ñ (MUST HAVE)
	{ 2,0xc3,0xa1,0,0, 165 }, // á (MUST HAVE)
	{ 2,0xc3,0x9a,0,0, 'U' }, // Ú
	{ 2,0xc3,0xba,0,0, 169 }, // ú
	{ 2,0xc3,0x89,0,0, 'E' }, // É
	{ 2,0xc3,0x81,0,0, 'A' }, // Á
	{ 2,0xc3,0x91,0,0, 'N' }, // Ñ
	{ 3,0xe2,0x80,0x8b,0,' ' }, // Zero width space (can happend)
	// Greek
 { 2 , 0xce, 0x91, 0 , 0 , 'A' }, // Α 
 { 2, 0xce, 0x86,0 , 0 , 'A' }, //  Ά
 { 2 , 0xce, 0xb1, 0 , 0 , 'a' }, // α
 { 2, 0xce , 0xac , 0, 0 , 'a' }, // ά
 { 2 , 0xce, 0x92, 0 , 0 , 'B' }, // Β
 { 2 , 0xce, 0xb2, 0 , 0 , 'b' }, // β
  { 2 , 0xce, 0x95, 0 , 0 , 'E' }, // Ε
 { 2, 0xce, 0x88,0 , 0 , 'E' }, //  Έ
 { 2 , 0xce, 0xb5, 0 , 0 , 'e' }, // ε
 { 2, 0xce, 0xad,0 , 0 , 'e' }, //  έ 
 { 2 , 0xce, 0x97, 0 , 0 , 'H' }, // Η
 { 2, 0xce, 0x89,0 , 0 , 'H' }, //  Ή
 { 2 , 0xce, 0xb7, 0 , 0 , 'h' }, // η
 { 2, 0xce , 0xae , 0, 0 , 'h' }, // ή
  { 2 , 0xce, 0x96, 0 , 0 , 'Z' }, // Ζ
 { 2 , 0xce, 0xb6, 0 , 0 , 'z' }, // ζ
 { 2 , 0xce, 0x99, 0 , 0 , 'I' }, // Ι
 { 2, 0xce, 0x8a,0 , 0 , 'I' }, //  Ί
 { 2, 0xcf, 0x8a,0 , 0 , 'i' }, //  ϊ
 { 2, 0xce, 0x90,0 , 0 , 'i' }, //  ΐ
 { 2, 0xce, 0xaf,0 , 0 , 'i' }, //  ί
 { 2 , 0xc3, 0xaa, 0 , 0 , 'e' }, // ê
 { 2 , 0xce, 0x9a, 0 , 0 , 'K' }, // Κ
 { 2 , 0xce, 0xba, 0 , 0 , 'k' }, // κ
 { 2 , 0xce, 0x9c, 0 , 0 , 'M' }, // Μ
 { 2 , 0xce, 0xbc, 0 , 0 , 'm' }, // μ
 { 2 , 0xce, 0x9d, 0 , 0 , 'N' }, // Ν
 { 2 , 0xce, 0xbd, 0 , 0 , 'n' }, // ν
 { 2 , 0xce, 0xb9, 0 , 0 , 'i' }, // ι
 { 2 , 0xce, 0x9f, 0 , 0 , 'O' }, // Ο
 { 2, 0xce, 0x8c,0 , 0 , 'O' }, //  Ό
 { 2, 0xcf, 0x8c,0 , 0 , 'o' }, //  ό
 { 2 , 0xce, 0xbf, 0 , 0 , 'o' }, // ο
 { 2 , 0xce, 0xa1, 0 , 0 , 'P' }, // Ρ
 { 2 , 0xcf, 0x81, 0 , 0 , 'p' }, // ρ
 { 2 , 0xce, 0xa4, 0 , 0 , 'T' }, // Τ
 { 2 , 0xcf, 0x84, 0 , 0 , 't' }, // τ
 { 2 , 0xce, 0xa5, 0 , 0 , 'Y' }, // Υ
 { 2 , 0xcf, 0x85, 0 , 0 , 'y' }, // υ
 { 2 , 0xce, 0xa7, 0 , 0 , 'X' }, // Χ
 { 2 , 0xcf, 0x87, 0 , 0 , 'x' }, // χ
 // Greek special characters. In font start in 12x8 = 96 (and ascii code is +32)
 // There are 16 characters in font, I need ti manage the lower case.
 // Lower cases will be +100 and then back to 
 // so UPPERCASES : 128 to 144 (96+32)
 // Lowercases    : 228 to 244
 { 2 , 0xce, 0x93, 0 , 0 , 188 },    // Γ // 96 uppercase
 { 2 , 0xce, 0xb3, 0 , 0 , 198 },    // γ // 96 lowercase of Γ
 { 2 , 0xce, 0x94, 0 , 0 , 191 },    // Δ // 97 uppercase
 { 2 , 0xce, 0xb4, 0 , 0 , 201 },    // δ // 97 lowercase of Δ
 { 2 , 0xce, 0x98, 0 , 0 , 192 },    // Θ // 98 uppercase
 { 2 , 0xce, 0xb8, 0 , 0 , 202 },    // θ // 98 lowercase of Θ
 { 2 , 0xce, 0x9b, 0 , 0 , 194 },    // Λ // 99 uppercase
 { 2 , 0xce, 0xbb, 0 , 0 , 204 },    // λ // 99 lowercase of Λ
 { 2 , 0xce, 0x9e, 0 , 0 , 193 },    // Ξ // 100 uppercase
 { 2 , 0xce, 0xbe, 0 , 0 , 203 },    // ξ // 100 lowercase of Ξ
  { 2 , 0xce, 0xa0, 0 , 0, 189 },    // Π // 102 uppercase
 { 2 , 0xcf, 0x80, 0 , 0 , 199 },    // π // 102 lowercase of Π
 { 2 , 0xce, 0xa3, 0 , 0 , 197 },    // Σ // 103 uppercase
 { 2 , 0xcf, 0x83, 0 , 0 , 207 },    // σ // 103 lowercase of Σ 
 { 2 , 0xcf , 0x82 , 0, 0, 207 },    // ς uppercase Σ
 { 2 , 0xce, 0xa6, 0 , 0 , 196 },    // Φ // 104 uppercase
 { 2 , 0xcf, 0x86, 0 , 0 , 206 },    // φ // 104 lowercase of Φ
 { 2 , 0xce, 0xa8, 0 , 0 , 195 },    // Ψ // 106 uppercase
 { 2 , 0xcf, 0x88, 0 , 0 , 205 },    // ψ // 106 lowercase of Ψ
 { 2 , 0xce, 0xa9, 0 , 0 , 190 },    // Ω // 107
 { 2 , 0xce, 0x8f,0 , 0 ,  190 },    // Ώ // 107
 { 2 , 0xcf, 0x89, 0 , 0 , 200 },    // ω lowercase of Ω
 { 2,  0xcf,  0x8e, 0, 0 , 200 },    // ώ lowercase of Ω
 { 2 , 0xcf, 0x8d,0 , 0 , 'y' },     //  ύ uppercase Υ	
 { 2 , 0xcf, 0x8b,0 , 0 , 'y' },     //  ϋ uppercase Υ

 // Polish

 // C3 93 Ó 136
 { 2 , 0xC4 ,0x84,0 , 0 , 194 },    //  Ą
 { 2 , 0xC4 ,0x86,0 , 0 , 195 },    //  Ć
 { 2 , 0xC4 ,0x98,0 , 0 , 196 },    //  Ę
 { 2 , 0xC5 ,0x81,0 , 0 , 197 },    //  Ł
 { 2 , 0xC5 ,0x83,0 , 0 , 198 },    //  Ń
 { 2 , 0xC5 ,0x9A,0 , 0 , 199 },    //  Ś
 { 2 , 0xC5 ,0xB9,0 , 0 , 200 },    //  Ź
 { 2 , 0xC5 ,0xBB,0 , 0 , 201 },    //  Ż

 // C3 B3 ó 168
 { 2 , 0xC4 ,0x85,0 , 0 , 202 },    //  ą
 { 2 , 0xC4 ,0x87,0 , 0 , 203 },    //  ć
 { 2 , 0xC4 ,0x99,0 , 0 , 204 },    //  ę
 { 2 , 0xC5 ,0x82,0 , 0 , 205 },    //  ł
 { 2 , 0xC5 ,0x84,0 , 0 , 206 },    //  ń
 { 2 , 0xC5 ,0x9B,0 , 0 , 207 },    //  ś
 { 2 , 0xC5 ,0xBA,0 , 0 , 208 },    //  ź
 { 2 , 0xC5 ,0xBC,0 , 0 , 209 },    //  ż

};
#endif // end Speccy version


// ------------------------------------------------------
void ConvertToAsciiAndReplaceUnsupportedCharacters(string& inputstring )
{
	// that string have special UTF-8 characters. 
	// All need to be converted to ASCII only characters (0 to 255).
	// Any character above 128 is unicode and should be converted
	for (int i = 0; i < inputstring.size(); i++)
	{
		if ( ( (unsigned char) inputstring[i] ) >= 128)
		{
			// printf("UTF-8 char detected\n");
			unsigned char charvalues[4];
			charvalues[0] = inputstring[i];
			charvalues[1] = inputstring[i+1];
			if (i+2 < inputstring.size() ) charvalues[2] = inputstring[i+2]; else charvalues[2] = 0;
			if (i+3 < inputstring.size() ) charvalues[3] = inputstring[i+3]; else charvalues[3] = 0;
						
			bool found = false;
			int nbchartotest;
			unsigned char replacechar = 0;

			// Search character in table
			for (int j = 0; j < REPLACEMENTCHARS; j++)
			{
				nbchartotest = gConversionTable[j][0];

				if (nbchartotest == 2 && charvalues[0] == gConversionTable[j][1] && charvalues[1] == gConversionTable[j][2])
				{
					found = true;
					replacechar = gConversionTable[j][5];
					break;
				}
				else if (nbchartotest == 3 && charvalues[0] == gConversionTable[j][1] && charvalues[1] == gConversionTable[j][2]
					&& charvalues[2] == gConversionTable[j][3] )
				{
					found = true;
					replacechar = gConversionTable[j][5];
					break;
				}
				else if (nbchartotest == 4 && charvalues[0] == gConversionTable[j][1] && charvalues[1] == gConversionTable[j][2]
					&& charvalues[2] == gConversionTable[j][3] && charvalues[3] == gConversionTable[j][4] )
				{
					found = true;
					replacechar = gConversionTable[j][5];
					break;
				}
			} // Parse all replace char, something must be found !!

			if (!found)
			{
				printf("Error, one character ( %02x %02x %02x %02x ) is not found in replace table in string %s\n", charvalues[0],charvalues[1],charvalues[2],charvalues[3],inputstring.substr(i).c_str());
				gError = true;
			}
			else
			{
				// replace 2 to 4 bytes by one.
				// Delete bytes nbchartotest-1
				inputstring.erase(i, nbchartotest - 1);
				inputstring[i] = replacechar;
			}

		}
	}

#ifdef SPECCY
	// Last for Spectrum, replace double dot with [
	while (inputstring.find("..", 0) != string::npos)
	{
		int temppos;
		temppos = inputstring.find("..", 0);
		inputstring.erase(temppos, 1);
		inputstring[temppos] = '[';
	};
#endif

}


// ------------------------------------------------------
// Set gError is total result is too long
void removespaceatend(string& result)
{
	// -- Remove spaces at end. (and check size)
	bool error = false;
	while (result.size() > BANNERSIZE && !error)
	{
		if (result[result.size()-1] == ' ')
			result.pop_back(); // Remove last char
		else
		{
			//printf("Error, string is too long\n");
			error = true;
			gError = true;
		}
	}
}

// ------------------------------------------------------
// For debug or text adjust
void DisplayText(string& result)
{
		string resultWithoutAltFont;
		for (int i = 0; i < result.size(); i++)
		{
			if (result[i] != '\1' && result[i] != '\2')
				resultWithoutAltFont.push_back(result[i]);
		}
		int excesschars = resultWithoutAltFont.size() - BANNERSIZE;
		printf("Line %d. ", gGlobalLevelLine);
		if (excesschars>0)
			printf("Excess chars (%d)", excesschars );
		printf("\n");
		printf("   /------------------------------\\ \n");
		printf("  /"); cout << resultWithoutAltFont.substr(0, 32); printf("\\ \n");
		printf(" /"); cout << resultWithoutAltFont.substr(32, 34); printf("\\ \n");
		printf("|"); cout << resultWithoutAltFont.substr(32+34, 36); printf("| \n");
		printf(" \\"); cout << resultWithoutAltFont.substr(32+34+36, 34); printf("/ \n");
		printf("  \\"); cout << resultWithoutAltFont.substr(32+34+36+34, 32); printf("/ \n");
		printf("   \\------------------------------/ ");
		if (excesschars>0)
		{
			printf("Missing: "); cout << resultWithoutAltFont.substr(32+34+36+34+32, resultWithoutAltFont.size()-(32+34+36+34+32) );
		}
		printf("\n");
		//:% s\n", result.size(), result.c_str() );
}

// ------------------------------------------------------
// Name : CheckIfTextCanFitWihoutWordCut
// Index 0 if nothing found.
bool CheckIfTextCanFitWihoutWordCut(string& inputstring)
{
	string result;
	result = inputstring;
	gError = false;
	InjectSpaces(result, 0          , 32         , false );
	InjectSpaces(result, 32         , 32+34      , false );
	InjectSpaces(result, 32+34      , 32+34+36   , false );
	InjectSpaces(result, 32+34+36   , 32+34+36+34, false );
	InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
	removespaceatend(result);
	return !gError;
}

// ------------------------------------------------------
// Center (0,32) for first line of 32 charcters
// indexstart included
// indexend expluded
void Center(string& inputstring, int indexstart, int indexend)
{
	// This is not done here for space at start.
	// If the whole line have no real displayed characters, then no need to center.
//	bool allblank = true;
//	for (int i=indexstart; i<indexend;i++)
//	{
//		if (inputstring[i] != ' ' && inputstring[i] != '\n')
//			allblank = false;
//	}
//	if (allblank)
//		return;
	
	// -- Centering.

	//if (inputstring.find("Boo hoo", indexstart) != std::string::npos)
	//	printf("youhou");

	// Count spaces at start
    int nbspacestart = 0;
    int currentindex = indexstart;
    while (inputstring[currentindex] == ' ' && currentindex <= indexend)
    {
        currentindex++;
        nbspacestart++;
    }

    // Count how much space at end.
    int nbspaceend = 0;
    currentindex = indexend - 1;
    while (inputstring[currentindex] == ' ' && currentindex > indexstart)
    {
        currentindex--;
        nbspaceend++;
    }

	// Case where there is one character at start
#ifdef SPECCY
		if (nbspacestart==1 && nbspaceend==0)
		{
			inputstring[indexstart] = 124;
		}
#endif

    if ( (nbspaceend-nbspacestart) >= 1)
    {
		bool isodd=false;
        int nbspacetoinsert = abs(nbspaceend-nbspacestart) / 2;
		if ((nbspaceend + nbspacestart) % 2 == 1)
		{
			isodd = true;
		}

        // Remove space at end
        inputstring.erase(indexend - 1 - nbspacetoinsert, nbspacetoinsert);
        // Add space at beginning
        inputstring.insert(indexstart, nbspacetoinsert, ' ');
#ifdef SPECCY
		if (isodd)
		{   // If number of character is odd then add a control character at end of line : 124
			// Remove a character from end and add it at start
			inputstring.erase(indexend - 1, 1);
			// Add space at beginning
			inputstring.insert(indexstart, 1, ' ');			
			inputstring[indexstart] = 124;
		}
#endif
    }

}

// ------------------------------------------------------
// ReplaceControlCharacters
// return 1 if line feed have been done
// ------------------------------------------------------
bool ReplaceControlCharacters(string& inputstring, int indexstart, int indexend)
{
	bool linefeeddone = false;
	// -- If we find \n, then complete with space until end of line.
	// If that line break is at end of line, then do not jump a new line.
	// Or at beginning of a line.
	int indexlinejump;
	indexlinejump = inputstring.find("\\n", indexstart);
//	if (indexlinejump==indexend) // If line break is exactly at end of line then do not do nothing
//	{
//		inputstring.erase(indexlinejump, 2); // Remove 2 chars.
//	}
	if (indexlinejump>=indexstart && indexlinejump <indexend )
	{
		// Remove 2 chars.
		inputstring.erase(indexlinejump, 2);
		// Add space until end
		inputstring.insert(indexlinejump, indexend-indexlinejump, ' ');
		linefeeddone = true;
	}

	// -- Centering.
	// If find \c, then count how much blank characters at end, and add half of them at beginning and remove half at end.
	int indexcentering;
	indexcentering = inputstring.find("\\c", indexstart);
	if (indexcentering>=indexstart && indexcentering <indexend )
	{
		// Remove 2 chars.
		inputstring.erase(indexcentering, 2);
		// Count how much space at end.
		int nbspace = 0;
		int currentindex = indexend-1;
		while ( inputstring[currentindex]==' ' &&  currentindex > indexstart )
		{
			currentindex--;
			nbspace++;
		}
		if (nbspace>1)
		{
			int nbspacetoinsert = nbspace / 2;
			// Remove space at end
			inputstring.erase(indexend-1-nbspacetoinsert, nbspacetoinsert);
			// Add space at beginning
			inputstring.insert(indexstart, nbspacetoinsert, ' ');
		}
	}
	return linefeeddone;
}

// ------------------------------------------------------
void ReplaceControlCharactersForString(string& result)
{
	ReplaceControlCharacters(result, 0          , 32            );
	ReplaceControlCharacters(result, 32         , 32+34         );
	ReplaceControlCharacters(result, 32+34      , 32+34+36      );
	ReplaceControlCharacters(result, 32+34+36   , 32+34+36+34   );
	ReplaceControlCharacters(result, 32+34+36+34, 32+34+36+34+32);
}

// ------------------------------------------------------
// Convert to u8 32 34 36 34 32 formatted string.
// This is main function.
string ConvertString(string inputstring)
{
	string result;
	result = inputstring;

	// -- First replace all special characters. We need ASCII characters only, not UTF-8
	ConvertToAsciiAndReplaceUnsupportedCharacters(result);

	// -- If string is too short, then add spaces at end
	if (result.size() < BANNERSIZE + 32)
	{
		result.insert(result.size(), (BANNERSIZE+32) - result.size(), ' ');
	}

	// -- For french, replace all space before "!" and "?" with an usued character "_". It will be removed at end
	for (int i = 1 ; i< result.size() ; i++ )
	{
		if (result[i - 1] == ' ' && result[i] == '!')
			result[i - 1] = '_';
		if (result[i - 1] == ' ' && result[i] == '?')
			result[i - 1] = '_';
	}

	/*
	// -- Replace all control characters. Like \n to jump a line.
	ReplaceControlCharactersForString(result);

	// -- Try to add some lin jump if possible.
	// Find sentence, end, add line jump and check if fit the string (without word cut).
	// Start by end.
	int currentposition = result.size();
	vector<int> resultsindex;
	resultsindex.empty();
	string result1 = result;
	while (currentposition!=0)
	{
		currentposition = FindPreviousSentenceEnd(result1, currentposition);
		if (currentposition!=0)
		{
			// Insert line jump
			// Insert two chars \n, that will be interpretted as line jump

			result1.insert(currentposition+1, 1,'n');
			result1.insert(currentposition+1, 1,'\\');
			ReplaceControlCharactersForString(result1);
			bool canfit = CheckIfTextCanFitWihoutWordCut(result1);
			if (canfit)
			{
				resultsindex.push_back(currentposition); // Store for later.
				//printf("Succefuly inserted line jump at position %d. .\n", currentposition);
			}
		}
	}
	// If some line insert where found, then add them.
	for (int i = 0 ; i < resultsindex.size() ; i++)
	{
			result.insert(resultsindex[i]+1, 1,'n');
			result.insert(resultsindex[i]+1, 1,'\\');
			// Remove space after
			while (result [ resultsindex[i] + 3 ] == ' ')
				result.erase(resultsindex[i] + 3, 1);
	}

	// -- Replace all control characters. Like \n to jump a line.
	ReplaceControlCharactersForString(result);
	*/

	string resultbeforeinject = result;

#ifdef AUTOLINEFEEDALLOWED
	// -- First try by adding auto line feed (and without cutting words)
	// This is fine for messages with lot of space
	gError = false;
	InjectSpaces(result, 0          , 32         , false, true );
	InjectSpaces(result, 32         , 32+34      , false, true );
	InjectSpaces(result, 32+34      , 32+34+36   , false, true );
	InjectSpaces(result, 32+34+36   , 32+34+36+34, false, true );
	InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
	removespaceatend(result);
	if (gError)
	{
		result = resultbeforeinject;
		gError = false;
		InjectSpaces(result, 0          , 32         , false, false );
		InjectSpaces(result, 32         , 32+34      , false, true );
		InjectSpaces(result, 32+34      , 32+34+36   , false, true );
		InjectSpaces(result, 32+34+36   , 32+34+36+34, false, true );
		InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
		removespaceatend(result);
		if (gError)
		{
			result = resultbeforeinject;
			gError = false;
			InjectSpaces(result, 0          , 32         , false, false );
			InjectSpaces(result, 32         , 32+34      , false, false );
			InjectSpaces(result, 32+34      , 32+34+36   , false, true );
			InjectSpaces(result, 32+34+36   , 32+34+36+34, false, true );
			InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
			removespaceatend(result);
			if (gError)
			{
				result = resultbeforeinject;
				gError = false;
				InjectSpaces(result, 0          , 32         , false, false );
				InjectSpaces(result, 32         , 32+34      , false, false );
				InjectSpaces(result, 32+34      , 32+34+36   , false, false );
				InjectSpaces(result, 32+34+36   , 32+34+36+34, false, true );
				InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
				removespaceatend(result);	
			}
		}
	}
#endif

	// -- Then try without cutting words.
#ifdef AUTOLINEFEEDALLOWED
	if (gError)
#endif
	{
		result = resultbeforeinject;
		gError = false;
		InjectSpaces(result, 0          , 32         , false );
		InjectSpaces(result, 32         , 32+34      , false );
		InjectSpaces(result, 32+34      , 32+34+36   , false );
		InjectSpaces(result, 32+34+36   , 32+34+36+34, false );
		InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
		removespaceatend(result);
	}

	// -- Then If error, then try to cut word if cut is allowed.
#ifdef CUTWORDALLOWED
	if (gError)
	{
#ifndef DEBUGONLYDISPLAYERRORSTEXT
#ifndef DISPLAYALLTEXTS
		printf("Warning. Tool have not been able to fit text without cutting words.\n");
		DisplayText(result);
		printf("Will now try to cut words line by line\n");
#endif
#endif
		result = resultbeforeinject;
		gError = false;
		InjectSpaces(result, 0          , 32         , true );
		InjectSpaces(result, 32         , 32+34      , false );
		InjectSpaces(result, 32+34      , 32+34+36   , false );
		InjectSpaces(result, 32+34+36   , 32+34+36+34, false );
		InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
		removespaceatend(result);

		if (gError)
		{
			result = resultbeforeinject;
			gError = false;
			InjectSpaces(result, 0          , 32         , true );
			InjectSpaces(result, 32         , 32+34      , true );
			InjectSpaces(result, 32+34      , 32+34+36   , false );
			InjectSpaces(result, 32+34+36   , 32+34+36+34, false );
			InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
			removespaceatend(result);

			if (gError)
			{
				result = resultbeforeinject;
				gError = false;
				InjectSpaces(result, 0          , 32         , true );
				InjectSpaces(result, 32         , 32+34      , true );
				InjectSpaces(result, 32+34      , 32+34+36   , true );
				InjectSpaces(result, 32+34+36   , 32+34+36+34, false );
				InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
				removespaceatend(result);

				if (gError)
				{
					result = resultbeforeinject;
					gError = false;
					InjectSpaces(result, 0          , 32         , true );
					InjectSpaces(result, 32         , 32+34      , true );
					InjectSpaces(result, 32+34      , 32+34+36   , true );
					InjectSpaces(result, 32+34+36   , 32+34+36+34, true );
					InjectSpaces(result, 32+34+36+34, 32+34+36+34+32);
					removespaceatend(result);
				}

			}

		}

#ifndef DEBUGONLYDISPLAYERRORSTEXT
#ifndef DISPLAYALLTEXTS
		if (!gError)
		{
			printf("The cut tool have found that solution:\n");
			DisplayText(result);
		}
#endif
#endif
	}
#endif

	// -- For french, replace all space before "!" and "?" with an usued character "_". It will be removed at end
	for (int i = 0 ; i< result.size() ; i++ )
	{
		if (result[i] == '_')
			result[i] = ' ';
	}

	// -- Center
#ifdef CENTERALL
	Center(result, 0          , 32            );
	Center(result, 32         , 32+34         );
	Center(result, 32+34      , 32+34+36      );
	Center(result, 32+34+36   , 32+34+36+34   );
	Center(result, 32+34+36+34, 32+34+36+34+32);
#endif

	// -- Detect uppercases words and add control caracters \2 \1.
	SetAlternateFont(result);

	// -- Convert to existing font charaters
	ConvertToExistingCharacters(result);

	if ( gError )
	{
#ifdef CUTWORDALLOWED
		printf("ERROR. No solution have been found to fit that text into GAME SCREEN.\n");
#else
		printf("ERROR. No solution have been found to fit that text into GAME SCREEN. (Note that CUT WORD is not allowed)\n");
#endif
		DisplayText(result);
	}
#ifdef DISPLAYALLTEXTS
	else
	{
		DisplayText(result);
	}
#endif

	gError = false;

	return result;
}

#ifdef SPECCY
int SaveSpectrumLine(string& InputString, int start, int numberstrings, FILE* file)
{
	int currentpos = start;
	bool shouldForceUppercase = false;
	int currentcharprocessed = 0;

	if (numberstrings==32)
	 fprintf(file, "DEFB   ");
	else if (numberstrings==34)
	 fprintf(file, "DEFB  ");
	else
	 fprintf(file, "DEFB ");

	while (currentcharprocessed != numberstrings)
	{
		if (InputString[currentpos] == 0x02)
			shouldForceUppercase = true;
		else if (InputString[currentpos] == 0x01)
			shouldForceUppercase = false;
		else
		{
			unsigned char currentchar = InputString[currentpos];
			if (shouldForceUppercase)
				currentchar = touppercase(currentchar);
			else
				currentchar = tolowercase(currentchar); // Speccy font do not have lowercases, only non bold and bold.

			// For all characters under 128, write ascii code, else write binary code
			if (currentchar<128 && currentchar!='"')
			{
				if (currentcharprocessed==0) // for being of line add a "
					fprintf(file, "\"");

				fprintf(file, "%c",currentchar);
			}
			else
			{
				// Write binary code.
				if (currentcharprocessed==0) // if not First caracter, add ",
					fprintf(file, "\"\",");
				else
					fprintf(file, "\",");

				fprintf(file, "%d,\"",currentchar);
			}

			currentcharprocessed++;
		}

		currentpos++;
	}

	fprintf(file, "\"\n");

	return currentpos;
}

void SaveSpectrumString(string& InputString, FILE* file)
{
	// String is exactly the correct length 32 34 36 34 32
	int currentpos = 0;
	currentpos = SaveSpectrumLine(InputString, currentpos, 32,file);
	currentpos = SaveSpectrumLine(InputString, currentpos, 34,file);
	currentpos = SaveSpectrumLine(InputString, currentpos, 36,file);
	currentpos = SaveSpectrumLine(InputString, currentpos, 34,file);
	currentpos = SaveSpectrumLine(InputString, currentpos, 32,file);

	if (currentpos != InputString.size())
		fprintf(file, ";ERROR, All chars should be process and fit the banner size. (processed chars %d, total in char %d)\n",currentpos,InputString.size());
	// End line, space
	//fprintf(file,"\n");
}
#endif


// ------------------------------------------------------
void InjectTexts(char* levelnumber)
{
	// Load binary level
	char levelname[128];
	char levelnamewithinputpath[128];
	sprintf(levelname,              "LEVEL%s.BIN", levelnumber);
#ifdef C64
	sprintf(levelnamewithinputpath, "REFBINC64\\LEVEL%s.BIN", levelnumber);
#else
	sprintf(levelnamewithinputpath, "REFBIN\\LEVEL%s.BIN", levelnumber);
#endif
	printf("-- Processing : %s\n", levelname);


	// Inject all texts to binary level.
	
	// We need u8 chars, that respect the format. 32 34 38 34 32 chars. (5 lines)
	gGlobalLevelLine = gGlobalLevelStartNumber +1 ; // Skip the "level" line

	gStringIntro = ConvertString(gStringIntro);

	gGlobalLevelLine++;

	for (int i = 0 ; i<gStringTexts.size(); i++)
	{
		gStringTexts[i] = ConvertString(gStringTexts[i]);
		gGlobalLevelLine++;
	}

	for (int i = 0 ; i<gStringDeaths.size(); i++)
	{
		gStringDeaths[i] = ConvertString(gStringDeaths[i]);
		gGlobalLevelLine++;
	}


	FILE* file = fopen(levelnamewithinputpath, "rb");
	if (file)
	{
		gLevelInputSize = fread(&gLevelInput, 1, 10240, file);
		fclose(file);
	}
	else
	{
		return; // Bin is not here, so skip
	}
	// Convert pointers to offset for data block
	// So remove 0x9c00 + 140
#ifdef CPC
	for (int i = 0; i < NBPOINTER; i++)
	{
		if (gLevelInput.pointers[i] != 0)
			gLevelInput.pointers[i] -= (STARTMEMORYCPC + NBPOINTER * 2);
	}
#endif
#ifdef C64
	for (int i = 0; i < NBPOINTER; i++)
	{
		if (gLevelInput.pointers[i] != 0)
			gLevelInput.pointers[i] -= (NBPOINTER * 2);
	}
#endif
#ifdef SPECCY
	for (int i = 0; i < NBPOINTER; i++)
	{
		if (gLevelInput.pointers[i] != 0)
			gLevelInput.pointers[i] -= (STARTMEMORYCPC + NBPOINTER * 2);
	}
#endif

	// -- Copy data to output.
	gLevelOutputSize = 0; // Without 140 pointer, just data size first
	// The 4 first pointers are the same
	for (int i = 0; i < 4; i++)
		gLevelOutput.pointers[i] = gLevelInput.pointers[i];
	// Size of data for 4 first pointers
	int size = gLevelInput.pointers[4];
	memcpy(gLevelOutput.data, gLevelInput.data, size);
	gLevelOutputSize += size;
#ifdef C64
	// Next level name should be lower case.
	char* nameptr = (char*)(gLevelOutput.pointers[1] + gLevelOutput.data);
	nameptr[7] = '.'; // Add . to have .bin
	//_strlwr(nameptr); // No need to convert to lowercase

	// For demo, replace 41 with z1
    #ifdef DEMO
	if (nameptr[5] == '4' && nameptr[6] == '1')
		nameptr[5] ='Z';
    #endif


#endif

	// Copy intro
	gLevelOutput.pointers[4] = gLevelOutputSize; // Intro text
	memcpy(&(gLevelOutput.data[gLevelOutputSize]), gStringIntro.c_str(), gStringIntro.size());
	gLevelOutputSize += gStringIntro.size();
	// -- All texts strings
	for (int i = 0; i < 32; i++)
	{
		gLevelOutput.pointers[5 + i] = 0;
		if (gLevelInput.pointers[5+i] != 0 )
		{
			gLevelOutput.pointers[5+i] = gLevelOutputSize; // text
			memcpy(&(gLevelOutput.data[gLevelOutputSize]), gStringTexts[i].c_str(), gStringTexts[i].size());
			gLevelOutputSize += gStringTexts[i].size();
		}
		else
		{
			// Check that we do not have left string.
			if (gStringTexts.size() > i)
				printf("This text string is not in original level ! (%d th string) \n", i);
		}
	}
	// -- All Death  strings
	for (int i = 0; i < 32; i++)
	{
		gLevelOutput.pointers[5 + 32 + i] = 0;

		if (gLevelInput.pointers[5+32 + i] != 0 )
		{
			if (i >= gStringDeaths.size() ) // Dummy death. Input have "none" but we do not have it in translation.
			{
				gLevelOutput.pointers[5+32+i] = gLevelOutputSize; // gStringDeaths text
				memcpy(&(gLevelOutput.data[gLevelOutputSize]), "NONE", 4);
				gLevelOutputSize += 4;
			}
			else
			{
				gLevelOutput.pointers[5+32+i] = gLevelOutputSize; // gStringDeaths text
				memcpy(&(gLevelOutput.data[gLevelOutputSize]), gStringDeaths[i].c_str(), gStringDeaths[i].size());
				gLevelOutputSize += gStringDeaths[i].size();
			}
		}
		else
		{
			// Check that we do not have left string.
			if (gStringDeaths.size() > i)
				printf("This death string is not in original level ! (%d th string) \n", i);
		}
	}
	// Final pointer is art data
	gLevelOutput.pointers[5 + 32 + 32] = gLevelOutputSize;
#ifdef CPC
	int sizegfx = gLevelInputSize - gLevelInput.pointers[5 + 32 + 32];
	memcpy(&(gLevelOutput.data[gLevelOutputSize]), gLevelInput.data + gLevelInput.pointers[5 + 32 + 32] , sizegfx);
	gLevelOutputSize += sizegfx;
#endif
#ifdef C64
	// C64 arts are inside PICC64/pic11.bin
	char picname[128];
	char picbuffer[8000];
	sprintf(picname, "PICC64\\pic%s.bin", levelnumber);
	FILE* filepic = fopen(picname, "rb");
	if (filepic)
	{
		int sizegfx = fread(picbuffer, 1, 8000, filepic);
		fclose(filepic);
		memcpy(&(gLevelOutput.data[gLevelOutputSize]), picbuffer , sizegfx);
		gLevelOutputSize += sizegfx;
	}
	else
	{
		printf("Error. C64 pic file is missing. %s\n", picname);
	}
#endif

	// Add memory offset to pointers
	// So add 0x9c00 + 140
#ifdef CPC
	for (int i = 0; i < NBPOINTER; i++)
	{
		if (gLevelOutput.pointers[i] != 0 || i==0)
			gLevelOutput.pointers[i] += (STARTMEMORYCPC + NBPOINTER * 2);
	}
#endif
#ifdef C64
	for (int i = 0; i < NBPOINTER; i++)
	{
		if (gLevelOutput.pointers[i] != 0 || i==0)
			gLevelOutput.pointers[i] += (NBPOINTER * 2);
	}
#endif
#ifdef SPECCY
	for (int i = 0; i < NBPOINTER; i++)
	{
		if (gLevelOutput.pointers[i] != 0 || i==0)
			gLevelOutput.pointers[i] += (NBPOINTER * 2);
	}
#endif
	// Save file
	file = fopen(levelname, "wb");
#ifdef C64
	int dummy = 0;
	fwrite(&dummy, 1, 2, file); // Write 2 dummy bytes
#endif
	if (file)
	{
		fwrite(&gLevelOutput, 1, ((6+32+32)*2)+gLevelOutputSize, file);
		fclose(file);
	}

//int gSpeccyCurrentTextNumber=0; // up to SPECCY_NUMTEXTSPERPARTS text in one part
//int gSpecctCurrentPart=0; // 0 1 
//int gSpeccyCurrentSide=0; // 0 1
//FILE* gSpeccyIntroFile = NULL;
//FILE* gSpeccyTextFile = NULL;
//FILE* gSpeccyTimeUpFile = NULL;

#ifdef SPECCY
	// Save intro
	if (gSpeccyIntroFile == NULL)
		gSpeccyIntroFile = fopen("intros.bas", "w");
	fprintf(gSpeccyIntroFile,"; -- Processing : %s\n", levelname);
	fprintf(gSpeccyIntroFile,"; %d\n", gGlobalLevelStartNumber +1);
	SaveSpectrumString(gStringIntro, gSpeccyIntroFile);
	// Check if need to open of change text file
	if (gSpeccyTextFile==NULL)
		gSpeccyTextFile = fopen("nctext-sidea-part1.bas", "w");
	if (strcmp(levelnumber,"51")==0)
	{
		fclose(gSpeccyTextFile);
		gSpeccyTextFile = fopen("nctext-sideb-part1.bas", "w");
		gSpeccyCurrentTextNumber = 0;
		gSpeccyCurrentSide = 1;
	}
	// Save texts
	SpeccycheckOpenOrChangeFile();
	fprintf(gSpeccyTextFile,"; -- Processing : %s\n", levelname);
	// Success texts
	fprintf(gSpeccyTextFile,";    Sucess Texts : %d\n", gStringTexts.size() );
	for (int i = 0; i < gStringTexts.size(); i++)
	{
		fprintf(gSpeccyTextFile,"; %d\n", gSpeccyCurrentGlobalTextNumber);
		gSpeccyCurrentGlobalTextNumber++;
		SaveSpectrumString(gStringTexts[i],gSpeccyTextFile);
		gSpeccyCurrentTextNumber++;
		SpeccycheckOpenOrChangeFile();
	}

	// Death texts
	fprintf(gSpeccyTextFile,";    Deaths Texts : %d\n", gStringDeaths.size() );
	for (int i = 0; i < gStringDeaths.size(); i++)
	{
		fprintf(gSpeccyTextFile,"; %d\n", gSpeccyCurrentGlobalTextNumber);
		gSpeccyCurrentGlobalTextNumber++;
		SaveSpectrumString(gStringDeaths[i],gSpeccyTextFile);
		gSpeccyCurrentTextNumber++;
		SpeccycheckOpenOrChangeFile();
	}

#endif

// Obsolete
#ifdef DUMPSPECTRUMTEXTS
	// TODO Dump level datas
	fprintf(SpectrumFile,"; -- Processing : %s\n", levelname);
	// Ok texts
	fprintf(SpectrumFile,";    Sucess Texts : %d\n", gStringTexts.size() );
	for (int i = 0; i < gStringTexts.size(); i++)
		SaveSpectrumString(gStringTexts[i]);

	// Death texts
	fprintf(SpectrumFile,";    Deaths Texts : %d\n", gStringDeaths.size() );
	for (int i = 0; i < gStringDeaths.size(); i++)
		SaveSpectrumString(gStringDeaths[i]);
#endif

}

// ------------------------------------------------------
// gSpeccyCurrentTextNumber
// Change file to part 2 when max number of text is reached
void SpeccycheckOpenOrChangeFile()
{
	if (gSpeccyCurrentTextNumber>SPECCY_NUMTEXTSPERPARTS)
	{
		fclose(gSpeccyTextFile);
		if (gSpeccyCurrentSide == 0)
			gSpeccyTextFile = fopen("nctext-sidea-part2.bas", "w");
		else
			gSpeccyTextFile = fopen("nctext-sideb-part2.bas", "w");
		gSpeccyCurrentTextNumber = 0;
	}
}

// ------------------------------------------------------
void SaveTimeUp()
{
	if (gStringTimeUpDeaths.size() != 7)
	{
		printf("ERROR, the time up and stat string should make a total of 7, not %d\n", gStringTimeUpDeaths.size());
		return;
	}

	int stringsize[7];
	for (int i = 0; i < 7; i++)
		stringsize[i] = gStringTimeUpDeaths[i].size();

	unsigned char buffer[512 * 3];
	// Write 7 pointers then all datas. Zero terminated
	unsigned short* ptr = (unsigned short*) buffer;
	ptr[0] = 0;
	for (int i = 1; i < 7; i++)
		ptr[i] = ptr[i - 1] + stringsize[i - 1] + 1;
	unsigned char* ptrchar = buffer + (7 * 2);
	for (int i = 0; i < 7; i++)
	{
		memcpy(ptrchar, gStringTimeUpDeaths[i].c_str(), gStringTimeUpDeaths[i].size());
		ptrchar[gStringTimeUpDeaths[i].size()] = 0;
		ptrchar += (gStringTimeUpDeaths[i].size() + 1);
	}
	// Save file
	FILE* file = fopen("TIMEUP.BIN", "wb");
#ifdef C64
	int dummy = 0;
	fwrite(&dummy, 1, 2, file); // Write 2 dummy bytes
#endif
	if (file)
	{
		fwrite(&buffer, 1, 512*3, file);
		fclose(file);
	}

#ifdef DUMPSPECTRUMTEXTS
	// TODO Dump level datas
	fprintf(SpectrumFile,"; -- Processing : TimeUp Deaths (and stats final string)\n");
	for (int i = 0; i < gStringTimeUpDeaths.size(); i++)
		SaveSpectrumString(gStringTimeUpDeaths[i]);
#endif
#ifdef SPECCY
	// Save intro
	if (gSpeccyTimeUpFile == NULL)
		gSpeccyTimeUpFile = fopen("nctext-generic.bas", "w");
	fprintf(gSpeccyTimeUpFile,"; -- Processing : TimeUp Deaths (and stats final string)\n");
	for (int i = 0; i < gStringTimeUpDeaths.size(); i++)
	{
		fprintf(gSpeccyTimeUpFile,"; %d\n", gSpeccyCurrentGlobalTextNumber);
		gSpeccyCurrentGlobalTextNumber++;
		SaveSpectrumString(gStringTimeUpDeaths[i],gSpeccyTimeUpFile);
	}
#endif

}

// ------------------------------------------------------
int main(int argc, char** argv)
{
	std::cout << "Starting!\n";
	//printf("Result (xxx) : --------------------------------**********************************------------------------------------**********************************--------------------------------\n");
	FILE* file;

	// Insert text in 2 or 3 binary files.

	if (argc == 1)
	{
		printf("Usage : Exe alltexts.tsv\n");
		exit(0);
	}

#ifdef  DUMPSPECTRUMTEXTS
	SpectrumFile = fopen("Spectrum.txt", "w");
#endif

	char* tsvname;
	tsvname = argv[1];

	//file = fopen(tsvname, "r");
	//if (!file)
	//{
	//	printf("Can not read file %s\n", tsvname);
	//	exit(0);
	//}

	//wifstream dataFilew; // Wide char
	//wdataFile.open(tsvname);

    // Set console code page to UTF-8 so console known how to interpret string data
    SetConsoleOutputCP(CP_UTF8);

	ifstream dataFile; // Wide char
	dataFile.open(tsvname);

	bool IsIntro = false;
	bool IsTexts = false; 
	bool IsDeaths = false;
	bool IsTimeUp = false;
	bool ignoreline = false;
	char levelnumber[512];
	levelnumber[0] = 0;

	// -- Parse the whole file.

	gGlobalLineNumber = 1;

	while(!dataFile.eof()) 
	{
		//-- Get a line
		char buf[8000]; 
		dataFile.getline( (char *) buf, 8000); 
		string stringline(buf); // Full line UTF-8
		// Create a stream with line.
		istringstream iline(stringline);
		int column = 1;
		while (iline.getline((char*)buf, 8000, 0x09)) // Tab delimiter
		{
			string stringcolomn(buf); // Full line UTF-8
			//cout << stringcolomn << endl; // If console well setup, it can display UTF-8

			// Text which case we are dealing with
			if (column==1)
			{
				// Column 1 can handle level number and tags.
				// Tags are intro, deaths, end
				// Special case for "timeupdeath" also
				// And any line can have alternative version for a specific platform.

				if (stringcolomn.rfind("Level", 0) == 0)
				{
					if (levelnumber[0] != 0)
					{
						//if (strcmp(levelnumber, "C1") == 0)
						//	printf("youpi");
						InjectTexts(levelnumber); // If we reach a new level, then save the previous one
#ifdef DOONLYFIRSTLEVEL
						exit(1); // Debug, exit after level 1 processing
#endif 
					}

					sscanf(stringcolomn.c_str(), "Level%s", levelnumber);
					levelnumber[2] = 0;
					IsIntro = false;
					IsTexts = false; 
					IsDeaths = false;
					IsTimeUp = false;
					ignoreline = false;
					gStringTexts.clear();
					gStringDeaths.clear();
					gGlobalLevelStartNumber = gGlobalLineNumber;

					// check if last level
					if (strcmp(levelnumber, "En") == 0) // Only two chars are kepts
					{
						printf("-------- End of levels reached\n");
						printf("-- Processing Time up strings ... \n");
					}
				}
				else if (stringcolomn == "intro")
				{
					IsIntro = true;
					IsTexts = false; 
					IsDeaths = false;
					IsTimeUp = false;
				}
				else if (stringcolomn == "deaths")
				{
					IsIntro = false;
					IsTexts = false; 
					IsDeaths = true;
					IsTimeUp = false;
				}
				else if (stringcolomn == "timeupdeath")
				{
					IsIntro = false;
					IsTexts = false; 
					IsDeaths = false;
					IsTimeUp = true;
				}
#ifdef CPC
				// CPC is master version so no alt should be there
				if (stringcolomn.rfind("alt-", 0) == 0)
					ignoreline = true; // We do nothing
#endif
#ifdef C64 // Alternative C64 version
				else if (stringcolomn == "alt-c64")
				{
					// Remove last text as we are going to replace it
					if (IsTexts)
					{
						if (gStringTexts.size() == 0) // Was intro
						{
							IsIntro = true;
							IsTexts = false; 
						}
						else
							gStringTexts.pop_back();
					}
					else if (IsDeaths)
						gStringDeaths.pop_back();
					else if (IsTimeUp)
						gStringTimeUpDeaths.pop_back();
				}
				else if (stringcolomn.rfind("alt-", 0) == 0)
					ignoreline = true; // We do nothing
#endif
#ifdef SPECCY // Alternative ZX-SPECTRUM version
				else if (stringcolomn == "alt-speccy")
				{
					// Remove last text as we are going to replace it
					if (IsTexts)
					{
						if (gStringTexts.size() == 0) // Was intro
						{
							IsIntro = true;
							IsTexts = false; 
						}
						else
							gStringTexts.pop_back();
					}
					else if (IsDeaths)
						gStringDeaths.pop_back();
					else if (IsTimeUp)
						gStringTimeUpDeaths.pop_back();
				}
				else if (stringcolomn.rfind("alt-", 0) == 0)
					ignoreline = true; // We do nothing
#endif
				else // no tag
				{
					IsTimeUp = false;
				}
			}

			// Store string (or do nothing for alternative texts)
			if (column == LANGAGE)
			{
				if (ignoreline)
				{
					ignoreline = false; // Do nothing, this is alt text, reset flag
				}
				else if (IsIntro)
				{
					gStringIntro = stringcolomn;
					// If line was intro then next are textes
					IsIntro = false;
					IsTexts = true; 
				}
				else if (IsTexts)
					gStringTexts.push_back(stringcolomn);
				else if (IsDeaths)
					gStringDeaths.push_back(stringcolomn);
				else if (IsTimeUp)
				{
					string tempstring = ConvertString(stringcolomn);
					gStringTimeUpDeaths.push_back(tempstring); // Store all time up string + stat string. Should be 6 + 1
				}
			}
			column++; // Go to next column
		}

		// Next line
		gGlobalLineNumber++;
	}

	//if (levelnumber[0] != 0)
	//	InjectTexts(levelnumber); // that was last level, save it.

	// Save TimeUpDeaths texts

	SaveTimeUp();

#ifdef  DUMPSPECTRUMTEXTS
	 fclose(SpectrumFile);
#endif
#ifdef SPECCY
	 if (gSpeccyIntroFile)
		 fclose(gSpeccyIntroFile);
	 if (gSpeccyTextFile)
		 fclose(gSpeccyTextFile);
	 if (gSpeccyTimeUpFile)
		 fclose(gSpeccyTimeUpFile);
#endif

	std::cout << "Ending!\n";
}
