#ifndef MARKER_HPP
#define MARKER_HPP

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/transform.hpp>


class Marker
{
  public:
    Marker();

    glm::mat4 matrix;
    glm::vec4 location;
    glm::quat rotation;

  protected:

  private:
};

#endif // MARKER_HPP
