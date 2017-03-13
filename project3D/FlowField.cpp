#include "FlowField.h"
#include <gl_core_4_4.h>


FlowField::FlowField()
{
}


FlowField::~FlowField()
{
}

void FlowField::init(glm::vec3 shape, VectorFieldGeneratorFunc vfg)
{
	m_shape = shape;

	m_storage = new glm::vec3[(uint)shape.x * (uint)shape.y * (uint)shape.z];

	for (int x = 0; x < (int)shape.x; x++)
	{
		for (int y = 0; y < (int)shape.y; y++)
		{
			for (int z = 0; z < (int)shape.z; z++)
			{
				glm::vec3 v = ((2*(glm::vec3(x, y, z) / shape)) - 1.f);
				m_storage[x + ((int)shape.x * (y + (z * (int)(shape.y))))] = vfg(v);
			}
		}
	}

	

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_3D, m_id);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.f, 0.f, 0.f, 0.0f };
	glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, color);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB32F, (int)shape.x, (int)shape.y, (int)shape.z, 0, GL_RGB, GL_FLOAT, m_storage);
	glBindTexture(GL_TEXTURE_3D, 0);
}