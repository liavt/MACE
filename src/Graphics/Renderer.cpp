#define MACE_ENTITY2D_EXPOSE_X_MACRO
#include <MC-Graphics/Renderer.h>
#include <MC-Graphics/GLUtil.h>

namespace mc {
namespace gfx {

int RenderImpl::index = -1;

std::queue<std::pair<Index,Entity*>> Renderer::renderQueue = std::queue<std::pair<Index, Entity*>>();
std::vector<RenderImpl*> Renderer::protocols = std::vector<RenderImpl*>();

void Renderer::init()
{
}//init
void Renderer::setUp()
{
	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->setUp();
	}
}//setUp
void Renderer::resize(const Size width, const Size height)
{
	glViewport(0,0,width,height);
	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->resize(width,height);
	}
}//tearDown
void Renderer::tearDown()
{
	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->tearDown();
	}
}//tearDown
//resize
void Renderer::renderFrame()
{
	setUp();
	while (!renderQueue.empty()) {
		const std::pair<Index, Entity*> pair = renderQueue.front();
		protocols.at(pair.first)->render(pair.second);
		renderQueue.pop();
	}
	tearDown();
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
}//setRefreshColor

RenderImpl::RenderImpl()
{
}//RenderImpl()

}//gfx
}//mc