#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;
uniform mat3 matrixMVP;
// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main()
{
	gl_Position.xyz = matrixMVP * vec3(vertexPosition_screenspace, 0);
	gl_Position.w = 1;
	UV = vertexUV;
}
