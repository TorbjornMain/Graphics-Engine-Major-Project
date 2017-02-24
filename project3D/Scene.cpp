#include "Scene.h"
#include <gl_core_4_4.h>


Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::drawToRenderTarget(const Camera & renderCam, FrameBuffer buf, float time)
{
	glBindFramebuffer(GL_FRAMEBUFFER, buf.getBuf());
	glViewport(0, 0, buf.getW(), buf.getH());
	glClearColor(0.75f, 0.75f, 0.75f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto iter = m_instances.begin(); iter != m_instances.end(); iter++)
	{
		iter->second.draw(renderCam.m_projection * renderCam.m_view, glm::column(renderCam.m_transform, 3), time, time);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1280, 720);
}

void Scene::draw(float time)
{
	for (auto iter = m_instances.begin(); iter != m_instances.end(); iter++)
	{
		iter->second.draw(m_camera.m_projection * m_camera.m_view, glm::column(m_camera.m_transform, 3), time, time);
	}
}

void Scene::AddInstance(char* name, Model * model, unsigned int shader, unsigned int texture, glm::mat4 transform)
{
	Instance ins = Instance();
	ins.setModel(model);
	ins.setShader(shader);
	ins.setTexture(texture);
	ins.setTransform(transform);
	m_instances.insert(std::pair<char*, Instance>(name, ins));
}

void Scene::AddInstance(char * name, Model * model, unsigned int shader, const char * textureFile, glm::mat4 transform)
{
	Instance ins = Instance();
	ins.setModel(model);
	ins.setShader(shader);
	ins.loadTex(textureFile);
	ins.setTransform(transform);
	m_instances.insert(std::pair<char*, Instance>(name, ins));
}

Instance & Scene::GetInstance(char * name)
{
	
	return m_instances[name];
}
