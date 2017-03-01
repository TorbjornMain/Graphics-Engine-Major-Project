#pragma once

#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include "typedefs.h"

struct Particle
{
	Particle() : lifetime(1), lifespan(0) {}
	glm::vec3 position;
	glm::vec3 velocity;
	float lifetime;
	float lifespan;
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void init(uint maxParticles, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, const glm::vec4& startColor, const glm::vec4& endColor, uint upShader, uint drawShader);

	void draw(float time, const glm::mat4& camTransform, const glm::mat4& projectionView);

	glm::vec3 getPos() { return m_position; }
	void setPos(glm::vec3 newPos) { m_position = newPos; }

private:
	void genBuffers();

	uint m_activeBuffer;
	uint m_vao[2];
	uint m_vbo[2];
	uint m_updateShader;
	uint m_drawShader;

	Particle* m_particles;
	uint m_maxParticles;

	glm::vec3 m_position;

	float m_lifespanMin;
	float m_lifespanMax;
	float m_velocityMin;
	float m_velocityMax;
	float m_startSize;
	float m_endSize;

	glm::vec4 m_startColor;
	glm::vec4 m_endColor;

	float m_lastDrawTime;

};

