#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include "Scene.h"
#include "FrameBuffer.h"

class Shader;
class Model;
class Instance;

class Application3D : public aie::Application {
public:

	Application3D();
	virtual ~Application3D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:
	glm::mat4	m_camTransform;
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	Shader*		m_mainShader;
	Model*		m_testModel;
	Model*		m_ppModel;
	FrameBuffer m_fb;
	Scene		m_scene;
	bool		m_postProcess;
	float m_lMX, m_lMY, m_FOV, m_LSCRL;
};