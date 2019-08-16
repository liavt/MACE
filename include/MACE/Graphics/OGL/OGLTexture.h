/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_OGL_OGLTEXTURE_H
#define MACE__GRAPHICS_OGL_OGLTEXTURE_H

//The api docs shouldn't include a bunch of internal classes, since any end user wouldn't care about them
#ifndef MACE__DOXYGEN_PASS

#include <MACE/Graphics/Texture.h>
#include <MACE/Graphics/OGL/OGL.h>
#include <MACE/Graphics/OGL/Dispatchable.h>

namespace mc {
	namespace internal {
		namespace ogl {
			class Texture: public gfx::TextureImpl, private ogl::Texture2D, private Dispatchable {
			public:
				Texture(std::shared_ptr<ogl::Context> context, const gfx::TextureDesc& desc);
				~Texture() override;

				void setData(const void* data, const int x, const int y, const Pixels w, const Pixels h, const int mipmap, const gfx::PixelStorageHints hints) override;

				void readPixels(void* data, const gfx::PixelStorageHints hints) const override;

				void bindTextureSlot(const gfx::TextureSlot slot) const override;
			};
		}//ogl
	}//internal
}//mc

#endif//MACE__DOXYGEN_PASS

#endif//MACE__GRAPHICS_OGL_OGLTEXTURE_H