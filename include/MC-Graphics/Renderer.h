#pragma once

#include <MC-Graphics/GraphicsEntity.h>
#include <MC-Graphics/Shaders.h>
#include <queue>

#define MACE_ENTITY2D_UNIFORM_VALUES \
	MACE_ENTITY2D_UNIFORM_ENTRY(opacity,float) \
	MACE_ENTITY2D_UNIFORM_ENTRY(paint,Color) \
	MACE_ENTITY2D_UNIFORM_ENTRY(scale,Vector3f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(rotation, Matrix4f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(translation, Vector3f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(inheritedScale,Vector3f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(inheritedRotation,Matrix4f) \
	MACE_ENTITY2D_UNIFORM_ENTRY(inheritedTranslation,Vector3f) \


namespace mc {
namespace gfx {


class RenderImpl {
public:
	static int index;

	RenderImpl();

	virtual void init() = 0;
	virtual void render(void* entity)=0;
	virtual void destroy() = 0;
};

template<typename T>
class RenderProtocol : public RenderImpl{
public:
	using RenderImpl::RenderImpl;

	void init() { }
	void render(T* entity) { std::cout << "hello"; }
	void destroy() {}
};

template<>
class RenderProtocol<Entity> : public RenderImpl {
public:
	using RenderImpl::RenderImpl;

	void init() {};
	void render(Entity* entity) { std::cout << "aw"; };
	void destroy() {};
};

template<>
class RenderProtocol<Entity2D> : public RenderImpl {
public:

	void init() { std::cout << "init"; };
	void render(void* entity) { std::cout << "yay!"; };
	void destroy() {};
};

class Renderer {
public:

	static void init();

	static void prepare();

	static void resize(const Size width, const Size height);

	template<typename T>
	static void queue(T* e) {
		if (RenderProtocol<T>::index == -1)registerProtocol<T>();
		renderQueue.push(std::pair<Index, Entity*>(RenderProtocol<T>::index, e));
	};

	static void renderFrame();

	static void destroy();

	static void setRefreshColor(const float r, const float g, const float b, const float a=1.0f);
	static void setRefreshColor(const Color& c);

private:

	const static char* vertexShader2D;
	const static char* fragmentShader2D;

	const static GLfloat squareVertices[12];

	const static GLfloat squareTextureCoordinates[8];

	const static GLuint squareIndices[6];

#define MACE_ENTITY2D_UNIFORM_ENTRY(a,type) \
	static type a##CurrentlyBound;

	MACE_ENTITY2D_UNIFORM_VALUES
#undef	MACE_ENTITY2D_UNIFORM_ENTRY

	static ShaderProgram shaders2D;
	static VAO square;

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

#ifndef MACE_ENTITY2D_EXPOSE_X_MACRO
#undef MACE_ENTITY2D_UNIFORM_VALUES
#endif