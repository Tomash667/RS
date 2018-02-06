#pragma once

struct QmshLoader
{
	QmshLoader(ResourceManager* res_mgr, ID3D11Device* device, ID3D11DeviceContext* context);
	Mesh* Load(cstring path);

	ResourceManager* res_mgr;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	vector<byte> buf;
};
