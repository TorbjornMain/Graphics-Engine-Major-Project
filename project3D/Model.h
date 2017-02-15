#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>

struct OpenGLInfo { 
	unsigned int m_VAO; 
	unsigned int m_VBO; 
	unsigned int m_faceCount;
};

struct OBJVertex { 
	float x, y, z; 
	float nx, ny, nz; 
	float u, v;
};

class Model
{
public:
	Model();
	Model(const Model &other);
	~Model();

	void draw(unsigned int shaderID, glm::mat4 camera, glm::vec4 camPos, float time, bool useTex);
	void setTransform(glm::mat4 trans) { m_transform = trans; }
	bool loadFromFile(const char* filename);
	glm::mat4 getTransform() { return m_transform; }
	void loadTex(const char* filename);
private:
	std::vector<OpenGLInfo> m_glInfo;
	glm::mat4 m_transform;
	unsigned int m_texture;
};

