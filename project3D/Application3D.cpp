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
#include <Texture.h>
#include <iostream>


using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

Application3D::Application3D() {

}

Application3D::~Application3D() {

}

bool Application3D::startup() {

	system("cd");

	m_scene = Scene();
	setBackgroundColour(0.25f, 0.25f, 0.25f);
	m_LSCRL = 0;
	m_FOV = glm::half_pi<float>();

	m_fb = FrameBuffer(1280, 720);
	m_previousWindowHeight = getWindowHeight();
	m_previousWindowWidth = getWindowWidth();


	std::cout << "Initialising Gizmos" << std::endl;
	// initialise gizmo primitive counts
	Gizmos::create(10000, 10000, 10000, 10000);

	// create simple camera transforms
	m_camTransform = glm::translate(vec3(0, 1, 0)) * glm::eulerAngleXYZ(0.f, 0.f, 0.f);
	m_viewMatrix = glm::inverse(m_camTransform);
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f,
		getWindowWidth() / (float)getWindowHeight(),
		0.1f, 1000.f);
	m_FOV = glm::pi<float>() * 0.5f;

	std::cout << "Generating Frame Buffer" << std::endl;
	m_fb.GenBuffer();

	m_mainShader = new Shader();
	m_mainShader->CompileShaders("Shaders/BasicVertShader.txt", "Shaders/PBRFragShader.txt");
	m_testModel = new Model();
	std::cout << "Loading Models" << std::endl;
	//m_testModel->loadFromOBJ("C:/Users/s171558/Documents/Graphics-Engine-Major-Project/Meshes/Bunny.obj");
	m_testModel->load("Meshes/mecanimloco.fbx");
	m_testModel->setUpperBound(glm::vec4(0.05, 0.1, 0.05, 1));
	m_testModel->setLowerBound(glm::vec4(-0.05, 0, -0.05, 1));
	m_scene.AddInstance("spicy boi", m_testModel, m_mainShader->GetID(), "Textures/woodtex.jpg", glm::translate(glm::vec3((rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f) * 20) * glm::scale(glm::vec3(10.f)));
	m_scene.AddInstance("angry boi", m_testModel, m_mainShader->GetID(), "Textures/woodtex.jpg", glm::translate(glm::vec3((rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f) * 20) * glm::scale(glm::vec3(10.f)));

	std::string st = "a";
	for (int i = 0; i < 15; i++)
	{
		st = st + st;
		m_scene.AddInstance(const_cast<char*>(st.c_str()), m_testModel, m_mainShader->GetID(), "Textures/woodtex.jpg", glm::translate(glm::vec3((rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f, (rand() / (float)INT16_MAX) - 0.5f) * 20) * glm::scale(glm::vec3(10.f)));
	}
	std::cout << "Compiling Shaders" << std::endl;
	m_ppShaders = new Shader[c_numShaders];
	m_ppShaders[0].CompileShaders("Shaders/PostProcessVert.txt", "Shaders/BloomFrag.txt");
	m_ppShaders[1].CompileShaders("Shaders/PostProcessVert.txt", "Shaders/EdgeDetectFrag.txt");
	m_ppShaders[2].CompileShaders("Shaders/PostProcessVert.txt", "Shaders/BoxBlurFrag.txt");
	m_ppShaders[3].CompileShaders("Shaders/PostProcessVert.txt", "Shaders/DepthFogFrag.txt");
	m_curShader = 0;
	m_mainShader = m_ppShaders;
	m_ppModel = new Model();
	m_ppModel->generateScreenSpaceQuad();

	Shader pShader = Shader();
	Shader puShader = Shader();
	pShader.CompileShaders("Shaders/BasicParticleVert.txt", "Shaders/BasicParticleFrag.txt", "Shaders/BasicParticleBillboardGeom.txt");
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	puShader.CompileUpdateShader("Shaders/BasicParticleUpdate.txt", varyings, 4);
	std::cout << "Creating Vector Fields" << std::endl;
	m_vfFuncs = new FlowField[c_funcs];
	vec3 ffSize = vec3(128);

	VectorFieldGeneratorFunc	f = [](glm::vec3 x) {
		glm::vec3 vec = vec3(0, 0, 0);
		return vec;
	}; 

	m_vfFuncs[0].init(ffSize, f);

	f = [](glm::vec3 x) {
		glm::vec3 vec = glm::ballRand(3.f);
		return vec;
	};

	m_vfFuncs[1].init(ffSize, f);

	f = [](glm::vec3 x) {
		glm::vec3 vec = vec3(x.z, 0.1f, -x.x) - (vec3(x.x, 0, x.z) *5);
		return vec;
	};

	m_vfFuncs[2].init(ffSize, f);

	f = [](glm::vec3 x) {
		return 5*vec3(x.y * x.z, x.x * x.z, x.y * x.x);
	};

	m_vfFuncs[3].init(ffSize, f);

	f = [](glm::vec3 x) {
		return -5*x;
	};

	m_vfFuncs[4].init(ffSize, f);

	std::cout << "Final Setup" << std::endl;
	//m_scene.AddParticleSystem("Bandaid", glm::vec3(0, 0, 0), puShader.GetID(), pShader.GetID(), 0);
	m_scene.AddParticleSystem("GreenerFlare", glm::vec3(0, 0, 0), puShader.GetID(), pShader.GetID(), 50000);
	m_scene.GetParticleSystem("GreenerFlare").getData().flowField = m_vfFuncs[0].getID();
	m_scene.GetParticleSystem("GreenerFlare").getData().fieldScale = 20;
	m_scene.GetParticleSystem("GreenerFlare").getData().startColor = vec4(1, 0.8f, 0, 1.f);
	m_scene.GetParticleSystem("GreenerFlare").getData().endColor = vec4(1, 0.8f, 0, 1.f);
	m_scene.GetParticleSystem("GreenerFlare").getData().startSize = 0.1f;
	m_scene.GetParticleSystem("GreenerFlare").getData().lifespanMax = 60;
	m_scene.GetParticleSystem("GreenerFlare").loadTexture("Textures/heart.png");
	m_scene.GetParticleSystem("GreenerFlare").initializeUniforms();


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
	if (input->isMouseButtonDown(aie::INPUT_MOUSE_BUTTON_RIGHT))
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

	//m_FOV = glm::min(glm::max(m_FOV - (((float)input->getMouseScroll() - m_LSCRL) * deltaTime), glm::pi<float>() / 5), glm::pi<float>() / 1.5f);

	ImGui::Begin("Options");

	//FPS Counter
	std::string str = "FPS: ";
	str = str + std::to_string(m_fps);
	ImGui::LabelText(str.c_str(), "");

	//Post Processing Toggle
	ImGui::Checkbox("Post Processing", &m_postProcess);

	//Model Render Toggle
	ImGui::Checkbox("Render Models", &m_scene.b_renderModels);


	//Particle Render Toggle
	ImGui::Checkbox("Render Particles", &m_scene.b_renderParticles);

	//Gizmo Render Toggle
	ImGui::Checkbox("Render Gizmos", &m_scene.b_renderGizmos);

	//Post Process Shader Selector
	ImGui::SliderInt("Post Process Shader", &(m_curShader), 0, c_numShaders - 1);
	m_mainShader = m_ppShaders + m_curShader;

	//Vector Field Function Selector
	ImGui::SliderInt("Vector Field Function", &(m_curFunc), 0, c_funcs - 1);
	m_scene.GetParticleSystem("GreenerFlare").getData().flowField = m_vfFuncs[m_curFunc].getID();

	//Max Velocity Tweaker
	ImGui::InputFloat3("Velocity Max", &m_scene.GetParticleSystem("GreenerFlare").getData().velocityMax.x);

	//Min Velocity Tweaker
	ImGui::InputFloat3("Velocity Min", &m_scene.GetParticleSystem("GreenerFlare").getData().velocityMin.x);

	//Start Color Tweaker
	ImGui::SliderFloat4("Start Colour", &m_scene.GetParticleSystem("GreenerFlare").getData().startColor.x, 0, 1);

	//End Color Tweaker
	ImGui::SliderFloat4("End Colour", &m_scene.GetParticleSystem("GreenerFlare").getData().endColor.x, 0, 1);

	//Lifetime Max
	ImGui::SliderFloat("Lifetime Max", &(m_scene.GetParticleSystem("GreenerFlare").getData().lifespanMax), 0.1f, 100);

	//Lifetime Min
	ImGui::SliderFloat("Lifetime Min", &(m_scene.GetParticleSystem("GreenerFlare").getData().lifespanMin), 0.1f, 100);

	//Field Scale Slider
	ImGui::SliderFloat("Field Scale", &(m_scene.GetParticleSystem("GreenerFlare").getData().fieldScale), 0.1f, 100);

	//Field Strength Slider
	ImGui::SliderFloat("Field Strength", &(m_scene.GetParticleSystem("GreenerFlare").getData().fieldStrength), 0.1f, 10);

	//Field Offset Input
	ImGui::InputFloat3("Field Offset", &(m_scene.GetParticleSystem("GreenerFlare").getData().fieldOffset.x));

	//Gravity
	ImGui::InputFloat3("Gravity", &(m_scene.GetParticleSystem("GreenerFlare").getData().gravity.x));

	ImGui::SliderFloat("Friction Coefficient", &(m_scene.GetParticleSystem("GreenerFlare").getData().frictionCoefficient), 0, 1);

	ImGui::End();
	if (m_previousWindowHeight + m_previousWindowWidth != getWindowWidth() + getWindowHeight())
	{
		m_fb.setW(getWindowWidth());
		m_fb.setH(getWindowHeight());
		m_fb.RegenBuffer();
	}

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
	c.projection = m_projectionMatrix;
	c.view = m_viewMatrix;
	c.transform = m_camTransform;
	m_scene.setCamera(c);

	if (m_postProcess)
	{
		m_scene.drawToRenderTarget(c, m_fb, getTime(), getWindowWidth(), getWindowHeight());
	}
		setBackgroundColour(0.25f, 0.25f, 0.25f);
	// wipe the screen to the background color
	clearScreen();
	if (!m_postProcess)
		m_scene.draw(getTime());
	//Gizmos::draw(m_projectionMatrix * m_viewMatrix);
	if (m_postProcess)
		m_ppModel->drawPostProcessQuad(m_mainShader->GetID(), m_fb);
}