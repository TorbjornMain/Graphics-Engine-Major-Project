#include "FluidSimulation.h"
#include <gl_core_4_4.h>


FluidSimulation::FluidSimulation()
{
}


FluidSimulation::~FluidSimulation()
{
}

void FluidSimulation::init(glm::ivec3 shape, uint fluidShader, uint velShader)
{
	m_shape = shape;
	m_fluid = Framebuffer3D(shape);
	m_vel = Framebuffer3D(shape);
	m_fluid.GenBuffer();
	m_vel.GenBuffer();

	glm::vec4 * fluStor = new glm::vec4[(uint)shape.x * (uint)shape.y * (uint)shape.z];

	//Generates Texture pixels
	for (int x = 0; x < shape.x; x++)
	{
		for (int y = 0; y < shape.y; y++)
		{
			for (int z = 0; z < shape.z; z++)
			{
				glm::vec3 v = ((2 * (glm::vec3(x, y, z) / glm::vec3(shape))) - 1.f);
				float vl = glm::length(v);
				fluStor[x + (shape.x * (y + (z * (shape.y))))] = (vl < 1.f) ? glm::vec4(vl, 1-vl, 0, 0.01) : glm::vec4(0,0,0,0);
			}
		}
	}

	setShaders(fluidShader, velShader);
	glBindTexture(GL_TEXTURE_3D, m_fluid.getTex());
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, shape.x, shape.y, shape.z, 0, GL_RGBA, GL_FLOAT, fluStor);
	glBindTexture(GL_TEXTURE_3D, 0);

	delete[] fluStor;

}

void FluidSimulation::update(uint buf, uint w, uint h, float time)
{
	m_vel.initDraw(m_vUpdateShader);
	int loc = glGetUniformLocation(m_vUpdateShader, "tField");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_fluid.getTex());
	glUniform1i(loc, 0);
	m_vel.draw(m_vUpdateShader, buf, w, h, time);
	
	m_fluid.initDraw(m_fUpdateShader);
	loc = glGetUniformLocation(m_vUpdateShader, "tField");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_fluid.getTex());
	glUniform1i(loc, 0);
	m_fluid.draw(m_fUpdateShader, buf, w, h, time);

}
