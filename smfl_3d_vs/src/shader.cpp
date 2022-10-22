#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>

#include <GL/glew.h>

#include "shader.hpp"


static std::size_t loadTextFile(const char *inFilePath, std::string &outText)
{
  std::ifstream iftream(inFilePath, std::ios::in);
	if (! iftream.is_open())
  {
		printf("Impossible to open %s\n", inFilePath);
		return (0);
	}

  std::stringstream sstr;
  sstr << iftream.rdbuf();
  iftream.close();

  outText = sstr.str();

  return (outText.size());
}


static GLint testShaderStatus(GLuint shaderID, const char *filePath)
{
	printf("Compiling %s\n", filePath);

  GLint Result = GL_FALSE;
	int InfoLogLength = 0;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if ( InfoLogLength > 0 )
  {
		char errorMessage[255+1];
		glGetShaderInfoLog(shaderID, 255, NULL, &errorMessage[0]);
		printf("%s\n", &errorMessage[0]);
	}

	return (Result);
}


static GLint testProgramStatus(GLuint programID)
{
	printf("Linking program\n");

  GLint Result = GL_FALSE;
	int InfoLogLength = 0;
  glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);

	if ( InfoLogLength > 0 )
  {
		char errorMessage[255+1];
		glGetProgramInfoLog(programID, 255, NULL, &errorMessage[0]);
		printf("%s\n", &errorMessage[0]);
	}

	return (Result);
}


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){

	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	if (loadTextFile(vertex_file_path, VertexShaderCode) == 0)
  {
    return (0);
  }

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	if (loadTextFile(fragment_file_path, FragmentShaderCode) == 0)
  {
    return (0);
  }

	// Compile Vertex Shader
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	testShaderStatus(VertexShaderID, vertex_file_path);

	// Compile Fragment Shader
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	testShaderStatus(FragmentShaderID, fragment_file_path);

	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	testProgramStatus(ProgramID);

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}


