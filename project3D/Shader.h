#pragma once
#include "typedefs.h"
class Shader
{
public:
	Shader();
	~Shader();

	const uint GetID(){ return m_id; }
	void CompileShaders(const char* vert, const char* frag, const char* geom = nullptr);
	void CompileUpdateShader(const char* vert, const char** varyings, uint numVaryings);
private:
	void loadAndCompileShader(uint shaderID, const char* filename);
	uint m_id;
};

