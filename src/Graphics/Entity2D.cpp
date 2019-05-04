/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Core/System.h>

#undef FT_CONFIG_OPTION_USE_HARFBUZZ
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H

#include <cmath>
#include <vector>
#include <clocale>

#include <iostream>

namespace mc {
	namespace gfx {
		namespace {
			FT_Library freetype;
			//error if freetype init failed or -1 if it hasnt been created
			int freetypeStatus = -1;

			std::vector<FT_Face> fonts = std::vector<FT_Face>();

			Texture convertBitmapToTexture(const FT_Bitmap& bitmap){
				TextureDesc desc = TextureDesc(bitmap.width, bitmap.rows);
				switch (bitmap.pixel_mode) {
				case FT_PIXEL_MODE_GRAY:
					desc.format = TextureDesc::Format::LUMINANCE;
					desc.internalFormat = TextureDesc::InternalFormat::RED;
					break;
				case FT_PIXEL_MODE_LCD:
					desc.width /= 3;
					desc.format = TextureDesc::Format::RGB;
					desc.internalFormat = TextureDesc::InternalFormat::RGB8;
					break;
				}
				desc.type = TextureDesc::Type::UNSIGNED_BYTE;
				desc.wrapS = TextureDesc::Wrap::CLAMP;
				desc.wrapT = TextureDesc::Wrap::CLAMP;
				desc.minFilter = TextureDesc::Filter::LINEAR;
				desc.magFilter = TextureDesc::Filter::LINEAR;

				Texture out = Texture(desc);
				out.bind();

				out.resetPixelStorage();
				out.setUnpackStorageHint(gfx::PixelStorage::ALIGNMENT, 1);

				out.setData(bitmap.buffer);
				
				return out;
			}

			MACE_NORETURN void throwFreetypeError (const FT_Error& status, const std::string message) {
				/*
				using a custom X-Macro instead of using FT_ERROR_H because
				certain error codes (like Cannot_Open_Resource) require custom
				error classes (for example, Invalid_File_Format needs to throw
				BadFileError instead of FontError)

				there is no way to do that using FreeType's built in error
				X-Macro as you can't do conditional macros
				*/
#define MACE__FREETYPE_ERROR_CASE_TYPE(err, msg, type) case FT_Err_##err: MACE__THROW(type, message + ": Status " + std::to_string(status) + ": " + msg); break;
#define MACE__FREETYPE_ERROR_CASE(err, msg) MACE__FREETYPE_ERROR_CASE_TYPE(err, msg, Font)
				switch (status) {
					MACE__FREETYPE_ERROR_CASE(Ok, "No error (invalid internal call to throwFreetypeError)")
					MACE__FREETYPE_ERROR_CASE_TYPE(Cannot_Open_Resource, "Unable to open font file", FileNotFound)
					MACE__FREETYPE_ERROR_CASE_TYPE(Unknown_File_Format, "Unknown file format", BadFile)
					MACE__FREETYPE_ERROR_CASE_TYPE(Invalid_File_Format, "Broken file", BadFile)
					MACE__FREETYPE_ERROR_CASE_TYPE(Invalid_Version, "Invalid FreeType version", InitializationFailed)
					MACE__FREETYPE_ERROR_CASE(Invalid_Argument, "Invalid argument")
					MACE__FREETYPE_ERROR_CASE(Unimplemented_Feature, "Unimplemented feature")
					MACE__FREETYPE_ERROR_CASE(Invalid_Glyph_Index, "Invalid glyph index")
					MACE__FREETYPE_ERROR_CASE(Invalid_Character_Code, "Invalid character code")
					MACE__FREETYPE_ERROR_CASE(Cannot_Render_Glyph, "Cannot render glyph")
					MACE__FREETYPE_ERROR_CASE(Invalid_Pixel_Size, "Invalid pixel size")
					MACE__FREETYPE_ERROR_CASE(Invalid_Handle, "Invalid object handle")
					MACE__FREETYPE_ERROR_CASE(Invalid_Library_Handle, "Invalid library handle")
					MACE__FREETYPE_ERROR_CASE(Invalid_Driver_Handle, "Invalid driver handle")
					MACE__FREETYPE_ERROR_CASE(Invalid_Face_Handle, "Invalid face handle")
					MACE__FREETYPE_ERROR_CASE(Invalid_Size_Handle, "Invalid size handle")
					MACE__FREETYPE_ERROR_CASE(Invalid_Slot_Handle, "Invalid slot handle")
					MACE__FREETYPE_ERROR_CASE_TYPE(Too_Many_Drivers, "Too many drivers", System)
					MACE__FREETYPE_ERROR_CASE_TYPE(Too_Many_Extensions, "Too many extensions", System)
					MACE__FREETYPE_ERROR_CASE_TYPE(Out_Of_Memory, "Out of memory", OutOfMemory)
				default:
					MACE__THROW(Font, message + ": Unknown error code " + std::to_string(freetypeStatus));
				}
#undef MACE__FREETYPE_ERROR_CASE_TYPE
#undef MACE__FREETYPE_ERROR_CASE
			}

			inline void checkFreetypeError(const FT_Error& status, const std::string message) {
				if (status != FT_Err_Ok) {
					throwFreetypeError(status, message);
				}
			}

			void ensureFreetypeInit() {
				if (freetypeStatus < 0) {
					checkFreetypeError(freetypeStatus = FT_Init_FreeType(&freetype), "Failed to initialize FreeType");

					//font 0 should be null
					fonts.resize(2);
				}
			}
		}//anon namespace

		Entity2D::Entity2D() : GraphicsEntity() {}

		bool Entity2D::operator==(const Entity2D & other) const {
			return GraphicsEntity::operator==(other);
		}

		bool Entity2D::operator!=(const Entity2D & other) const {
			return !operator==(other);
		}

		bool Selectable::isClicked() const {
			return selectableProperties & Selectable::CLICKED;
		}

		bool Selectable::isDisabled() const {
			return selectableProperties & Selectable::DISABLED;
		}

		bool Selectable::isHovered() const {
			return selectableProperties & Selectable::HOVERED;
		}

		void Selectable::click() {
			selectableProperties |= Selectable::CLICKED;

			onClick();
		}

		void Selectable::disable() {
			selectableProperties |= Selectable::DISABLED;

			onDisable();
		}

		void Selectable::enable() {
			selectableProperties &= ~Selectable::DISABLED;

			onEnable();
		}

		void Selectable::trigger() {
			onTrigger();
		}

		void Selectable::onClick() {}


		void Selectable::onEnable() {}

		void Selectable::onDisable() {}

		void Selectable::onTrigger() {}

		void Selectable::doHover() {
			if (!isDisabled()) {
				if (gfx::Input::isKeyDown(gfx::Input::MOUSE_LEFT)) {
					click();
				}

				if (gfx::Input::isKeyReleased(gfx::Input::MOUSE_LEFT) && isClicked()) {
					selectableProperties &= ~Selectable::CLICKED;

					trigger();
				}
			}
		}

		//IMAGE

		Image::Image() noexcept : texture() {}

		Image::Image(const Texture& tex) : texture(tex) {}

		void Image::onInit() {}

		void Image::onUpdate() {}

		void Image::onRender(Painter& p) {
			p.drawImage(texture);
		}

		void Image::onDestroy() {
			if (texture.isCreated()) {
				texture.destroy();
			}
		}

		void Image::onClean() {}

		void Image::setTexture(const Texture & tex) {
			if (tex != texture) {
				makeDirty();

				texture = tex;
			}
		}

		Texture & Image::getTexture() {
			makeDirty();

			return texture;
		}

		const Texture & Image::getTexture() const {
			return texture;
		}

		bool Image::operator==(const Image & other) const {
			return Entity2D::operator==(other) && texture == other.texture;
		}

		bool Image::operator!=(const Image & other) const {
			return !operator==(other);
		}

		//PROGRESS BAR

		ProgressBar::ProgressBar() noexcept: ProgressBar(0, 0, 0) {}

		ProgressBar::ProgressBar(const float minimum, const float maximum, const float prog) noexcept : minimumProgress(minimum), maximumProgress(maximum), progress(prog) {}

		void ProgressBar::setBackgroundTexture(const Texture & tex) {
			if (backgroundTexture != tex) {
				makeDirty();

				backgroundTexture = tex;
			}
		}

		Texture & ProgressBar::getBackgroundTexture() {
			makeDirty();

			return backgroundTexture;
		}

		const Texture & ProgressBar::getBackgroundTexture() const {
			return backgroundTexture;
		}

		void ProgressBar::setForegroundTexture(const Texture & tex) {
			if (foregroundTexture != tex) {
				makeDirty();

				foregroundTexture = tex;
			}
		}

		Texture & ProgressBar::getForegroundTexture() {
			makeDirty();

			return foregroundTexture;
		}

		const Texture & ProgressBar::getForegroundTexture() const {
			return foregroundTexture;
		}

		void ProgressBar::setSelectionTexture(const Texture & tex) {
			if (selectionTexture != tex) {
				makeDirty();

				selectionTexture = tex;
			}
		}

		Texture & ProgressBar::getSelectionTexture() {
			makeDirty();

			return selectionTexture;
		}

		const Texture & ProgressBar::getSelectionTexture() const {
			return selectionTexture;
		}


		void ProgressBar::setMinimum(const float minimum) {
			if (minimumProgress != minimum) {
				makeDirty();

				minimumProgress = minimum;
			}
		}

		float ProgressBar::getMinimum() {
			return minimumProgress;
		}

		const float ProgressBar::getMinimum() const {
			return minimumProgress;
		}

		void ProgressBar::setMaximum(const float maximum) {
			if (maximumProgress != maximum) {
				makeDirty();

				maximumProgress = maximum;
			}
		}

		float ProgressBar::getMaximum() {
			return maximumProgress;
		}

		const float ProgressBar::getMaximum() const {
			return maximumProgress;
		}

		void ProgressBar::setProgress(const float prog) {
			if (progress != prog) {
				makeDirty();

				progress = prog;
			}
		}

		float & ProgressBar::getProgress() {
			makeDirty();

			return progress;
		}

		const float & ProgressBar::getProgress() const {
			return progress;
		}

		void ProgressBar::easeTo(const float destination, const EaseSettings settings) {
			addComponent(std::shared_ptr<Component>(new EaseComponent([](Entity* e, float progress) {
				Progressable* prog = dynamic_cast<Progressable*>(e);

#ifdef MACE_DEBUG_CHECK_NULLPTR
				if (prog == nullptr) {
					MACE__THROW(NullPointer, "Internal Error: Progressable in EaseComponent is nullptr");
				}
#endif

				prog->setProgress(progress);
			}, settings, getProgress(), destination)));
		}

		bool ProgressBar::operator==(const ProgressBar & other) const {
			return Entity2D::operator==(other) && maximumProgress == other.maximumProgress&&minimumProgress == other.minimumProgress&&progress == other.progress&&backgroundTexture == other.backgroundTexture&&foregroundTexture == other.foregroundTexture&&selectionTexture == other.selectionTexture;
		}

		bool ProgressBar::operator!=(const ProgressBar & other) const {
			return !operator==(other);
		}

		void ProgressBar::onRender(Painter& p) {
			p.enableRenderFeatures(Painter::RenderFeatures::DISCARD_INVISIBLE);
			p.blendImagesMasked(foregroundTexture, backgroundTexture, selectionTexture,
								minimumProgress / maximumProgress,
								(progress - minimumProgress) / (maximumProgress - minimumProgress));
		}

		void ProgressBar::onDestroy() {
			if (backgroundTexture.isCreated()) {
				backgroundTexture.destroy();
			}

			if (foregroundTexture.isCreated()) {
				foregroundTexture.destroy();
			}

			if (selectionTexture.isCreated()) {
				selectionTexture.destroy();
			}
		}


		Slider::Slider() noexcept : ProgressBar() {}

		Slider::Slider(const float minimum, const float maximum, const float progress) noexcept : ProgressBar(minimum, maximum, progress) {}

		void Slider::onRender(Painter & p) {
			ProgressBar::onRender(p);

			p.setTarget(FrameBufferTarget::DATA);
			p.drawImage(selectionTexture);
		}

		void Slider::onClick() {
			const Renderer* renderer = getCurrentWindow()->getContext()->getRenderer();

			const int mouseX = gfx::Input::getMouseX(), mouseY = gfx::Input::getMouseY();
			if (mouseX >= 0 && mouseY >= 0) {
				const Color pixel = renderer->getPixelAt(static_cast<unsigned int>(mouseX), static_cast<unsigned int>(mouseY), FrameBufferTarget::DATA);

				setProgress(minimumProgress + (pixel.r * (maximumProgress - minimumProgress)));
			}
		}

		void Slider::onHover() {
			doHover();
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

		Font Font::loadFontFromMemory(const unsigned char * data, unsigned long int size, unsigned int fontSize) {
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

			checkFreetypeError(FT_Load_Char(fonts[id], c, FT_LOAD_RENDER | FT_LOAD_PEDANTIC | FT_LOAD_TARGET_LCD), "Failed to load glyph");

			character->width = fonts[id]->glyph->metrics.width >> 6;
			character->height = fonts[id]->glyph->metrics.height >> 6;
			character->bearingX = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character->bearingY = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character->advanceX = fonts[id]->glyph->advance.x >> 6;
			character->advanceY = fonts[id]->glyph->advance.y >> 6;

			if (character->width == 0 || character->height == 0) {
				character->glyph = gfx::Texture::getGradient();
			} else {
				GraphicsContext* const context = getCurrentWindow()->getContext();

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

		int Font::getLineHeight() const
		{
			return fonts[id]->size->metrics.height >> 6;
		}

		Vector<unsigned int, 2> Font::getKerning(const wchar_t prev, const wchar_t current) const {
			FT_Vector vec;

			checkFreetypeError(FT_Get_Kerning(fonts[id], prev, current, FT_KERNING_DEFAULT, &vec), "Failed to get kerning from font");

			return{ static_cast<unsigned int>(vec.x >> 6), static_cast<unsigned int>(vec.y >> 6) };
		}

		void Font::calculateMetrics() const
		{
			const Vector<int, 2> dpi = getCurrentWindow()->getMonitor().getDPI();

			checkFreetypeError(FT_Set_Char_Size(fonts[id], 0, height << 6, dpi[0], dpi[1]), "Failed to change char size");
		}

		bool Font::operator==(const Font& other) const {
			return id == other.id&&height == other.height;
		}

		bool Font::operator!=(const Font& other) const {
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

		Letter::Letter()  {}

		const Texture& Letter::getGlyph() const {
			return glyph;
		}

		const Texture & Letter::getTexture() const {
			return texture;
		}
		const unsigned int& Letter::getCharacterWidth() const {
			return width;
		}

		const unsigned int& Letter::getCharacterHeight() const {
			return height;
		}

		const signed long& Letter::getXBearing() const {
			return bearingX;
		}

		const signed long& Letter::getYBearing() const {
			return bearingY;
		}

		const signed long& Letter::getXAdvance() const {
			return advanceX;
		}

		const signed long& Letter::getYAdvance() const {
			return advanceY;
		}

		bool Letter::operator==(const Letter& other) const {
			return Entity2D::operator==(other) && texture == other.texture && glyph == other.glyph &&width == other.width&&height == other.height&&bearingX == other.bearingX&&bearingY == other.bearingY&&advanceX == other.advanceX&&advanceY == other.advanceY;
		}

		bool Letter::operator!=(const Letter& other) const {
			return !operator==(other);
		}

		void Letter::onInit() {}

		void Letter::onUpdate() {}

		void Letter::onRender(Painter& p) {
			p.disableRenderFeatures(Painter::RenderFeatures::INHERIT_SCALE | Painter::RenderFeatures::STORE_ID);
			p.setTexture(texture, TextureSlot::FOREGROUND);
			p.setTexture(glyph, TextureSlot::BACKGROUND);
			p.drawQuad(Painter::Brush::TEXT);
		}

		void Letter::onDestroy() {
			if (texture.isCreated()) {
				texture.destroy();
			}

			if (glyph.isCreated()) {
				glyph.destroy();
			}
		}

		void Letter::onClean() {}

		Text::Text(const std::string & s, const Font & f) : Text(os::toWideString(s), f) {}

		Text::Text(const std::wstring & t, const Font& f) : TexturedEntity2D(), text(t), font(f) {}

		void mc::gfx::Text::setText(const std::string& newText)
		{
			setText(os::toWideString(newText));
		}

		void Text::setText(const std::wstring & newText) {
			if (text != newText) {
				makeDirty();

				text = newText;
			}
		}

		std::wstring & Text::getText() {
			makeDirty();

			return text;
		}

		const std::wstring & Text::getText() const {
			return text;
		}

		void Text::setFont(const Font& f) {
			if (font != f) {
				makeDirty();

				font = f;
			}
		}

		Font & Text::getFont() {
			makeDirty();

			return font;
		}

		const Font & Text::getFont() const {
			return font;
		}

		const std::vector<std::shared_ptr<Letter>>& Text::getLetters() const {
			return letters;
		}

		void Text::setTexture(const Texture & tex) {
			if (tex != texture) {
				makeDirty();

				texture = tex;
			}
		}

		Texture & Text::getTexture() {
			makeDirty();

			return texture;
		}

		const Texture & Text::getTexture() const {
			return texture;
		}

		bool Text::operator==(const Text & other) const {
			return Entity2D::operator==(other) && letters == other.letters && text == other.text && texture == other.texture;
		}

		bool Text::operator!=(const Text & other) const {
			return !operator==(other);
		}

		void Text::onInit() {}

		void Text::onUpdate() {}

		void Text::onRender(Painter& p) {
			p.setForegroundColor(Colors::BLACK);
			p.fillRect();
		}

		void Text::onDestroy() {}

		void Text::onClean() {
			if (font.getID() == 0) {
				MACE__THROW(InitializationFailed, "Can\'t render Text with unitialized font!");
			}

			while (letters.size() > text.length()) {
				removeChild(letters.back());
				letters.pop_back();
			}
			while (letters.size() < text.length()) {
				std::shared_ptr<Letter> letter = std::shared_ptr<Letter>(new Letter());
				letters.push_back(letter);
				addChild(letter);
			}

			font.calculateMetrics();

			const Entity::Metrics metrics = getMetrics();

			const WindowModule::LaunchConfig config = gfx::getCurrentWindow()->getLaunchConfig();

			const float origWidth = static_cast<const float>(config.width),
				origHeight = static_cast<const float>(config.height);

			const bool hasKerning = font.hasKerning();

			const float lineHeight = static_cast<float>(font.getLineHeight()) / origHeight;

			float x = 0, y = 0;

			float width = 0, height = lineHeight;
			for (Index i = 0; i < text.length(); ++i) {
				if (text[i] == '\n') {
					if (x > width) {
						width = x;
					}

					y -= lineHeight;

					height += lineHeight;

					x = 0;
					font.getCharacter(' ', letters[i]);
					letters[i]->getPainter().setOpacity(0.0f);
				} else {
					font.getCharacter(text[i], letters[i]);

					//freetype uses absolute values (pixels) and we use relative. so by dividing the pixel by the size, we get relative values
					letters[i]->setWidth(static_cast<float>(letters[i]->width) / origWidth);
					letters[i]->setHeight(static_cast<float>(letters[i]->height) / origHeight);

					Vector<float, 2> position = { x, y };

					if (i > 0 && hasKerning) {
						const Vector<unsigned int, 2> delta = font.getKerning(text[i - 1], text[i]);

						position[0] += static_cast<float>(delta[0]) / origWidth;
						position[1] += static_cast<float>(delta[1]) / origHeight;
					}

					position[1] += letters[i]->getHeight();
					//i cant bear this
					position[1] -= static_cast<float>(letters[i]->height - letters[i]->bearingY) / origHeight;

					letters[i]->setX((position[0] + (static_cast<float>(letters[i]->width) / origWidth)));
					letters[i]->setY(position[1]);

					letters[i]->getPainter().setOpacity(getPainter().getOpacity());

					x += static_cast<float>(letters[i]->advanceX) / origWidth;
					x += letters[i]->getWidth();

					if (this->texture.isCreated()) {
						letters[i]->texture = this->texture;
					} else {
						letters[i]->texture = Colors::WHITE;
					}
				}
			}

			if (x > width) {
				width = x;
			}
			
			width /= 2.0f;
			height /= 2.0f;

			for (auto letter : letters) {
				letter->translate(-width, -height);
			}

			setWidth(width);
			setHeight(height);
		}

		const Texture & Button::getTexture() const {
			return texture;
		}

		Texture & Button::getTexture() {
			makeDirty();

			return texture;
		}

		void Button::setTexture(const Texture & c) {
			if (texture != c) {
				makeDirty();

				texture = c;
			}
		}

		const Texture & Button::getHoverTexture() const {
			return hoverTexture;
		}

		Texture & Button::getHoverTexture() {
			makeDirty();

			return hoverTexture;
		}

		void Button::setHoverTexture(const Texture & c) {
			if (hoverTexture != c) {
				makeDirty();

				hoverTexture = c;
			}
		}

		const Texture & Button::getClickedTexture() const {
			return clickedTexture;
		}

		Texture & Button::getClickedTexture() {
			makeDirty();

			return clickedTexture;
		}

		void Button::setClickedTexture(const Texture & c) {
			if (clickedTexture != c) {
				makeDirty();

				clickedTexture = c;
			}
		}

		const Texture & Button::getDisabledTexture() const {
			return disabledTexture;
		}

		Texture & Button::getDisabledTexture() {
			makeDirty();

			return disabledTexture;
		}

		void Button::setDisabledTexture(const Texture & c) {
			if (disabledTexture != c) {
				makeDirty();

				disabledTexture = c;
			}
		}

		void Button::onRender(Painter& p) {
			p.drawImage(texture);
			if (isDisabled()) {
				p.drawImage(disabledTexture);
			} else {
				if (isHovered()) {
					p.drawImage(hoverTexture);
				}
				if (isClicked()) {
					p.drawImage(clickedTexture);
				}
			}
		}

		void Button::onHover() {
			doHover();
		}

		void Button::onDestroy() {
			if (texture.isCreated()) {
				texture.destroy();
			}

			if (hoverTexture.isCreated()) {
				hoverTexture.destroy();
			}

			if (clickedTexture.isCreated()) {
				clickedTexture.destroy();
			}

			if (disabledTexture.isCreated()) {
				disabledTexture.destroy();
			}
		}
	}//gfx
}//mc
