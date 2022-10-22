#version 330 core

in vec2 UV;
uniform sampler2D myTextureSampler;
uniform mat3 matrixUV;
uniform vec4 myColorMask;

// Ouput data
out vec4 color;

void main()
{
	vec3 texel = matrixUV * vec3( UV, 0 );
	color = texture( myTextureSampler, texel.xy ) * myColorMask;
}

