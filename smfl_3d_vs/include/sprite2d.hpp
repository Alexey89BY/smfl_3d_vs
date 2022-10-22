#ifndef SPRITE2D_HPP
#define SPRITE2D_HPP


class Sprite2D
{
public:
  static void initialize();
  static void cleanup();

  static void begin();
  static void setTexture(unsigned int texture);
  static void setColor(float colorR, float colorB, float colorG, float colorA);
  static void setRect(float left, float top, float width, float height);
  static void draw(float posX, float posY, float sizeX, float sizeY);
  static void end();
};


#endif // SPRITE2D_HPP
