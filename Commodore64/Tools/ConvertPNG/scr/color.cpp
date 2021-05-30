#include "color.h"
#include "math.h"
#include <algorithm>


color::color()
{
}

color::color(int _r, int _g, int _b)
{
  r = _r;
  g = _g;
  b = _b;

}

color::color(unsigned char* buffer)
{
	r = buffer[0];
	g = buffer[1];
	b = buffer[2];
}

color::~color()
{
}

void color::writetobuffer(unsigned char* buff)
{
  buff[0] = r;
  buff[1] = g;
  buff[2] = b;
}

void color::writetobufferas16bits(unsigned char* buff)
{
  int value;
  value = ((r >> 4) << 8) | ((g >> 4) << 4) | ((b >> 4));

  buff[0] = (value>>8);
  buff[1] = (value&0x000000FF);
}


void color::convertto16bits()
{
  // Add 8 and clamp to 255
  r = ( ((r+8) / 17) * 17 );
  g = ((g+8) / 17) * 17;
  b = ((b+8) / 17) * 17;
  r = std::min(r, 255);
  g = std::min(g, 255);
  b = std::min(b, 255);
}

// Brighness 0 to 1.
// and 1 to 2.
void color::writetobufferwithbrightness(unsigned char* buff, float brightness)
{
  int newr, newg, newb;

  newr = r * brightness;
  if (newr > 255)
    newr = 255;

  newg = g * brightness;
  if (newg > 255)
    newg = 255;

  newb = b * brightness;
  if (newb > 255)
    newb = 255;


  buff[0] = newr;
  buff[1] = newg;
  buff[2] = newb;
}

int color::getdiff(color othercolor)
{
  // Get diff of 2 colors
  int diff;
  diff = 10.0f*0.299f*abs(r - othercolor.r) + 10.0f* 0.587f*abs(g - othercolor.g) + 10.0f* 0.114f*abs(b - othercolor.b);
  //diff = 10.0f * 1.0f * abs(r - othercolor.r) + 10.0f * 1.0f * abs(g - othercolor.g) + 10.0f * 1.0f * abs(b - othercolor.b);
  return diff;
}


// Sort color by intensity.
int compare(const void* a, const void* b)
{
  color* colora = (color*)a;
  color* colorb = (color*)b;

  //Y'=0.299R' + 0.587G'+0.114B'
  float luma = 0.299f*colora->r + 0.587f * colora->g + 0.114f*colora->b;
  float lumb = 0.299f*colorb->r + 0.587f * colorb->g + 0.114f*colorb->b;

  float diff = 10.0f * (luma - lumb); // *10.0 to keep accuracy on sorting

  return (int)diff;
}

// Coef can be from 0 to 1.0, we simply fade to black
// Coef can be 1.0 to 2.0; we fade to white
void color::setandaddbrighnessfloat(float coef)
{
  int _r, _g, _b;

  if (coef <= 1.0f)
  {
    _r = r * coef;
    _g = g * coef;
    _b = b * coef;
  }
  else
  {
    // From 1.0f to 2.0f, fade to white
    float coeftowhite = coef - 1.0f; // 0 to 1 (0 = color, 1=white)
    coeftowhite = 1.0f - coeftowhite;
    float coeftowhiteinv = 1.0f - coeftowhite;
    _r = coeftowhite*r + 255.0f*coeftowhiteinv;
    _g = coeftowhite*g + 255.0f*coeftowhiteinv;
    _b = coeftowhite*b + 255.0f*coeftowhiteinv;
  }

  if (_r > 255) _r = 255;
  if (_g > 255) _g = 255;
  if (_b > 255) _b = 255;
  r = _r;
  g = _g;
  b = _b;
}



// Color Blend

color* colorblend::AllColors; 

colorblend::colorblend()
{

}
colorblend::colorblend(int indexcolor1, int indexcolor2, int ditherlevel, float ditherstepvalue)
{
  set(indexcolor1, indexcolor2, ditherlevel, ditherstepvalue);
}

colorblend::~colorblend()
{

}

void colorblend::set(int _indexcolor1, int _indexcolor2, int _ditherlevel, float ditherstepvalue)
{
  indexcolor1 = _indexcolor1;
  indexcolor2 = _indexcolor2;
  ditherlevel = _ditherlevel;

  //int r, g, b; // Obtained color
  //int errorvalue; // Error from real color.

  // Compute the value we get.
  color color1 = AllColors[indexcolor1];
  color color2 = AllColors[indexcolor2];
  float blendcoef = 1.0f - (ditherlevel * ditherstepvalue) / 100.0f ; // 0 to 87.5%
  float blendcoefinv = 1.0f - blendcoef;

  r = ((float)color1.r) * blendcoef + ((float)color2.r) * blendcoefinv;
  g = ((float)color1.g) * blendcoef + ((float)color2.g) * blendcoefinv;
  b = ((float)color1.b) * blendcoef + ((float)color2.b) * blendcoefinv;

#define ERRORMAX 0.25f

  // Compute the error.
  // 0 = blend 0%    = 0%
  // 1 = Blend 12.5% = ERRORMAX * 0.25
  // 2 = Blend 25%   = ERRORMAX * 0.5f
  // 3 = Blend 37.5% = ERRORMAX * 0.75
  // 4 = blend 50%   = ERRORMAX * 1.0f
  // 5 = blend 62.5% = ERRORMAX * 0.75
  // 6 = Blend 75%   = ERRORMAX * 0.5f
  // 7 = Blend 87.5% = ERRORMAX * 0.25
  // 8 = Blend 100%  = 0%

  errorvalue = fabs(color1.r - color2.r) + fabs(color1.g - color2.g) + fabs(color1.b - color2.b);
  if (_ditherlevel == 0) errorvalue = (float)errorvalue*ERRORMAX*0.0f;
  /*
  else if (_ditherlevel == 1) errorvalue = (float)errorvalue*ERRORMAX*0.25f;
  else if (_ditherlevel == 2) errorvalue = (float)errorvalue*ERRORMAX*0.5f;
  else if (_ditherlevel == 3) errorvalue = (float)errorvalue*ERRORMAX*0.75f;
  else if (_ditherlevel == 4) errorvalue = (float)errorvalue*ERRORMAX*1.0f;
  else if (_ditherlevel == 5) errorvalue = (float)errorvalue*ERRORMAX*0.75f;
  else if (_ditherlevel == 6) errorvalue = (float)errorvalue*ERRORMAX*0.5f;
  else if (_ditherlevel == 7) errorvalue = (float)errorvalue*ERRORMAX*0.25f;
  else if (_ditherlevel == 8) errorvalue = (float)errorvalue*ERRORMAX*0.0f;
  */
  
  else if (_ditherlevel == 1) errorvalue = (float)errorvalue*ERRORMAX*0.50f;
  else if (_ditherlevel == 2) errorvalue = (float)errorvalue*ERRORMAX*0.666f;
  else if (_ditherlevel == 3) errorvalue = (float)errorvalue*ERRORMAX*0.833f;
  else if (_ditherlevel == 4) errorvalue = (float)errorvalue*ERRORMAX*1.0f;
  else if (_ditherlevel == 5) errorvalue = (float)errorvalue*ERRORMAX*0.833f;
  else if (_ditherlevel == 6) errorvalue = (float)errorvalue*ERRORMAX*0.666f;
  else if (_ditherlevel == 7) errorvalue = (float)errorvalue*ERRORMAX*0.50f;
  else if (_ditherlevel == 8) errorvalue = (float)errorvalue*ERRORMAX*0.0f;
  
}

int colorblend::getdiff(color othercolor)
{
  // Get diff of 2 colors
  int diff;
  diff = abs(r - othercolor.r) + abs(g - othercolor.g) + abs(b - othercolor.b);
  diff += errorvalue; // Add dithering error
  return diff;
}