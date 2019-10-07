#pragma once

#include<string>
#include<unordered_map>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererId;
	std::unordered_map<std::string, int> m_UniformLocationCache;

	ShaderProgramSource parseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int shaderType, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

	int GetUniformLocation(const std::string& name);
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int i0);
	void SetUniform2i(const std::string& name, int i0, int i1);
	void SetUniform1f(const std::string& name, float f0);
	void SetUniform2f(const std::string& name, float f0, float f1);
	void SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3);
};