/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/UI.h>
#include <MACE/Graphics/Renderer.h>

namespace mc {
	namespace gfx {
		bool NineSliceDesc::operator==(const NineSliceDesc& other) const {
			return topLeft == other.topLeft && top == other.top && topRight == other.topRight && right == other.right && bottomRight == other.bottomRight && bottom == other.bottom && bottomLeft == other.bottomLeft && left == other.left && center == other.center;
		}

		bool NineSliceDesc::operator!=(const NineSliceDesc& other) const {
			return !operator==(other);
		}

		NineSliceComponent::NineSliceComponent(const NineSliceDesc& desc) : desc(desc), en(nullptr) {}

		NineSliceDesc& NineSliceComponent::getDesc() {
			parent->makeDirty();

			return desc;
		}

		const NineSliceDesc& NineSliceComponent::getDesc() const {
			return desc;
		}

		void NineSliceComponent::setDesc(const NineSliceDesc& de) {
			if (this->desc != de) {
				parent->makeDirty();
				this->desc = de;
			}
		}

		void NineSliceComponent::init() {
			en = dynamic_cast<GraphicsEntity*>(parent);

#ifdef MACE_DEBUG_CHECK_NULLPTR
			if (en == nullptr) {
				MACE__THROW(InvalidType, "NineSliceComponent added to a non-graphical Entity");
			}
#endif
		}

		void NineSliceComponent::render() {
			Painter& painter = en->getPainter();
			const Beginner beginner(&painter);
			painter.push();
			painter.reset();
			painter.disableRenderFeatures(Painter::RenderFeatures::FILTER);
			painter.enableRenderFeatures(Painter::RenderFeatures::DISCARD_INVISIBLE | Painter::RenderFeatures::STORE_ID);
			painter.drawImage(desc.center);
			painter.pop();
		}

		void NineSliceComponent::clean(Metrics& metrics) {}
	}//gfx
}//mc