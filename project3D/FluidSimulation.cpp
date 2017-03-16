#include "FluidSimulation.h"
#include <gl_core_4_4.h>


FluidSimulation::FluidSimulation()
{
}


FluidSimulation::~FluidSimulation()
{
}

void FluidSimulation::init(glm::vec3 shape)
{
	m_shape = shape;


	glm::vec4 * fluStor = new glm::vec4[(uint)shape.x * (uint)shape.y * (uint)shape.z];

	//Generates Texture pixels
	for (int x = 0; x < (int)shape.x; x++)
	{
		for (int y = 0; y < (int)shape.y; y++)
		{
			for (int z = 0; z < (int)shape.z; z++)
			{
				glm::vec3 v = ((2 * (glm::vec3(x, y, z) / shape)) - 1.f);
				float vl = glm::length(v);
				fluStor[x + ((int)shape.x * (y + (z * (int)(shape.y))))] = (vl < 1.f) ? glm::vec4(vl, 1-vl, 0, 0.01) : glm::vec4(0,0,0,0);
			}
		}
	}



	//Generates Texture Buffer
	glGenTextures(1, &m_fluid);
	glBindTexture(GL_TEXTURE_3D, m_fluid);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.f, 0.f, 0.f, 0.0f };
	glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, color);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, (int)shape.x, (int)shape.y, (int)shape.z, 0, GL_RGBA, GL_FLOAT, fluStor);
	glBindTexture(GL_TEXTURE_3D, 0);

	delete[] fluStor;

}
