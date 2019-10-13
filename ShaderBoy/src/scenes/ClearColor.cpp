#include "ClearColor.h"

#include "imgui/imgui.h"

#include "Renderer.h"

ClearColor::ClearColor()
	: m_ClearColor{ 0.2f, 0.3f, 0.8f, 1.0f } {}

ClearColor::~ClearColor() {}

void ClearColor::OnRender()
{
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void ClearColor::OnImGuiRender()
{
	ImGui::ColorEdit4("Clear Color", m_ClearColor);
}