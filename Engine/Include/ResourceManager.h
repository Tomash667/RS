#pragma once

#include "Resource.h"

struct ResourceManager
{
	ResourceManager(Render* render);
	~ResourceManager();
	Texture* GetTexture(Cstring name);
	Mesh* GetMesh(Cstring name);

private:
	struct ResourceComparer
	{
		bool operator () (const Resource* r1, const Resource* r2) const
		{
			return _stricmp(r1->name.c_str(), r2->name.c_str()) > 0;
		}
	};
	typedef std::set<Resource*, ResourceComparer> ResourceContainer;
	typedef ResourceContainer::iterator ResourceIterator;

	Resource* Get(cstring name, Resource::Type type);
	wchar_t* GetPath(cstring name);

	Render* render;
	QmshLoader* qmsh_loader;
	ResourceContainer resources;
	Resource res_search;
	wchar_t tmp_path[260];
};
