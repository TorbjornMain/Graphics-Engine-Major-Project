
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Model.h"
#include <vector>
#include <string>
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
	m_vertices[0].position = glm::vec4(0, 0.7, 0, 1);
	m_vertices[0].color = glm::vec4(1,0,0,1);
	m_vertices[1].position = glm::vec4(0.5, 0, 0.5, 1);
	m_vertices[1].color = glm::vec4(0, 1, 0, 1);
	m_vertices[2].position = glm::vec4(-0.7, 0, 0, 1);
	m_vertices[2].color = glm::vec4(0, 0, 1, 1);
	m_vertices[3].position = glm::vec4(0, 0, -0.7, 1);
	m_vertices[3].color = glm::vec4(1, 1, 1, 1);

	m_numIndices = 12;
	m_indices = new unsigned int[12];
	m_indices[0] = 1;
	m_indices[1] = 0;
	m_indices[2] = 2;

	m_indices[3] = 2;
	m_indices[4] = 0;
	m_indices[5] = 3;

	m_indices[6] = 3;
	m_indices[7] = 0;
	m_indices[8] = 1;


	m_indices[9] = 1;
	m_indices[10] = 2;
	m_indices[11] = 3;
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
	glUniformMatrix4fv(pvu, 1, false, glm::value_ptr(camera * m_transform));
	glDrawElements(GL_TRIANGLES, m_numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
}

bool Model::loadFromFile(const char * filename)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> meshes;
	std::vector <tinyobj::material_t> mtl;
	std::string err;
	bool ret = tinyobj::LoadObj(&attribs, &meshes, &mtl, &err, filename);

	if (ret)
	{
		m_numVertices = attribs.vertices.size()/3;
		if (m_vertices != nullptr)
			delete[] m_vertices;
		m_vertices = new Vertex[m_numVertices];
		for (int i = 0; i < attribs.vertices.size(); i += 3)
		{
			m_vertices[i / 3].position = glm::vec4(attribs.vertices[i], attribs.vertices[i + 1], attribs.vertices[i + 2], 1.0f);
			m_vertices[i / 3].color = glm::vec4(1, 1, 0, 1);
		}
		m_numIndices = 0;
		if (m_indices != nullptr)
			delete[] m_indices;
		for (int i = 0; i < meshes.size(); i++)
		{
			m_numIndices += meshes[i].mesh.indices.size();
		}
		m_indices = new unsigned int[m_numIndices];
		int c_ind = 0;
		for (int i = 0; i < meshes.size(); i++)
		{
			for (int j = 0; j < meshes[i].mesh.indices.size(); j++)
			{
				m_indices[c_ind + j] = meshes[i].mesh.indices[j].vertex_index;
			}
			c_ind += meshes[i].mesh.indices.size();
		}
	}

	return ret;
}
