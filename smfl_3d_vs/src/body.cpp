#include "body.hpp"


Body::Body()
{
/*
  Linear.Mass=1.5;
  Linear.Translation.x=0;
  Linear.Translation.y=0;
  Linear.Translation.z=0;
  Linear.Velocity.x=0;
  Linear.Velocity.y=0;
  Linear.Velocity.z=0;

  Angular.Mass=500;
  Angular.Rotation=0;
  Angular.Axis.x=0;
  Angular.Axis.y=1;
  Angular.Axis.z=0;
  Angular.Velocity.x=0;
  Angular.Velocity.y=0;
  Angular.Velocity.z=0;
  */
}


void Body::NullForces()
{
  Linear.Force.x=0;
  Linear.Force.y=0;
  Linear.Force.z=0;
  Angular.Force.x=0;
  Angular.Force.y=0;
  Angular.Force.z=0;
}


glm::mat4 const *Body::RebuildMatrix()
{
  glm::mat4 ClearRotation = glm::mat4_cast(Angular.Rotation);
  glm::mat4 ClearTranslation = glm::translate(Linear.Translation);
  SizeMatrix = glm::scale(Size);
  Matrix = ClearTranslation * ClearRotation * SizeMatrix;
  return (nullptr);
}

void Body::Step()
{
/*
  Linear.Acceleration.x=Linear.Force.x/Linear.Mass;
  Linear.Acceleration.y=Linear.Force.y/Linear.Mass;
  Linear.Acceleration.z=Linear.Force.z/Linear.Mass;

  Linear.Velocity.x+=Linear.Acceleration.x;
  Linear.Velocity.y+=Linear.Acceleration.y;
  Linear.Velocity.z+=Linear.Acceleration.z;

  Linear.Translation.x+=Linear.Velocity.x;
  Linear.Translation.y+=Linear.Velocity.y;
  Linear.Translation.z+=Linear.Velocity.z;

  Angular.Acceleration.x=Angular.Force.x/Angular.Mass;
  Angular.Acceleration.y=Angular.Force.y/Angular.Mass;
  Angular.Acceleration.z=Angular.Force.z/Angular.Mass;

  Angular.Velocity.x+=Angular.Acceleration.x;
  Angular.Velocity.y+=Angular.Acceleration.y;
  Angular.Velocity.z+=Angular.Acceleration.z;

  glm::vec3 AngularTranslation;

  AngularTranslation.x=Angular.Axis.x*Angular.Rotation+Angular.Velocity.x;
  AngularTranslation.y=Angular.Axis.y*Angular.Rotation+Angular.Velocity.y;
  AngularTranslation.z=Angular.Axis.z*Angular.Rotation+Angular.Velocity.z;

  //Angular.Rotation=D3DXVec3Length((CONST glm::vec3 *)&AngularTranslation);

  if (Angular.Rotation)
  {
    Angular.Axis.x=AngularTranslation.x/Angular.Rotation;
    Angular.Axis.y=AngularTranslation.y/Angular.Rotation;
    Angular.Axis.z=AngularTranslation.z/Angular.Rotation;
  }
  */
}

void Body::CollidePlane(void const  *plane)
{
/*
  glm::vec3 v[8];
  v[0].x=Matrix._11+Matrix._21+Matrix._31;
  v[0].y=Matrix._12+Matrix._22+Matrix._32;
  v[0].z=Matrix._13+Matrix._23+Matrix._33;
  v[1].x=Matrix._11-Matrix._21+Matrix._31;
  v[1].y=Matrix._12-Matrix._22+Matrix._32;
  v[1].z=Matrix._13-Matrix._23+Matrix._33;
  v[2].x=Matrix._11+Matrix._21-Matrix._31;
  v[2].y=Matrix._12+Matrix._22-Matrix._32;
  v[2].z=Matrix._13+Matrix._23-Matrix._33;
  v[3].x=Matrix._11-Matrix._21-Matrix._31;
  v[3].y=Matrix._12-Matrix._22-Matrix._32;
  v[3].z=Matrix._13-Matrix._23-Matrix._33;
  v[4].x=-Matrix._11+Matrix._21+Matrix._31;
  v[4].y=-Matrix._12+Matrix._22+Matrix._32;
  v[4].z=-Matrix._13+Matrix._23+Matrix._33;
  v[5].x=-Matrix._11-Matrix._21+Matrix._31;
  v[5].y=-Matrix._12-Matrix._22+Matrix._32;
  v[5].z=-Matrix._13-Matrix._23+Matrix._33;
  v[6].x=-Matrix._11+Matrix._21-Matrix._31;
  v[6].y=-Matrix._12+Matrix._22-Matrix._32;
  v[6].z=-Matrix._13+Matrix._23-Matrix._33;
  v[7].x=-Matrix._11-Matrix._21-Matrix._31;
  v[7].y=-Matrix._12-Matrix._22-Matrix._32;
  v[7].z=-Matrix._13-Matrix._23-Matrix._33;

  glm::vec3 p;
  glm::float_t c;

  for (int i=0;i<8;i++)
  {
    p.x=v[i].x+Matrix._41;
    p.y=v[i].y+Matrix._42;
    p.z=v[i].z+Matrix._43;
    c=plane->a*p.x+plane->b*p.y+plane->c*p.z+plane->d;
    if (c<0)
    {
      Linear.Force.x-=plane->a*c*0.1f;
      Linear.Force.y-=plane->b*c*0.1f;
      Linear.Force.z-=plane->c*c*0.1f;
      D3DXVec3Cross((glm::vec3*)&p,(glm::vec3*)&Linear.Force,(glm::vec3*)&v[i]);
      Angular.Force.x-=p.x;
      Angular.Force.y-=p.y;
      Angular.Force.z-=p.z;
    }
  }
*/
}
