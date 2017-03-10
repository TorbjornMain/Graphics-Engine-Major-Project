#pragma once
#include "Model.h"
#include "typedefs.h"
#include <glm/ext.hpp>
#include <glm/glm.hpp>

struct Camera;

class FieldVisualiser
{
public:
	FieldVisualiser();
	~FieldVisualiser();

	void init(uint shader, uint field, glm::vec3 fieldShape, glm::mat4 fieldTransform, int scanRes);

	void draw(const Camera* c);

private:
	Model m_model;
	uint m_shader;
	uint m_field;
	glm::vec3 m_fieldShape;
	glm::mat4 m_fieldTransform;
	int m_scanResolution;
};

