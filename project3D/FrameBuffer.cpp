#include "FrameBuffer.h"
#include <iostream>
#include <gl_core_4_4.h>




FrameBuffer::FrameBuffer(unsigned int w, unsigned int h)
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
	glBindTexture(GL_TEXTURE_2D, m_tex);

	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_w, m_h);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tex, 0);

	glGenRenderbuffers(1, &m_dep);
	glBindRenderbuffer(GL_RENDERBUFFER, m_dep);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_w, m_h);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_dep);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "RIP, Framebuffer Error" << std::endl;
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
