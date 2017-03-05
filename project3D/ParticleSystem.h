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


struct ParticleSystemData
{
	uint maxParticles;
	float lifespanMin;
	float lifespanMax;
	float velocityMin;
	float velocityMax;
	float startSize;
	float endSize;

	uint texture;

	uint flowField;
	float fieldScale;

	glm::vec4 startColor;
	glm::vec4 endColor;
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void init(uint maxParticles, float lifespanMin, float lifespanMax, float velocityMin, float velocityMax, float startSize, float endSize, const glm::vec4& startColor, const glm::vec4& endColor, uint upShader, uint drawShader, uint flowField = -1, float fieldScale = 0, uint texture = 0);

	void initializeUniforms();


	void draw(float time, const glm::mat4& camTransform, const glm::mat4& projectionView);

	glm::vec3 getPos() { return m_position; }
	void setPos(glm::vec3 newPos) { m_position = newPos; }

	void loadTexture(const char* filename);

	ParticleSystemData& getData() { return m_data; }

private:
	void genBuffers();

	uint m_activeBuffer;
	uint m_vao[2];
	uint m_vbo[2];
	uint m_updateShader;
	uint m_drawShader;

	Particle* m_particles;

	glm::vec3 m_position;

	ParticleSystemData m_data;

	float m_lastDrawTime;

};

