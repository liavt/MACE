/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#include <MACE/Graphics/Entity.h>
#include <MACE/Window/Window.h>
#include <deque>


namespace mc {
namespace gfx {

//if the container we use is ever going to be changed, we typedef 
using RenderQueue = std::deque<std::pair<Index, Entity*>>;

//forward define for friends
class Renderer;

//we declare RenderImpl which RenderProtocol extends. WE can't store a pointer to RenderProtocol (because its templated), but we can point to RenderImpl
class RenderImpl {
	friend class Renderer;
public:
	RenderImpl();
	virtual ~RenderImpl() = default;

	virtual void resize(const Size width, const Size height) = 0;

	virtual void init(const Size originalWidth, const Size originalHeight) = 0;
	virtual void setUp(win::Window* win, RenderQueue* queue) = 0;
	virtual void render(win::Window* win,void* entity)=0;
	virtual void tearDown(win::Window* win, RenderQueue* queue) = 0;
	virtual void destroy() = 0;
private:
	static int index;
};

template<typename T>
class RenderProtocol : public RenderImpl{
public:
	using RenderImpl::RenderImpl;

	void resize(const Size width, const Size height) {};

	void init(const Size originalWidth, const Size originalHeight) {}
	void setUp(win::Window* win, RenderQueue* queue) {};
	void render(win::Window* win,void* entity) {};
	void tearDown(win::Window* win,RenderQueue* queue) {};
	void destroy() {};
};

class Renderer {
public:

	static void resize(const Size width, const Size hieght);

	static void init(const Size originalWidth, const Size originalHeight);

	static void setUp(win::Window* win);

	template<typename T>
	static void queue(T* e) {
		if (e == nullptr || e == NULL)throw NullPointerException("Input pointer to an entity must not be null in queue()");
		if (RenderProtocol<T>::index == -1)registerProtocol<T>();
		renderQueue.push_back(std::pair<Index, Entity*>(RenderProtocol<T>::index, e));
	};

	static void tearDown(win::Window* win);

	static void renderFrame(win::Window* win);

	static void destroy();

	static void setRefreshColor(const float r, const float g, const float b, const float a=1.0f);
	static void setRefreshColor(const Color& c);

private:
	Renderer();
	~Renderer();

	static RenderQueue renderQueue;
	static std::vector<RenderImpl*> protocols;

	static Size originalWidth, originalHeight;

	template<typename T>
	static void registerProtocol() {
		//in destroy(), this memory is deleted
		RenderImpl* protocol = new RenderProtocol<T>();
		protocol->init(originalWidth, originalHeight);
		protocols.push_back(protocol);
		RenderProtocol<T>::index = protocols.size() - 1;
	}
};//Renderer

}//gfx
}//mc