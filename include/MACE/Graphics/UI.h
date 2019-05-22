/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_UI_H
#define MACE__GRAPHICS_UI_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Context.h>

namespace mc {
	namespace gfx {
		class GraphicsEntity;

		struct NineSliceDesc {
			Texture topLeft, top, topRight, right, bottomRight, bottom, bottomLeft, left, center;

			bool operator==(const NineSliceDesc& other) const;
			bool operator!=(const NineSliceDesc& other) const;
		};

		class NineSliceComponent: public Component {
		public:
			NineSliceComponent(const NineSliceDesc& desc);

			/**
			@dirty
			*/
			NineSliceDesc& getDesc();
			const NineSliceDesc& getDesc() const;
			/**
			@desc
			*/
			void setDesc(const NineSliceDesc& desc);
		private:
			NineSliceDesc desc;
			GraphicsEntity* en;

			void init() override;

			void render() override;

			void clean(Metrics& metrics) override;
		};
	}//gfx
}//mc

#endif //MACE__GRAPHICS_UI_H
