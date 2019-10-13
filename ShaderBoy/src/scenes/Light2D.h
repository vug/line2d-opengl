#pragma once

#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Scene.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class Light2D : public Scene
{
public:
	Light2D(GLFWwindow* window);
	~Light2D();

	void OnRender() override;
	void OnImGuiRender() override;

private:
	GLFWwindow* m_Window;

	std::unique_ptr<VertexArray> m_VertexArray;
	std::unique_ptr<VertexBuffer> m_VertexBuffer;
	std::unique_ptr<IndexBuffer> m_IndexBuffer;
	std::unique_ptr<Shader> m_Shader;

	float m_ReflectionCoef = 0.9;
};