#pragma once

struct Scene
{
	Scene(Render* render);
	~Scene();
	void Add(SceneNode* node);
	void Draw();
	void Init();

	Camera* GetCamera() { return camera; }

private:
	Render* render;
	Camera* camera;
	vector<SceneNode*> nodes;
	MeshShader* shader;
};
