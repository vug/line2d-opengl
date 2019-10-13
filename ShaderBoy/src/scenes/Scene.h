#pragma once

class Scene
{
public:
	Scene() {}
	virtual ~Scene() {}

	virtual void OnUpdate(double time) {}
	virtual void OnRender() {}
	virtual void OnImGuiRender() {}
};