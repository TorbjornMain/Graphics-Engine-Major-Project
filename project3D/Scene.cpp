#include "Scene.h"
#include <gl_core_4_4.h>
#include <Gizmos.h>

Scene::Scene()
{
}


Scene::~Scene()
{
}

void Scene::drawToRenderTarget(const Camera & renderCam, FrameBuffer & buf, float time, int screenWidth, int screenHeight)
{
	glBindFramebuffer(GL_FRAMEBUFFER, buf.getBuf());
	glViewport(0, 0, buf.getW(), buf.getH());
	glClearColor(0.f, 0.f, 0.f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


	//render models (guaranteed opaque) -> particles (hard to see through, even if translucent) -> volumes (easy to see through when translucent)
	if (b_renderModels)
	{
		for (auto iter = m_instances.begin(); iter != m_instances.end(); iter++)
		{
			iter->second.draw(renderCam.projection * renderCam.view, renderCam.transform, time, time);
		}
	}
	if (b_renderParticles)
	{
		for (auto iter = m_particleSystems.begin(); iter != m_particleSystems.end(); iter++)
		{
			iter->second.draw(time, renderCam.transform, renderCam.projection * renderCam.view);
		}
	}
	if (b_renderVolumes)
	{
		for (auto iter = m_volumetricFields.begin(); iter != m_volumetricFields.end(); iter++)
		{
			iter->second.draw(&renderCam, buf.getBuf(), buf.getW(), buf.getH());
		}
	}
	if (b_renderGizmos)
	{
		aie::Gizmos::draw(m_camera.projection * m_camera.view);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, screenWidth, screenHeight);
}

void Scene::draw(float time, int screenWidth, int screenHeight)
{
	//render models (guaranteed opaque) -> particles (hard to see through, even if translucent) -> volumes (easy to see through when translucent)
	if (b_renderModels)
	{
		for (auto iter = m_instances.begin(); iter != m_instances.end(); iter++)
		{
			iter->second.draw(m_camera.projection * m_camera.view, m_camera.transform, time, time);
		}
	}
	if (b_renderParticles)
	{
		for (auto iter = m_particleSystems.begin(); iter != m_particleSystems.end(); iter++)
		{
			iter->second.draw(time, m_camera.transform, m_camera.projection * m_camera.view);
		}
	}
	if (b_renderVolumes)
	{
		for (auto iter = m_volumetricFields.begin(); iter != m_volumetricFields.end(); iter++)
		{
			iter->second.draw(&m_camera, 0, screenWidth, screenHeight);
		}
	}
	if (b_renderGizmos)
	{
		aie::Gizmos::draw(m_camera.projection * m_camera.view);
	}


}

void Scene::AddInstance(char* name, Model * model, uint shader, uint texture, glm::mat4 transform)
{
	Instance ins = Instance();
	ins.setModel(model);
	ins.setShader(shader);
	ins.setTexture(texture);
	ins.setTransform(transform);
	m_instances.insert(std::pair<char*, Instance>(name, ins));
}

void Scene::AddInstance(char * name, Model * model, uint shader, const char * textureFile, glm::mat4 transform)
{
	Instance ins = Instance();
	ins.setModel(model);
	ins.setShader(shader);
	ins.loadTex(textureFile);
	ins.setTransform(transform);
	m_instances.insert(std::pair<char*, Instance>(name, ins));
}

void Scene::AddParticleSystem(char * name, glm::vec3 position, uint upShader, uint dShader, uint numParticles)
{
	ParticleSystem p = ParticleSystem();
	p.init(numParticles, 20.f, 40.f, glm::vec3(-0.01f), glm::vec3(0.01f), 0.01f, 0.01f, glm::vec4(0, 0.5, 0, 1), glm::vec4(1, 1, 0, 0.1f), upShader, dShader);
	p.setPos(position);
	m_particleSystems.insert(std::pair<char*, ParticleSystem>(name, p));
}

void Scene::AddVisualiser(char * name, glm::mat4 transform, uint field, glm::vec3 fieldShape)
{
	FieldVisualiser v = FieldVisualiser();
	v.init(field, fieldShape, transform, 100);
	m_volumetricFields.insert(std::pair<char*, FieldVisualiser>(name, v));
}

ParticleSystem & Scene::GetParticleSystem(char * name)
{
	return m_particleSystems[name];
}

Instance & Scene::GetInstance(char * name)
{
	
	return m_instances[name];
}

FieldVisualiser & Scene::GetVisualiser(char * name)
{
	return m_volumetricFields[name];
}
