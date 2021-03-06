#include "ParticleSystem.h"
#include <gl_core_4_4.h>
#include <Texture.h>
#include <Gizmos.h>
#include <iostream>

ParticleSystem::ParticleSystem() : m_particles(nullptr)
{
	m_data = ParticleSystemData();
	m_data.maxParticles = 0;
	m_position = glm::vec3(0, 0, 0);
	m_data.startColor = glm::vec4(0, 0, 0, 0);
	m_data.endColor = glm::vec4(0, 0, 0, 0);
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(uint maxParticles, float lifespanMin, float lifespanMax, glm::vec3 velocityMin, glm::vec3 velocityMax, float startSize, float endSize, const glm::vec4 & startColor, const glm::vec4 & endColor, uint upShader, uint drawShader, uint flowField, float fieldScale, uint texture)
{
	m_data.startColor = startColor;
	m_data.endColor = endColor;

	m_data.startSize = startSize;
	m_data.endSize = endSize;

	m_data.velocityMin = velocityMin;
	m_data.velocityMax = velocityMax;
	m_data.gravity = glm::vec3(0, 0, 0);

	m_data.lifespanMin = lifespanMin;
	m_data.lifespanMax = lifespanMax;

	m_data.maxParticles = maxParticles;

	m_data.flowField = flowField;
	m_data.fieldScale = fieldScale;
	m_data.fieldOffset = glm::vec3(0, 0, 0);
	m_data.fieldStrength = 1;

	m_particles = new Particle[maxParticles];

	m_data.frictionCoefficient = 1;

	m_activeBuffer = 0;

	m_updateShader = upShader;
	m_drawShader = drawShader;

	m_data.texture = texture;

	genBuffers();

	initializeUniforms();

}

void ParticleSystem::initializeUniforms()
{
	glUseProgram(m_drawShader);

	int loc = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(loc, m_data.startSize);
	loc = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(loc, m_data.endSize);
	loc = glGetUniformLocation(m_drawShader, "colorStart");
	glUniform4fv(loc, 1, &m_data.startColor[0]);
	loc = glGetUniformLocation(m_drawShader, "colorEnd");
	glUniform4fv(loc, 1, &m_data.endColor[0]);
	

	glUseProgram(m_updateShader);

	loc = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(loc, m_data.lifespanMin);
	loc = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(loc, m_data.lifespanMax);
	loc = glGetUniformLocation(m_updateShader, "fieldScale");
	glUniform1f(loc, m_data.fieldScale);
	loc = glGetUniformLocation(m_updateShader, "velMax");
	glUniform3fv(loc, 1, glm::value_ptr(m_data.velocityMax));
	loc = glGetUniformLocation(m_updateShader, "velMin");
	glUniform3fv(loc, 1, glm::value_ptr(m_data.velocityMin));
	loc = glGetUniformLocation(m_updateShader, "terminalVel");
	glUniform1f(loc, c_terminalVelocity);
	loc = glGetUniformLocation(m_updateShader, "gravity");
	glUniform3fv(loc, 1, glm::value_ptr(m_data.gravity));
	loc = glGetUniformLocation(m_updateShader, "fieldOffset");
	glUniform3fv(loc, 1, glm::value_ptr(m_data.fieldOffset));
	loc = glGetUniformLocation(m_updateShader, "frictionCoefficient");
	glUniform1f(loc, m_data.frictionCoefficient);
	loc = glGetUniformLocation(m_updateShader, "fieldStrength");
	glUniform1f(loc, m_data.fieldStrength);
}

void ParticleSystem::draw(float time, const glm::mat4 & camTransform, const glm::mat4 & projectionView)
{

	initializeUniforms();


	//Frustum cull
	glm::vec4 planes[6];
	planes[0] = glm::vec4(projectionView[0][3] - projectionView[0][0], projectionView[1][3] - projectionView[1][0], projectionView[2][3] - projectionView[2][0], projectionView[3][3] - projectionView[3][0]);
	planes[1] = glm::vec4(projectionView[0][3] + projectionView[0][0], projectionView[1][3] + projectionView[1][0], projectionView[2][3] + projectionView[2][0], projectionView[3][3] + projectionView[3][0]);
	planes[2] = glm::vec4(projectionView[0][3] - projectionView[0][1], projectionView[1][3] - projectionView[1][1], projectionView[2][3] - projectionView[2][1], projectionView[3][3] - projectionView[3][1]);
	planes[3] = glm::vec4(projectionView[0][3] + projectionView[0][1], projectionView[1][3] + projectionView[1][1], projectionView[2][3] + projectionView[2][1], projectionView[3][3] + projectionView[3][1]);
	planes[4] = glm::vec4(projectionView[0][3] - projectionView[0][2], projectionView[1][3] - projectionView[1][2], projectionView[2][3] - projectionView[2][2], projectionView[3][3] - projectionView[3][2]);
	planes[5] = glm::vec4(projectionView[0][3] + projectionView[0][2], projectionView[1][3] + projectionView[1][2], projectionView[2][3] + projectionView[2][2], projectionView[3][3] + projectionView[3][2]);

	bool render = true;
	for (int i = 0; i < 6; i++)
	{
		float d = glm::length(glm::vec3(planes[i]));
		planes[i] /= d;

		float dist = glm::dot(glm::vec3(planes[i]), m_position) + planes[i].w;
		if (dist < -c_terminalVelocity * m_data.lifespanMax)
		{
			render = false;
			break;
		}
	}

	int loc;
	uint otherBuf = (m_activeBuffer + 1) % 2;

	//Update
	glUseProgram(m_updateShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, m_data.flowField);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_data.texture);

	loc = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(loc, time);

	loc = glGetUniformLocation(m_updateShader, "flowField");
	glUniform1i(loc, 0);

	float deltaTime = time - m_lastDrawTime; m_lastDrawTime = time;

	loc = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(loc, deltaTime);

	loc = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(loc, 1, &m_position[0]);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_activeBuffer]);


	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[otherBuf]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_data.maxParticles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);


	//Draw if in frustum
	if (render == true)
	{
		glUseProgram(m_drawShader);
		loc = glGetUniformLocation(m_drawShader, "projectionView");
		glUniformMatrix4fv(loc, 1, false, &projectionView[0][0]);
		loc = glGetUniformLocation(m_drawShader, "cameraTransform");
		glUniformMatrix4fv(loc, 1, false, &camTransform[0][0]);

		loc = glGetUniformLocation(m_drawShader, "tex");
		glUniform1i(loc, 1);


		glBindVertexArray(m_vao[otherBuf]);
		glDrawArrays(GL_POINTS, 0, m_data.maxParticles - 1);
	}
	m_activeBuffer = otherBuf;
}

void ParticleSystem::loadTexture(const char * filename)
{
	aie::Texture t;
	t.load(filename);
	glGenTextures(1, &(m_data.texture));
	glBindTexture(GL_TEXTURE_2D, m_data.texture);
	uint format = 0;
	//Find format and apply to texture
	switch (t.getFormat())
	{
	case(aie::Texture::Format::RED):
		format = GL_RED;
		break;
	case(aie::Texture::Format::RG):
		format = GL_RG;
		break;
	case(aie::Texture::Format::RGB):
		format = GL_RGB;
		break;
	case(aie::Texture::Format::RGBA):
		format = GL_RGBA;
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, t.getWidth(), t.getHeight(), 0, format, GL_UNSIGNED_BYTE, t.getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}


void ParticleSystem::genBuffers()
{
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_data.maxParticles * sizeof(Particle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 28);

	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_data.maxParticles * sizeof(Particle), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
