#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include "Scene.h"
#include "FrameBuffer.h"
#include "FlowField.h"

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
	const int	c_numShaders = 4;
	const int	c_funcs = 3;
	glm::mat4	m_camTransform;
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	Shader*		m_mainShader;
	Shader*		m_ppShaders;
	FlowField*	m_vfFuncs;
	uint		m_curShader = 0;
	uint		m_curFunc = 0;
	bool		m_drawModels = true;
	Model*		m_testModel;
	Model*		m_ppModel;
	FrameBuffer m_fb;
	Scene		m_scene;
	bool		m_postProcess = true;
	float m_lMX, m_lMY, m_FOV, m_LSCRL;
};