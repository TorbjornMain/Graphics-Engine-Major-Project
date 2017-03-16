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
	m_model = Model();
	m_model.generateScreenSpaceQuad();
	glGenFramebuffers(1, &m_buf);
	glBindFramebuffer(GL_FRAMEBUFFER, m_buf);
	
	glGenTextures(1, &m_tex);
	glBindTexture(GL_TEXTURE_3D, m_tex);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, m_shape.x, m_shape.y, m_shape.z, 0, GL_RGBA, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float color[] = { 0.f, 0.f, 0.f, 0.0f };
	glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, color);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_tex, 0);

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "RIP, Framebuffer Error" << glGetError() << std::endl;
	
}

void Framebuffer3D::draw(uint shader, uint buf, uint w, uint h, float time)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_buf);
	glViewport(0, 0, m_shape.x, m_shape.y);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shader);
	int loc = glGetUniformLocation(shader, "slices");
	glUniform1i(loc, m_shape.z);
	loc = glGetUniformLocation(shader, "time");
	glUniform1f(loc, time);

	m_model.drawInstancedGenericScreenQuad(m_shape.z);
	glBindFramebuffer(GL_FRAMEBUFFER, buf);
	glViewport(0, 0, w, h);
}
