#pragma once
#include <glm\glm.hpp>
#include <glm\ext.hpp>
struct Vertex
{
	glm::vec4 position;
	glm::vec4 color;
};

class Model
{
public:
	Model();
	~Model();

	void GenerateTetrahedron();

	void draw(unsigned int shaderID, glm::mat4 camera);
	void setTransform(glm::mat4 trans) { m_transform = trans; }
	glm::mat4 getTransform() { return m_transform; }
private:
	glm::mat4 m_transform;
	int m_numVertices;
	Vertex* m_vertices;
	int m_numIndices;
	unsigned int* m_indices;
};

