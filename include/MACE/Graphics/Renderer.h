#pragma once

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Shaders.h>
#include <queue>


namespace mc {
namespace gfx {


class RenderImpl {
public:
	static int index;

	RenderImpl();

	virtual void resize(const Size width, const Size height) = 0;

	virtual void init() = 0;
	virtual void setUp() = 0;
	virtual void render(void* entity)=0;
	virtual void tearDown() = 0;
	virtual void destroy() = 0;
};

template<typename T>
class RenderProtocol : public RenderImpl{
public:
	using RenderImpl::RenderImpl;

	void resize(const Size width, const Size height) {};

	void init() {}
	void setUp() {};
	void render(void* entity) { std::cout << "hello"; };
	void tearDown() {};
	void destroy() {};
};

class Renderer {
public:

	static void resize(const Size width, const Size height);

	static void init();

	static void setUp();

	template<typename T>
	static void queue(T* e) {
#ifdef MACE_ERROR_CHECK
		if (e == nullptr || e == NULL)throw NullPointer("Input pointer to an entity must not be null in queue()");
#endif
		if (RenderProtocol<T>::index == -1)registerProtocol<T>();
		renderQueue.push(std::pair<Index, Entity*>(RenderProtocol<T>::index, e));
	};

	static void tearDown();

	static void renderFrame();

	static void destroy();

	static void setRefreshColor(const float r, const float g, const float b, const float a=1.0f);
	static void setRefreshColor(const Color& c);

private:

	static std::queue<std::pair<Index,Entity*>> renderQueue;
	static std::vector<RenderImpl*> protocols;

	template<typename T>
	static void registerProtocol() {
		//in destroy(), this memory is deleted
		RenderImpl* protocol = new RenderProtocol<T>();
		protocol->init();
		protocols.push_back(protocol);
		RenderProtocol<T>::index = protocols.size() - 1;
	}
};//Renderer

}//gfx
}//mc