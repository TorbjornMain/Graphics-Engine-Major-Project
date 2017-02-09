#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Model.h"
#include "Shader.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::startup() {
	
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_camTransform = glm::translate(vec3(0, 1, 0)) * glm::eulerAngleXYZ(0.f, 0.f, 0.f);
	m_viewMatrix = glm::inverse(m_camTransform);
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);
	m_FOV = glm::pi<float>() * 0.5f;
	m_mainShader = new Shader();
	m_mainShader->CompileShaders("../../../Shaders/TestVertShader.txt", "../../../Shaders/TestFragShader.txt");
	m_testModel = new Model();
	m_testModel->loadFromFile("../../../Meshes/Bunny.obj");
	m_testModel->setTransform(glm::translate(glm::vec3(1, 0, 0)));
	return true;
}

void Application3D::shutdown() {

	Gizmos::destroy();
}

void Application3D::update(float deltaTime) {

	// query time since application started
	float time = getTime();

	// wipe the gizmos clean for this frame
	Gizmos::clear();

	// draw a simple grid with gizmos
	vec4 white(1);
	vec4 black(0, 0, 0, 1);
	for (int i = 0; i < 21; ++i) {
		Gizmos::addLine(vec3(-10 + i, 0, 10),
			vec3(-10 + i, 0, -10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(10, 0, -10 + i),
			vec3(-10, 0, -10 + i),
			i == 10 ? white : black);
	}
	// add a transform so that we can see the axis
	Gizmos::addTransform(mat4(1));

	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();
	vec3 deltaTranslate((input->isKeyDown(aie::INPUT_KEY_D) ? 1 : 0) - (input->isKeyDown(aie::INPUT_KEY_A) ? 1 : 0), (input->isKeyDown(aie::INPUT_KEY_LEFT_SHIFT) ? 1 : 0) - (input->isKeyDown(aie::INPUT_KEY_LEFT_CONTROL) ? 1 : 0), (input->isKeyDown(aie::INPUT_KEY_S) ? 1 : 0) - (input->isKeyDown(aie::INPUT_KEY_W) ? 1 : 0));
	deltaTranslate *= deltaTime;
	float mx = input->getMouseX(); 
	float my = input->getMouseY();
	float dx, dy;
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT))
	{
		dx = -deltaTime * (mx - m_lMX) * 0.5;
		dy = deltaTime * (my - m_lMY) * 0.5;
		m_camTransform *= glm::translate(deltaTranslate) * glm::eulerAngleXYZ(dy, dx, ((input->isKeyDown(aie::INPUT_KEY_Q) ? 1 : 0) - (input->isKeyDown(aie::INPUT_KEY_E) ? 1 : 0)) * deltaTime);
	}
	else
		m_camTransform *= glm::translate(deltaTranslate);
	
	//rotate camera
	 m_viewMatrix = glm::inverse(m_camTransform);

	 m_lMX = mx;
	 m_lMY = my;

	 //m_FOV += -((float)input->getMouseScroll() - m_LSCRL) * deltaTime;
	 m_LSCRL = (float)input->getMouseScroll();
	 if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(m_FOV,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);

	
	m_testModel->draw(m_mainShader->GetID(), m_projectionMatrix * m_viewMatrix);
	Gizmos::draw(m_projectionMatrix * m_viewMatrix);
}