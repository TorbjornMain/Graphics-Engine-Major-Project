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

private:
	int m_numVertices;
	Vertex* m_vertices;
	int m_numIndices;
	unsigned int* m_indices;
};

