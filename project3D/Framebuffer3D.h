#pragma once
#include "typedefs.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Framebuffer3D
{
public:
	Framebuffer3D() { m_shape = glm::ivec3(100); }
	Framebuffer3D(glm::ivec3 shape);
	~Framebuffer3D();

	void GenBuffer();

	uint getTex() { return m_tex; }
	uint getBuf() { return m_buf; }
	glm::ivec3 getShape() { return m_shape; }
private:
	glm::ivec3 m_shape;

	uint m_tex = 0;
	uint m_buf = 0;
};

