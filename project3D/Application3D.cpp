#include "Application3D.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "Model.h"
#include "Shader.h"
#include <string>
#include <iostream>
#include <imgui.h>

#define HOME_PATH "C:/Users/Zac/Documents/Graphics-Engine-Major-Project/"
#define AIE_PATH "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::startup() {
	m_scene = Scene();
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	m_fb = FrameBuffer(1280, 720);

	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_camTransform = glm::translate(vec3(0, 1, 0)) * glm::eulerAngleXYZ(0.f, 0.f, 0.f);
	m_viewMatrix = glm::inverse(m_camTransform);
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
										  getWindowWidth() / (float)getWindowHeight(),
										  0.1f, 1000.f);
	m_FOV = glm::pi<float>() * 0.5f;
	
	m_fb.GenBuffer();

	m_mainShader = new Shader();
	m_mainShader->CompileShaders("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/BasicVertShader.txt", "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/PBRFragShader.txt");
	m_testModel = new Model();
	//m_testModel->loadFromOBJ("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/Meshes/Bunny.obj");
	m_testModel->loadFromFBX("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/Meshes/mecanimloco.fbx");
	m_scene.AddInstance("spicy boi", m_testModel, m_mainShader->GetID(), "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/Textures/woodtex.jpg", glm::translate(glm::vec3((rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f) * 20) * glm::scale(glm::vec3(10.f)));
	m_mainShader->CompileShaders("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/BasicVertShader.txt", "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/BasicFragShaderPlanarMap.txt");
	m_scene.AddInstance("angry boi", m_testModel, m_mainShader->GetID(), "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/Textures/woodtex.jpg" , glm::translate(glm::vec3((rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f) * 20) * glm::scale(glm::vec3(10.f)));
	
	std::string st = "a";
	for (int i = 0; i < 15; i++)
	{
		st = st + st;
		m_scene.AddInstance(const_cast<char*>(st.c_str()), m_testModel, m_mainShader->GetID(), "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/Textures/woodtex.jpg", glm::translate(glm::vec3((rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f) * 20) * glm::scale(glm::vec3(10.f)));
	}
	m_mainShader->CompileShaders("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/PostProcessVert.txt","C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/DepthFogFrag.txt");
	m_ppModel = new Model();
	m_ppModel->generateScreenSpaceQuad();

	Shader pShader = Shader();
	Shader puShader = Shader();
	pShader.CompileShaders("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/BasicParticleVert.txt", "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/BasicParticleFrag.txt", "C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/BasicParticleBillboardGeom.txt");
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	puShader.CompileUpdateShader("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/project3D/BasicParticleUpdate.txt", varyings, 4);


	m_scene.AddParticleSystem("GreenerFlare", glm::vec3(0, 0, 0), puShader.GetID(), pShader.GetID());


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
	int mx = input->getMouseX(); 
	int my = input->getMouseY();
	float dx, dy;
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_LEFT))
	{
		dx = -deltaTime * (mx - m_lMX) * 0.5f;
		dy = deltaTime * (my - m_lMY) * 0.5f;
		m_camTransform *= glm::translate(deltaTranslate) * glm::eulerAngleXYZ(dy, dx, ((input->isKeyDown(aie::INPUT_KEY_Q) ? 1 : 0) - (input->isKeyDown(aie::INPUT_KEY_E) ? 1 : 0)) * deltaTime);
	}
	else
		m_camTransform *= glm::translate(deltaTranslate);
	
	//rotate camera
	 m_viewMatrix = glm::inverse(m_camTransform);

	 m_lMX = (float)mx;
	 m_lMY = (float)my;


	 ImGui::Begin(std::to_string(m_fps).c_str());
	 ImGui::Button("                ");
	 ImGui::End();

	 //m_FOV += -((float)input->getMouseScroll() - m_LSCRL) * deltaTime;
	 m_LSCRL = (float)input->getMouseScroll();
	 if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application3D::draw() {

	// update perspective in case window resized
	m_projectionMatrix = glm::perspective(m_FOV,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);

	Camera c = Camera();
	c.m_projection = m_projectionMatrix;
	c.m_view = m_viewMatrix;
	c.m_transform = m_camTransform;
	m_scene.setCamera(c);

	m_scene.drawToRenderTarget(c, m_fb, getTime());

	setBackgroundColour(0.25f, 0.25f, 0.25f);
	// wipe the screen to the background colour
	clearScreen();
	


	
	//m_scene.draw(getTime());
	//Gizmos::draw(m_projectionMatrix * m_viewMatrix);

	m_ppModel->drawPostProcessQuad(m_mainShader->GetID(), m_fb);
}