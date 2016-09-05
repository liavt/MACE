#pragma once

#include <MACE/Graphics/Entity.h>
#include <MACE/Window/Window.h>
#include <queue>


namespace mc {
namespace gfx {


class RenderImpl {
public:
	static int index;

	RenderImpl();

	virtual void resize(const Size width, const Size height) = 0;

	virtual void init(const Size originalWidth, const Size originalHeight) = 0;
	virtual void setUp(win::Window* win) = 0;
	virtual void render(win::Window* win,void* entity)=0;
	virtual void tearDown(win::Window* win) = 0;
	virtual void destroy(win::Window* win) = 0;
};

template<typename T>
class RenderProtocol : public RenderImpl{
public:
	using RenderImpl::RenderImpl;

	void resize(const Size width, const Size height) {};

	void init(const Size originalWidth, const Size originalHeight) {}
	void setUp(win::Window* win) {};
	void render(win::Window* win,void* entity) { std::cout << "hello"; };
	void tearDown(win::Window* win) {};
	void destroy(win::Window* win) {};
};

class Renderer {
public:

	static void resize(const Size width, const Size hieght);

	static void init(const Size originalWidth, const Size originalHeight);

	static void setUp(win::Window* win);

	template<typename T>
	static void queue(T* e) {
#ifdef MACE_ERROR_CHECK
		if (e == nullptr || e == NULL)throw NullPointer("Input pointer to an entity must not be null in queue()");
#endif
		if (RenderProtocol<T>::index == -1)registerProtocol<T>();
		renderQueue.push(std::pair<Index, Entity*>(RenderProtocol<T>::index, e));
	};

	static void tearDown(win::Window* win);

	static void renderFrame(win::Window* win);

	static void destroy(win::Window* win);

	static void setRefreshColor(const float r, const float g, const float b, const float a=1.0f);
	static void setRefreshColor(const Color& c);

private:

	static std::queue<std::pair<Index,Entity*>> renderQueue;
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