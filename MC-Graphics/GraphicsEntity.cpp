#define MACE_ENTITY2D_EXPOSE_X_MACRO
#include <MC-Graphics/GraphicsEntity.h>
#include <MC-Graphics/GLUtil.h>
#include <MC-Graphics/Renderer.h>

namespace mc {
namespace gfx {
GraphicsEntity::GraphicsEntity() : Entity()
{
}

GraphicsEntity::GraphicsEntity(Texture & t) : Entity()
{
	texture = t;
}

GraphicsEntity::~GraphicsEntity()
{
}

void GraphicsEntity::setTexture(Texture & tex)
{
	texture = tex;
}

Texture & GraphicsEntity::getTexture()
{
	return texture;
}

const mc::gfx::Texture & GraphicsEntity::getTexture() const
{
	return texture;
}

Entity2D::Entity2D() : GraphicsEntity()
{
}

void Image::customInit()
{
}

void Image::customUpdate()
{
}

void Image::customRender()
{
	Renderer::queue<Entity2D>(this);
}

void Image::customDestroy()
{
}

void RenderProtocol<Entity2D>::resize(const Size width, const Size height) {}

void RenderProtocol<Entity2D>::init() {
	//vao loading
	square.loadVertices(4, squareVertices);
	square.loadTextureCoordinates(4, squareTextureCoordinates);
	square.loadIndices(6, squareIndices);

	//shader stuff
	shaders2D.createVertex(vertexShader2D);
	shaders2D.createFragment(fragmentShader2D);

	shaders2D.init();

	paintData.init(1);
	entityData.init(2);

	paintData.bind();
	paintData.bindToUniformBlock(shaders2D.getProgramID(), "paint_data");
	const float defaultTextureData[5] = { 1.0f,1.0f,1.0f,0.0f,1.0f };//set it to something, we dont want to access uncreated memeory
	paintData.setData(sizeof(defaultTextureData), defaultTextureData);
	paintData.unbind();


	entityData.bind();
	entityData.bindToUniformBlock(shaders2D.getProgramID(), "entity_data");
	const float defaultEntityData[48] = { 0.0f,0.0f,0.0f,0,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0.0f,0.0f,0.0f,0,1,1,1,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	entityData.setData(sizeof(defaultEntityData),defaultEntityData);
	entityData.unbind();

#define MACE_ENTITY2D_UNIFORM_ENTRY(a, type)	\
			shaders2D.createUniform(#a);				

	MACE_ENTITY2D_UNIFORM_VALUES
#undef	MACE_ENTITY2D_UNIFORM_ENTRY

	//gl states
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	checkGLError();
}

void RenderProtocol<Entity2D>::setUp() {};

void RenderProtocol<Entity2D>::render(void* e) {
	Entity2D * entity = static_cast<Entity2D*>(e);
	const Texture& tex = entity->getTexture();

	paintData.bind();
	const Color& paint = tex.getPaint();
	const float data[5] = { paint.r,paint.g,paint.b,paint.a,tex.getOpacity() };
	paintData.setData(sizeof(data), data);
	paintData.bindForRender();

	const TransformMatrix& transform = entity->getBaseTransformation();

	//setting uniform costs quite a bit of performance when done constantly. We cache the current setting and only change it if its different
	const Vector3f& translation = transform.translation, scale = transform.scaler;
	const Matrix4f& rotation = (math::rotate(transform.rotation));

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

	entityData.bind();
	Index offset = 0;
	entityData.setDataRange(offset, sizeof(float) * 3, translation.getContents().data());
	offset += sizeof(float) * 3;
	entityData.setDataRange(offset, sizeof(float) * 3, &scale);
	offset += sizeof(float) * 5;
	entityData.setDataRange(offset, (sizeof(float)*rotation.size()), math::transpose(rotation).toArray().data());
	offset += sizeof(float)*16;
	entityData.setDataRange(offset, sizeof(float) * 3, inheritedTranslation.getContents().data());
	offset += sizeof(float) * 3;
	entityData.setDataRange(offset, sizeof(float) * 3, &inheritedScale);
	offset += sizeof(float) * 5;
	entityData.setDataRange(offset, (sizeof(float)*inheritedRotation.size()), math::transpose(inheritedRotation).toArray().data());
	entityData.bindForRender();


	tex.bind();
	square.bind();
	shaders2D.bind();

#define MACE_ENTITY2D_UNIFORM_ENTRY(a,type) \
			if(a != a##CurrentlyBound){ \
				shaders2D.setUniform(#a,a); \
				a##CurrentlyBound = a; \
			}

	MACE_ENTITY2D_UNIFORM_VALUES
#undef	MACE_ENTITY2D_UNIFORM_ENTRY



	square.draw();

	checkGLError();
}

void RenderProtocol<Entity2D>::tearDown() {}

void RenderProtocol<Entity2D>::destroy() {
	shaders2D.destroy();
}

}//gfx
}//mc