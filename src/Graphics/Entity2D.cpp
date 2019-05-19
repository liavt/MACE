/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
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
		Entity2D::Entity2D() : GraphicsEntity() {}

		bool Entity2D::operator==(const Entity2D& other) const {
			return GraphicsEntity::operator==(other);
		}

		bool Entity2D::operator!=(const Entity2D& other) const {
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

		void Image::setTexture(const Texture& tex) {
			if (tex != texture) {
				makeDirty();

				texture = tex;
			}
		}

		Texture& Image::getTexture() {
			makeDirty();

			return texture;
		}

		const Texture& Image::getTexture() const {
			return texture;
		}

		bool Image::operator==(const Image& other) const {
			return Entity2D::operator==(other) && texture == other.texture;
		}

		bool Image::operator!=(const Image& other) const {
			return !operator==(other);
		}

		//PROGRESS BAR

		ProgressBar::ProgressBar() noexcept : ProgressBar(0, 0, 0) {}

		ProgressBar::ProgressBar(const float minimum, const float maximum, const float prog) noexcept : minimumProgress(minimum), maximumProgress(maximum), progress(prog) {}

		void ProgressBar::setBackgroundTexture(const Texture& tex) {
			if (backgroundTexture != tex) {
				makeDirty();

				backgroundTexture = tex;
			}
		}

		Texture& ProgressBar::getBackgroundTexture() {
			makeDirty();

			return backgroundTexture;
		}

		const Texture& ProgressBar::getBackgroundTexture() const {
			return backgroundTexture;
		}

		void ProgressBar::setForegroundTexture(const Texture& tex) {
			if (foregroundTexture != tex) {
				makeDirty();

				foregroundTexture = tex;
			}
		}

		Texture& ProgressBar::getForegroundTexture() {
			makeDirty();

			return foregroundTexture;
		}

		const Texture& ProgressBar::getForegroundTexture() const {
			return foregroundTexture;
		}

		void ProgressBar::setSelectionTexture(const Texture& tex) {
			if (selectionTexture != tex) {
				makeDirty();

				selectionTexture = tex;
			}
		}

		Texture& ProgressBar::getSelectionTexture() {
			makeDirty();

			return selectionTexture;
		}

		const Texture& ProgressBar::getSelectionTexture() const {
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

		float& ProgressBar::getProgress() {
			makeDirty();

			return progress;
		}

		const float& ProgressBar::getProgress() const {
			return progress;
		}

		void ProgressBar::easeTo(const float destination, const EaseSettings settings) {
			addComponent(std::shared_ptr<Component>(new EaseComponent([](Entity * e, float progress) {
				Progressable* prog = dynamic_cast<Progressable*>(e);

#ifdef MACE_DEBUG_CHECK_NULLPTR
				if (prog == nullptr) {
					MACE__THROW(NullPointer, "Internal Error: Progressable in EaseComponent is nullptr");
				}
#endif

				prog->setProgress(progress);
			}, settings, getProgress(), destination)));
		}

		bool ProgressBar::operator==(const ProgressBar& other) const {
			return Entity2D::operator==(other) && maximumProgress == other.maximumProgress && minimumProgress == other.minimumProgress && progress == other.progress && backgroundTexture == other.backgroundTexture && foregroundTexture == other.foregroundTexture && selectionTexture == other.selectionTexture;
		}

		bool ProgressBar::operator!=(const ProgressBar& other) const {
			return !operator==(other);
		}

		void ProgressBar::onRender(Painter& p) {
			p.enableRenderFeatures(Painter::RenderFeatures::DISCARD_INVISIBLE);
			p.conditionalMaskImages(foregroundTexture, backgroundTexture, selectionTexture,
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

		Letter::Letter() {}

		const Texture& Letter::getGlyph() const {
			return glyph;
		}

		const Texture& Letter::getTexture() const {
			return texture;
		}
		const GlyphMetrics& mc::gfx::Letter::getGlpyhMetrics() const {
			return glyphMetrics;
		}

		bool Letter::operator==(const Letter & other) const {
			return Entity2D::operator==(other) && texture == other.texture && glyph == other.glyph && glyphMetrics == other.glyphMetrics;
		}

		bool Letter::operator!=(const Letter & other) const {
			return !operator==(other);
		}

		void Letter::onInit() {}

		void Letter::onUpdate() {}

		void Letter::onRender(Painter & p) {
			p.disableRenderFeatures(Painter::RenderFeatures::INHERIT_SCALE | Painter::RenderFeatures::STORE_ID);
			p.setTexture(texture, TextureSlot::FOREGROUND);
			p.setTexture(glyph, TextureSlot::BACKGROUND);
			p.drawQuad(Painter::Brush::MULTICOMPONENT_BLEND);
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

		Text::Text(const std::wstring & t, const Font & f) : TexturedEntity2D(), text(t), font(f) {}

		void mc::gfx::Text::setText(const std::string & newText) {
			setText(os::toWideString(newText));
		}

		void Text::setText(const std::wstring & newText) {
			if (text != newText) {
				makeDirty();

				text = newText;
			}
		}

		std::wstring& Text::getText() {
			makeDirty();

			return text;
		}

		const std::wstring& Text::getText() const {
			return text;
		}

		void Text::setFont(const Font & f) {
			if (font != f) {
				makeDirty();

				font = f;
			}
		}

		Font& Text::getFont() {
			makeDirty();

			return font;
		}

		const Font& Text::getFont() const {
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

		Texture& Text::getTexture() {
			makeDirty();

			return texture;
		}

		const Texture& Text::getTexture() const {
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

		void Text::onRender(Painter & p) {
			p.setForegroundColor(Colors::BLACK);
			p.fillRect();
		}

		void Text::onDestroy() {
			if(font.isCreated()){
				font.destroy();
			}
		}

		void Text::onClean() {
			if (!font.isCreated()) {
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

			const WindowModule* window = gfx::getCurrentWindow();

			const FontMetrics fontMetrics = font.getFontMetrics();

			const signed long linegap = fontMetrics.ascent - fontMetrics.descent + fontMetrics.height;

			signed long x = 0;

			std::vector<signed long> lineWidths{};
			for (Index i = 0; i < text.length(); ++i) {
				const Glyph& glyph = font.getGlyph(text[i]);
				const GlyphMetrics& glyphMetrics = glyph.metrics;

				letters[i]->glyph = glyph.texture;

				if (this->texture.isCreated()) {
					letters[i]->texture = this->texture;
				} else {
					letters[i]->texture = Colors::WHITE;
				}

				if (text[i] == '\n') {
					lineWidths.push_back(x);

					x = 0;
					letters[i]->getPainter().setOpacity(0.0f);

					letters[i]->setX(0.0f);
					letters[i]->setY(0.0f);
					letters[i]->setWidth(0.0f);
					letters[i]->setHeight(0.0f);
				} else {
					//freetype uses absolute values (pixels) and we use relative. so by dividing the pixel by the size, we get relative values
					letters[i]->setWidth(window->convertPixelsToRelativeXCoordinates(glyphMetrics.width >> 6));
					letters[i]->setHeight(window->convertPixelsToRelativeYCoordinates(glyphMetrics.height >> 6));

					Vector<signed long, 2> position = {x, -static_cast<signed long>(lineWidths.size()) * linegap};

					if (i > 0 && fontMetrics.kerning) {
						const Vector<signed long, 2> delta = font.getKerning(text[i - 1], text[i]);

						position[0] += delta[0];
						position[1] += delta[1];
					}

					position[1] += glyphMetrics.height;
					//i cant bear this
					position[1] -= (glyphMetrics.height - glyphMetrics.bearingY) << 1;
					position[1] -= linegap;
					position[1] -= fontMetrics.descent << 1;

					letters[i]->setX(window->convertPixelsToRelativeXCoordinates((position[0] + glyphMetrics.width) >> 6));
					letters[i]->setY(window->convertPixelsToRelativeYCoordinates(position[1] >> 6));

					letters[i]->getPainter().setOpacity(getPainter().getOpacity());

					x += glyphMetrics.advanceX;
					x += glyphMetrics.width;
				}
			}

			lineWidths.push_back(x);

			signed long height = static_cast<signed long>(lineWidths.size()) * linegap;
			signed long width = 0;
			for (auto lineWidth : lineWidths) {
				if (lineWidth > width) {
					width = lineWidth;
				}
			}

			// it is >> 7 instead of >> 6 because we also want to divide the total of it in half (so shift an additional bit to the right)
			const float widthPx = window->convertPixelsToRelativeXCoordinates(width >> 7);
			const float heightPx = window->convertPixelsToRelativeYCoordinates(height >> 7);

			setWidth(widthPx);
			setHeight(heightPx);

			for (auto letter : letters) {
				letter->translate(-widthPx, heightPx);
			}
		}

		const Texture& Button::getTexture() const {
			return texture;
		}

		Texture& Button::getTexture() {
			makeDirty();

			return texture;
		}

		void Button::setTexture(const Texture & c) {
			if (texture != c) {
				makeDirty();

				texture = c;
			}
		}

		const Texture& Button::getHoverTexture() const {
			return hoverTexture;
		}

		Texture& Button::getHoverTexture() {
			makeDirty();

			return hoverTexture;
		}

		void Button::setHoverTexture(const Texture & c) {
			if (hoverTexture != c) {
				makeDirty();

				hoverTexture = c;
			}
		}

		const Texture& Button::getClickedTexture() const {
			return clickedTexture;
		}

		Texture& Button::getClickedTexture() {
			makeDirty();

			return clickedTexture;
		}

		void Button::setClickedTexture(const Texture & c) {
			if (clickedTexture != c) {
				makeDirty();

				clickedTexture = c;
			}
		}

		const Texture& Button::getDisabledTexture() const {
			return disabledTexture;
		}

		Texture& Button::getDisabledTexture() {
			makeDirty();

			return disabledTexture;
		}

		void Button::setDisabledTexture(const Texture & c) {
			if (disabledTexture != c) {
				makeDirty();

				disabledTexture = c;
			}
		}

		void Button::onRender(Painter & p) {
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
