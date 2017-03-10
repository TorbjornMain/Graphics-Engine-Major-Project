
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include "Model.h"
#include <vector>
#include <string>
#include <gl_core_4_4.h>
#include <iostream>
#include <Texture.h>
#include <Gizmos.h>

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

		for (uint bone_index = 0; bone_index < skeleton->m_boneCount; ++bone_index)
		{
			skeleton->m_nodes[bone_index]->updateGlobalTransform();
		}
	}
}

void Model::draw(uint shaderID, glm::mat4 camera, glm::vec4 camPos, float time, uint textureID, glm::mat4 transform)
{
	glUseProgram(shaderID);

	uint pvw = glGetUniformLocation(shaderID, "PVW");
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

void Model::drawPostProcessQuad(uint shaderID, FrameBuffer buf)
{
	glUseProgram(shaderID);

	uint pvw;

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

void Model::drawGenericScreenQuad()
{
	glBindVertexArray(m_glInfo[0].m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool Model::load(const char * filename)
{
	m_model = new FBXFile();
	bool ret = m_model->load(filename);
 	if (ret)
	{
		m_isAnimated = m_model->getSkeletonCount() > 0;
		m_upperBound = glm::zero<glm::vec4>();
		m_lowerBound = glm::zero<glm::vec4>();
		m_glInfo.resize(m_model->getMeshCount());
		for (uint i = 0; i < m_glInfo.size(); i++)
		{
			auto mesh = m_model->getMeshByIndex(i);
			for (auto i : mesh->m_vertices)
			{
				m_upperBound = glm::vec4(glm::max(m_upperBound.x, i.position.x), glm::max(m_upperBound.y, i.position.y), glm::max(m_upperBound.z, i.position.z), 1);
				m_lowerBound = glm::vec4(glm::min(m_lowerBound.x, i.position.x), glm::min(m_lowerBound.y, i.position.y), glm::min(m_lowerBound.z, i.position.z), 1);
			}
			glGenVertexArrays(1, &m_glInfo[i].m_VAO);
			glGenBuffers(1, &m_glInfo[i].m_VBO);
			glGenBuffers(1, &m_glInfo[i].m_IBO);
			
			glBindVertexArray(m_glInfo[i].m_VAO);
			glBindBuffer(GL_ARRAY_BUFFER, m_glInfo[i].m_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_glInfo[i].m_IBO);

			glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(uint), mesh->m_indices.data(), GL_STATIC_DRAW);

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
			m_glInfo[i].m_faceCount = (uint)mesh->m_indices.size()/3;
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
	uint format = 0;
	switch (t.getFormat())
	{
	case(aie::Texture::Format::RED):
		format = GL_RED;
		break;
	case(aie::Texture::Format::RG):
		format = GL_RG;
		break;
	case(aie::Texture::Format::RGB):
		format = GL_RGB;
		break;
	case(aie::Texture::Format::RGBA):
		format = GL_RGBA;
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, format, t.getWidth(), t.getHeight(), 0, format, GL_UNSIGNED_BYTE, t.getPixels());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Instance::draw(glm::mat4 camera, glm::mat4 camTransform, float time, float animTime)
{
	glm::vec4 centre = m_transform * ((m_model->getUpperBound() + m_model->getLowerBound())/2);
	float radius = (glm::vec3(m_transform*m_model->getUpperBound() - m_transform*m_model->getLowerBound())).length()/2;

	aie::Gizmos::addSphere(glm::vec3(centre), radius, 5, 5, glm::vec4(0));


	glm::vec4 planes[6];
	planes[0] = glm::vec4(camera[0][3] - camera[0][0], camera[1][3] - camera[1][0], camera[2][3] - camera[2][0], camera[3][3] - camera[3][0]);
	planes[1] = glm::vec4(camera[0][3] + camera[0][0], camera[1][3] + camera[1][0], camera[2][3] + camera[2][0], camera[3][3] + camera[3][0]);
	planes[2] = glm::vec4(camera[0][3] - camera[0][1], camera[1][3] - camera[1][1], camera[2][3] - camera[2][1], camera[3][3] - camera[3][1]);
	planes[3] = glm::vec4(camera[0][3] + camera[0][1], camera[1][3] + camera[1][1], camera[2][3] + camera[2][1], camera[3][3] + camera[3][1]);
	planes[4] = glm::vec4(camera[0][3] - camera[0][2], camera[1][3] - camera[1][2], camera[2][3] - camera[2][2], camera[3][3] - camera[3][2]);
	planes[5] = glm::vec4(camera[0][3] + camera[0][2], camera[1][3] + camera[1][2], camera[2][3] + camera[2][2], camera[3][3] + camera[3][2]);

	bool render = true;
	for (int i = 0; i < 6; i++)
	{
		float d = glm::length(glm::vec3(planes[i]));
		planes[i] /= d;

		float dist = glm::dot(glm::vec3(planes[i]), glm::vec3(centre)) + planes[i].w;
		if (dist < -radius)
		{
			render = false;
			break;
		}

	}


	if (render)
	{
		m_model->update(animTime);
		m_model->draw(m_shader, camera, glm::column(camTransform, 3), time, m_texture, m_transform);
	}
}
