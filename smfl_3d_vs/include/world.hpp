#ifndef WORLD_HPP
#define WORLD_HPP


#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include "body.hpp"

//--------------------------------------------------------------------------------------------------
namespace World
{

class StaticWorld
{
public:
    int BoxCount;
    Body Boxes[4];

  protected:

  private:

};


class DynamicWorld
{
  public:
    int BoxCount;
    Body Boxes[100];

    //D3DXQUATERNION PlayerBodyRotation;
    //D3DXMATRIX PlayerBodyRotationMatrix;
    //D3DXQUATERNION PlayerCameraRotation;
    //D3DXVECTOR3 PlayerPosition;
    //D3DXVECTOR3 PlayerVelocity;
    //D3DXVECTOR3 PlayerAcceleration;

    //CBody Boxes[BoxCount];

  protected:

  private:

};

//D3DCOLOR ColorBackground = 0x00000000;
//DWORD FlagsClear = D3DCLEAR_ZBUFFER | D3DCLEAR_TARGET;
//--------------------------------------------------------------------------------------------------

extern StaticWorld *staticWorld;
extern DynamicWorld *dynamicWorld;


void Begin();
void Refresh(float TimeStep, float TimeHalfStep);
void End();

}

#endif // WORLD_HPP
