#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include "Scene.h"
#include "FrameBuffer.h"
#include "FlowField.h"
#include "Framebuffer3D.h"

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
	const int	c_funcs = 5;
	glm::mat4	m_camTransform;
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	Shader*		m_mainShader;
	Shader*		m_ppShaders;
	Shader*		m_fluidShader;
	FlowField*	m_vfFuncs;
	int			m_curShader = 0;
	int			m_curFunc = 0;
	int			m_previousWindowWidth = 0;
	int			m_previousWindowHeight = 0;
	bool		m_drawModels = true;
	Model*		m_testModel;
	Model*		m_ppModel;
	FrameBuffer m_fb;
	Scene		m_scene;
	Framebuffer3D m_fluidBuf;
	bool		m_postProcess = true;
	float m_lMX, m_lMY, m_FOV;
};