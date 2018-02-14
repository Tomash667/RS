#pragma once

struct Color
{
	union
	{
		uint value;
		struct
		{
			byte r;
			byte g;
			byte b;
			byte a;
		};
	};

	Color() {}
	Color(uint value) : value(value) {}
	Color(byte r, byte g, byte b, byte a) : r(r), g(g), b(b), a(a) {}
	Color(const Color& c) : value(c.value) {}

	bool operator == (const Color& c) const
	{
		return value == c.value;
	}
	bool operator != (const Color& c) const
	{
		return value != c.value;
	}
	operator Vec4 () const
	{
		return Vec4(float(r) / 255.f, float(g) / 255.f, float(b) / 255.f, float(a) / 255.f);
	}

	static Color Black;
	static Color White;
};
