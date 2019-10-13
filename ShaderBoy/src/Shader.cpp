#include <fstream>
#include <iostream>
#include <sstream>

#include "Shader.h"
#include "Renderer.h"

Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererId(0)
{
	ShaderProgramSource programSource = parseShader(m_FilePath);
	m_RendererId = CreateShader(programSource.VertexSource, programSource.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererId);
}

ShaderProgramSource Shader::parseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);
	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream stringStreams[2];
	ShaderType shaderType = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				shaderType = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				shaderType = ShaderType::FRAGMENT;
			}
		}
		else
		{
			stringStreams[(int)shaderType] << line << '\n';
		}
	}

	return { stringStreams[0].str(), stringStreams[1].str() };
}

unsigned int Shader::CompileShader(unsigned int shaderType, const std::string& source)
{
	unsigned int shaderId = glCreateShader(shaderType);
	const char* src = source.c_str();  // source should exists, not be null.
	glShaderSource(shaderId, 1, &src, nullptr);
	glCompileShader(shaderId);

	int result;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shaderId, length, &length, message);
		std::cout << "Failed to compile " <<
			(shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment") <<
			" shader!" << std::endl;
		std::cout << message << std::endl;
		glDeleteShader(shaderId);
		return 0;
	}

	m_UniformLocationCache.clear();
	return shaderId;
}

unsigned int Shader::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
	unsigned int program = glCreateProgram();  // could have used GLuint
	unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

void Shader::Bind() const
{
	glUseProgram(m_RendererId);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string& name, int i0)
{
	int location = GetUniformLocation(name);
	glUniform1i(location, i0);
}

void Shader::SetUniform2i(const std::string& name, int i0, int i1)
{
	int location = GetUniformLocation(name);
	glUniform2i(location, i0, i1);
}

void Shader::SetUniform1f(const std::string& name, float f0)
{
	int location = GetUniformLocation(name);
	glUniform1f(location, f0);
}

void Shader::SetUniform2f(const std::string& name, float f0, float f1)
{
	int location = GetUniformLocation(name);
	glUniform2f(location, f0, f1);
}

void Shader::SetUniform4f(const std::string& name, float f0, float f1, float f2, float f3)
{
	int location = GetUniformLocation(name);
	glUniform4f(location, f0, f1, f2, f3);
}

void Shader::SetUniformMat4f(const std::string& name, const glm::mat4& matrix)
{
	int location = GetUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]);
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}
	int location = glGetUniformLocation(m_RendererId, name.c_str());
	if (location == -1)
	{
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}