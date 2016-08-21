#include <MC-Graphics/Entity2D.h>
#include <MC-Graphics/GLUtil.h>
#include <MC-Graphics/Renderer.h>

//constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

//how many bytes in the paintData uniform buffer
#define MACE_PAINT_DATA_BUFFER_SIZE 8
//which binding location the paintdata uniform buffer should be bound to
#define MACE_PAINT_DATA_LOCATION 1

//how many bytes in the entityData uniform buffer
#define MACE_ENTITY_DATA_BUFFER_SIZE 48
//which binding location the paintdata uniform buffer should be bound to
#define MACE_ENTITY_DATA_LOCATION 2

//how many values a rotation matrix contains
#define MACE_ROTATION_MATRIX_SIZE 16

namespace mc {
namespace gfx {
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

	paintData.init();
	entityData.init();

	paintData.setLocation(MACE_PAINT_DATA_LOCATION);
	paintData.bind();
	paintData.bindToUniformBlock(shaders2D.getProgramID(), "paint_data");
	//we set it to null, because during the actual rendering we set the data
	paintData.setData(sizeof(float)*MACE_PAINT_DATA_BUFFER_SIZE, nullptr);
	paintData.unbind();

	entityData.setLocation(MACE_ENTITY_DATA_LOCATION);
	entityData.bind();
	entityData.bindToUniformBlock(shaders2D.getProgramID(), "entity_data");
	//we set it to null, because during the actual rendering we set the data
	entityData.setData(sizeof(float)*MACE_ENTITY_DATA_BUFFER_SIZE,nullptr);
	entityData.unbind();

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
	paintData.setData(sizeof(data), data,GL_STATIC_DRAW);
	paintData.bindForRender();

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

	//now we set the uniform buffer defining the transformations of the entity
	entityData.bind();
	//holy crap thats a lot of flags. this is the fastest way to map the buffer. the difference is MASSIVE. try it.
	Byte* mappedEntityData = static_cast<Byte*>(entityData.mapRange(0,MACE_ENTITY_DATA_BUFFER_SIZE,GL_MAP_WRITE_BIT|GL_MAP_UNSYNCHRONIZED_BIT|GL_MAP_INVALIDATE_RANGE_BIT|GL_MAP_INVALIDATE_BUFFER_BIT));//we need to cast it to a Byte* so we can do pointer arithmetic on it
	Index offset = 0;
	memcpy((mappedEntityData), transform.translation.getContents().data(), sizeof(float) * 3);
	offset += sizeof(float) * 3;
	memcpy(mappedEntityData+offset,&transform.scaler,sizeof(float)*3);
	offset += sizeof(float) * 5;
	memcpy(mappedEntityData+offset, math::transpose(math::rotate(transform.rotation)).toArray().data(), (sizeof(float)*MACE_ROTATION_MATRIX_SIZE));
	offset += sizeof(float)*MACE_ROTATION_MATRIX_SIZE;
	memcpy(mappedEntityData+offset, inheritedTranslation.getContents().data(),sizeof(float)*3);
	offset += sizeof(float) * 3;
	memcpy(mappedEntityData+offset,&inheritedScale,sizeof(float)*3);
	offset += sizeof(float) * 5;
	memcpy(mappedEntityData+offset, math::transpose(inheritedRotation).toArray().data(), (sizeof(float)*MACE_ROTATION_MATRIX_SIZE));
	entityData.unmap();
	entityData.bindForRender();


	tex.bind();
	square.bind();
	shaders2D.bind();

	square.draw();

	checkGLError();
}

void RenderProtocol<Entity2D>::tearDown() {}

void RenderProtocol<Entity2D>::destroy() {
	shaders2D.destroy();
}

}//gfx
}//mc

#undef MACE_PAINT_DATA_BUFFER_SIZE
#undef MACE_PAINT_DATA_LOCATION
#undef MACE_ENTITY_DATA_BUFFER_SIZE
#undef MACE_ENTITY_DATA_LOCATION
#undef MACE_ROTATION_MATRIX_SIZE