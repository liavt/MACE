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

void Renderer::init(const Size originalWidth, const Size originalHeight)
{
	Renderer::originalWidth = originalWidth;
	Renderer::originalHeight = originalHeight;
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
void Renderer::destroy(win::Window* win)
{
	while(!protocols.empty()){
		RenderImpl* protocol = protocols.back();
		protocol->destroy(win);
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