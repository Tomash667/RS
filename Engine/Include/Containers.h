#pragma once

template<typename Container, typename Element>
void DeleteElement(Container& c, Element e)
{
	for(auto it = c.begin(), end = c.end(); it != end; ++it)
	{
		if(*it == e)
		{
			delete e;
			std::iter_swap(it, end - 1);
			c.pop_back();
			return;
		}
	}
	assert(0);
}

template<typename T>
void DeleteElements(T& c)
{
	for(auto& e : c)
		delete e;
	c.clear();
}
