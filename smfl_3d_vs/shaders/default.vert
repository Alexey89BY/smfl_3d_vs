// vertex shader
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
//layout(location = 2) in vec3 vertexColor;
uniform mat4 matrixMVP;
out vec2 pixelUV;
//out vec3 fragmentColor;

void main()
{
  gl_Position = matrixMVP * vec4( vertexPosition_modelspace, 1 );
  pixelUV = vertexUV;
  //fragmentColor = vertexColor;
}
