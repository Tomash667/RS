#pragma once

struct Scene
{
	Scene();
	~Scene();
	void Add(SceneNode* node);
	Camera* GetCamera() { return camera; }

private:
	Camera* camera;
};
