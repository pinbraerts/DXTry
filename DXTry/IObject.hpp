#ifndef DXTRY_IOBJECT_HPP
#define DXTRY_IOBJECT_HPP

#include "includes.hpp"

struct IObject {
	virtual void init(Engine& engine) = 0;
	virtual void update(Engine& engine) = 0;
	virtual void render(Engine& engine) = 0;
	// void destroy(Engine& engine) = 0;
};

#endif // !DXTRY_IOBJECT_HPP
