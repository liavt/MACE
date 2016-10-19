#include <MACE/Graphics/Entity2D.h>
#include <MACE/Graphics/GLUtil.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/System/Utility/Matrix.h>
#include <MACE/System/Utility/Preprocessor.h>
#include <cstring>

//constants will be defined up here, and undefined at the bottom. the only reason why they are defined by the preproccessor is so other coders can quickly change values.

//how many floats in the windowData uniform buffer
#define MACE_WINDOW_DATA_BUFFER_SIZE 8
//which binding location the windowData uniform buffer should be bound to
#define MACE_WINDOW_DATA_LOCATION 0

//how many floats in the paintData uniform buffer
#define MACE_PAINT_DATA_BUFFER_SIZE 8
//which binding location the paintdata uniform buffer should be bound to
#define MACE_PAINT_DATA_LOCATION 1

//how many floats in the entityData uniform buffer. Needs to be multiplied by sizeof(GLfloat)
#define MACE_ENTITY_DATA_BUFFER_SIZE 52
//which binding location the paintdata uniform buffer should be bound to
#define MACE_ENTITY_DATA_LOCATION 2

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

void RenderProtocol<Entity2D>::resize(const Size width, const Size height) {
	windowData.bind();
	float newSize[2] = { static_cast<float>(width),static_cast<float>(height)};
	windowData.setDataRange(sizeof(float) * 2, sizeof(float) * 2, newSize);
	windowData.unbind();
}

void RenderProtocol<Entity2D>::init(const Size originalWidth, const Size originalHeight) {
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
	windowData.init();

	windowData.setLocation(MACE_WINDOW_DATA_LOCATION);
	windowData.bind();
	windowData.bindToUniformBlock(shaders2D.getProgramID(),"window_data");
	float defaultWindowData[MACE_WINDOW_DATA_BUFFER_SIZE] = {static_cast<float>(originalWidth), static_cast<float>(originalHeight),static_cast<float>(originalWidth),static_cast<float>(originalHeight),0,0};
	windowData.setData(sizeof(GLfloat)*MACE_WINDOW_DATA_BUFFER_SIZE,defaultWindowData);
	windowData.unbind();

	paintData.setLocation(MACE_PAINT_DATA_LOCATION);
	paintData.bind();
	paintData.bindToUniformBlock(shaders2D.getProgramID(), "paint_data");
	//we set it to null, because during the actual rendering we set the data
	paintData.setData(sizeof(GLfloat)*MACE_PAINT_DATA_BUFFER_SIZE, nullptr,GL_STREAM_DRAW);
	paintData.unbind();

	entityData.setLocation(MACE_ENTITY_DATA_LOCATION);
	entityData.bind();
	entityData.bindToUniformBlock(shaders2D.getProgramID(), "entity_data");
	//we set it to null, because during the actual rendering we set the data
	entityData.setData(sizeof(GLfloat)*MACE_ENTITY_DATA_BUFFER_SIZE,nullptr);
	entityData.unbind();

	//gl states
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	checkGLError();
}

void RenderProtocol<Entity2D>::setUp(win::Window* win, RenderQueue* queue) {};

void RenderProtocol<Entity2D>::render(win::Window* win,void* e) {
	Entity2D * entity = static_cast<Entity2D*>(e);
	const Texture& tex = entity->getTexture();

	
	paintData.bind();
	const Color& paint = tex.getPaint();
	const float data[5] = { paint.r,paint.g,paint.b,paint.a,tex.getOpacity() };
	paintData.setData(sizeof(data), data,GL_STATIC_DRAW);
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
	Index offset = 0;
	
	std::memcpy((mappedEntityData)+offset, transform.translation.flatten(flattenedData), sizeof(Vector3f));
	offset +=  3;
	std::memcpy(mappedEntityData+offset,&stretch_x,sizeof(stretch_x));
	++offset;
	std::memcpy(mappedEntityData + offset, transform.scaler.flatten(flattenedData), sizeof(Vector3f));
	offset +=  3;
	std::memcpy(mappedEntityData + offset, &stretch_y, sizeof(stretch_y));
	++offset;
	std::memcpy(mappedEntityData + offset, inheritedTranslation.flatten(flattenedData), sizeof(Vector3f));
	offset += 4;
	std::memcpy(mappedEntityData + offset, inheritedScale.flatten(flattenedData), sizeof(Vector3f));
	offset += 4;
	std::memcpy(mappedEntityData + offset, (math::rotate(transform.rotation)).flatten(flattenedData), sizeof(Matrix4f));
	offset += 16;
	std::memcpy(mappedEntityData + offset, (inheritedRotation).flatten(flattenedData), sizeof(Matrix4f));

	entityData.unmap();
	entityData.bindForRender();

	windowData.bindForRender();
	
	tex.bind();
	square.bind();
	shaders2D.bind();

	square.draw();

	checkGLError();
}

void RenderProtocol<Entity2D>::tearDown(win::Window* win, RenderQueue* queue) {}

void RenderProtocol<Entity2D>::destroy(win::Window* win) {
	shaders2D.destroy();
}

}//gfx
}//mc

#undef MACE_WINDOW_DATA_BUFFER_SIZE
#undef MACE_WINDOW_DATA_LOCATION
#undef MACE_PAINT_DATA_BUFFER_SIZE
#undef MACE_PAINT_DATA_LOCATION
#undef MACE_ENTITY_DATA_BUFFER_SIZE
#undef MACE_ENTITY_DATA_LOCATION
