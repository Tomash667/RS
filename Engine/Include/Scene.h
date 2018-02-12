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
	void DrawNodes(vector<SceneNode*>& nodes);

	Render* render;
	Camera* camera;
	vector<SceneNode*> nodes;
	MeshShader* shader;
	Matrix mat_view_proj, mat_world, mat_combined;
};
