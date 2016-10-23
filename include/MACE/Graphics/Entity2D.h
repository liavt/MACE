/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#include <MACE/Graphics/Model.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Shaders.h>
#include <MACE/Graphics/Buffer.h>

namespace mc {
namespace gfx {

	class Entity2D : public GraphicsEntity {
	public:
		Entity2D();
	protected:
	};//Entity2D

	class Image : public Entity2D {
	protected:
		void customInit();
		void customUpdate();
		void customRender();
		void customDestroy();

	};

	template<>
	class RenderProtocol<Entity2D> : public RenderImpl {
	public:

		void resize(const Size width, const Size height);

		void init(const Size originalWidth, const Size originalHeight);

		void setUp(win::Window* win, RenderQueue* queue);

		void render(win::Window* win,void* entity);

		void tearDown(win::Window* win, RenderQueue* queue);

		void destroy();
	private:

		UBO windowData = UBO();
		UBO paintData = UBO();
		UBO entityData = UBO();

		ShaderProgram shaders2D = ShaderProgram();
		VAO square = VAO();
	};

}//gfx
}//mc