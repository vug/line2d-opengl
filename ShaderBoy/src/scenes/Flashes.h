#pragma once

#include <memory>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include "parameter.h"

class Flashes : public Scene
{
public:
	Flashes(GLFWwindow* window);
	~Flashes();

	void OnUpdate(double time) override;
	void OnRender() override;
	void OnImGuiRender() override;

private:
	GLFWwindow* m_Window;

	std::unique_ptr<VertexArray> m_VertexArray;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<Shader> m_Shader;

	float m_ReflectionCoef = 0.9;
	float m_RefractionCoef = 0.8;
	float m_EmissionCoef = 2.0;
	KeyFramedParameter<double> emitParam;
};