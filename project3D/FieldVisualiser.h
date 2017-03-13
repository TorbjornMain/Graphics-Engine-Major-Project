#pragma once
#include "Model.h"
#include "typedefs.h"
#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "FrameBuffer.h"
#include "Shader.h"

struct Camera;

class FieldVisualiser
{
public:
	FieldVisualiser();
	~FieldVisualiser();

	void init(uint field, glm::vec3 fieldShape, glm::mat4 fieldTransform, int scanRes);

	void draw(const Camera* c, uint m_buffer, int width, int height);

private:
	Model m_model, m_ppModel;
	uint m_field;
	glm::vec3 m_fieldShape;
	glm::mat4 m_fieldTransform;
	int m_scanResolution;
	FrameBuffer m_frontFace;
	FrameBuffer m_backFace;
	Shader m_preProcess, m_postProcess;
};

