#pragma once

struct Scene
{
	Scene();
	~Scene();
	void Add(SceneNode* node);
	void Draw();
	Camera* GetCamera() { return camera; }

private:
	Camera* camera;
	vector<SceneNode*> nodes;
};
