/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#include <MACE/Window/WindowModule.h>
#include <MACE/Graphics/Entity.h>

namespace mc {

/**
Namespace containing classes used for Graphics in MACE.
*/
namespace gfx{

class OpenGLContext : public mc::gfx::Entity, public mc::win::GraphicsContext {
	bool vsync;

public:
	OpenGLContext();

	void update();

	void setUpWindow(win::Window* win);
	void init(win::Window* win);
	void render(win::Window* win);
	void destroy(win::Window* win);

	void setVSync(const bool& sync);
private:
	//these are for the Entity inheritence
	void customUpdate();
	void customRender();
	void customDestroy();
	void customInit();
};

}
}

