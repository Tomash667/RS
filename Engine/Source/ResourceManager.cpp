#include "Pch.h"
#include "ResourceManager.h"
#include "WICTextureLoader.h"
#include "QmshLoader.h"
#include "Render.h"
#include "Texture.h"
#include "Mesh.h"


static_assert(MAX_PATH == 260, "Invalid max path.");


Texture::~Texture()
{
	tex->Release();
}


ResourceManager::ResourceManager(Render* render) : render(render)
{
	assert(render);
	qmsh_loader = new QmshLoader(this, render->GetDevice(), render->GetContext());
}

ResourceManager::~ResourceManager()
{
	DeleteElements(resources);
	delete qmsh_loader;
}

Texture* ResourceManager::GetTexture(Cstring name)
{
	Texture* tex = (Texture*)Get(name, Resource::Texture);
	if(!tex)
	{
		ID3D11ShaderResourceView* view;
		HRESULT result = CreateWICTextureFromFile(render->GetDevice(), render->GetContext(), GetPath(name), nullptr, &view);
		if(FAILED(result))
			throw Format("Failed to load texture '%s' (%u).", name, result);

		tex = new Texture;
		tex->name = name;
		tex->type = Resource::Texture;
		tex->tex = view;
		resources.insert(tex);
	}
	return tex;
}

Mesh* ResourceManager::GetMesh(Cstring name)
{
	Mesh* mesh = (Mesh*)Get(name, Resource::Mesh);
	if(!mesh)
	{
		cstring path = Format("data/%s", name);
		try
		{
			mesh = qmsh_loader->Load(path);
		}
		catch(cstring err)
		{
			throw Format("Failed to load mesh '%s': %s", name, err);
		}
		mesh->name = name;
		mesh->type = Resource::Mesh;
		resources.insert(mesh);
	}
	return mesh;
}

Sound* ResourceManager::GetSound(Cstring name)
{
	Sound* sound = (Sound*)Get(name, Resource::Sound);
	if(!sound)
	{
		cstring path = Format("data/%s", name);
		sound_mgr->Load(path);
	}
	return sound;
}

Resource* ResourceManager::Get(cstring name, Resource::Type type)
{
	assert(name);

	static Resource res_search;
	res_search.name = name;
	auto it = resources.find(&res_search);
	if(it == resources.end())
		return nullptr;
	if((*it)->type != type)
		throw Format("Resource '%s' type mismatch.", name);
	return *it;
}

wchar_t* ResourceManager::GetPath(cstring name)
{
	cstring path = Format("Data/%s", name);
	size_t len;
	mbstowcs_s(&len, tmp_path, path, MAX_PATH);
	return tmp_path;
}
