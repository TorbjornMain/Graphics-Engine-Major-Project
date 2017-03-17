#pragma once
#include "typedefs.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>

typedef glm::vec3(*VectorFieldGeneratorFunc)(glm::vec3);

class FlowField
{
public:
	FlowField();
	~FlowField();

	void init(glm::vec3 shape, VectorFieldGeneratorFunc vfg);

	uint getID() { return m_id; }

	glm::vec3 getShape() { return m_shape; }

private:

	glm::vec3 m_shape;

	glm::vec3* m_storage;
	uint m_id;
};

