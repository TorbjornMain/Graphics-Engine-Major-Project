#include "Shader.h"
#include <gl_core_4_4.h>
#include <string>
#include <fstream>
#include <iostream>

Shader::Shader()
{
}


Shader::~Shader()
{
}

void Shader::CompileShaders(const char * vert, const char * frag)
{
	int success = GL_FALSE;
	unsigned int vertShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	loadAndCompileShader(vertShader, vert);
	loadAndCompileShader(fragShader, frag);
	m_id = glCreateProgram();
	glAttachShader(m_id, vertShader);
	glAttachShader(m_id, fragShader);
	glLinkProgram(m_id);

	glGetProgramiv(m_id, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_id, infoLogLength, 0, infoLog);
		std::cout << "RIP, Linking failed" << std::endl;
		std::cout << infoLog;

		delete[] infoLog;
	}

	glDeleteShader(fragShader);
	glDeleteShader(vertShader);
}

void Shader::loadAndCompileShader(unsigned int shaderID, const char * filename)
{
	std::ifstream file(filename);
	std::string str;
	std::string fullFile;
	while (std::getline(file, str))
	{
		fullFile += str;
		fullFile.push_back('\n');
	}

	const char * cstr = fullFile.c_str();

	glShaderSource(shaderID, 1, (const char**)&cstr, 0);
	glCompileShader(shaderID);
}
