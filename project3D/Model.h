#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <FBXFile.h>
#include "FrameBuffer.h"
#include "typedefs.h"

struct OpenGLInfo { 
	uint m_VAO; 
	uint m_VBO; 
	uint m_IBO = -1;
	uint m_faceCount;
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
	void draw(uint shaderID, glm::mat4 camera, glm::vec4 camPos, float time, uint textureID, glm::mat4 transform);
	void drawPostProcessQuad(uint shaderID, FrameBuffer buf);
	void drawGenericScreenQuad();
	void drawInstancedGenericScreenQuad(uint numSlices);

	bool load(const char* filename);
	void generateScreenSpaceQuad();

	glm::vec4 getUpperBound() { return m_upperBound; }
	void setUpperBound(glm::vec4 val) { m_upperBound = val; }
	glm::vec4 getLowerBound() { return m_lowerBound; }
	void setLowerBound(glm::vec4 val) { m_lowerBound = val; }

	bool getIsAnimated() { return m_isAnimated; }
private:
	std::vector<OpenGLInfo> m_glInfo;
	glm::vec4 m_upperBound;
	glm::vec4 m_lowerBound;
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

	uint getTexture() { return m_texture; }
	void setTexture(uint tex) { m_texture = tex; }

	uint getShader() { return m_shader; }
	void setShader(uint shad) { m_shader = shad; }

	Model* getModel() { return m_model; }
	void setModel(Model* model) { m_model = model; }

	void loadTex(const char * filename);

	void draw(glm::mat4 camera, glm::mat4 camTransform, float time, float animTime);
private:
	glm::mat4 m_transform;

	uint m_texture;
	uint m_shader;
	Model* m_model;

};

