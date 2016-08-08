#include <MC-Graphics/Entity2D.h>
#include <MC-Graphics/GLUtil.h>

namespace mc {
namespace gfx {
	const char* Renderer2D::vertexShader2D = R"(
			#version 330 core

			precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

			layout(location = 0) in vec3 vertexPosition;
			layout(location = 1) in vec2 texCoord;

			uniform mat4 transform;
			uniform float width;
			uniform float height;

			out lowp vec2 textureCoord;


			void main(){
				gl_Position.xyz = vertexPosition;
				gl_Position.xy*=vec2(width,height);
				gl_Position*=transform;

															
				textureCoord=texCoord;
			}
		)";

	const char* Renderer2D::fragmentShader2D = R"(
			#version 330 core

			precision highp float; // Defines precision for float and float-derived (vector/matrix) types.

			in lowp vec2 textureCoord;

			out lowp vec4 color;

			uniform lowp vec4 paint;
			uniform lowp float paintStrength;

			uniform lowp sampler2D tex;

			void main (void)  
			{     
			   color= mix(paint,texture(tex,textureCoord),paintStrength);
			}       
		)";

	const GLfloat Renderer2D::squareVertices[] = {
		-1.0f,-1.0f,0.0f,
		-1.0f,1.0f,0.0f,
		1.0f,1.0f,0.0f,
		1.0f,-1.0f,0.0f
	};

	const GLfloat Renderer2D::squareTextureCoordinates[] = {
		0.0f,1.0f,
		0.0f,0.0f,
		1.0f,0.0f,
		1.0f,1.0f,
	};

	const GLuint Renderer2D::squareIndices[] = {
		0,1,3,
		1,2,3
	};


	bool Renderer<Entity2D>::isInit = false;

	Renderer2D Renderer<Entity2D>::instance = Renderer2D();

	Renderer2D::Renderer2D()
	{
	}
	void Renderer2D::init()
	{
		square.loadVertices(4, squareVertices);
		square.loadTextureCoordinates(4, squareTextureCoordinates);
		square.loadIndices(6, squareIndices);


		shaders2D.createVertex(vertexShader2D);
		shaders2D.createFragment(fragmentShader2D);

		shaders2D.init();

		shaders2D.createUniform("paint");
		shaders2D.createUniform("paintStrength");
		shaders2D.createUniform("transform");
		shaders2D.createUniform("width");
		shaders2D.createUniform("height");

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);



		checkGLError();
	}
	void Renderer2D::draw(Entity2D * e)
	{
		const Texture& tex = e->getTexture();
		if (boundTexture != tex.getID()) {
			boundTexture = tex.getID();
			tex.bind();
		}
		if (boundModel != square.vaoID) {
			boundModel = square.vaoID;
			square.bind();
		}
		if (boundProgram != shaders2D.getProgramID()) {
			boundProgram = shaders2D.getProgramID();
			shaders2D.bind();
		}

		//setting uniform costs quite a bit of performance when done constantly. We cache the current setting and only change it if its different
		const float& paintStrength = tex.getPaintStrength(), width = e->getWidth(), height = e->getHeight();
		const TransformMatrix& transform = e->getBaseTransformation();
		const Color& paint = tex.getPaint();
		if (paint != boundPaint) { 
			shaders2D.setUniform("paint", paint); 
			boundPaint = paint;
		}
		if(paintStrength!=boundPaintStrength){
			shaders2D.setUniform("paintStrength", paintStrength);
			boundPaintStrength = paintStrength;
		}
		if(transform!=boundTransformation){
			shaders2D.setUniform("transform", transform);
			boundTransformation = transform;
		}
		if (width != boundWidth) {
			shaders2D.setUniform("width", width);
			boundWidth = width;
		}
		if (height!=boundHeight) {
			shaders2D.setUniform("height", height);
			boundHeight = height;
		}
		square.draw();

		checkGLError();
	}
	void Renderer2D::destroy()
	{
		square.destroy();
		shaders2D.destroy();
	}
	Entity2D::Entity2D() : GraphicsEntity()
	{
	}
	Entity2D::Entity2D(const float & width, const float & height)
	{
		this->width = width;
		this->height = height;
	}
	float & Entity2D::getWidth()
	{
		return width;
	}
	const float & Entity2D::getWidth() const
	{
		return width;
	}
	void Entity2D::setWidth(const float & s)
	{
		width = s;
	}
	float & Entity2D::getHeight()
	{
		return height;
	}
	const float & Entity2D::getHeight() const
	{
		return height;
	}
	void Entity2D::setHeight(const float & s)
	{
		height = s;
	}
}
}