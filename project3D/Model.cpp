
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Model.h"
#include <vector>
#include <string>
#include <gl_core_4_4.h>
#include <iostream>


Model::Model()
{
}

Model::Model(const Model & other)
{
	m_transform = glm::mat4(other.m_transform);
	m_glInfo = other.m_glInfo;
}

Model::~Model()
{

}

void Model::draw(unsigned int shaderID, glm::mat4 camera, glm::vec4 camPos, float time)
{
	glUseProgram(shaderID);

	unsigned int pvw = glGetUniformLocation(shaderID, "PVW");
	glUniformMatrix4fv(pvw, 1, false, glm::value_ptr(camera * m_transform));
	pvw = glGetUniformLocation(shaderID, "lightDir");
	glUniform3fv(pvw, 1, glm::value_ptr(glm::vec3(glm::half_pi<float>() / 2, 0, 0)));
	pvw = glGetUniformLocation(shaderID, "M");
	glUniformMatrix4fv(pvw, 1, false, glm::value_ptr(m_transform));
	pvw = glGetUniformLocation(shaderID, "camPos");
	glUniform4fv(pvw, 1, glm::value_ptr(camPos));
	pvw = glGetUniformLocation(shaderID, "time");
	glUniform1f(pvw, time);
	for (auto& gl : m_glInfo) {
		glBindVertexArray(gl.m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount * 3);
	}
}

bool Model::loadFromFile(const char * filename)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> meshes;
	std::vector <tinyobj::material_t> mtl;
	std::string err;
	bool ret = tinyobj::LoadObj(&attribs, &meshes, &mtl, &err, filename);

	if (err.length() > 0)
	{
		std::cout << err;
	}

	if (ret)
	{
		m_glInfo.resize(meshes.size());
		// grab each shape
		int shapeIndex = 0;
		for (auto& shape : meshes) {
			// setup OpenGL data 
			glGenVertexArrays(1, &m_glInfo[shapeIndex].m_VAO);
			glGenBuffers(1, &m_glInfo[shapeIndex].m_VBO);
			glBindVertexArray(m_glInfo[shapeIndex].m_VAO);
			m_glInfo[shapeIndex].m_faceCount = shape.mesh.num_face_vertices.size();
			// collect triangle vertices 
			std::vector<OBJVertex> vertices;
			int index = 0;
			for (auto face : shape.mesh.num_face_vertices) {
				for (int i = 0; i < 3; ++i) {
					tinyobj::index_t idx = shape.mesh.indices[index + i]; OBJVertex v = { 0 };
					// positions
					v.x = attribs.vertices[3 * idx.vertex_index + 0];
					v.y = attribs.vertices[3 * idx.vertex_index + 1];
					v.z = attribs.vertices[3 * idx.vertex_index + 2];
					// normals
					if (attribs.normals.size() > 0) {
						v.nx = attribs.normals[3 * idx.normal_index + 0];
						v.ny = attribs.normals[3 * idx.normal_index + 1];
						v.nz = attribs.normals[3 * idx.normal_index + 2];
					}
					// texture coordinates 
					if (attribs.texcoords.size() > 0) {
						v.u = attribs.texcoords[2 * idx.texcoord_index + 0];
						v.v = attribs.texcoords[2 * idx.texcoord_index + 1];
					} vertices.push_back(v);
				}
				index += face;
			} // bind vertex data 
			glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[shapeIndex].m_VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(OBJVertex), vertices.data(), GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			//position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), 0); glEnableVertexAttribArray(1);
			//normal data 
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(OBJVertex), (void*)12); glEnableVertexAttribArray(2);
			//texture data 
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(OBJVertex), (void*)24); 
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0); 
			shapeIndex++;
		}
	}
	return ret;
}