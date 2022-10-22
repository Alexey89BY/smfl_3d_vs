#include "world.hpp"

namespace World
{
//--------------------------------------------------------------------------------------------------
//D3DCOLOR ColorBackground = 0x00000000;
StaticWorld *staticWorld;
DynamicWorld *dynamicWorld;
static StaticWorld C;
static DynamicWorld A, B;
static DynamicWorld *OldWorld, *NewWorld;
static glm::vec3 GravityAcceleration;

//--------------------------------------------------------------------------------------------------
static void collideBoxes(Body* outBody, Body const* firstBody, Body const* secondBody)
{
  glm::vec3 size = (firstBody->Size + secondBody->Size) * glm::float_t(0.5);
  glm::vec3 offset = firstBody->Linear.Translation - secondBody->Linear.Translation;

  // x-axis
  {
    if (
        (glm::abs(offset.x) < size.x)
     && (glm::abs(offset.y) < size.y)
     && (glm::abs(offset.z) < size.z)
       )
    {
      //outBody->Linear.Velocity.x = - outBody->Linear.Velocity.x;
      outBody->Linear.Velocity.y = - outBody->Linear.Velocity.y * 1.1f;
      //outBody->Linear.Velocity.z = - outBody->Linear.Velocity.z;

      //outBody->Linear.Translation.y = (offset.y < 0)? (secondBody->Linear.Translation.y - size.y): (secondBody->Linear.Translation.y + size.y);
      //outBody->Linear.Translation.x = (offset.x < 0)? (secondBody->Linear.Translation.x - size.x): (secondBody->Linear.Translation.x + size.x);
      //outBody->Linear.Translation.z = (offset.z < 0)? (secondBody->Linear.Translation.z - size.z): (secondBody->Linear.Translation.z + size.z);
    }
  }
}

//--------------------------------------------------------------------------------------------------
void Refresh(float TimeStep, float TimeHalfStep)
{
  NewWorld->BoxCount = OldWorld->BoxCount;

  for (int i  =0; i< NewWorld->BoxCount; i++)
  {
    Body const* OldBody = &OldWorld->Boxes[i];
    Body* NewBody = &NewWorld->Boxes[i];

    *NewBody = *OldBody;

    for (int j = 0; j < staticWorld->BoxCount; j++)
    {
      Body const* secondBody = &staticWorld->Boxes[j];
      collideBoxes(NewBody, OldBody, secondBody);
    }

    for (int j = 0; j < i; j++)
    {
      Body const* secondBody = &OldWorld->Boxes[j];
      collideBoxes(NewBody, OldBody, secondBody);
    }
  }

  for (int i  =0; i< NewWorld->BoxCount; i++)
  {
    Body* NewBody = &NewWorld->Boxes[i];

    NewBody->Linear.Translation += NewBody->Linear.Velocity * TimeStep;
    NewBody->Linear.Velocity += GravityAcceleration * TimeStep;
    //NewBody->Angular.Translation = OldBody->Angular.Translation + OldBody->Angular.Velocity*TimeStep;
    //NewBody->Angular.Velocity = glm::vec3(0.1f,0.0f,0.0f);//OldBody->AngularVelocity+0.0f*TimeStep;

    NewBody->RebuildMatrix();
  }

  dynamicWorld = NewWorld;
  NewWorld = OldWorld;
  OldWorld = dynamicWorld;
}

//--------------------------------------------------------------------------------------------------
void Begin()
{
  OldWorld = &A;
  NewWorld = &B;
  dynamicWorld = NewWorld;
  staticWorld = &C;
  GravityAcceleration = glm::vec3(0.0f, -9.81f, 0.0f);

  {
    staticWorld->BoxCount = 4;
    Body* Box = &staticWorld->Boxes[0];

    //
    Box[0].Linear.Translation.x=0;
    Box[0].Linear.Translation.y=-5;
    Box[0].Linear.Translation.z=0;
    Box[0].Size.x=100;
    Box[0].Size.y=10;
    Box[0].Size.z=100;
    //----------------------
    Box[1].Linear.Translation.x=2;
    Box[1].Linear.Translation.y=0;
    Box[1].Linear.Translation.z=0;
    Box[1].Size.x=4;
    Box[1].Size.y=0.1;
    Box[1].Size.z=0.1;
    //--------------
    Box[2].Linear.Translation.x=0;
    Box[2].Linear.Translation.y=1;
    Box[2].Linear.Translation.z=0;
    Box[2].Size.x=0.1;
    Box[2].Size.y=2;
    Box[2].Size.z=0.1;
    //--------------
    Box[3].Linear.Translation.x=0;
    Box[3].Linear.Translation.y=0;
    Box[3].Linear.Translation.z=0.5;
    Box[3].Size.x=0.1;
    Box[3].Size.y=0.1;
    Box[3].Size.z=1;

    for (int i=0; i< staticWorld->BoxCount; i++)
    {
      Box[i].RebuildMatrix();
    }
  }

  OldWorld->BoxCount = 60;
  Body* Box = &OldWorld->Boxes[0];

  //--------------
  Box[3].Linear.Translation.x=0;
  Box[3].Linear.Translation.y=1;
  Box[3].Linear.Translation.z=2;
  Box[3].Size.x=2;
  Box[3].Size.y=2;
  Box[3].Size.z=2;
  //--------------
  Box[4].Linear.Translation.x=-1.5;
  Box[4].Linear.Translation.y=0.5f;
  Box[4].Linear.Translation.z=-1.5;
  Box[4].Size.x=1;
  Box[4].Size.y=1;
  Box[4].Size.z=1;
  //--------------
  Box[5].Linear.Translation.x=0;
  Box[5].Linear.Translation.y=2;
  Box[5].Linear.Translation.z=-2;
  Box[5].Size.x=2;
  Box[5].Size.y=2;
  Box[5].Size.z=2;
  //--------------
  Box[6].Linear.Translation.x=1.5;
  Box[6].Linear.Translation.y=0.5f;
  Box[6].Linear.Translation.z=-1.5;
  Box[6].Size.x=1;
  Box[6].Size.y=1;
  Box[6].Size.z=1;
  //--------------
  Box[7].Linear.Translation.x=0;
  Box[7].Linear.Translation.y=3;
  Box[7].Linear.Translation.z=0;
  Box[7].Size.x=2;
  Box[7].Size.y=2;
  Box[7].Size.z=2;
  //--------------
  //--------------
  Box[8].Linear.Translation.x=0;
  Box[8].Linear.Translation.y=0.5;
  Box[8].Linear.Translation.z=-4;
  Box[8].Size.x=1;
  Box[8].Size.y=1;
  Box[8].Size.z=1;
  //--------------
  Box[9].Linear.Translation.x=0;
  Box[9].Linear.Translation.y=1.5;
  Box[9].Linear.Translation.z=-5;
  Box[9].Size.x=1;
  Box[9].Size.y=1;
  Box[9].Size.z=1;
  //--------------
  Box[10].Linear.Translation.x=0;
  Box[10].Linear.Translation.y=2.5;
  Box[10].Linear.Translation.z=-6;
  Box[10].Size.x=1;
  Box[10].Size.y=1;
  Box[10].Size.z=1;
  //--------------
  Box[11].Linear.Translation.x=0;
  Box[11].Linear.Translation.y=3.5;
  Box[11].Linear.Translation.z=-7;
  Box[11].Size.x=1;
  Box[11].Size.y=1;
  Box[11].Size.z=1;
  //--------------
  Box[12].Linear.Translation.x=0;
  Box[12].Linear.Translation.y=4.5;
  Box[12].Linear.Translation.z=-8;
  Box[12].Size.x=1;
  Box[12].Size.y=1;
  Box[12].Size.z=1;
  //--------------
  Box[13].Linear.Translation.x=0;
  Box[13].Linear.Translation.y=5.5;
  Box[13].Linear.Translation.z=-9;
  Box[13].Size.x=1;
  Box[13].Size.y=1;
  Box[13].Size.z=1;
  //--------------
  Box[14].Linear.Translation.x=0;
  Box[14].Linear.Translation.y=6.5;
  Box[14].Linear.Translation.z=-10;
  Box[14].Size.x=1;
  Box[14].Size.y=1;
  Box[14].Size.z=1;
  //--------------
  //--------------
  Box[15].Linear.Translation.x=-20;
  Box[15].Linear.Translation.y=5;
  Box[15].Linear.Translation.z=-20;
  Box[15].Size.x=10;
  Box[15].Size.y=10;
  Box[15].Size.z=10;
  //--------------
  Box[16].Linear.Translation.x=20;
  Box[16].Linear.Translation.y=5;
  Box[16].Linear.Translation.z=-20;
  Box[16].Size.x=10;
  Box[16].Size.y=10;
  Box[16].Size.z=10;
  //--------------
  Box[17].Linear.Translation.x=-20;
  Box[17].Linear.Translation.y=5;
  Box[17].Linear.Translation.z=20;
  Box[17].Size.x=10;
  Box[17].Size.y=10;
  Box[17].Size.z=10;
  //--------------
  Box[18].Linear.Translation.x=20;
  Box[18].Linear.Translation.y=5;
  Box[18].Linear.Translation.z=20;
  Box[18].Size.x=10;
  Box[18].Size.y=10;
  Box[18].Size.z=10;
  //--------------
  //--------------
  Box[19].Linear.Translation.x=12.5;
  Box[19].Linear.Translation.y=7.5;
  Box[19].Linear.Translation.z=-20;
  Box[19].Size.x=5;
  Box[19].Size.y=5;
  Box[19].Size.z=5;
  //--------------
  Box[20].Linear.Translation.x=20;
  Box[20].Linear.Translation.y=7.5;
  Box[20].Linear.Translation.z=-12.5;
  Box[20].Size.x=5;
  Box[20].Size.y=5;
  Box[20].Size.z=5;
  //--------------
  Box[21].Linear.Translation.x=-20;
  Box[21].Linear.Translation.y=7.5;
  Box[21].Linear.Translation.z=12.5;
  Box[21].Size.x=5;
  Box[21].Size.y=5;
  Box[21].Size.z=5;
  //--------------
  Box[22].Linear.Translation.x=-12.5;
  Box[22].Linear.Translation.y=7.5;
  Box[22].Linear.Translation.z=20;
  Box[22].Size.x=5;
  Box[22].Size.y=5;
  Box[22].Size.z=5;
  //--------------
  //--------------
  Box[22].Linear.Translation.x=0;
  Box[22].Linear.Translation.y=9;
  Box[22].Linear.Translation.z=-14;
  Box[22].Size.x=2;
  Box[22].Size.y=2;
  Box[22].Size.z=2;
  //--------------
  Box[23].Linear.Translation.x=-2;
  Box[23].Linear.Translation.y=9;
  Box[23].Linear.Translation.z=-10;
  Box[23].Size.x=2;
  Box[23].Size.y=2;
  Box[23].Size.z=2;
  //--------------
  Box[24].Linear.Translation.x=-4;
  Box[24].Linear.Translation.y=9;
  Box[24].Linear.Translation.z=-14;
  Box[24].Size.x=2;
  Box[24].Size.y=2;
  Box[24].Size.z=2;
  //--------------
  Box[25].Linear.Translation.x=-6;
  Box[25].Linear.Translation.y=9;
  Box[25].Linear.Translation.z=-10;
  Box[25].Size.x=2;
  Box[25].Size.y=2;
  Box[25].Size.z=2;
  //--------------
  Box[26].Linear.Translation.x=-8;
  Box[26].Linear.Translation.y=9;
  Box[26].Linear.Translation.z=-14;
  Box[26].Size.x=2;
  Box[26].Size.y=2;
  Box[26].Size.z=2;
  //--------------
  Box[27].Linear.Translation.x=-10;
  Box[27].Linear.Translation.y=9;
  Box[27].Linear.Translation.z=-10;
  Box[27].Size.x=2;
  Box[27].Size.y=2;
  Box[27].Size.z=2;
  //--------------
  Box[28].Linear.Translation.x=-12;
  Box[28].Linear.Translation.y=9;
  Box[28].Linear.Translation.z=-14;
  Box[28].Size.x=2;
  Box[28].Size.y=2;
  Box[28].Size.z=2;
  //--------------
  Box[29].Linear.Translation.x=-14;
  Box[29].Linear.Translation.y=9;
  Box[29].Linear.Translation.z=-10;
  Box[29].Size.x=2;
  Box[29].Size.y=2;
  Box[29].Size.z=2;
  //--------------
  Box[30].Linear.Translation.x=2;
  Box[30].Linear.Translation.y=9;
  Box[30].Linear.Translation.z=-14;
  Box[30].Size.x=2;
  Box[30].Size.y=2;
  Box[30].Size.z=2;
  //--------------
  Box[31].Linear.Translation.x=4;
  Box[31].Linear.Translation.y=9;
  Box[31].Linear.Translation.z=-10;
  Box[31].Size.x=2;
  Box[31].Size.y=2;
  Box[31].Size.z=2;
  //--------------
  Box[32].Linear.Translation.x=6;
  Box[32].Linear.Translation.y=9;
  Box[32].Linear.Translation.z=-14;
  Box[32].Size.x=2;
  Box[32].Size.y=2;
  Box[32].Size.z=2;
  //--------------
  Box[33].Linear.Translation.x=8;
  Box[33].Linear.Translation.y=9;
  Box[33].Linear.Translation.z=-10;
  Box[33].Size.x=2;
  Box[33].Size.y=2;
  Box[33].Size.z=2;
  //--------------
  Box[34].Linear.Translation.x=10;
  Box[34].Linear.Translation.y=9;
  Box[34].Linear.Translation.z=-14;
  Box[34].Size.x=2;
  Box[34].Size.y=2;
  Box[34].Size.z=2;
  //--------------
  Box[35].Linear.Translation.x=12;
  Box[35].Linear.Translation.y=9;
  Box[35].Linear.Translation.z=-10;
  Box[35].Size.x=2;
  Box[35].Size.y=2;
  Box[35].Size.z=2;
  //--------------
  Box[36].Linear.Translation.x=14;
  Box[36].Linear.Translation.y=9;
  Box[36].Linear.Translation.z=-14;
  Box[36].Size.x=2;
  Box[36].Size.y=2;
  Box[36].Size.z=2;
  //--------------
  Box[37].Linear.Translation.x=0;
  Box[37].Linear.Translation.y=6;
  Box[37].Linear.Translation.z=-12;
  Box[37].Size.x=2;
  Box[37].Size.y=2;
  Box[37].Size.z=2;
  //--------------
  Box[38].Linear.Translation.x=-2;
  Box[38].Linear.Translation.y=7;
  Box[38].Linear.Translation.z=-12;
  Box[38].Size.x=2;
  Box[38].Size.y=2;
  Box[38].Size.z=2;
  //--------------
  Box[39].Linear.Translation.x=-2;
  Box[39].Linear.Translation.y=8.5;
  Box[39].Linear.Translation.z=-12.5;
  Box[39].Size.x=1;
  Box[39].Size.y=1;
  Box[39].Size.z=1;
  //
  Box[40].Linear.Translation.x=0;
  Box[40].Linear.Translation.y=9;
  Box[40].Linear.Translation.z=16;
  Box[40].Size.x=2;
  Box[40].Size.y=2;
  Box[40].Size.z=2;
  //
  Box[41].Linear.Translation.x=2;
  Box[41].Linear.Translation.y=9;
  Box[41].Linear.Translation.z=14;
  Box[41].Size.x=2;
  Box[41].Size.y=2;
  Box[41].Size.z=2;
  //
  Box[42].Linear.Translation.x=4;
  Box[42].Linear.Translation.y=9;
  Box[42].Linear.Translation.z=10;
  Box[42].Size.x=2;
  Box[42].Size.y=2;
  Box[42].Size.z=2;
  //
  Box[43].Linear.Translation.x=6;
  Box[43].Linear.Translation.y=9;
  Box[43].Linear.Translation.z=14;
  Box[43].Size.x=2;
  Box[43].Size.y=2;
  Box[43].Size.z=2;
  //
  Box[44].Linear.Translation.x=8;
  Box[44].Linear.Translation.y=9;
  Box[44].Linear.Translation.z=10;
  Box[44].Size.x=2;
  Box[44].Size.y=2;
  Box[44].Size.z=2;
  //
  Box[45].Linear.Translation.x=10;
  Box[45].Linear.Translation.y=9;
  Box[45].Linear.Translation.z=14;
  Box[45].Size.x=2;
  Box[45].Size.y=2;
  Box[45].Size.z=2;
  //
  Box[46].Linear.Translation.x=12;
  Box[46].Linear.Translation.y=9;
  Box[46].Linear.Translation.z=10;
  Box[46].Size.x=2;
  Box[46].Size.y=2;
  Box[46].Size.z=2;
  //
  Box[47].Linear.Translation.x=14;
  Box[47].Linear.Translation.y=9;
  Box[47].Linear.Translation.z=14;
  Box[47].Size.x=2;
  Box[47].Size.y=2;
  Box[47].Size.z=2;
  //
  Box[48].Linear.Translation.x=-2;
  Box[48].Linear.Translation.y=9;
  Box[48].Linear.Translation.z=10;
  Box[48].Size.x=2;
  Box[48].Size.y=2;
  Box[48].Size.z=2;
  //
  Box[50].Linear.Translation.x=-4;
  Box[50].Linear.Translation.y=9;
  Box[50].Linear.Translation.z=14;
  Box[50].Size.x=2;
  Box[50].Size.y=2;
  Box[50].Size.z=2;
  //
  Box[51].Linear.Translation.x=-6;
  Box[51].Linear.Translation.y=9;
  Box[51].Linear.Translation.z=10;
  Box[51].Size.x=2;
  Box[51].Size.y=2;
  Box[51].Size.z=2;
  //
  Box[52].Linear.Translation.x=-8;
  Box[52].Linear.Translation.y=9;
  Box[52].Linear.Translation.z=14;
  Box[52].Size.x=2;
  Box[52].Size.y=2;
  Box[52].Size.z=2;
  //
  Box[53].Linear.Translation.x=-10;
  Box[53].Linear.Translation.y=9;
  Box[53].Linear.Translation.z=10;
  Box[53].Size.x=2;
  Box[53].Size.y=2;
  Box[53].Size.z=2;
  //
  Box[54].Linear.Translation.x=-12;
  Box[54].Linear.Translation.y=9;
  Box[54].Linear.Translation.z=14;
  Box[54].Size.x=2;
  Box[54].Size.y=2;
  Box[54].Size.z=2;
  //
  Box[55].Linear.Translation.x=-14;
  Box[55].Linear.Translation.y=9;
  Box[55].Linear.Translation.z=14;
  Box[55].Size.x=2;
  Box[55].Size.y=2;
  Box[55].Size.z=2;
  //
  Box[56].Linear.Translation.x=0;
  Box[56].Linear.Translation.y=8;
  Box[56].Linear.Translation.z=10;
  Box[56].Size.x=2;
  Box[56].Size.y=2;
  Box[56].Size.z=2;
  //
  Box[57].Linear.Translation.x=0;
  Box[57].Linear.Translation.y=7;
  Box[57].Linear.Translation.z=8;
  Box[57].Size.x=2;
  Box[57].Size.y=2;
  Box[57].Size.z=2;
  //
  Box[58].Linear.Translation.x=0;
  Box[58].Linear.Translation.y=6;
  Box[58].Linear.Translation.z=4;
  Box[58].Size.x=2;
  Box[58].Size.y=2;
  Box[58].Size.z=2;
  //
  Box[59].Linear.Translation.x=0;
  Box[59].Linear.Translation.y=5;
  Box[59].Linear.Translation.z=2;
  Box[59].Size.x=2;
  Box[59].Size.y=2;
  Box[59].Size.z=2;
}

//--------------------------------------------------------------------------------------------------
void End()
{

}

//--------------------------------------------------------------------------------------------------

}
