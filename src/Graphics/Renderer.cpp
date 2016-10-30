/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#define MACE_ENTITY2D_EXPOSE_X_MACRO
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/Buffer.h>
#include <MACE/Graphics/Model.h>
#include <MACE/System/Utility/Preprocessor.h>
namespace mc {
namespace gfx {

int RenderImpl::index = -1;

RenderQueue Renderer::renderQueue = RenderQueue();
std::vector<RenderImpl*> Renderer::protocols = std::vector<RenderImpl*>();

Size Renderer::originalWidth = 0;
Size Renderer::originalHeight = 0;

//this variable is used for both ssl and Renderer. Each iteration through the queue, this is incremented. It is then passed to the shader, and the shader returns which entity was hovered over
Index entityIndex = 0;

void Renderer::init(const Size width, const Size height)
{
	Renderer::originalWidth = width;
	Renderer::originalHeight = height;

	ssl::init(width, height);
}//init
void Renderer::setUp(win::Window* win)
{
	ssl::setUp(win);

	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->setUp(win,&renderQueue);
	}
}//setUp
void Renderer::resize(const Size width, const Size height)
{
	glViewport(0,0,width,height);

	ssl::resize(width, height);

	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->resize(width,height);
	}
}//tearDown
void Renderer::tearDown(win::Window* win)
{
	for (Index i = 0; i < protocols.size(); ++i) {
		protocols[i]->tearDown(win, &renderQueue);
	}

	ssl::tearDown(win);
}//tearDown
//resize
void Renderer::renderFrame(win::Window* win)
{
	setUp(win);
	for (RenderQueue::iterator pair = renderQueue.begin(); pair != renderQueue.end();++pair) {
		entityIndex = pair - renderQueue.begin();
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

	ssl::destroy();
}//destroy()
void Renderer::setRefreshColor(const float r, const float g, const float b, const float a)
{
	glClearColor(r,g,b,a);
}//setRefreshColor(r,g,b,a)
void Renderer::setRefreshColor(const Color & c)
{
	setRefreshColor(c.r,c.g,c.b,c.a);
}//setRefreshColor(Color)


RenderImpl::RenderImpl()
{
}//RenderImpl()

//ssl
namespace ssl {
//constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

//how many floats in the windowData uniform buffer
#define MACE_WINDOW_DATA_BUFFER_SIZE 6
//which binding location the windowData uniform buffer should be bound to
#define MACE_WINDOW_DATA_LOCATION 0

//how many floats in the paintData uniform buffer
#define MACE_PAINT_DATA_BUFFER_SIZE 6
//which binding location the paintdata uniform buffer should be bound to
#define MACE_PAINT_DATA_LOCATION 1

//how many floats in the entityData uniform buffer. Needs to be multiplied by sizeof(GLfloat)
#define MACE_ENTITY_DATA_BUFFER_SIZE 52
//which binding location the paintdata uniform buffer should be bound to
#define MACE_ENTITY_DATA_LOCATION 2

//ssl resources
//preprocessor
Preprocessor sslPreprocessor = Preprocessor("");

mc::IncludeString vertexLibrary = mc::IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_vertex.glsl>
},"ssl_vertex");

mc::IncludeString fragmentLibrary = mc::IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_frag.glsl>
}, "ssl_frag");
mc::IncludeString colorLibrary = mc::IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_color.glsl>
}, "ssl_color");
mc::IncludeString positionLibrary = mc::IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_position.glsl>
}, "ssl_position");
mc::IncludeString mouseLibrary = mc::IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl_window.glsl>
}, "ssl_window");
mc::IncludeString sslLibrary = mc::IncludeString({
#	include <MACE/Graphics/Shaders/include/ssl.glsl>
}, "ssl");
//ssl buffer objects
UBO windowData = UBO();
UBO paintData = UBO();
UBO entityData = UBO();

//fbo resources
FBO frameBuffer = FBO();
RBO depthBuffer = RBO();

Texture sceneTexture;

//this is for rendering the fbo
VAO renderBuffer = VAO();

//this is from rendering the fbo as well
ShaderProgram renderProgram = ShaderProgram();

const GLfloat squareVertices[12] = {
	-0.5f,-0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	0.5f,0.5f,0.0f,
	0.5f,-0.5f,0.0f
};

const GLfloat squareTextureCoordinates[8] = {
	0.0f,1.0f,
	0.0f,0.0f,
	1.0f,0.0f,
	1.0f,1.0f,
};

const GLuint squareIndices[6] = {
	0,1,3,
	1,2,3
};

void init(const Size & originalWidth, const Size & originalHeight)
{
	paintData.init();
	entityData.init();
	windowData.init();

	windowData.setLocation(MACE_WINDOW_DATA_LOCATION);
	windowData.bind();
	float defaultWindowData[MACE_WINDOW_DATA_BUFFER_SIZE] = { static_cast<float>(originalWidth), static_cast<float>(originalHeight),static_cast<float>(originalWidth),static_cast<float>(originalHeight),0,0 };
	windowData.setData(sizeof(GLfloat)*MACE_WINDOW_DATA_BUFFER_SIZE, defaultWindowData);
	windowData.unbind();

	paintData.setLocation(MACE_PAINT_DATA_LOCATION);
	paintData.bind();
	//we set it to null, because during the actual rendering we set the data
	paintData.setData(sizeof(GLfloat)*MACE_PAINT_DATA_BUFFER_SIZE, nullptr, GL_STREAM_DRAW);
	paintData.unbind();

	entityData.setLocation(MACE_ENTITY_DATA_LOCATION);
	entityData.bind();
	//we set it to null, because during the actual rendering we set the data
	entityData.setData(sizeof(GLfloat)*MACE_ENTITY_DATA_BUFFER_SIZE, nullptr);
	entityData.unbind();

	renderProgram.createVertex({
#		include <MACE/Graphics/Shaders/scene.v.glsl>
	});
	renderProgram.createFragment({
#		include <MACE/Graphics/Shaders/scene.f.glsl>
	});
	renderProgram.init();

	sceneTexture = Texture(0,originalWidth,originalHeight,GL_UNSIGNED_BYTE);
	sceneTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	sceneTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	depthBuffer.init();
	depthBuffer.bind();
	depthBuffer.setStorage(GL_DEPTH_COMPONENT, originalWidth, originalHeight);

	//for our custom FBO
	frameBuffer.init();
	frameBuffer.attachTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, sceneTexture.getID(), 0);
	frameBuffer.attachRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer);

	GLenum buffers[1] = { GL_COLOR_ATTACHMENT0 };
	frameBuffer.setDrawBuffers(1, buffers);

	const GLfloat squareVertices[12] = {
		-1.0f,-0.5f,0.0f,
		-1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,-1.0f,0.0f
	};

	const GLfloat squareTextureCoordinates[8] = {
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
	};

	const GLuint squareIndices[6] = {
		0,1,3,
		1,2,3
	};

	renderBuffer.loadVertices(4, squareVertices);
	renderBuffer.storeDataInAttributeList(4, squareTextureCoordinates, 1, 2);
	renderBuffer.loadIndices(6, squareIndices);

	//gl states
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	checkGLError();
}

void setUp(win::Window * win)
{
	GLfloat mouseX, mouseY;

	{
		//we want mouseX and mouseY as floats, so we temporarly create some doubles (hence the {}) and then we cast it float.
		double tempMouseX, tempMouseY;

		glfwGetCursorPos(win->getGLFWWindow(), &tempMouseX, &tempMouseY);

		mouseX = static_cast<GLfloat>(mc::math::floor(tempMouseX));
		mouseY = static_cast<GLfloat>(mc::math::floor(tempMouseY));
	}

	windowData.bind();
	GLfloat* mappedWindowData = static_cast<GLfloat*>(windowData.mapRange(4 * sizeof(GLfloat), 2, GL_MAP_WRITE_BIT));//we need to cast it to a Byte* so we can do pointer arithmetic on it
	std::memcpy(mappedWindowData, &mouseX, sizeof(mouseY));
	std::memcpy(mappedWindowData + 1, &mouseY, sizeof(mouseY));
	windowData.unmap();

	frameBuffer.bind();
}

void bindEntity(const GraphicsEntity * entity)
{
	const Texture& tex = entity->getTexture();


	paintData.bind();
	const Color& paint = tex.getPaint();
	const float data[6] = { paint.r,paint.g,paint.b,paint.a,tex.getOpacity(), static_cast<float>(entityIndex) };
	paintData.setData(sizeof(data), data, GL_STATIC_DRAW);
	paintData.bindForRender();



	//setting all of these values is quite slow, need to change it
	const TransformMatrix& transform = entity->getBaseTransformation();

	//setting uniform costs quite a bit of performance when done constantly. We cache the current setting and only change it if its different

	Vector3f inheritedTranslation = { 0,0,0 };
	Vector3f inheritedScale = { 1,1,1 };
	Matrix4f inheritedRotation = math::identity<float, 4>();

	if (entity->hasParent()) {
		const Entity* const parent = entity->getParent();
		const TransformMatrix& parentTransform = parent->getBaseTransformation();

		inheritedTranslation = parentTransform.translation;
		inheritedScale = parentTransform.scaler;
		inheritedRotation = math::rotate(parentTransform.rotation);
	}

	const float stretch_x = entity->getProperty(ENTITY_STRETCH_X) ? 1.0f : 0.0f;
	const float stretch_y = entity->getProperty(ENTITY_STRETCH_Y) ? 1.0f : 0.0f;

	//the following are containers for the flatten() call
	float flattenedData[16];

	//now we set the uniform buffer defining the transformations of the entity
	entityData.bind();
	//holy crap thats a lot of flags. this is the fastest way to map the buffer. the difference is MASSIVE. try it.
	float* mappedEntityData = static_cast<float*>(entityData.mapRange(0, sizeof(GLfloat)*MACE_ENTITY_DATA_BUFFER_SIZE, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));//we need to cast it to a Byte* so we can do pointer arithmetic on it	
	std::memcpy((mappedEntityData), transform.translation.flatten(flattenedData), sizeof(Vector3f));
	mappedEntityData += 3;
	std::memcpy(mappedEntityData, &stretch_x, sizeof(stretch_x));
	++mappedEntityData;
	std::memcpy(mappedEntityData, transform.scaler.flatten(flattenedData), sizeof(Vector3f));
	mappedEntityData += 3;
	std::memcpy(mappedEntityData, &stretch_y, sizeof(stretch_y));
	++mappedEntityData;
	std::memcpy(mappedEntityData, inheritedTranslation.flatten(flattenedData), sizeof(Vector3f));
	mappedEntityData += 4;
	std::memcpy(mappedEntityData, inheritedScale.flatten(flattenedData), sizeof(Vector3f));
	mappedEntityData += 4;
	std::memcpy(mappedEntityData, (math::rotate(transform.rotation)).flatten(flattenedData), sizeof(Matrix4f));
	mappedEntityData += 16;
	std::memcpy(mappedEntityData, (inheritedRotation).flatten(flattenedData), sizeof(Matrix4f));

	entityData.unmap();
	entityData.bindForRender();

	windowData.bindForRender();

	tex.bind();
}

void bindShaderProgram(const ShaderProgram & prog)
{
	windowData.bindToUniformBlock(prog.getProgramID(), "ssl_WindowData");
	entityData.bindToUniformBlock(prog.getProgramID(), "ssl_EntityData");
	paintData.bindToUniformBlock(prog.getProgramID(), "ssl_PaintData");
}

void resize(const Size & width, const Size & height)
{
	windowData.bind();
	float newSize[2] = { static_cast<float>(width),static_cast<float>(height) };
	windowData.setDataRange(sizeof(float) * 2, sizeof(float) * 2, newSize);
	windowData.unbind();
}

std::string ssl::processShader(const std::string & shader, const GLenum& type)
{
	Preprocessor shaderPreprocessor = Preprocessor(shader, getSSLPreprocessor());
	shaderPreprocessor.defineMacro(mc::Macro("__SHADER_TYPE__", std::to_string(type)));

	if (type == GL_VERTEX_SHADER) {
		shaderPreprocessor.defineMacro(mc::Macro("__VERTEX_SHADER__", "1"));
	}
	else if (type == GL_FRAGMENT_SHADER) {
		shaderPreprocessor.defineMacro(mc::Macro("__FRAGMENT_SHADER__", "1"));
	}
	else if (type == GL_GEOMETRY_SHADER) {
		shaderPreprocessor.defineMacro(mc::Macro("__GEOMETRY_SHADER__", "1"));
	}
	else if (type == GL_TESS_CONTROL_SHADER) {
		shaderPreprocessor.defineMacro(mc::Macro("__TESS_CONTROL_SHADER__", "1"));
	}
	else if (type == GL_TESS_EVALUATION_SHADER) {
		shaderPreprocessor.defineMacro(mc::Macro("__TESS_EVALUATION_SHADER__", "1"));
	}


	const std::string processedShader = shaderPreprocessor.preprocess();

	return processedShader;
}

const mc::Preprocessor& ssl::getSSLPreprocessor()
{
	if (sslPreprocessor.macroNumber() == 0) {
		sslPreprocessor.defineOSMacros();
		sslPreprocessor.defineStandardMacros();

		sslPreprocessor.defineMacro(mc::Macro("__SSL__", "1"));

		sslPreprocessor.addInclude(vertexLibrary);
		sslPreprocessor.addInclude(fragmentLibrary);
		sslPreprocessor.addInclude(colorLibrary);
		sslPreprocessor.addInclude(positionLibrary);
		sslPreprocessor.addInclude(mouseLibrary);
		sslPreprocessor.addInclude(sslLibrary);
	}
	return sslPreprocessor;
}

void tearDown(win::Window * win)
{
	frameBuffer.unbind();

	sceneTexture.bind();
	renderProgram.bind();
	renderBuffer.bind();
	
	renderBuffer.draw();
}

void destroy()
{
	windowData.destroy();
	entityData.destroy();
	paintData.destroy();
}

#undef MACE_WINDOW_DATA_BUFFER_SIZE
#undef MACE_WINDOW_DATA_LOCATION
#undef MACE_PAINT_DATA_BUFFER_SIZE
#undef MACE_PAINT_DATA_LOCATION
#undef MACE_ENTITY_DATA_BUFFER_SIZE
#undef MACE_ENTITY_DATA_LOCATION

}//ssl
}//gfx
}//mc

