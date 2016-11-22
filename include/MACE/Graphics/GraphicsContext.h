/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_GRAPHICSCONTEXT_H
#define MACE_GRAPHICS_GRAPHICSCONTEXT_H

#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Entity.h>

namespace mc {
	namespace gfx {

		class OpenGLContext: public Entity, public GraphicsContext {
			bool vsync;

		public:
			OpenGLContext();

			void update();

			void setUpWindow(Window* win);
			void init(Window* win);
			void render(Window* win);
			void destroy(Window* win);

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

#endif