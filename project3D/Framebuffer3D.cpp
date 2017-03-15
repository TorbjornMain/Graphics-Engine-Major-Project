#include "Framebuffer3D.h"
#include <gl_core_4_4.h>
#include <iostream>


Framebuffer3D::Framebuffer3D(glm::ivec3 shape)
{
	m_shape = shape;
}


Framebuffer3D::~Framebuffer3D()
{
}

void Framebuffer3D::GenBuffer()
{
	glGenFramebuffers(1, &m_buf);
	glBindFramebuffer(GL_FRAMEBUFFER, m_buf);
	
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_3D, m_tex);
	glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA32F, m_shape.x, m_shape.y, m_shape.z);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tex, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "RIP, Framebuffer Error" << std::endl;
}
