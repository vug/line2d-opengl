#include "Flashes.h"

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"

#include "Renderer.h"
#include "VertexBufferLayout.h"

#include "parameter.h"

Flashes::Flashes(GLFWwindow* window)
	: m_Window(window)
{
	float quad[] = {
		-1.0f, -1.0f,   0.0f, 0.0f, // 0
		 1.0f, -1.0f,   1.0f, 0.0f, // 1
		 1.0f,  1.0f,   1.0f, 1.0f, // 2
		-1.0f,  1.0f,   0.0f, 1.0f, // 3
	};
	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0,
	};
	
	m_VertexArray = std::make_unique<VertexArray>();
	m_VertexBuffer = std::make_unique<VertexBuffer>(quad, 4 * 4 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(2); // 2D position
	layout.Push<float>(2); // 2D texture-coordinates
	m_VertexArray->AddBuffer(*m_VertexBuffer, layout);
	m_IndexBuffer = std::make_unique<IndexBuffer>(indices, 2 * 3);

	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);  // normalized coordinates
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 mvp = projection * view * model;

	m_Shader = std::make_unique<Shader>("res/shaders/Flashes.shader");
	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_MVP", mvp);

	ConstantParameter<int> p1(5);
	std::cout << "HAYDAR!!" << p1.getValueAt(10.0f) << std::endl;
	KeyFramedParameter p2({ {0.0, 1.0}, {1.0, 5.0}, {2.0, -3.0} }, Interpolation::linear, Extrapolation::constant);
	
	for (auto& fr : p2.keyFrames) {
		std::cout << fr.first << " " << fr.second << std::endl;
	}
	for (auto &x : { -0.5, 0.1, 0.4, 0.6, 1.1, 1.5, 1.7, 5.5 }) {
		std::cout << x << " -> " << p2.getValueAt(x) << std::endl;
	}
}

Flashes::~Flashes() {}

void Flashes::OnUpdate(double time)
{
	m_EmissionCoef = sin(2 * 3.14159265 * time) + 1.0;
}

void Flashes::OnRender()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	m_Shader->Bind();
	m_Shader->SetUniform1f("u_ReflectionCoef", m_ReflectionCoef);
	m_Shader->SetUniform1f("u_RefractionCoef", m_RefractionCoef);
	m_Shader->SetUniform1f("u_EmissionCoef", m_EmissionCoef);
	int windowWidth, windowHeight;
	glfwGetWindowSize(m_Window, &windowWidth, &windowHeight);
	m_Shader->SetUniform2i("u_Resolution", windowWidth, windowHeight);
	double time = glfwGetTime();
	m_Shader->SetUniform1f("u_Time", time);
	double mouseX, mouseY;
	glfwGetCursorPos(m_Window, &mouseX, &mouseY);
	int state = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT);
	if (state == GLFW_PRESS)
	{
		m_Shader->SetUniform2f("u_Mouse", mouseX, windowHeight - mouseY);
	}

	Renderer renderer;
	renderer.Draw(*m_VertexArray, *m_IndexBuffer, *m_Shader);
}

void Flashes::OnImGuiRender()
{
	ImGui::SliderFloat("Reflection Coefficient", &m_ReflectionCoef, 0.0, 2.0);
	ImGui::SliderFloat("Refraction Coefficient", &m_RefractionCoef, 0.0, 5.0);
	ImGui::SliderFloat("Emission Coefficient", &m_EmissionCoef, 0.0, 5.0);
}