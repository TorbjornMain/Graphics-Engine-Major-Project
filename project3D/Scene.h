#pragma once
#include <map>
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FrameBuffer.h"
#include "ParticleSystem.h"
#include "typedefs.h"

struct Camera
{
	glm::mat4 projection;
	glm::mat4 view;
	glm::mat4 transform;
	float frustumCentreZ;
};

class Scene
{
public:
	Scene();
	~Scene();

	void drawToRenderTarget(const Camera& renderCam, FrameBuffer& buf, float time, int screenWidth, int screenHeight);
	void draw(float time);
	void AddInstance(char* name, Model* model, uint shader, uint texture, glm::mat4 transform);
	void AddInstance(char* name, Model* model, uint shader, const char* textureFile, glm::mat4 transform);
	void AddParticleSystem(char* name, glm::vec3 position, uint upShader, uint dShader, uint numParticles = 100000);
	ParticleSystem& GetParticleSystem(char* name);
	Instance& GetInstance(char* name);
	Camera getCamera() { return m_camera; }
	void setCamera(Camera cam) { m_camera = cam; };

	bool b_renderParticles = true;
	bool b_renderModels = true;
	bool b_renderGizmos = false;

private:
	std::map<char* , Instance> m_instances;
	std::map<char*, ParticleSystem> m_particleSystems;
	glm::vec3 lightDir;
	Camera m_camera;
};

