#ifndef TEXT2D_HPP
#define TEXT2D_HPP

class Text2D
{
public:
  static void initialize(int texturePath, int width, int height, int gryphsX, int gryphsY);
  static void cleanup();

  static void begin();
  static int limit();
  static char * buffer();
  static void setColor(float colorR, float colorB, float colorG, float colorA);
  static void draw(float x, float y, float sizeX, float sizeY);
  static void end();
};

#endif
