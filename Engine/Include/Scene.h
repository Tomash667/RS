#pragma once

struct Scene
{
	Scene(Render* render);
	~Scene();
	void Add(SceneNode* node);
	void Draw();
	void Init();
	void Remove(SceneNode* node);

	Camera* GetCamera() { return camera; }

private:
	void DrawNodes(vector<SceneNode*>& nodes, SceneNode* parent, Matrix* parent_matrix);

	Render* render;
	Camera* camera;
	vector<SceneNode*> nodes;
	MeshShader* shader;
	Matrix mat_view_proj, mat_combined;
};
