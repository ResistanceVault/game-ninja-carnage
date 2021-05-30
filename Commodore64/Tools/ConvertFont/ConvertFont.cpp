// convert RAW photoshop to C64 font.
// 2 colors. Pink and not pink.
// Bitmap characters, one by one.
// Input is 320x200 RGB

#include <iostream>

unsigned char gBufferPicture[320 * 200 * 3]; // Input picture 192Ko
// Background should be pure pink 

#define NUMBERCHAR (40+40+40+26)

// ------------------------------------------------------
bool IsPixelPink(int index)
{
	if (gBufferPicture[index] == 255 && gBufferPicture[index + 1] == 0 && gBufferPicture[index + 2] == 255)
		return true;
	return false;
}

// ------------------------------------------------------
void SaveOneChar(int idchar, FILE* fileout)
{
	int startindex = (idchar % 40) * 8 * 3 + (idchar / 40) * 320 * 3 * 8;
	fprintf(fileout, " ");
	// 8 pixels ont 8 lines
	for (int y = 0; y < 8; y++)
	{
		unsigned char result = 0;
		for (int x = 0; x < 8; x++)
		{
			result = result << 1;
			int currentindex = startindex + x * 3 + y * 320 * 3;
			if (!IsPixelPink(currentindex))
				result ^= 0x01;
		}
		fprintf(fileout, "0x%02x, ",result);
		//if (y!=7)
		//	fprintf(fileout, ", ");
	}
	fprintf(fileout, "\n");

}



// ------------------------------------------------------
int main(int argc, char** argv)
{
	std::cout << "Starting!\n";

	FILE* file;
	FILE* fileout;

	if (argc == 1)
	{
		printf("Usage : Exe Font.raw\n");
		exit(0);
	}

	char* filename;
	filename = argv[1];

	file = fopen(filename, "rb");
	if (!file)
	{
		printf("Can not read file %s\n", filename);
		exit(0);
	}

	// Read file
	fread(gBufferPicture, 1, 320 * 200 * 3, file);
	fclose(file);

	// Open file out
	fileout = fopen("output.txt","w");
	if (!fileout)
	{
		printf("Can not read file output.txt\n");
		exit(0);
	}

	for (int i = 0; i < NUMBERCHAR; i++)
		SaveOneChar(i, fileout);



	fclose(fileout);

	std::cout << "Ending!\n";
}
