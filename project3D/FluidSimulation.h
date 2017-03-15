#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "typedefs.h"


//Not actually a fluid simulation :D
class FluidSimulation
{
public:
	FluidSimulation();
	~FluidSimulation();

	void init(glm::vec3 shape);

	uint getFluid() { return m_fluid; }

private:
	glm::vec3 m_shape;
	uint m_fluid;
};

