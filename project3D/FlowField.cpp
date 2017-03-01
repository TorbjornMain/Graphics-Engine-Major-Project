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

	m_storage = new glm::vec3[shape.x * shape.y * shape.z];

	for (int x = 0; x < shape.x; x++)
	{
		for (int y = 0; y < shape.y; y++)
		{
			for (int z = 0; z < shape.z; z++)
			{
				m_storage[x + ((int)shape.x * (y + (z * (int)(shape.y))))] = vfg(glm::vec3(x,y,z));
			}
		}
	}

	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_3D, m_id);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, color);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, (int)shape.x, (int)shape.y, (int)shape.z, 0, GL_RGB, GL_FLOAT, (float*)m_storage);
	glBindTexture(GL_TEXTURE_3D, 0);
}
