#pragma once

template<typename T>
void DeleteElements(T& c)
{
	for(auto& e : c)
		delete e;
	c.clear();
}
