#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "typedefs.h"


#include "Framebuffer3D.h"
class FluidSimulation
{
public:
	FluidSimulation();
	~FluidSimulation();

	void init(glm::ivec3 shape, uint fluidShader, uint velShader);

	Framebuffer3D& getFluid() { return m_fluid; }

	void setShaders(uint fluid, uint velocity) { m_fUpdateShader = fluid; m_vUpdateShader = velocity; }

	void update(uint buf, uint w, uint h, float time);

private:
	glm::ivec3 m_shape;
	Framebuffer3D m_fluid, m_vel;
	uint m_fUpdateShader, m_vUpdateShader;
};

