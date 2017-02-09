#pragma once
class Shader
{
public:
	Shader();
	~Shader();

	const unsigned int GetID(){ return m_id; }
	void CompileShaders(const char* vert, const char* frag);
private:
	void loadAndCompileShader(unsigned int shaderID, const char* filename);
	unsigned int m_id;
};

