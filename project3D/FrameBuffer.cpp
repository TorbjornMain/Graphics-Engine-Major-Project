#include "FrameBuffer.h"
#include <iostream>
#include <gl_core_4_4.h>




FrameBuffer::FrameBuffer(uint w, uint h)
{
	m_w = w; m_h = h;
	m_buf = 0;
	m_tex = 0;
	m_dep = 0;
}


FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::GenBuffer()
{

	glGenFramebuffers(1, &m_buf);
	glBindFramebuffer(GL_FRAMEBUFFER, m_buf);

	glGenTextures(1, &m_tex);
	glGenTextures(1, &m_dep);
	glBindTexture(GL_TEXTURE_2D, m_tex);


	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_w, m_h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tex, 0);
	
	glBindTexture(GL_TEXTURE_2D, m_dep);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_w, m_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_dep, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "RIP, Framebuffer Error" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::RegenBuffer()
{
	glDeleteFramebuffers(1, &m_buf);
	glDeleteTextures(1, &m_tex);
	glDeleteTextures(1, &m_dep);
	GenBuffer();
}
