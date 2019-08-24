/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_UI_H
#define MACE__GRAPHICS_UI_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Entity2D.h>

namespace mc {
	namespace gfx {
		class UIButton: public Selectable, public GraphicsEntity {
		private:
			NineSliceComponent nineSlice;

			void onInit() override final;
			void onRender(Painter& p) override final;
		};
	}//gfx
}//mc

#endif //MACE__GRAPHICS_UI_H
