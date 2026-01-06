#pragma once
#include <scene.h>
#include <renderObject.h>
#include<demoShaderLoader.h>
class TestingScene : public Scene
{
private:
	RenderObject object1;
	Shader defaultShader;
public:
	void init() override;
	void update() override;
};
