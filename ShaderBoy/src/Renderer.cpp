#include <iostream>

#include "Renderer.h"

void GLAPIENTRY MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	// TODO: instead of enum values print strings http://docs.gl/gl4/glDebugMessageControl
	std::cerr << "[GL Debug Call]"
		<< "debug type: 0x" << std::hex << type
		<< ", severity: 0x" << std::hex << severity
		<< ", message[" << id << "]: " << message
		<< std::endl;
	if (severity == GL_DEBUG_SEVERITY_HIGH) {
		std::cerr << "High severity. Aborting..." << std::endl;
		std::abort();
	}
}

void Renderer::Clear() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);

	// TODO: unbind in Debug mode
}