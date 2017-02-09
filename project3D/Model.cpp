#include "Model.h"
#include <gl_core_4_4.h>


Model::Model()
{
}


Model::~Model()
{
}

void Model::GenerateTetrahedron()
{
	m_numVertices = 4;
	m_vertices = new Vertex[4];
	m_vertices[0].position = glm::vec4(0, 0.7, 0, 0);
	m_vertices[0].color = glm::vec4(1,0,0,1);
	m_vertices[1].position = glm::vec4(0.5, 0, 0.5, 0);
	m_vertices[1].color = glm::vec4(0, 1, 0, 1);
	m_vertices[2].position = glm::vec4(-0.5, 0, 0.5, 0);
	m_vertices[2].color = glm::vec4(0, 0, 1, 1);
	m_vertices[3].position = glm::vec4(0.5, 0, -0.5, 0);
	m_vertices[3].color = glm::vec4(1, 1, 1, 1);

	m_numIndices = 12;
	m_indices[0] = 2;
	m_indices[1] = 0;
	m_indices[2] = 1;

	m_indices[3] = 3;
	m_indices[4] = 0;
	m_indices[5] = 2;

	m_indices[6] = 3;
	m_indices[7] = 0;
	m_indices[8] = 1;


	m_indices[9] = 3;
	m_indices[10] = 2;
	m_indices[11] = 1;
}

void Model::draw(unsigned int shaderID, glm::mat4 camera)
{
	unsigned int VAO, VBO, IBO;
	
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_numVertices * sizeof(Vertex), m_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec4)));

	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_numIndices * sizeof(unsigned int), m_indices, GL_STATIC_DRAW);

	glUseProgram(shaderID);
	unsigned int pvu = glGetUniformLocation(shaderID, "projectionViewWorldMatrix");
	glUniformMatrix4fv(pvu, 1, false, glm::value_ptr(camera));
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}
