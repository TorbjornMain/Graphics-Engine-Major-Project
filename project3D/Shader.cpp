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

void Shader::CompileShaders(const char * vert, const char * frag, const char* geom)
{
	int success = GL_FALSE;
	uint vertShader = glCreateShader(GL_VERTEX_SHADER);
	uint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	uint geomShader;
	loadAndCompileShader(vertShader, vert);
	loadAndCompileShader(fragShader, frag);
	m_id = glCreateProgram();
	glAttachShader(m_id, vertShader);
	glAttachShader(m_id, fragShader);

	//check if geometry shader exists
	if (geom != nullptr)
	{
		geomShader = glCreateShader(GL_GEOMETRY_SHADER);
		loadAndCompileShader(geomShader, geom);
		glAttachShader(m_id, geomShader);
	}
	glLinkProgram(m_id);

	//Error checking
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
	if (geom != nullptr) glDeleteShader(geomShader);
}

void Shader::CompileUpdateShader(const char * vert, const char ** varyings, uint numVaryings)
{
	int success = GL_FALSE;
	uint vs = glCreateShader(GL_VERTEX_SHADER);
	loadAndCompileShader(vs, vert);
	m_id = glCreateProgram();
	glAttachShader(m_id, vs);
	glTransformFeedbackVaryings(m_id, numVaryings, varyings, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(m_id);
	
	//Error Check
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
	glDeleteShader(vs);
}

void Shader::loadAndCompileShader(uint shaderID, const char * filename)
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
