#include "sprite2d.hpp"

#include <GL/glew.h>

#include <glm/glm.hpp>
#include "shader.hpp"


const unsigned int vertexesNumber = 4;

struct VERTEX
{
  glm::vec2 p;
  glm::vec2 t;
};

static unsigned int vertexArrayID;
static unsigned int vertexBufferID;
static unsigned int shaderID;
static unsigned int uniformID;
static unsigned int matrixMvpID;
static unsigned int matrixUvID;
static unsigned int colorID;
static glm::mat3 textureMatrix;
static glm::vec4 colorMask;


void Sprite2D::initialize()
{
  // Initialize VBO
  static VERTEX vertexes[vertexesNumber] =
  {
    {{-1.0f, 1.0f}, {0.0f, 0.0f}},
    {{-1.0f, -1.0f}, {0.0f, 1.0f}},
    {{1.0f, 1.0f}, {1.0f, 0.0f}},
    {{1.0f, -1.0f}, {1.0f, 1.0f}},
  };

	glGenVertexArrays(1, &vertexArrayID);
	glGenBuffers(1, &vertexBufferID);

  glBindVertexArray(vertexArrayID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertexes), &vertexes[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)offsetof(VERTEX, p) );
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VERTEX), (void*)offsetof(VERTEX, t) );
  glBindVertexArray(0);

  	// Initialize Shader
	shaderID = LoadShaders( "shaders/SpriteShader.vert", "shaders/SpriteShader.frag" );

	// Initialize uniforms' IDs
	uniformID = glGetUniformLocation( shaderID, "myTextureSampler" );
  colorID = glGetUniformLocation( shaderID, "myColorMask" );
  matrixMvpID = glGetUniformLocation( shaderID, "matrixMVP" );
  matrixUvID = glGetUniformLocation( shaderID, "matrixUV" );
}


void Sprite2D::cleanup()
{
  glDeleteVertexArrays(1, &vertexArrayID);
	glDeleteBuffers(1, &vertexBufferID);
	glDeleteProgram(shaderID);
}


void Sprite2D::begin()
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Bind shader
  glUseProgram(shaderID);
	//glUseProgram(Text2DShaderID);
	glBindVertexArray(vertexArrayID);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(uniformID, 0);
}


void Sprite2D::setTexture(unsigned int textureID)
{
  glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
}


void Sprite2D::setColor(float colorR, float colorB, float colorG, float colorA)
{
  colorMask.x = colorR;
  colorMask.y = colorG;
  colorMask.z = colorB;
  colorMask.w = colorA;
}


void Sprite2D::setRect(float left, float top, float width, float height)
{
  glm::mat3 &m = textureMatrix;
  m[0][0] = width,  m[0][1] = 0.0f,   m[0][2] = 0.0f;
  m[1][0] = 0.0f,   m[1][1] = height, m[1][2] = 0.0f;
  m[2][0] = left,   m[2][1] = top,    m[2][2] = 1.0f;
}


void Sprite2D::draw(float posX, float posY, float sizeX, float sizeY)
{
  {
    glm::mat3 m;
    m[0][0] = sizeX,  m[0][1] = 0.0f,   m[0][2] = 0.0f,
    m[1][0] = 0.0f,   m[1][1] = sizeY,  m[1][2] = 0.0f,
    m[2][0] = posX,   m[2][1] = posY,   m[2][2] = 1.0f;
    glUniformMatrix3fv(matrixMvpID, 1, GL_FALSE, &m[0][0]);
  }

  glUniformMatrix3fv(matrixUvID, 1, GL_FALSE, &textureMatrix[0][0]);
  glUniform4fv(colorID, 1, &colorMask.x);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, vertexesNumber );
}


void Sprite2D::end()
{
  glUseProgram(0);
  //glBindTexture(GL_TEXTURE_2D, 0);
  glBindVertexArray(0);
}
