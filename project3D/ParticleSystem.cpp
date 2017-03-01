#include "ParticleSystem.h"
#include <gl_core_4_4.h>


ParticleSystem::ParticleSystem() : m_particles(nullptr), m_maxParticles(0), m_position(0, 0, 0), m_drawShader(0), m_updateShader(0), m_lastDrawTime(0)
{
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::init(uint maxParticles, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, const glm::vec4 & startColor, const glm::vec4 & endColor, uint upShader, uint drawShader)
{
	m_startColor = startColor;
	m_endColor = endColor;

	m_startSize = startSize;
	m_endSize = endSize;

	m_velocityMin = velocityMin;
	m_velocityMax = velocityMax;
	
	m_lifespanMin = lifespanMin;
	m_lifespanMax = lifespanMax;

	m_maxParticles = maxParticles;

	m_particles = new Particle[maxParticles];

	m_activeBuffer = 0;

	m_updateShader = upShader;
	m_drawShader = drawShader;

	genBuffers();

	glUseProgram(m_drawShader);

	int loc = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(loc, m_startSize);
	loc = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(loc, m_endSize);
	loc = glGetUniformLocation(m_drawShader, "colorStart");
	glUniform4fv(0, 1, &m_startColor[0]);
	loc = glGetUniformLocation(m_drawShader, "colorEnd");
	glUniform4fv(0, 1, &m_endColor[0]);

	glUseProgram(m_updateShader);

	loc = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(loc, m_lifespanMin);
	loc = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(loc, m_lifespanMax);
}

void ParticleSystem::draw(float time, const glm::mat4 & camTransform, const glm::mat4 & projectionView)
{
	glUseProgram(m_updateShader);

	int loc = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(loc, time);

	float deltaTime = time - m_lastDrawTime; m_lastDrawTime = time;

	loc = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(loc, deltaTime);

	loc = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(loc, 1, &m_position[0]);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_activeBuffer]);
	uint otherBuf = (m_activeBuffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[otherBuf]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	glUseProgram(m_drawShader);
	loc = glGetUniformLocation(m_drawShader,"projectionView");
	glUniformMatrix4fv(loc, 1, false, &projectionView[0][0]);
	loc = glGetUniformLocation(m_drawShader, "cameraTransform");
	glUniformMatrix4fv(loc, 1, false, &camTransform[0][0]);

	glBindVertexArray(m_vao[otherBuf]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	m_activeBuffer = otherBuf;
}


void ParticleSystem::genBuffers()
{
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(Particle), m_particles, GL_STREAM_DRAW);
	
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
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(Particle), 0, GL_STREAM_DRAW);

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
