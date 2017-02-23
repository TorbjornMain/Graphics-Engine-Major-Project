#pragma once
#include <map>
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "FrameBuffer.h"

struct Camera
{
	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::mat4 m_transform;
};

class Scene
{
public:
	Scene();
	~Scene();

	void drawToRenderTarget(const Camera& renderCam, FrameBuffer buf, float time);
	void draw(float time);
	void AddInstance(char* name, Model* model, unsigned int shader, unsigned int texture, glm::mat4 transform);
	void AddInstance(char* name, Model* model, unsigned int shader, const char* textureFile, glm::mat4 transform);
	Instance& GetInstance(char* name);
	Camera getCamera() { return m_camera; }
	void setCamera(Camera cam) { m_camera = cam; };


private:
	std::map<char* , Instance> m_instances;
	glm::vec3 lightDir;
	Camera m_camera;
};

