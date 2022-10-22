// fragment shader
#version 330 core

uniform sampler2D textureSampler;
uniform mat4 matrixUV;
in vec2 pixelUV;
//in vec3 fragmentColor;
out vec4 color;

void main()
{
  vec4 texel = matrixUV * vec4( pixelUV, 0, 1 );
  color = texture( textureSampler, texel.xy ).rgba;
  //color = fragmentColor;
}
