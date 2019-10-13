#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "IndexBuffer.h"
#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


int main(void)
{
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	window = glfwCreateWindow(1024, 1024, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEBUG_OUTPUT); // TODO: wrap with IF DEBUG
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(MessageCallback, 0);

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

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	VertexArray vertexArray;
	VertexBuffer vertexBuffer(quad, 4 * 4 * sizeof(float));
	VertexBufferLayout layout;
	layout.Push<float>(2); // 2D position
	layout.Push<float>(2); // 2D texture-coordinates
	vertexArray.AddBuffer(vertexBuffer, layout);

	IndexBuffer indexBuffer(indices, 2 * 3);
	glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);  // normalized coordinates
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));

	glm::mat4 mvp = projection * view * model;

	Shader shader("res/shaders/Light2Db.shader");
	shader.Bind();
	shader.SetUniformMat4f("u_MVP", mvp);

	// Unbound/Clear everything
	vertexArray.Unbind();
	shader.Unbind();
	vertexBuffer.Unbind();
	indexBuffer.Unbind();

	Renderer renderer;

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 460";
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark();

	float red = 0.0f;
	float increment = 0.05f;
	float time;
	int windowWidth, windowHeight;
	double mouseX, mouseY;
	/* Loop until the user closes the window */
	glfwSetTime(0.0);
	while (!glfwWindowShouldClose(window))
	{
		renderer.Clear();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		shader.Bind();
		glfwGetWindowSize(window, &windowWidth, &windowHeight);
		shader.SetUniform2i("u_Resolution", windowWidth, windowHeight);
		time = glfwGetTime();
		shader.SetUniform1f("u_Time", time);
		glfwGetCursorPos(window, &mouseX, &mouseY);
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if (state == GLFW_PRESS)
		{
			shader.SetUniform2f("u_Mouse", mouseX, windowHeight - mouseY);
		}

		renderer.Draw(vertexArray, indexBuffer, shader);

		ImGui::Begin("My GUI");
		ImGui::SliderFloat("redness", &red, 0.0, 1.0);
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (red > 1.0f) increment = -0.05f;
		else if (red < 0.0f) increment = 0.05f;
		red += increment;

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}