#define MACE_ENTITY2D_EXPOSE_X_MACRO
#include <MC-Graphics/Renderer.h>

namespace mc {
namespace gfx {

int RenderImpl::index = -1;

//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
const char* Renderer::vertexShader2D ={
#include "shaders/entity2D.vert"
};

const char* Renderer::fragmentShader2D ={
#include "shaders/entity2D.frag"
};

const GLfloat Renderer::squareVertices[] = {
	-0.5f,-0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	0.5f,0.5f,0.0f,
	0.5f,-0.5f,0.0f
};

const GLfloat Renderer::squareTextureCoordinates[] = {
	0.0f,1.0f,
	0.0f,0.0f,
	1.0f,0.0f,
	1.0f,1.0f,
};

const GLuint Renderer::squareIndices[] = {
	0,1,3,
	1,2,3
};

VAO Renderer::square = VAO();

ShaderProgram Renderer::shaders2D = ShaderProgram();

#define MACE_ENTITY2D_UNIFORM_ENTRY(a,type) \
type Renderer::a##CurrentlyBound = type##();	

MACE_ENTITY2D_UNIFORM_VALUES
#undef MACE_ENTITY2D_UNIFORM_ENTRY

std::queue<std::pair<Index,Entity*>> Renderer::renderQueue = std::queue<std::pair<Index, Entity*>>();
std::vector<RenderImpl*> Renderer::protocols = std::vector<RenderImpl*>();

void Renderer::init()
{
	//vao loading
	square.loadVertices(4, squareVertices);
	square.loadTextureCoordinates(4, squareTextureCoordinates);
	square.loadIndices(6, squareIndices);

	//shader stuff
	shaders2D.createVertex(vertexShader2D);
	shaders2D.createFragment(fragmentShader2D);

	shaders2D.init();

#define MACE_ENTITY2D_UNIFORM_ENTRY(a, type)	\
	shaders2D.createUniform(#a);				

	MACE_ENTITY2D_UNIFORM_VALUES
#undef	MACE_ENTITY2D_UNIFORM_ENTRY

//	shaders2D.createUniform("ortho");

	shaders2D.bind();
	//these are uniforms that dont change
//	shaders2D.setUniform("ortho", math::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	shaders2D.unbind();

	//gl states
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	checkGLError();
}//init
void Renderer::prepare()
{
}//prepare
void Renderer::resize(const Size width, const Size height)
{
	glViewport(0,0,width,height);
}
//resize
void Renderer::renderFrame()
{
	while (!renderQueue.empty()) {
		const std::pair<Index, Entity*> pair = renderQueue.front();
		protocols.at(pair.first)->render(pair.second);
		renderQueue.pop();
	}
}//renderFrame
/*
void Renderer::draw(Entity* e) {
	std::cout << "hello";
}
void Renderer::draw(Entity2D * e)
{
	const Texture& tex = e->getTexture();

	tex.bind();
	square.bind();
	shaders2D.bind();

	const TransformMatrix& transform = e->getBaseTransformation();

	//setting uniform costs quite a bit of performance when done constantly. We cache the current setting and only change it if its different
	const float& opacity = tex.getOpacity();
	const Vector3f& translation = transform.translation, scale = transform.scaler;
	const Matrix4f& rotation = (math::rotate(transform.rotation));
	const Color& paint = tex.getPaint();

	Vector3f inheritedTranslation = {0,0,0};
	Vector3f inheritedScale = { 1,1,1 };
	Matrix4f inheritedRotation = math::identity<float, 4>();
	if (e->hasParent()) {
		const Entity* const parent = e->getParent();
		const TransformMatrix& parentTransform = parent->getBaseTransformation();

		inheritedTranslation = parentTransform.translation;
		inheritedScale = parentTransform.scaler;
		inheritedRotation = math::rotate(parentTransform.rotation);
	}

#define MACE_ENTITY2D_UNIFORM_ENTRY(a,type) \
	if(a != a##CurrentlyBound){ \
		shaders2D.setUniform(#a,a); \
		a##CurrentlyBound = a; \
	}

	MACE_ENTITY2D_UNIFORM_VALUES
#undef	MACE_ENTITY2D_UNIFORM_ENTRY

	square.draw();

	checkGLError();
}//draw*/
void Renderer::destroy()
{
	shaders2D.destroy();

	while(!protocols.empty()){
		RenderImpl* protocol = protocols.back();
		protocol->destroy();
		delete protocol;
		protocols.pop_back();
	}
}
void Renderer::setRefreshColor(const float r, const float g, const float b, const float a)
{
	glClearColor(r,g,b,a);
}
void Renderer::setRefreshColor(const Color & c)
{
	setRefreshColor(c.r,c.g,c.b,c.a);
}
//destroy

RenderImpl::RenderImpl()
{
}

}//gfx
}//mc