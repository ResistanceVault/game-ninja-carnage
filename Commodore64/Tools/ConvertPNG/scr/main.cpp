//
// Convert png
// ConvertPNG.exe name.png ForceColor=012A
// ForceColor=X
// ForceColor=XX
// ForceColor=XXX
// ForceColor=XXXX Force 1 to 4 color (in hexa). Back color is force to 0 by default.
// RemoveFirstPalette : Remove the first main palette (used to create an animation)


#include "lodepng/lodepng.h"

#include <stdio.h>
#include <stdlib.h>
#include "color.h"

// -- Prototypes
int FindBestIndex(color inputcolor);
class colorBlock;

// -- Global vars

// Original PNG
unsigned char* image = 0; // RGBA loaded from PNG
unsigned width, height;

int gNumberpixels;
int gNumberblocks;

// Main picture
colorBlock* gPictureBlock; 

// Alternate pictures. Max is 6
#define MAXALTERNATEPICTURES 6
colorBlock* gPictureBlockAlt[MAXALTERNATEPICTURES];
int         gNumAlternatePictures = 0; // Number of found alternate pictures

char gFilename[512];

// -- C64 colors

int gForcedColor = 0; // Force color (background)
int gForcedColor1 = -1; // Force color 1
int gForcedColor2 = -1; // Force color 2
int gForcedColor3 = -1; // Force color 3
int gRemoveFirstPal = -1; // RemoveFirstPalette

color gAllColors[16] = {
    //https://www.lemon64.com/forum/viewtopic.php?t=8718&sid=7c3f4b874210de2623abc2ebaa348847
    // And same from file 
    // d:\Kristof\C64_Dev\8bit-Unity\docs\Palettes\Palette-C64.png

    // From pic Palette-C64.png
    color(0x00,0x00,0x00), // Black
    color(0xff,0xff,0xff), // White
    color(137,64,54),
    color(122,191,199),
    color(138,70,174),
    color(104,169,65),
    color(62,49,162),
    color(208,220,113),
    color(144,95,37),
    color(92,71,0),
    color(187,119,109),
    color(85,85,85),
    color(128,128,128),
    color(172,234,136),
    color(124,112,218),
    color(171,171,171)  //Light grey

    /*
    color(0x00,0x00,0x00), 
    color(0xff,0xff,0xff), 
    color(0x68,0x37,0x2b),
    color(0x70,0xa4,0xb2),
    color(0x6f,0x3d,0x86),
    color(0x58,0x8d,0x43),
    color(0x35,0x28,0x79),
    color(0xb8,0xc7,0x6f),
    color(0x6f,0x4f,0x25),
    color(0x43,0x39,0x00),
    color(0x9a,0x67,0x59),
    color(0x44,0x44,0x44),
    color(0x6c,0x6c,0x6c),
    color(0x9a,0xd2,0x84),
    color(0x6c,0x5e,0xb5),
    color(0xAB,0xAB,0xAB), 
    */
};

// ----------------------------------------------------------------
// One 8x8 pixel block (as pixel is 2 pixels wide, this is 4*8 in fact)
class colorBlock
{
public:
    colorBlock();
    ~colorBlock() {};
    void InitFromRGBABuffer(unsigned char* image, int width);
    void SetBackgroundColor(int index);
    void QuantizeColorsAndRemap(); // Compute 4 best colors
    void WriteToRGBABuffer(unsigned char* image, int width); // Write color limited
    void WriteBitmapDataToBinaryFile(FILE* file);
    void WriteColorDataToBinaryFile(FILE* file);
    void WriteScreenDataToBinaryFile(FILE* file);
    void WriteBitmapDataToTextFile(FILE* file);
    void WriteColorDataToTextFile(FILE* file);
    void WriteScreenDataToTextFile(FILE* file);
    // For alt pictures
    int* GetPixel4colorsPtr() { return pixels4colors; };
    int* GetColorsPtr() { return colors; };
    void FindBestColorToMatchMainPicture();
private:
    unsigned char encodepixelindex(int index);
private:
    int pixels[4 * 8];        // Indexed (0 to 16)
    int colors[4];            // 4 best colors. background, 1&2, 3
    int pixels4colors[4 * 8]; // Indexed and limited to 4 colors (no reference a colors)
};

// ----------------------------------------------------------------
colorBlock::colorBlock()
{
    colors[0] = 0;
    colors[1] = 0;
    colors[2] = 0;
    colors[3] = 0;
    for (int i = 0; i < 4 * 8; i++)
        pixels4colors[i] = 0;
};

// ----------------------------------------------------------------
// Set back color (default is 0 black)
// Others forced color are gForcedColor1 to gForcedColor3 (if -1, not forced)
void colorBlock::SetBackgroundColor(int index)
{
    colors[0] = index;
}
// ----------------------------------------------------------------
// Compute 4 best colors
// 
void colorBlock::QuantizeColorsAndRemap()
{
    int countcolor[16];
    for (int i = 0; i < 16; i++) countcolor[i] = 0;
    for (int i = 0; i < 4 * 8; i++) countcolor[pixels[i]]++;
    countcolor[colors[0]] = 0; // Do not count back color, always forced
    // Check forced color.
    int startcolor = 1;
    if (gForcedColor1 >= 0)
    {
        countcolor[gForcedColor1] = 0;
        colors[1] = gForcedColor1;
        startcolor++;
    }
    if (gForcedColor2 >= 0)
    {
        countcolor[gForcedColor2] = 0;
        colors[2] = gForcedColor2;
        startcolor++;
    }
    if (gForcedColor3 >= 0)
    {
        countcolor[gForcedColor3] = 0;
        colors[3] = gForcedColor3;
        startcolor++;
    }
    // For remaining colors, find best
    for (int loop = startcolor; loop < 4; loop++)
    {
        int greatestedvalue = 0;
        int bestindex = 0;
        for (int i = 0; i < 16; i++)
        {
            if (countcolor[i] > greatestedvalue)
            {
                greatestedvalue = countcolor[i];
                bestindex = i;
            }
        }
        colors[loop] = bestindex;
        countcolor[bestindex] = 0;
    }
    // Remap to one of the 4 colors.
    for (int i = 0; i < 4 * 8; i++)
    {
        int currentindex = pixels[i];
        if (currentindex == colors[0] || currentindex == colors[1] || currentindex == colors[2] || currentindex == colors[3])
            pixels4colors[i] = currentindex;
        else
        {   // Find best of 4 colors
            int besterror = 99999999;
            int bestindex = 0;
            color currentcolor = gAllColors[currentindex];
            for (int j = 0; j < 4; j++)
            {
                color refcolor = gAllColors[colors[j]];
                int error = currentcolor.getdiff(refcolor);
                if (error < besterror)
                {
                    besterror = error;
                    bestindex = colors[j];
                }
            }
            pixels4colors[i] = bestindex;
         }
    }

}; 

// ----------------------------------------------------------------
void colorBlock::InitFromRGBABuffer(unsigned char* image, int width)
{
    for (int y = 0; y < 8; y++)
    {
        unsigned char* pRGBA = image + width * 4 * y;
        for (int x = 0; x < 4; x++)
        {
            unsigned char R, G, B; // , A;
            R = pRGBA[x * 4 + 0];
            G = pRGBA[x * 4 + 1];
            B = pRGBA[x * 4 + 2];
            //A = pRGBA[x*4+3];
            color currentpixel(R, G, B);
            int index = FindBestIndex(currentpixel);
            pixels[y * 4 + x] = index;
        }
    }
}

// ----------------------------------------------------------------
void colorBlock::WriteToRGBABuffer(unsigned char* image, int width)
{
    for (int y = 0; y < 8; y++)
    {
        unsigned char* pRGBA = image + (width * 4 * y);
        for (int x = 0; x < 4; x++)
        {
            int index = pixels4colors[(y * 4) + x];
            color currentpixel = gAllColors[index];
            pRGBA[x * 4 + 0] = currentpixel.r;
            pRGBA[x * 4 + 1] = currentpixel.g;
            pRGBA[x * 4 + 2] = currentpixel.b;
            pRGBA[x * 4 + 3] = 255;
        }
    }
}

// ----------------------------------------------------------------
unsigned char colorBlock::encodepixelindex(int index)
{
    unsigned char result = 0;
    unsigned char colorindex = pixels4colors[index];
    if (colorindex == colors[0])
        result = 0x00;
    else if (colorindex == colors[1])
        result = 0x01;
    else if (colorindex == colors[2])
        result = 0x02;
    else if (colorindex == colors[3])
        result = 0x03;
    else
        printf("Error, colorBlock::encodepixelindex index is not in 4 colors block\n");

    return result;
}

// ----------------------------------------------------------------
void colorBlock::WriteBitmapDataToBinaryFile(FILE* file)
{
    // Write 8 lines.
    // Each have 4 pixels. Each pixel is one of the 4 colors (so 2 bits 00 01 10 11)
    for (int j = 0; j < 8; j++)
    {
        int startindex = j * 4;
        unsigned char finalvalue = 0;
        finalvalue |= encodepixelindex(startindex);
        finalvalue = (finalvalue << 2);
        finalvalue |= encodepixelindex(startindex+1);
        finalvalue = (finalvalue << 2);
        finalvalue |= encodepixelindex(startindex+2);
        finalvalue = (finalvalue << 2);
        finalvalue |= encodepixelindex(startindex+3);
        fwrite(&finalvalue, 1, 1, file);
    }
}
// ----------------------------------------------------------------
// Color 3
void colorBlock::WriteColorDataToBinaryFile(FILE* file)
{
    fwrite(&colors[3], 1, 1, file);
}
// ----------------------------------------------------------------
// Color 1 2
void colorBlock::WriteScreenDataToBinaryFile(FILE* file)
{
    unsigned char value;
    value = colors[1];
    value = value << 4;
    value |= colors[2];
    fwrite(&value, 1, 1, file);
}
// ----------------------------------------------------------------
void colorBlock::WriteBitmapDataToTextFile(FILE* file)
{
    // Write 8 lines.
    // Each have 4 pixels. Each pixel is one of the 4 colors (so 2 bits 00 01 10 11)
    for (int j = 0; j < 8; j++)
    {
        int startindex = j * 4;
        unsigned char finalvalue = 0;
        finalvalue |= encodepixelindex(startindex);
        finalvalue = (finalvalue << 2);
        finalvalue |= encodepixelindex(startindex+1);
        finalvalue = (finalvalue << 2);
        finalvalue |= encodepixelindex(startindex+2);
        finalvalue = (finalvalue << 2);
        finalvalue |= encodepixelindex(startindex+3);
        fprintf(file,"0x%02x,",finalvalue);
    }
}
// ----------------------------------------------------------------
void colorBlock::WriteColorDataToTextFile(FILE* file)
{
    fprintf(file,"0x%02x,",colors[3]);
}
// ----------------------------------------------------------------
void colorBlock::WriteScreenDataToTextFile(FILE* file)
{
    unsigned char value;
    value = colors[1];
    value = value << 4;
    value |= colors[2];
    fprintf(file,"0x%02x,",value);
}
// ----------------------------------------------------------------
// FindBestColorToMatchMainPicture
// From colors index of main picture ( pixels4colors )
// we search for the best colors ( 1 2 3 ) to have the minimum error
// from alternate picture (index are in "pixels" array)
void colorBlock::FindBestColorToMatchMainPicture()
{
    // First color is always forced (usually 0)
    // So for color 1 2 3 we test all colors and take the minimum error.
    // That mean 16x16x16 possibilities (4096)

    // -- First convert index to colors table
    for (int i = 0; i < 8 * 4; i++)
    {
        int indexabsolute = pixels4colors[i];
        int indextocolors=0;
        if (indexabsolute == colors[0])
            indextocolors = 0;
        else if (indexabsolute == colors[1])
            indextocolors = 1;
        else if (indexabsolute == colors[2])
            indextocolors = 2;
        else if (indexabsolute == colors[3])
            indextocolors = 3;
        else
            printf("ERROR finding color. Should never go there\n");
        pixels4colors[i] = indextocolors;
    }

    int bestcolor1 = 0;
    int bestcolor2 = 0; 
    int bestcolor3 = 0;
    int besterror = 999999999;
    for (int col1 = 0; col1 < 16; col1++)
    {
        for (int col2 = 0; col2 < 16; col2++)
        {
            for (int col3 = 0; col3 < 16; col3++)
            {
                // Here we have col1, col2, col3 which are 3 index.
                // We compute error for all 4*8 pixels.
                // And if error is less than actual best error, we keep that combinaison
                colors[1] = col1;
                colors[2] = col2;
                colors[3] = col3;
                int error = 0;
                for (int i = 0; i < 4 * 8; i++)
                {
                    int altindex = colors[pixels4colors[i]];
                    color altcolor = gAllColors[altindex];
                    color refcolor = gAllColors[pixels[i]];
                    error += altcolor.getdiff(refcolor);
                }
                // Check total error
                if (error < besterror)
                {
                    besterror = error;
                    bestcolor1 = col1;
                    bestcolor2 = col2;
                    bestcolor3 = col3;
                }
            }
        }
    }
    // End store final color into table
    colors[1] = bestcolor1;
    colors[2] = bestcolor2;
    colors[3] = bestcolor3;

    // convert back to absolute index
    for (int i = 0; i < 8 * 4; i++)
    {
        pixels4colors[i] = colors [ pixels4colors[i] ];
    }
}

// ----------------------------------------------------------------
void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height)
{
    unsigned error = lodepng_encode32_file(filename, image, width, height);
    if (error)
        printf("error %u: %s\n", error, lodepng_error_text(error));
}

// -------------------------------------------
int FindBestIndex(color inputcolor)
{
    int minerror = 99999999;
    int bestindex = 0;
    for (int i = 0; i < 16; i++)
    {
        int error = inputcolor.getdiff(gAllColors[i]);
        if (error < minerror)
        {
            bestindex = i;
            minerror = error;
        }
    }
    return bestindex;
}

// ----------------------------------------------
// Get filename without extension
// gFilename[512]
void getFilenamefromPath(char* path)
{
    char* finaldot;
    char* pathdelimiter;
    finaldot = strrchr(path, '.');
    pathdelimiter = strrchr(finaldot, '/');
    if(!pathdelimiter)
        pathdelimiter = strrchr(finaldot, '\\');
    int startindex = 0;
    int endindex = finaldot-path;
    if (pathdelimiter)
        startindex = (pathdelimiter - path)+1;
    int namesize = endindex - startindex;
    strncpy(gFilename, &path[startindex], namesize);
    gFilename[namesize] = 0;
}

// -------------------------------------------
// Convert C64
int main(int argc, char* argv[])
{
    //const char* filename = argc > 1 ? argv[1] : "test.png";

    if (argc <= 1)
    {
        printf("exe name.png (ForceColor=0123)\n");
        exit(0);
    }

    getFilenamefromPath(argv[1]); // Extract filename without extension to gFilename

    // Check if forceColor item is there.
    // or RemoveFirstPalette
    if (argc == 3)
    {
        if ( _stricmp("RemoveFirstPalette",argv[2])==0 )
        {
            gRemoveFirstPal = 1;
        }
        else
        {
            char colors[128];
            colors[0] = 0;
            sscanf(argv[2], "ForceColor=%s", colors);
            if (colors[0] == 0)
                printf("Forcing color 0 to 0 (black). No ForceColor param detrected.\n");
            else
            {
                char char0 = 0;
                char char1 = 0;
                char char2 = 0;
                char char3 = 0;
                sscanf(colors, "%c%c%c%c", &char0, &char1, &char2, &char3);
                sscanf(&char0, "%x", &gForcedColor);
                sscanf(&char1, "%x", &gForcedColor1);
                sscanf(&char2, "%x", &gForcedColor2);
                sscanf(&char3, "%x", &gForcedColor3);
                printf("ForceColor Detected. ");
                if (gForcedColor>=0)  printf("Color0=%d ",gForcedColor);
                if (gForcedColor1>=0) printf("Color1=%d ",gForcedColor1);
                if (gForcedColor2>=0) printf("Color2=%d ",gForcedColor2);
                if (gForcedColor3>=0) printf("Color3=%d ",gForcedColor3);
                printf("\n");
            }
        }
            
    }
    else
    {
        printf("Forcing color 0 to 0 (black). No ForceColor param detrected.\n");
    }

    unsigned error;
    error = lodepng_decode32_file(&image, &width, &height, argv[1]);
    if (error)
    {
        printf("error %u: %s\n", error, lodepng_error_text(error));
        exit(0);
    }

    if ((width % 4) != 0 || (height % 8) != 0)
    {
        printf("error : size should be multiple of 4 for x and 8 for Y (current x=%d y=%d)\n", width, height);
        exit(0);
    }

    // Create 4x8 block. Filled index buffer named "pixels".
    int gNumberblocks = (width / 4) * (height / 8);
    gPictureBlock = (colorBlock*)malloc(gNumberblocks * sizeof(colorBlock));
    int blockwidth = (width / 4);
    int blocklinewidth = width * 4 * 8;
    int blockheight = (height / 8);
    for (int i = 0; i < gNumberblocks; i++)
    {   // Compute index from RGBA buffer
        int startindex = 4 * 4 * (i % blockwidth) + ((i / blockwidth) * blocklinewidth);
        //printf("Index %d\n", startindex);
        gPictureBlock[i].InitFromRGBABuffer(&(image[startindex]), width);
    }

    // Quantisize all blocks 
    // Find best colors and fill "colors" and "pixels4colors"
    for (int i = 0; i < gNumberblocks; i++)
    { 
        gPictureBlock[i].SetBackgroundColor(gForcedColor);
        gPictureBlock[i].QuantizeColorsAndRemap();
    }
    // Free input image
    free(image);

    // -- End of main image encoding.
    // Now we check if there are any alternate picture to encore.
    // We only change the colors in block (colors 1 2 3).
    // To simulate a palette animation.
    // color*      gPictureAlt[MAXALTERNATEPICTURES];
    // int         gNumAlternatePictures = 0; // Number of found alternate pictures
    for (int altloop = 0; altloop < MAXALTERNATEPICTURES; altloop++)
    {
        // Search if _alt1 to _alt6 exists
        char altname[256];
        unsigned int widthalt;
        unsigned int heightalt;
        sprintf(altname, "%s_alt%d.png", gFilename,altloop+1);

        error = lodepng_decode32_file(&image, &widthalt, &heightalt, altname);
        if (error)
            break; // Stop parsing alt
        if ( width != widthalt || height != heightalt)
        {
            printf("Warning : size of alterate picture must be the same (main x=%d y=%d) (alt x=%d y=%d)\n", width, height, widthalt, heightalt);
            break; // Stop parsing
        }
        gNumAlternatePictures++;
        printf("processing alt picture %d\n", altloop + 1);
        // There we got a valid alternate file.
        // We first create all wanted indexes (in "pixels" of colorblock)
        // And copy the index from main picture.
        // Then vary colors 1 2 3 and find smallest error.
        // Create 4x8 block. Filled index buffer named "pixels".
        int gNumberblocks = (widthalt / 4) * (heightalt / 8);
        gPictureBlockAlt[altloop] = (colorBlock*)malloc(gNumberblocks * sizeof(colorBlock));
        int blockwidth = (widthalt / 4);
        int blocklinewidth = widthalt * 4 * 8;
        int blockheight = (heightalt / 8);
        for (int i = 0; i < gNumberblocks; i++)
        {   // Compute index from RGBA buffer
            int startindex = 4 * 4 * (i % blockwidth) + ((i / blockwidth) * blocklinewidth);
            //printf("Index %d\n", startindex);
            gPictureBlockAlt[altloop][i].InitFromRGBABuffer(&(image[startindex]), widthalt);
        }
        // Free input image
        free(image);
        // Copy index from main picture
        for (int i = 0; i < gNumberblocks; i++)
        {   
            int* src = gPictureBlock[i].GetPixel4colorsPtr();
            int* dest = gPictureBlockAlt[altloop][i].GetPixel4colorsPtr();
            memcpy(dest, src, 4 * 8 * sizeof(int) );
            // Copy colors
            src = gPictureBlock[i].GetColorsPtr();
            dest = gPictureBlockAlt[altloop][i].GetColorsPtr();
            memcpy(dest, src, 4 * sizeof(int) );
        }
        // Find best colors
        for (int i = 0; i < gNumberblocks; i++)
        {   
            gPictureBlockAlt[altloop][i].SetBackgroundColor(gForcedColor);
            gPictureBlockAlt[altloop][i].FindBestColorToMatchMainPicture();
        }

    }
    // -- End processing

    // -- Save Binary
    // Format is (u8) size x, size y, background color, nb screenram color buffer
    //  bitmapdata : 8 bytes * gNumberblocks (color 0 1 2 3)
    //  colorRamdata : 1 bytes * gNumberblocks (color 3)
    //  ScreenRamData : 1 bytes * gNumberblocks (color 2 3) * nb screenram color buffer
    char outputname[512];
    FILE* outfile;
    sprintf(outputname, "%s.bin", gFilename);
    outfile = fopen(outputname, "wb");
    if (!outfile)
    {
        printf("Error can not write output file %s\n", outputname);
        exit(0);
    }
    // Header
    unsigned char valuetowrite;
    valuetowrite = width;
    fwrite(&valuetowrite, 1, 1, outfile);
    valuetowrite = height;
    fwrite(&valuetowrite, 1, 1, outfile);
    valuetowrite = gForcedColor;
    fwrite(&valuetowrite, 1, 1, outfile);
    if (gRemoveFirstPal!=1)
        valuetowrite = gNumAlternatePictures; // Num of alternate pictures
    else
        valuetowrite = gNumAlternatePictures-1; // Num of alternate pictures
    fwrite(&valuetowrite, 1, 1, outfile);
    // Write Bitmapdata
    for (int i = 0; i < gNumberblocks; i++)
        gPictureBlock[i].WriteBitmapDataToBinaryFile(outfile);
    if (gRemoveFirstPal != 1) // We do not save the first PAL
    {
        // Write ColorRamData (color 3)
        for (int i = 0; i < gNumberblocks; i++)
            gPictureBlock[i].WriteColorDataToBinaryFile(outfile);
        // Write ScreneRamData (color 1 2)
        for (int i = 0; i < gNumberblocks; i++)
            gPictureBlock[i].WriteScreenDataToBinaryFile(outfile);
    }
    // Write alternate pictures data
    for (int altloop = 0; altloop < gNumAlternatePictures; altloop++)
    {
        // Write ColorRamData (color 3)
        for (int i = 0; i < gNumberblocks; i++)
            gPictureBlockAlt[altloop][i].WriteColorDataToBinaryFile(outfile);
        // Write ScreneRamData (color 1 2)
        for (int i = 0; i < gNumberblocks; i++)
            gPictureBlockAlt[altloop][i].WriteScreenDataToBinaryFile(outfile);
    }
    fclose(outfile);

    // -- Save Koala format (full screen)
    sprintf(outputname, "%s.kla", gFilename);
    outfile = fopen(outputname, "wb");
    if (!outfile)
    {
        printf("Error can not write output file %s\n", outputname);
        exit(0);
    }
    // Header. For Koala picture header is 0x00 0x60 (adress to write file)
    // 20 blocs per line
    // 25 lines high
    valuetowrite = 0x00;
    fwrite(&valuetowrite, 1, 1, outfile);
    valuetowrite = 0x60;
    fwrite(&valuetowrite, 1, 1, outfile);
    colorBlock emptyblock;
    // Write Bitmapdata
    for (int j = 0 ; j < blockheight ; j++)
    {
        for (int i = 0; i < blockwidth; i++)
            gPictureBlock[j*blockwidth+i].WriteBitmapDataToBinaryFile(outfile);
        for (int i = blockwidth; i < 40; i++)
            emptyblock.WriteBitmapDataToBinaryFile(outfile);
    }
    for (int j = blockheight ; j < 25 ; j++)
        for (int i = 0; i < 40; i++)
            emptyblock.WriteBitmapDataToBinaryFile(outfile);
    // Write ScreneRamData (color 1 2)
    for (int j = 0 ; j < blockheight ; j++)
    {
        for (int i = 0; i < blockwidth; i++)
            gPictureBlock[j*blockwidth+i].WriteScreenDataToBinaryFile(outfile);
        for (int i = blockwidth; i < 40; i++)
            emptyblock.WriteScreenDataToBinaryFile(outfile);
    }
    for (int j = blockheight ; j < 25 ; j++)
        for (int i = 0; i < 40; i++)
            emptyblock.WriteScreenDataToBinaryFile(outfile);
    // Write ColorRamData (color 3)
    for (int j = 0 ; j < blockheight ; j++)
    {
        for (int i = 0; i < blockwidth; i++)
            gPictureBlock[j*blockwidth+i].WriteColorDataToBinaryFile(outfile);
        for (int i = blockwidth; i < 40; i++)
            emptyblock.WriteColorDataToBinaryFile(outfile);
    }
    for (int j = blockheight ; j < 25 ; j++)
        for (int i = 0; i < 40; i++)
            emptyblock.WriteColorDataToBinaryFile(outfile);
    valuetowrite = gForcedColor;
    fwrite(&valuetowrite, 1, 1, outfile);
    fclose(outfile);

    // -- Save C text code
    sprintf(outputname, "%s.c", gFilename);
    outfile = fopen(outputname, "w");
    if (!outfile)
    {
        printf("Error can not write output C file %s\n", outputname);
        exit(0);
    }
    // Header
    fprintf(outfile, "unsigned char %s[%d]={\n    ", gFilename, 4 + gNumberblocks * 8 + gNumberblocks + gNumberblocks);
    fprintf(outfile, "%d,%d,%d,%d, // x,y,back color, nb colors buffers\n    ", width, height, gForcedColor, gNumAlternatePictures);
    // Write Bitmapdata
    for (int i = 0; i < gNumberblocks; i++)
        gPictureBlock[i].WriteBitmapDataToTextFile(outfile);
    fprintf(outfile, "\n    ");
    // Write ColorRamData (color 3)
    for (int i = 0; i < gNumberblocks; i++)
        gPictureBlock[i].WriteColorDataToTextFile(outfile);
    fprintf(outfile, "\n    ");
    // Write ScreneRamData (color 1 2)
    for (int i = 0; i < gNumberblocks; i++)
        gPictureBlock[i].WriteScreenDataToTextFile(outfile);
    fprintf(outfile, "\n    ");
    // Alt pictures
    for (int altloop = 0; altloop < gNumAlternatePictures; altloop++)
    {
        fprintf(outfile, "//data for alt picture %d\n    ",altloop+1);
        // Write ColorRamData (color 3)
        for (int i = 0; i < gNumberblocks; i++)
            gPictureBlockAlt[altloop][i].WriteColorDataToTextFile(outfile);
        fprintf(outfile, "\n    ");
        // Write ScreneRamData (color 1 2)
        for (int i = 0; i < gNumberblocks; i++)
            gPictureBlockAlt[altloop][i].WriteScreenDataToTextFile(outfile);
        fprintf(outfile, "\n    ");
    }
    fprintf(outfile, "\n};");
    fclose(outfile);
    
    
    // -- Save outpicture (for debug)
    blockwidth = (width / 4);
    blocklinewidth = width * 4 * 8;
    image = (unsigned char*) malloc(width * height * 4);
    for (int i = 0; i < gNumberblocks; i++)
    {   // Compute index from RGBA buffer
        int startindex = 4 * 4 * (i % blockwidth) + ((i / blockwidth) * blocklinewidth);
        gPictureBlock[i].WriteToRGBABuffer(&(image[startindex]), width);
    }
    char outputdebugname[512];
    sprintf(outputdebugname, "%s_debugoutput.png", gFilename);
    encodeOneStep(outputdebugname, image, width, height);
    free(image);

    // Save alt pictures
    for (int altloop = 0; altloop < gNumAlternatePictures; altloop++)
    {
        image = (unsigned char*) malloc(width * height * 4);
        for (int i = 0; i < gNumberblocks; i++)
        {   // Compute index from RGBA buffer
            int startindex = 4 * 4 * (i % blockwidth) + ((i / blockwidth) * blocklinewidth);
            gPictureBlockAlt[altloop][i].WriteToRGBABuffer(&(image[startindex]), width);
        }
        char outputdebugname[512];
        sprintf(outputdebugname, "%s_debugoutput%d.png", gFilename,altloop+1);
        encodeOneStep(outputdebugname, image, width, height);
        free(image);
    }



    



    return 0;
}



