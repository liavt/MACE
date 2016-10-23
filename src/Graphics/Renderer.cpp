/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define MACE_ENTITY2D_EXPOSE_X_MACRO
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/GLUtil.h>

namespace mc {
namespace gfx {

int RenderImpl::index = -1;

RenderQueue Renderer::renderQueue = RenderQueue();
std::vector<RenderImpl*> Renderer::protocols = std::vector<RenderImpl*>();

Size Renderer::originalWidth = 0;
Size Renderer::originalHeight = 0;

void Renderer::init(const Size width, const Size height)
{
	Renderer::originalWidth = width;
	Renderer::originalHeight = height;
}//init
void Renderer::setUp(win::Window* win)
{
	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->setUp(win,&renderQueue);
	}
}//setUp
void Renderer::resize(const Size width, const Size height)
{
	glViewport(0,0,width,height);
	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->resize(width,height);
	}
}//tearDown
void Renderer::tearDown(win::Window* win)
{
	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->tearDown(win, &renderQueue);
	}
}//tearDown
//resize
void Renderer::renderFrame(win::Window* win)
{
	setUp(win);
	for (RenderQueue::iterator pair = renderQueue.begin(); pair != renderQueue.end();++pair) {
		protocols[pair->first]->render(win, pair->second);
	}
	tearDown(win);
	renderQueue.clear();
}//renderFrame
void Renderer::destroy()
{
	while(!protocols.empty()){
		RenderImpl* protocol = protocols.back();
		protocol->destroy();
		delete protocol;
		protocols.pop_back();
	}
}//destroy
void Renderer::setRefreshColor(const float r, const float g, const float b, const float a)
{
	glClearColor(r,g,b,a);
}//setRefreshColor
void Renderer::setRefreshColor(const Color & c)
{
	setRefreshColor(c.r,c.g,c.b,c.a);
}
Renderer::~Renderer()
{
	destroy();
}
//setRefreshColor

RenderImpl::RenderImpl()
{
}//RenderImpl()

}//gfx
}//mc