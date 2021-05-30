// Class to manage a RGB color and do operation
// 3 component 

class color
{
public:
 color();
 color(int r, int g, int b);
 color(unsigned char* buffer);
 ~color();

 void setandaddbrighnessfloat(float coef); // 0 to 2.0f (1.0f is normal)
 void writetobuffer(unsigned char* buff);
 void writetobufferwithbrightness(unsigned char* buff, float brightness);
 void writetobufferas16bits(unsigned char* buff);
 void convertto16bits();
 int getdiff(color othercolor);
 
 int r,g,b;
};

int compare(const void* a, const void* b);

// Class blended color.
// Two color with blend between them
class colorblend
{
public:
  colorblend();
  colorblend(int indexcolor1, int indexcolor2, int ditherlevel, float ditherstepvalue);
  ~colorblend();

  void set(int indexcolor1, int indexcolor2, int ditherlevel, float ditherstepvalue);
  int getdiff(color othercolor);

  //void writetobuffer(unsigned char* buff);
  //void writetobufferwithbrightness(unsigned char* buff, float brightness);
  //void writetobufferas16bits(unsigned char* buff);
  //void convertto16bits();
public:
  int indexcolor1;
  int indexcolor2;
  int ditherlevel; // 0 to 8 (0% to 100%)

  int r, g, b; // Obtained color
  int errorvalue; // Error from real color.

  static color* AllColors; // Only defintion , have no existance
};


