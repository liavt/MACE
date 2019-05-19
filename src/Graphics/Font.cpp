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
		Font Font::loadFont(const std::string& name, const FontSize size) {
			return loadFont(name.c_str(), size);
		}
		Font Font::loadFont(CString name, const FontSize size) {
			FontDesc desc{};
			desc.loadType = FontLoadType::FILE;
			desc.input.path = name;
			return Font(desc, size);
		}
		Font Font::loadFontFromMemory(const unsigned char* data, Size dataSize, const FontSize size) {
			FontDesc desc{};
			desc.loadType = FontLoadType::MEMORY;
			desc.input.memory.data = data;
			desc.input.memory.size = dataSize;
			return Font(desc, size);
		}
		Font::Font() noexcept : impl(nullptr) {}
		Font::Font(const FontDesc& desc, const FontSize size) : impl(nullptr), size(size) {
			init(desc);
		}
		Font::Font(const Font& other) : impl(other.impl), size(other.getSize()) {}

		Font::Font(const Font& other, const FontSize size) : impl(other.impl), size(size) {}

		//font data, compiled in another file to increase compilation time
		extern unsigned char sourceCodeProData[];
		extern unsigned int sourceCodeProLength;

		extern unsigned char sourceSansProData[];
		extern unsigned int sourceSansProLength;

		extern unsigned char sourceSerifProData[];
		extern unsigned int sourceSerifProLength;

		Font::Font(const Fonts f, const FontSize h) : impl(nullptr), size(h) {
			static Font sourceCodePro, sourceSerifPro, sourceSansPro;

			//TODO make this not use static Font's
			switch (f) {
			case Fonts::CODE:
				if (!sourceCodePro.isCreated()) {
					sourceCodePro = Font::loadFontFromMemory(sourceCodeProData, sourceCodeProLength);
				}
				impl = sourceCodePro.impl;
				break;
			case Fonts::SANS:
				if (!sourceSansPro.isCreated()) {
					sourceSansPro = Font::loadFontFromMemory(sourceSansProData, sourceSansProLength);
				}

				impl = sourceSansPro.impl;
				break;
			case Fonts::SERIF:
				if (!sourceSerifPro.isCreated()) {
					sourceSerifPro = Font::loadFontFromMemory(sourceSerifProData, sourceSerifProLength);
				}

				impl = sourceSerifPro.impl;
				break;
				default MACE_UNLIKELY:
				//should never be reached, but just to be safe
				MACE__THROW(Font, "Unknown Fonts enum constant");
			}
		}

		void Font::init(const FontDesc& desc) {
			if (desc.loadType == FontLoadType::FILE) {
				if (desc.input.path == nullptr) {
					MACE__THROW(NullPointer, "Input file path in FontDesc was nullptr");
				}
			} else if (desc.loadType == FontLoadType::MEMORY) {
				if (desc.input.memory.data == nullptr) {
					MACE__THROW(NullPointer, "Input memory in FontDesc was nullptr");
				}
			}

			impl = gfx::getCurrentWindow()->getContext()->createFontImpl(desc);
		}
		void Font::destroy() {
			impl.reset();
		}
		Glyph& Font::getGlyph(const wchar_t character) {
			MACE__VERIFY_FONT_INIT();

			Glyph& glyph = glyphs[size][character];
			if (glyph.texture.isCreated()) {
				return glyph;
			}

			checkHeight();

			impl->fillGlyph(glyph, character);

			return glyph;
		}

		void Font::checkHeight() {
			if (size != lastCalculatedSize && impl != nullptr) {
				impl->calculateMetricsForSize(size);
				lastCalculatedSize = size;
			}
		}

		FontMetrics Font::getFontMetrics() {
			MACE__VERIFY_FONT_INIT();

			checkHeight();

			return impl->getFontMetrics();
		}
		Vector<signed long, 2> Font::getKerning(const wchar_t prev, const wchar_t current) {
			MACE__VERIFY_FONT_INIT();

			checkHeight();

			return impl->getKerning(prev, current);
		}

		MACE_GETTER_SETTER_DEF(Font, Size, size, FontSize);

		bool Font::isCreated() const {
			return impl != nullptr;
		}
		bool Font::operator==(const Font& other) const {
			return impl == other.impl;
		}
		bool Font::operator!=(const Font& other) const {
			return !operator==(other);
		}
	}//gfx
}//mc