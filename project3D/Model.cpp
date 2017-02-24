
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Model.h"
#include <vector>
#include <string>
#include <gl_core_4_4.h>
#include <iostream>
#include <Texture.h>

Model::Model()
{
}

Model::Model(const Model & other)
{
	m_glInfo = other.m_glInfo;
	m_isAnimated = other.m_isAnimated;
	m_model = other.m_model;
}

Model::~Model()
{

}



void Model::update(float time)
{
	if (m_isAnimated)
	{
		FBXSkeleton* skeleton = m_model->getSkeletonByIndex(0);
		skeleton->evaluate(m_model->getAnimationByIndex(0), time);

		for (unsigned int bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
		{
			skeleton->m_nodes[bone_index]->updateGlobalTransform();
		}
	}
}

void Model::draw(unsigned int shaderID, glm::mat4 camera, glm::vec4 camPos, float time, unsigned int textureID, glm::mat4 transform)
{
	glUseProgram(shaderID);

	unsigned int pvw = glGetUniformLocation(shaderID, "PVW");
	glUniformMatrix4fv(pvw, 1, false, glm::value_ptr(camera * transform));
	pvw = glGetUniformLocation(shaderID, "lightDir");
	glUniform3fv(pvw, 1, glm::value_ptr(glm::vec3(0, 1, 1)));
	pvw = glGetUniformLocation(shaderID, "M");
	glUniformMatrix4fv(pvw, 1, false, glm::value_ptr(transform));
	pvw = glGetUniformLocation(shaderID, "camPos");
	glUniform4fv(pvw, 1, glm::value_ptr(camPos));
	pvw = glGetUniformLocation(shaderID, "time");
	glUniform1f(pvw, time);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);

	pvw = glGetUniformLocation(shaderID, "diffuse");
	glUniform1i(pvw, 0);
	

	pvw = glGetUniformLocation(shaderID, "animated");
	if (m_isAnimated)
	{
		glUniform1i(pvw, 1);
		FBXSkeleton* skeleton = m_model->getSkeletonByIndex(0);
		skeleton->updateBones();
		pvw = glGetUniformLocation(shaderID, "bones");
		glUniformMatrix4fv(pvw, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);
	}
	else
		glUniform1i(pvw, 0);

	for (auto& gl : m_glInfo) {
		glBindVertexArray(gl.m_VAO);
		if (gl.m_IBO == -1)
			glDrawArrays(GL_TRIANGLES, 0, gl.m_faceCount);
		else
			glDrawElements(GL_TRIANGLES, gl.m_faceCount * 3, GL_UNSIGNED_INT, 0);
	}
}

void Model::drawPostProcessQuad(unsigned int shaderID, FrameBuffer buf)
{
	glUseProgram(shaderID);

	unsigned int pvw;

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, buf.getTex());

	pvw = glGetUniformLocation(shaderID, "screen");
	glUniform1i(pvw, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, buf.getDep());
	
	pvw = glGetUniformLocation(shaderID, "depthBuffer");
	glUniform1i(pvw, 0);


	glBindVertexArray(m_glInfo[0].m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool Model::loadFromFBX(const char * filename)
{
	m_model = new FBXFile();
	bool ret = m_model->load(filename);
 	if (ret)
	{

		m_isAnimated = m_model->getSkeletonCount() > 0;

		m_glInfo.resize(m_model->getMeshCount());
		for (int i = 0; i < m_glInfo.size(); i++)
		{
			auto mesh = m_model->getMeshByIndex(i);

			glGenVertexArrays(1, &m_glInfo[i].m_VAO);
			glGenBuffers(1, &m_glInfo[i].m_VBO);
			glGenBuffers(1, &m_glInfo[i].m_IBO);
			
			glBindVertexArray(m_glInfo[i].m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[i].m_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glInfo[i].m_IBO);

			glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::NormalOffset);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TexCoord1Offset);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(FBXVertex), ((char*)0) + FBXVertex::TangentOffset);
			if (m_isAnimated)
			{
				glEnableVertexAttribArray(4);
				glEnableVertexAttribArray(5);
				glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::WeightsOffset);
				glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), ((char*)0) + FBXVertex::IndicesOffset);
			}

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			m_glInfo[i].m_faceCount = (unsigned int)mesh->m_indices.size()/3;
		}
	}

	return ret;

}

void Model::generateScreenSpaceQuad()
{
	m_glInfo.push_back(OpenGLInfo());
	glm::vec2 halfTexel = 1.0f / glm::vec2(1280, 720) * 0.5f;
	float vertexData[] =	{ -1, -1, 0, 1, halfTexel.x, halfTexel.y,
							1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y,
							-1, 1, 0, 1, halfTexel.x, 1 - halfTexel.y,

							-1, -1, 0, 1, halfTexel.x, halfTexel.y,
							1, -1, 0, 1, 1 - halfTexel.x, halfTexel.y,
							1, 1, 0, 1, 1 - halfTexel.x, 1 - halfTexel.y, };
	glGenVertexArrays(1, &m_glInfo[0].m_VAO);
	glBindVertexArray(m_glInfo[0].m_VAO);
	glGenBuffers(1, &m_glInfo[0].m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[0].m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6, vertexData, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, ((char*)0) + 16);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

Instance::Instance()
{
}

Instance::~Instance()
{
}

Instance::Instance(const Instance & other)
{
	m_transform = other.m_transform;
	m_texture = other.m_texture;
	m_shader = other.m_shader;
	m_model = other.m_model;
}

void Instance::loadTex(const char * filename)
{
	aie::Texture t;
	t.load(filename);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t.getWidth(), t.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, t.getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Instance::draw(glm::mat4 camera, glm::vec4 camPos, float time, float animTime)
{
	m_model->update(animTime);
	m_model->draw(m_shader, camera, camPos, time, m_texture, m_transform);
}
