#include "FieldVisualiser.h"
#include <gl_core_4_4.h>
#include "Scene.h"

FieldVisualiser::FieldVisualiser()
{
}


FieldVisualiser::~FieldVisualiser()
{
}

void FieldVisualiser::init(uint field, glm::vec3 fieldShape, glm::mat4 fieldTransform, int scanRes)
{
	m_model = Model();
	m_model.load("Meshes/Cube.obj");
	m_ppModel = Model();
	m_ppModel.generateScreenSpaceQuad();
	m_backFace = FrameBuffer(1280, 720);
	m_backFace.GenBuffer();
	m_frontFace = FrameBuffer(1280, 720);
	m_frontFace.GenBuffer();
	m_field = field;
	m_fieldShape = fieldShape;
	m_fieldTransform = fieldTransform;
	m_scanResolution = scanRes;

	m_preProcess = Shader();
	m_preProcess.CompileShaders("Shaders/BasicVertShaderStatic.txt", "Shaders/PositionFrag.txt");
	m_postProcess = Shader();
	m_postProcess.CompileShaders("Shaders/PostProcessVert.txt", "Shaders/RaycastVolumeFrag.txt");

}

void FieldVisualiser::draw(const Camera * c, uint m_buffer, int width, int height)
{
	//Draw front face positions
	glBindFramebuffer(GL_FRAMEBUFFER, m_frontFace.getBuf());
	glViewport(0, 0, m_frontFace.getW(), m_frontFace.getH());
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_model.draw(m_preProcess.GetID(), c->projection * c->view, glm::vec4(0), 0, 0, m_fieldTransform);


	//Draw back face positions
	glBindFramebuffer(GL_FRAMEBUFFER, m_backFace.getBuf());
	glViewport(0, 0, m_backFace.getW(), m_backFace.getH());
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	m_model.draw(m_preProcess.GetID(), c->projection * c->view, glm::vec4(0), 0, 0, m_fieldTransform);
	glCullFace(GL_BACK);

	//Rebind main buffer
	glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
	glViewport(0, 0, width, height);
	
	//Draw screen space quad with custom depth buffer
	glUseProgram(m_postProcess.GetID());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_backFace.getTex());
	int loc = glGetUniformLocation(m_postProcess.GetID(), "back");
	glUniform1i(loc, 0);


	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_frontFace.getTex());
	loc = glGetUniformLocation(m_postProcess.GetID(), "front");
	glUniform1i(loc, 1);
	
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_3D, m_field);
	loc = glGetUniformLocation(m_postProcess.GetID(), "field");
	glUniform1i(loc, 2);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_frontFace.getDep());
	loc = glGetUniformLocation(m_postProcess.GetID(), "depth");
	glUniform1i(loc, 3);

	loc = glGetUniformLocation(m_postProcess.GetID(), "maxDim");
	glUniform1f(loc, glm::compMax(m_fieldShape));

	loc = glGetUniformLocation(m_postProcess.GetID(), "steps");
	glUniform1i(loc, m_scanResolution);

	m_ppModel.drawGenericScreenQuad();
}
