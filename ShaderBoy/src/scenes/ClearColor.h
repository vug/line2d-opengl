#pragma once

#include "Scene.h"

class ClearColor : public Scene
{
public:
	ClearColor();
	~ClearColor();

	void OnRender() override;
	void OnImGuiRender() override;

private:
	float m_ClearColor[4];
};