#include <vector>
#include <cstring>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.hpp"
#include "texture.hpp"

#include "text2D.hpp"

static const int textLimit = 80;

// Fill buffers
struct VERTEX
{
  glm::vec2 p;
  glm::vec2 t;
};

struct UV
{
  glm::vec2 uv_up_left;
  glm::vec2 uv_down_right;
};

struct VERTEX_QUAD
{
  VERTEX up_left;
  VERTEX down_left;
  VERTEX up_right;
  VERTEX down_right;
};

static unsigned int Text2DTextureID;
static unsigned int Text2DVertexArrayID;
static unsigned int Text2DVertexBufferID;
static unsigned int Text2DShaderID;
static unsigned int Text2DUniformID;
static unsigned int Text2DColorID;

static glm::vec4 colorMask;
static UV uvs[256];
static char textBuffer[textLimit+1];
static VERTEX_QUAD vertices[textLimit]; //0, 1, 2, 3, 2, 1


void Text2D::initialize(int texturePath, int width, int height, int gryphsX, int gryphsY)
{
	// Initialize texture
	Text2DTextureID = texturePath;// loadDDS(texturePath);

	glm::vec2 uvShift = glm::vec2 (0.5f / width, 0.5f / height);
  glm::vec2 gryphSize = glm::vec2(1.0f / gryphsX, 1.0f / gryphsY);

	for (unsigned int character = 0; character < sizeof(uvs)/sizeof(uvs[0]); ++character)
  {
		UV &uv = uvs[character];
    uv.uv_up_left.x = (character % gryphsX) * gryphSize.x - uvShift.x;
		uv.uv_up_left.y = (character / gryphsX) * gryphSize.y - uvShift.y;

		uv.uv_down_right = uv.uv_up_left + glm::vec2(gryphSize.x, gryphSize.y) ;
  }

  	// Initialize VBO
	glGenVertexArrays(1, &Text2DVertexArrayID);
	glGenBuffers(1, &Text2DVertexBufferID);
  glBindVertexArray(Text2DVertexArrayID);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)offsetof(VERTEX, p) );
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)offsetof(VERTEX, t) );
  glBindVertexArray(0);

	// Initialize Shader
	Text2DShaderID = LoadShaders( "shaders/TextVertexShader.vertexshader", "shaders/TextVertexShader.fragmentshader" );

	// Initialize uniforms' IDs
	Text2DUniformID = glGetUniformLocation( Text2DShaderID, "myTextureSampler" );
  Text2DColorID = glGetUniformLocation( Text2DShaderID, "myColorMask" );
}


void Text2D::begin()
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Bind shader
	glUseProgram(Text2DShaderID);
	glBindVertexArray(Text2DVertexArrayID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Text2DTextureID);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(Text2DUniformID, 0);
}


int Text2D::limit()
{
  return (textLimit);
}


char *Text2D::buffer()
{
  return (&textBuffer[0]);
}


void Text2D::setColor(float colorR, float colorB, float colorG, float colorA)
{
  colorMask.x = colorR;
  colorMask.y = colorG;
  colorMask.z = colorB;
  colorMask.w = colorA;
}


void Text2D::draw(float x, float y, float sizeX, float sizeY)
{
	auto index = 0;
	while ( index < textLimit )
  {
		auto character = static_cast<unsigned char>(textBuffer[index]);
		if (character == 0)
    {
      break;
    }

    VERTEX_QUAD &vertex = vertices[index];

		vertex.up_left.p = glm::vec2( x + index * sizeX, y );
		vertex.down_right.p = vertex.up_left.p + glm::vec2( sizeX, sizeY );
		vertex.up_right.p = glm::vec2(vertex.down_right.p.x, vertex.up_left.p.y);
		vertex.down_left.p = glm::vec2(vertex.up_left.p.x, vertex.down_right.p.y);

		UV &uv = uvs[character];

		vertex.up_left.t = uv.uv_up_left;
		vertex.down_right.t = uv.uv_down_right;
		vertex.up_right.t = glm::vec2(uv.uv_down_right.x, uv.uv_up_left.y);
		vertex.down_left.t = glm::vec2(uv.uv_up_left.x, uv.uv_down_right.y);

    ++index;
	}

  auto vertexesNumber = index * (sizeof(VERTEX_QUAD) / sizeof(VERTEX));

  glUniform4fv(Text2DColorID, 1, &colorMask.x);


  glBufferData(GL_ARRAY_BUFFER, vertexesNumber * sizeof(VERTEX), &vertices[0], GL_STATIC_DRAW);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexesNumber );
}


void Text2D::end()
{
  glUseProgram(0);
  glBindVertexArray(0);
}


void Text2D::cleanup()
{
  glDeleteVertexArrays(1, &Text2DVertexArrayID);
	glDeleteBuffers(1, &Text2DVertexBufferID);
	glDeleteTextures(1, &Text2DTextureID);
	glDeleteProgram(Text2DShaderID);
}
