#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <glm/glm.hpp>


extern bool g_playerIsForwardPressed;
extern bool g_playerIsBackwardPressed;
extern bool g_playerIsUpPressed;
extern bool g_playerIsDownPressed;
extern bool g_playerIsRightPressed;
extern bool g_playerIsLeftPressed;

//enum PLAYER_CONTROL
//static const int g_playerControlsNumber = 6;
//extern struct PLAYER_CONTROL
//{
//  int button;
//  bool state;
//} g_playerControls[g_playerControlsNumber];

class Player
{
  public:
    Player();
    virtual ~Player();

    static void setLookXY(float deltaX, float deltaY);
    static void setLookZ(float deltaZ);
    static void setLookW(float deltaW);
    static void setControls(int keyCode, bool isPressed);
    static void setViewAspect(float viewAspect);

    static void update();

    static void resetView();
    static glm::mat4 const & getViewHeadMatrix();
    static glm::mat4 const & getViewBodyMatrix();
    static glm::mat4 const & getProjectionMatrix();
    static void computeMatricesFromInputs(float deltaTime);

    static glm::vec3 getPosition();
    static glm::vec3 getDirection();
    static glm::vec3 getUpVector();

  protected:

  private:
};



#endif // PLAYER_HPP
