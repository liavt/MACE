#include <MC-Graphics/GraphicsEntity.h>
#include <MC-Graphics/GLUtil.h>

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

}//gfx
}//mc