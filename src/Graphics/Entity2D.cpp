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

#include <cmath>
#include <vector>
#include <clocale>

namespace mc {
	namespace gfx {
		namespace {
			ogl::Texture2D whiteTexture;

			FT_Library freetype;
			//error if freetype init failed or -1 if it hasnt been created
			int freetypeStatus = -1;

			std::vector<FT_Face> fonts = std::vector<FT_Face>();
		}//anon namespace

		Entity2D::Entity2D() : GraphicsEntity() {}

		bool Entity2D::operator==(const Entity2D & other) const {
			return GraphicsEntity::operator==(other);
		}

		bool Entity2D::operator!=(const Entity2D & other) const {
			return !operator==(other);
		}

		//IMAGE

		Image::Image() noexcept : texture() {}

		Image::Image(const ColorAttachment& tex) : texture(tex) {}

		void Image::onInit() {
			if (!texture.isCreated()) {
				texture.init();
			}
		}

		void Image::onUpdate() {}

		void Image::onRender() {
			Painter p = Painter(this);
			p->init();
			p->drawImage(texture, -0.5f, -0.5f, 0.5f, 0.5f);
			p->setColor(texture.getPaint());
			p->fillRect(0.5f, 0.5f, 0.5f, 0.5f);
			p->destroy();
		}

		void Image::onDestroy() {
			if (texture.isCreated()) {
				texture.destroy();
			}
		}

		void Image::onClean() {}

		void Image::setTexture(const ColorAttachment & tex) {
			if (tex != texture) {
				makeDirty();

				texture = tex;
			}
		}

		ColorAttachment & Image::getTexture() {
			makeDirty();

			return texture;
		}

		const ColorAttachment & Image::getTexture() const {
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

		void ProgressBar::setBackgroundTexture(const ColorAttachment & tex) {
			if (backgroundTexture != tex) {
				makeDirty();

				backgroundTexture = tex;
			}
		}

		ColorAttachment & ProgressBar::getBackgroundTexture() {
			makeDirty();

			return backgroundTexture;
		}

		const ColorAttachment & ProgressBar::getBackgroundTexture() const {
			return backgroundTexture;
		}

		void ProgressBar::setForegroundTexture(const ColorAttachment & tex) {
			if (foregroundTexture != tex) {
				makeDirty();

				foregroundTexture = tex;
			}
		}

		ColorAttachment & ProgressBar::getForegroundTexture() {
			makeDirty();

			return foregroundTexture;
		}

		const ColorAttachment & ProgressBar::getForegroundTexture() const {
			return foregroundTexture;
		}

		void ProgressBar::setSelectionTexture(const ColorAttachment & tex) {
			if (selectionTexture != tex) {
				makeDirty();

				selectionTexture = tex;

				selectionTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			}
		}

		ColorAttachment & ProgressBar::getSelectionTexture() {
			makeDirty();

			return selectionTexture;
		}

		const ColorAttachment & ProgressBar::getSelectionTexture() const {
			return selectionTexture;
		}


		void ProgressBar::setMinimum(const float minimum) {
			if (minimumProgress != minimum) {
				makeDirty();

				minimumProgress = minimum;
			}
		}

		float & ProgressBar::getMinimum() {
			makeDirty();

			return minimumProgress;
		}

		const float & ProgressBar::getMinimum() const {
			return minimumProgress;
		}

		void ProgressBar::setMaximum(const float maximum) {
			if (maximumProgress != maximum) {
				makeDirty();

				maximumProgress = maximum;
			}
		}

		float & ProgressBar::getMaximum() {
			makeDirty();

			return maximumProgress;
		}

		const float & ProgressBar::getMaximum() const {
			return maximumProgress;
		}

		void ProgressBar::setProgress(const float prog) {
			if (progress != prog) {
				makeDirty();

				progress = prog;
			}
		}

		void ProgressBar::addProgress(const float prog) {
			if (prog != 0) {
				makeDirty();

				progress += prog;
			}
		}

		float & ProgressBar::getProgress() {
			makeDirty();

			return progress;
		}

		const float & ProgressBar::getProgress() const {
			return progress;
		}


		void ProgressBar::easeTo(const float destination, const float time, const EaseFunction function, const EaseComponent::EaseDoneCallback callback) {
			addComponent(SmartPointer<Component>(new EaseComponent(time, this->progress, destination, [](Entity* e, float progress) {
				ProgressBar* bar = dynamic_cast<ProgressBar*>(e);

				if (bar == nullptr) {
					MACE__THROW(NullPointer, "Internal Error: ProgressBar in EaseComponent is nullptr");
				}

				bar->setProgress(progress);
			}, function, callback), true));
		}

		bool ProgressBar::operator==(const ProgressBar & other) const {
			return Entity2D::operator==(other) && maximumProgress == other.maximumProgress&&minimumProgress == other.minimumProgress&&progress == other.progress&&backgroundTexture == other.backgroundTexture&&foregroundTexture == other.foregroundTexture&&selectionTexture == other.selectionTexture;
		}

		bool ProgressBar::operator!=(const ProgressBar & other) const {
			return !operator==(other);
		}

		void ProgressBar::onInit() {
			if (!backgroundTexture.isCreated()) {
				backgroundTexture.init();
			}

			if (!foregroundTexture.isCreated()) {
				foregroundTexture.init();
			}

			if (!selectionTexture.isCreated()) {
				selectionTexture.init();
			}
		}

		void ProgressBar::onUpdate() {}

		void ProgressBar::onRender() {}

		void ProgressBar::onClean() {}

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

		Font Font::loadFont(const std::string& name) {
			return loadFont(name.c_str());
		}

		Font Font::loadFont(const char* name) {
			if (freetypeStatus < 0) {
				if ((freetypeStatus = FT_Init_FreeType(&freetype)) != FT_Err_Ok) {
					throw FontError("Freetype failed to initialize with error code " + std::to_string(freetypeStatus));
				}

				//font 0 should be null
				fonts.resize(2);
			}

			//on 64 bit systems this cast is required
			Index id = static_cast<Index>(fonts.size());

			fonts.push_back(FT_Face());
			if (int result = FT_New_Face(freetype, name, 0, &fonts[id])) {
				throw FontError("Freetype failed to create font at " + std::string(name) + " with result " + std::to_string(result));
			}

			if (int result = FT_Select_Charmap(fonts[id], FT_ENCODING_UNICODE)) {
				throw FontError("Freetype failed to change charmap with result " + std::to_string(result));
			}

			return Font(id);
		}

		Font Font::loadFontFromMemory(const unsigned char * data, long int size) {
			if (size <= 0) {
				throw IndexOutOfBoundsError("Input size for loadFontFromMemory is less or equal to than 0!");
			}

			if (freetypeStatus < 0) {
				if ((freetypeStatus = FT_Init_FreeType(&freetype)) != FT_Err_Ok) {
					throw FontError("Freetype failed to initailize with error code " + std::to_string(freetypeStatus));
				}

				fonts.resize(2);
			}

			//on 64 bit systems this cast is required
			Index id = static_cast<Index>(fonts.size());

			fonts.push_back(FT_Face());
			if (int result = FT_New_Memory_Face(freetype, data, size, 0, &fonts[id])) {
				throw FontError("Freetype failed to create font from memory with result " + std::to_string(result));
			}

			if (int result = FT_Select_Charmap(fonts[id], FT_ENCODING_UNICODE)) {
				throw FontError("Freetype failed to change charmap with result " + std::to_string(result));
			}

			return Font(id);
		}

		void Font::destroy() {
			if (int result = FT_Done_Face(fonts[id])) {
				throw FontError("Freetype failed to delete font with result " + std::to_string(result));
			}
		}

		void Font::setSize(const Size h) {
			this->height = h;
		}

		Size& Font::getSize() {
			return height;
		}

		const Size& Font::getSize() const {
			return height;
		}

		bool Font::hasKerning() const {
			return FT_HAS_KERNING(fonts[id]) == 1;
		}

		Index Font::getID() const {
			return id;
		}

		void Font::getCharacter(const wchar_t c, Letter& character) const {
			if (height <= 0) {
				throw IndexOutOfBoundsError("The height of the font cannot be 0 - you must set it!");
			}

			FT_Set_Pixel_Sizes(fonts[id], 0, height);

			if (int result = FT_Load_Char(fonts[id], c, FT_LOAD_RENDER | FT_LOAD_PEDANTIC | FT_LOAD_TARGET_LIGHT)) {
				throw FontError("Failed to load glyph with error code " + std::to_string(result));
			}

			character.width = fonts[id]->glyph->metrics.width >> 6;
			character.height = fonts[id]->glyph->metrics.height >> 6;
			character.bearingX = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character.bearingY = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character.advanceX = fonts[id]->glyph->advance.x >> 6;
			character.advanceY = fonts[id]->glyph->advance.y >> 6;

			character.mask.init();
			character.mask.bind();

			character.mask.resetPixelStorage();

			character.mask.setPixelStorage(GL_UNPACK_ALIGNMENT, 1);

			character.mask.setData(fonts[id]->glyph->bitmap.buffer, character.width, character.height, GL_UNSIGNED_BYTE, GL_RED, GL_RED);
			character.mask.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			character.mask.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			character.mask.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			character.mask.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		Vector<unsigned int, 2> Font::getKerning(const wchar_t prev, const wchar_t current) const {
			FT_Vector vec;

			FT_Get_Kerning(fonts[id], prev, current, FT_KERNING_DEFAULT, &vec);

			return{ static_cast<unsigned int>(vec.x >> 6), static_cast<unsigned int>(vec.y >> 6) };
		}

		bool Font::operator==(const Font& other) const {
			return id == other.id&&height == other.height;
		}

		bool Font::operator!=(const Font& other) const {
			return !operator==(other);
		}

		Font::Font(const Index fontID, const Size h) : id(fontID), height(h) {}

		//font data, compiled in another file to increase compilation time
		extern unsigned char sourceCodeProData[];
		extern unsigned int sourceCodeProLength;

		extern unsigned char sourceSansProData[];
		extern unsigned int sourceSansProLength;

		extern unsigned char sourceSerifProData[];
		extern unsigned int sourceSerifProLength;

		Font::Font(const Fonts f, const Size h) : Font(0, h) {
			if (f == Fonts::CODE) {
				static Font sourceCodePro;

				if (sourceCodePro.getID() == 0) {
					sourceCodePro = Font::loadFontFromMemory(sourceCodeProData, sourceCodeProLength);
				}

				id = sourceCodePro.getID();
			} else if (f == Fonts::SANS) {
				static Font sourceSansPro;

				if (sourceSansPro.getID() == 0) {
					sourceSansPro = Font::loadFontFromMemory(sourceSansProData, sourceSansProLength);
				}

				id = sourceSansPro.getID();
			} else if (f == Fonts::SERIF) {
				static Font sourceSerifPro;

				if (sourceSerifPro.getID() == 0) {
					sourceSerifPro = Font::loadFontFromMemory(sourceSerifProData, sourceSerifProLength);
				}

				id = sourceSerifPro.getID();
			} else {
				//should never be reached, but just to be safe
				throw FontError("Unknown Fonts enum constant");
			}
		}

		Font::Font(const Font & f) : Font(f.id, f.height) {}

		Letter::Letter(const ogl::Texture2D& tex) : mask(tex) {}

		const ogl::Texture2D& Letter::getMask() const {
			return mask;
		}
		const ColorAttachment & Letter::getTexture() const {
			return texture;
		}
		const Size& Letter::getCharacterWidth() const {
			return width;
		}

		const Size& Letter::getCharacterHeight() const {
			return height;
		}

		const Index& Letter::getXBearing() const {
			return bearingX;
		}

		const Index& Letter::getYBearing() const {
			return bearingY;
		}

		const Index& Letter::getXAdvance() const {
			return advanceX;
		}

		const Index& Letter::getYAdvance() const {
			return advanceY;
		}

		bool Letter::operator==(const Letter& other) const {
			return Entity2D::operator==(other) && mask == other.mask&&width == other.width&&height == other.height&&bearingX == other.bearingX&&bearingY == other.bearingY&&advanceX == other.advanceX&&advanceY == other.advanceY;
		}

		bool Letter::operator!=(const Letter& other) const {
			return !operator==(other);
		}

		void Letter::onInit() {
			if (!mask.isCreated()) {
				mask.init();
			}

			if (!texture.isCreated()) {
				texture.init();
			}
		}

		void Letter::onUpdate() {}

		void Letter::onRender() {}

		void Letter::onDestroy() {
			if (mask.isCreated()) {
				mask.destroy();
			}

			if (texture.isCreated()) {
				texture.destroy();
			}
		}

		void Letter::onClean() {}

		Text::Text(const std::string & s, const Font & f) : Text(os::toWideString(s), f) {}

		Text::Text(const std::wstring & t, const Font& f) : Entity2D(), text(t), font(f) {}

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

		const std::vector<Letter>& Text::getLetters() const {
			return letters;
		}

		void Text::setVerticalAlign(const VerticalAlign align) {
			if (vertAlign != align) {
				makeDirty();

				vertAlign = align;
			}
		}

		const VerticalAlign Text::getVerticalAlign() const {
			return vertAlign;
		}

		void Text::setHorizontalAlign(const HorizontalAlign align) {
			if (horzAlign != align) {
				makeDirty();

				horzAlign = align;
			}
		}

		const HorizontalAlign Text::getHorizontalAlign() const {
			return horzAlign;
		}

		void Text::setTexture(const ColorAttachment & tex) {
			if (tex != texture) {
				makeDirty();

				texture = tex;
			}
		}

		ColorAttachment & Text::getTexture() {
			makeDirty();

			return texture;
		}

		const ColorAttachment & Text::getTexture() const {
			return texture;
		}

		bool Text::operator==(const Text & other) const {
			return Entity2D::operator==(other) && letters == other.letters && text == other.text && texture == other.texture && vertAlign == other.vertAlign && horzAlign == other.horzAlign;
		}

		bool Text::operator!=(const Text & other) const {
			return !operator==(other);
		}

		void Text::onInit() {}

		void Text::onUpdate() {}

		void Text::onRender() {}

		void Text::onDestroy() {}

		void Text::onClean() {
			if (font.getID() == 0) {
				MACE__THROW(InitializationFailed, "Can\'t render Text with unitialized font!");
			}

			for (Index i = 0; i < letters.size(); ++i) {
				if (hasChild(letters[i])) {
					removeChild(letters[i]);
				}
			}

			letters.clear();

			letters.resize(text.length());

			Entity::Metrics metrics = getMetrics();

			const float widthScale = 1.0f / metrics.scale[0], heightScale = 1.0f / metrics.scale[1];

			const float origWidth = static_cast<const float>(getRenderer()->getOriginalWidth()),
				origHeight = static_cast<const float>(getRenderer()->getOriginalHeight());

			const bool hasKerning = font.hasKerning();

			float x = 0, y = 0;

			float width = 0, height = 0;

			for (Index i = 0; i < text.length(); ++i) {
				if (text[i] == '\n') {
					if (x > width) {
						width = x;
					}

					y -= static_cast<float>(font.getSize()) / origHeight;

					height = y;

					x = 0;
				} else {
					letters[i] = Letter();
					font.getCharacter(text[i], letters[i]);

					//freetype uses absolute values (pixels) and we use relative. so by dividing the pixel by the size, we get relative values
					letters[i].setWidth((static_cast<float>(letters[i].width) / origWidth) * widthScale);
					letters[i].setHeight((static_cast<float>(letters[i].height) / origHeight) * heightScale);

					Vector<float, 2> position = { x, y };

					if (i > 0 && hasKerning) {
						Vector<unsigned int, 2> delta = font.getKerning(text[i - 1], text[i]);

						position[0] += static_cast<float>(delta[0]) / origWidth;
						position[1] += static_cast<float>(delta[1]) / origHeight;
					}

					position[1] += letters[i].getHeight();
					//i cant bear this
					position[1] -= static_cast<float>(letters[i].height - letters[i].bearingY) / origHeight;

					letters[i].setX((position[0] + (static_cast<float>(letters[i].width) / origWidth)));
					letters[i].setY(position[1]);

					letters[i].setOpacity(getOpacity());

					x += static_cast<float>(letters[i].advanceX) / origWidth;
					x += letters[i].getWidth();

					letters[i].texture = this->texture;

					addChild(letters[i]);
				}
			}

			y += static_cast<float>(font.getSize() << 1) / origHeight;

			if (x > width) {
				width = x;
			}

			height += y;

			//alignment has to be calculated after the initial pass so it knows how much screen space the text takes up
			float xAlignment, yAlignment;

			switch (horzAlign) {
				default:
				case HorizontalAlign::CENTER:
					xAlignment = ((-width / 2) + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
				case HorizontalAlign::RIGHT:
					xAlignment = ((1.0f - width) + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
				case HorizontalAlign::LEFT:
					xAlignment = (-1.0f + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
			}

			switch (vertAlign) {
				default:
				case VerticalAlign::CENTER:
					yAlignment = 0.0f;
					break;
				case VerticalAlign::BOTTOM:
					yAlignment = ((-1.0f + height / 2) - static_cast<const float>(font.getSize() >> 1) / origHeight);
					break;
				case VerticalAlign::TOP:
					yAlignment = ((1.0f - height) + static_cast<const float>(font.getSize() >> 1) / origHeight);
					break;
			}

			for (Index i = 0; i < letters.size(); ++i) {
				letters[i].translate(xAlignment, yAlignment);
			}
		}

		const ColorAttachment & Button::getTexture() const {
			return texture;
		}

		ColorAttachment & Button::getTexture() {
			makeDirty();

			return texture;
		}

		void Button::setTexture(const ColorAttachment & c) {
			if (texture != c) {
				makeDirty();

				texture = c;
			}
		}

		const ColorAttachment & Button::getHoverTexture() const {
			return hoverTexture;
		}

		ColorAttachment & Button::getHoverTexture() {
			makeDirty();

			return hoverTexture;
		}

		void Button::setHoverTexture(const ColorAttachment & c) {
			if (hoverTexture != c) {
				makeDirty();

				hoverTexture = c;
			}
		}

		const ColorAttachment & Button::getClickedTexture() const {
			return clickedTexture;
		}

		ColorAttachment & Button::getClickedTexture() {
			makeDirty();

			return clickedTexture;
		}

		void Button::setClickedTexture(const ColorAttachment & c) {
			if (clickedTexture != c) {
				makeDirty();

				clickedTexture = c;
			}
		}

		const ColorAttachment & Button::getDisabledTexture() const {
			return disabledTexture;
		}

		ColorAttachment & Button::getDisabledTexture() {
			makeDirty();

			return disabledTexture;
		}

		void Button::setDisabledTexture(const ColorAttachment & c) {
			if (disabledTexture != c) {
				makeDirty();

				disabledTexture = c;
			}
		}

		void Button::onInit() {
			if (!texture.isCreated()) {
				texture.init();
			}

			if (!hoverTexture.isCreated()) {
				hoverTexture.init();
			}

			if (!clickedTexture.isCreated()) {
				clickedTexture.init();
			}

			if (!disabledTexture.isCreated()) {
				disabledTexture.init();
			}
		}

		void Button::onUpdate() {}

		void Button::onRender() {}

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

		void Button::onHover() {
			hover();

			if (os::Input::isKeyDown(os::Input::MOUSE_LEFT)) {
				click();
			}

			if (os::Input::isKeyReleased(os::Input::MOUSE_LEFT) && isClicked()) {
				selectableProperties.setBit(Selectable::CLICKED, false);

				trigger();
			}
		}

		void Button::onClean() {}
	}//gfx
}//mc
