#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <FBXFile.h>

struct OpenGLInfo { 
	unsigned int m_VAO; 
	unsigned int m_VBO; 
	unsigned int m_IBO = -1;
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


	void update(float time);
	void draw(unsigned int shaderID, glm::mat4 camera, glm::vec4 camPos, float time, unsigned int textureID, glm::mat4 transform);

	bool loadFromOBJ(const char* filename);
	bool loadFromFBX(const char* filename);
	
	bool getIsAnimated() { return m_isAnimated; }
private:
	std::vector<OpenGLInfo> m_glInfo;
	bool m_isAnimated = false;
	FBXFile* m_model;
};

class Instance
{
public:
	Instance();
	~Instance();
	Instance(const Instance &other);

	void setTransform(glm::mat4 trans) { m_transform = trans; }
	glm::mat4 getTransform() { return m_transform; }

	unsigned int getTexture() { return m_texture; }
	void setTexture(unsigned int tex) { m_texture = tex; }

	unsigned int getShader() { return m_shader; }
	void setShader(unsigned int shad) { m_shader = shad; }

	Model* getModel() { return m_model; }
	void setModel(Model* model) { m_model = model; }

	void loadTex(const char * filename);

	void draw(glm::mat4 camera, glm::vec4 camPos, float time, float animTime);
private:
	glm::mat4 m_transform;
	unsigned int m_texture;
	unsigned int m_shader;
	Model* m_model;

};

