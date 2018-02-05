#pragma once

struct Scene
{
	Scene(Render* render);
	~Scene();
	void Add(SceneNode* node);
	void Draw();
	Camera* GetCamera() { return camera; }

private:
	Render* render;
	Camera* camera;
	vector<SceneNode*> nodes;
};
