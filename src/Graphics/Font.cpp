/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Graphics/Font.h>

namespace mc {
	namespace gfx {
#ifdef MACE_DEBUG_CHECK_NULLPTR
#	define MACE__VERIFY_FONT_INIT() do{if(impl == nullptr){ MACE__THROW(InvalidState, "This Font has not had init() called yet"); }}while(0)
#else
#	define MACE__VERIFY_FONT_INIT()
#endif

		bool GlyphMetrics::operator==(const GlyphMetrics& other) const {
			return width == other.width && height == other.height && bearingX == other.bearingX && bearingY == other.bearingY && advanceX == other.advanceX && advanceY == other.advanceY;
		}

		bool GlyphMetrics::operator!=(const GlyphMetrics& other) const {
			return !operator==(other);
		}
		bool Glyph::operator==(const Glyph& other) const {
			return metrics == other.metrics && texture == other.texture;
		}
		bool Glyph::operator!=(const Glyph& other) const {
			return !operator==(other);
		}
		bool FontMetrics::operator==(const FontMetrics& other) const {
			return height == other.height && descent == other.descent && ascent == other.ascent && kerning == other.kerning;
		}
		bool FontMetrics::operator!=(const FontMetrics& other) const {
			return !operator==(other);
		}
		Font2 Font2::loadFont(const std::string& name, const FontSize size) {
			return loadFont(name.c_str(), size);
		}
		Font2 Font2::loadFont(CString name, const FontSize size) {
			FontDesc desc{};
			desc.loadType = FontLoadType::MEMORY;
			desc.input.path = name;
			return Font2(desc, size);
		}
		Font2 Font2::loadFontFromMemory(const unsigned char* data, Size dataSize, const FontSize size) {
			FontDesc desc{};
			desc.loadType = FontLoadType::MEMORY;
			desc.input.memory.data = data;
			desc.input.memory.size = dataSize;
			return Font2(desc, size);
		}
		Font2::Font2() noexcept : impl(nullptr) {}
		Font2::Font2(const FontDesc& desc, const FontSize size) : impl(nullptr), size(size) {
			init(desc);
		}
		Font2::Font2(const Font2& other) : impl(other.impl), size(other.getSize()) {}

		Font2::Font2(const Font2& other, const FontSize size) : impl(other.impl), size(size) {}

		void Font2::init(const FontDesc& desc) {
			impl = gfx::getCurrentWindow()->getContext()->createFontImpl(desc);
		}
		void Font2::destroy() {
			impl.reset();
		}
		Glyph& Font2::getGlyph(const wchar_t character) {
			MACE__VERIFY_FONT_INIT();

			Glyph& glyph = glyphs[size][character];
			if (glyph.texture.isCreated()) {
				return glyph;
			}

			checkHeight();

			impl->fillGlyph(glyph, character);

			return glyph;
		}

		void Font2::checkHeight() {
			if (size != lastCalculatedSize && impl != nullptr) {
				impl->calculateMetricsForSize(size);
				lastCalculatedSize = size;
			}
		}

		FontMetrics Font2::getFontMetrics() {
			MACE__VERIFY_FONT_INIT();

			checkHeight();

			return impl->getFontMetrics();
		}
		Vector<signed long, 2> Font2::getKerning(const wchar_t prev, const wchar_t current) {
			MACE__VERIFY_FONT_INIT();

			checkHeight();

			return impl->getKerning(prev, current);
		}

		MACE_GETTER_SETTER_DEF(Font2, Size, size, FontSize);

		bool Font2::isCreated() const {
			return impl != nullptr;
		}
		bool Font2::operator==(const Font2& other) const {
			return impl == other.impl;
		}
		bool Font2::operator!=(const Font2& other) const {
			return !operator==(other);
		}
	}//gfx
}//mc