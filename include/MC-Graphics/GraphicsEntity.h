#pragma once

#include <MC-Graphics/Model.h>
#include <MC-Graphics/Entity.h>
#include <MC-Graphics/GLUtil.h>

namespace mc {
namespace gfx {
	class GraphicsEntity : public Entity {
	public:

		GraphicsEntity();

		GraphicsEntity(Texture& t);
		virtual ~GraphicsEntity();

		void setTexture(Texture& tex);
		Texture& getTexture();
		const Texture& getTexture() const;
	protected:
		Texture texture = Texture();
	};//GraphicsEntity

	class Entity2D : public GraphicsEntity {
	public:
		Entity2D();
	protected:
	};//Entity2D

	class Image : public Entity2D {
	protected:
		void customInit();
		void customUpdate();
		void customRender();
		void customDestroy();

	};
}//gfx
}//mc