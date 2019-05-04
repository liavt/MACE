/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Font.h>
#include <MACE/Graphics/Entity2D.h>

namespace mc {
	namespace gfx {
		bool FontImpl::operator==(const FontImpl& other) const
		{
			return size == other.size;
		}
		bool FontImpl::operator!=(const FontImpl& other) const
		{
			return !operator==(other);
		}

		Font Font::loadFont(const std::string& name, unsigned int size) {
			return loadFont(name.c_str(), size);
		}

		Font Font::loadFont(const char* name, unsigned int size) {
			ensureFreetypeInit();

			//on 64 bit systems this cast is required
			Index id = static_cast<Index>(fonts.size());

			fonts.push_back(FT_Face());
			checkFreetypeError(FT_New_Face(freetype, name, 0, &fonts[id]), "Failed to create font");

			checkFreetypeError(FT_Select_Charmap(fonts[id], FT_ENCODING_UNICODE), "Failed to change charmap from font");

			return Font(id, size);
		}

		Font Font::loadFontFromMemory(const unsigned char* data, unsigned long int size, unsigned int fontSize) {
#ifdef MACE_DEBUG_CHECK_ARGS
			if (size <= 0) {
				MACE__THROW(OutOfBounds, "Input size for loadFontFromMemory is less or equal to than 0!");
			}
#endif

			ensureFreetypeInit();

			//on 64 bit systems this cast is required
			Index id = static_cast<Index>(fonts.size());

			fonts.push_back(FT_Face());

			checkFreetypeError(FT_New_Memory_Face(freetype, data, size, 0, &fonts[id]), "Failed to create font from memory");

			checkFreetypeError(FT_Select_Charmap(fonts[id], FT_ENCODING_UNICODE), "Failed to change charmap from font");

			return Font(id, fontSize);
		}

		void Font::destroy() {
			checkFreetypeError(FT_Done_Face(fonts[id]), "Failed to delete font");
		}

		void Font::setSize(const unsigned int h) {
			this->height = h;
		}

		unsigned int& Font::getSize() {
			return height;
		}

		const unsigned int& Font::getSize() const {
			return height;
		}

		bool Font::hasKerning() const {
			return FT_HAS_KERNING(fonts[id]) == 1;
		}

		Index Font::getID() const {
			return id;
		}

		void Font::getCharacter(const wchar_t c, std::shared_ptr<Letter> character) const {
#ifdef MACE_DEBUG_CHECK_ARGS
			if (height <= 0) {
				MACE__THROW(OutOfBounds, "The height of the font cannot be 0 - you must set it!");
			}
#endif

			WindowModule* window = getCurrentWindow();

			const Vector<int, 2> dpi = window->getMonitor().getDPI();

			checkFreetypeError(FT_Set_Char_Size(fonts[id], 0, height << 6, dpi[0], dpi[1]), "Failed to change char size");

			checkFreetypeError(FT_Load_Char(fonts[id], c, FT_LOAD_RENDER | FT_LOAD_PEDANTIC | FT_LOAD_TARGET_LCD), "Failed to load glyph");

			character->width = fonts[id]->glyph->metrics.width >> 6;
			character->height = fonts[id]->glyph->metrics.height >> 6;
			character->bearingX = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character->bearingY = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character->advanceX = fonts[id]->glyph->advance.x >> 6;
			character->advanceY = fonts[id]->glyph->advance.y >> 6;

			if (character->width == 0 || character->height == 0) {
				character->glyph = gfx::Texture::getGradient();
			}
			else {
				GraphicsContext* const context = window->getContext();

				character->glyph = context->getOrCreateTexture("MC/Fonts/" + std::to_string(id) + "/Sizes/" + std::to_string(id) + "/Glyphs/" + std::to_string(static_cast<int>(c)), [&]() {
					FT_Bitmap targetBitmap;
					FT_Bitmap_New(&targetBitmap);

					checkFreetypeError(FT_Bitmap_Convert(freetype, &fonts[id]->glyph->bitmap, &targetBitmap, 1), "Failed to convert bitmaps");

					targetBitmap.pixel_mode = FT_PIXEL_MODE_LCD;

					Texture out = convertBitmapToTexture(targetBitmap);

					checkFreetypeError(FT_Bitmap_Done(freetype, &targetBitmap), "Failed to delete bitmap");

					return out;
					});
			}
		}

		Vector<unsigned int, 2> Font::getKerning(const wchar_t prev, const wchar_t current) const {
			FT_Vector vec;

			checkFreetypeError(FT_Get_Kerning(fonts[id], prev, current, FT_KERNING_DEFAULT, &vec), "Failed to get kerning from font");

			return{ static_cast<unsigned int>(vec.x >> 6), static_cast<unsigned int>(vec.y >> 6) };
		}

		bool Font::operator==(const Font & other) const {
			return impl == other.impl;
		}

		bool Font::operator!=(const Font & other) const {
			return !operator==(other);
		}

		Font::Font(const Index fontID, const unsigned int h) : id(fontID), height(h) {}

		//font data, compiled in another file to increase compilation time
		extern unsigned char sourceCodeProData[];
		extern unsigned int sourceCodeProLength;

		extern unsigned char sourceSansProData[];
		extern unsigned int sourceSansProLength;

		extern unsigned char sourceSerifProData[];
		extern unsigned int sourceSerifProLength;

		Font::Font(const Fonts f, const unsigned int h) : Font(0, h) {
			static Font sourceCodePro, sourceSerifPro, sourceSansPro;

			switch (f) {
			case Fonts::CODE:
				if (sourceCodePro.getID() == 0) {
					sourceCodePro = Font::loadFontFromMemory(sourceCodeProData, sourceCodeProLength);
				}

				id = sourceCodePro.getID();
				break;
			case Fonts::SANS:
				if (sourceSansPro.getID() == 0) {
					sourceSansPro = Font::loadFontFromMemory(sourceSansProData, sourceSansProLength);
				}

				id = sourceSansPro.getID();
				break;
			case Fonts::SERIF:
				if (sourceSerifPro.getID() == 0) {
					sourceSerifPro = Font::loadFontFromMemory(sourceSerifProData, sourceSerifProLength);
				}

				id = sourceSerifPro.getID();
				break;
				default MACE_UNLIKELY:
				//should never be reached, but just to be safe
				MACE__THROW(Font, "Unknown Fonts enum constant");
			}
		}

		Font::Font(const Font & f) : Font(f.id, f.height) {}
	}//gfx
}//mc
