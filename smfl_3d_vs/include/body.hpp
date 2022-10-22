#ifndef BODY_HPP
#define BODY_HPP

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>


class Body
{
public:
  struct LINEAR_MOTION {
    glm::vec3 Force;
    glm::vec3 Translation;
    glm::vec3 Velocity;
    glm::vec3 Acceleration;
    glm::vec3 Mass;
  } Linear;

  struct ANGULAR_MOTION {
    glm::vec3 Force;
    glm::vec3 Translation;
    glm::quat Rotation;
    glm::vec3 Velocity;
    glm::vec3 Acceleration;
    glm::vec3 Mass;
  } Angular;

  glm::vec3 Size;
  glm::mat4 SizeMatrix;
  glm::mat4 Matrix;

  Body();
  glm::mat4 const *RebuildMatrix();
  void NullForces();
  void Step();
  void CollidePlane(void const *plane);

protected:

private:
};


#endif // BODY_HPP
