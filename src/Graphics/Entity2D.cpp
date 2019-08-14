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

		Image::Image(const Texture& tex){
			setTexture(tex);
		}

		void Image::onRender(Painter& p) {
			p.drawImage(getTexture());
		}

		bool Image::operator==(const Image& other) const {
			return TexturedEntity::operator==(other);
		}

		bool Image::operator!=(const Image& other) const {
			return !operator==(other);
		}

		//PROGRESS BAR

		SimpleProgressBar::SimpleProgressBar() noexcept : SimpleProgressBar(0, 0, 0) {}

		SimpleProgressBar::SimpleProgressBar(const Progress minimum, const Progress maximum, const Progress prog) noexcept : minimumProgress(minimum), maximumProgress(maximum), progress(prog) {}


		void SimpleProgressBar::setMinimum(const Progress minimum) {
			if (minimumProgress != minimum) {
				makeDirty();

				minimumProgress = minimum;
			}
		}

		Progress SimpleProgressBar::getMinimum() {
			return minimumProgress;
		}

		const Progress SimpleProgressBar::getMinimum() const {
			return minimumProgress;
		}

		void SimpleProgressBar::setMaximum(const Progress maximum) {
			if (maximumProgress != maximum) {
				makeDirty();

				maximumProgress = maximum;
			}
		}

		Progress SimpleProgressBar::getMaximum() {
			return maximumProgress;
		}

		const Progress SimpleProgressBar::getMaximum() const {
			return maximumProgress;
		}

		void SimpleProgressBar::setProgress(const Progress prog) {
			if (progress != prog) {
				makeDirty();

				progress = prog;
			}
		}

		Progress& SimpleProgressBar::getProgress() {
			makeDirty();

			return progress;
		}

		const Progress& SimpleProgressBar::getProgress() const {
			return progress;
		}

		void SimpleProgressBar::easeTo(const Progress destination, const EaseSettings settings) {
			addComponent(ComponentPtr<EaseComponent>(new EaseComponent(this, settings, getProgress(), destination)));
		}

		bool SimpleProgressBar::operator==(const SimpleProgressBar& other) const {
			return GraphicsEntity::operator==(other) && maximumProgress == other.maximumProgress && minimumProgress == other.minimumProgress;
		}

		bool SimpleProgressBar::operator!=(const SimpleProgressBar& other) const {
			return !operator==(other);
		}

		void SimpleProgressBar::onRender(Painter& p) {
			p.enableRenderFeatures(Painter::RenderFeatures::FILTER | Painter::RenderFeatures::DISCARD_INVISIBLE);
			p.conditionalMaskImages(getTexture<SimpleProgressBarTextureSlots::FOREGROUND>(),
									getTexture<SimpleProgressBarTextureSlots::BACKGROUND>(),
									getTexture<SimpleProgressBarTextureSlots::MASK>(),
									minimumProgress / maximumProgress,
									(progress - minimumProgress) / (maximumProgress - minimumProgress));
		}

		void SimpleProgressBar::onDestroy() {}


		SimpleSlider::SimpleSlider() noexcept : SimpleProgressBar() {}

		SimpleSlider::SimpleSlider(const Progress minimum, const Progress maximum, const Progress progress) noexcept : SimpleProgressBar(minimum, maximum, progress) {}

		void SimpleSlider::onRender(Painter& p) {
			SimpleProgressBar::onRender(p);

			p.setTarget(FrameBufferTarget::DATA);
			p.drawImage(getTexture<SimpleProgressBarTextureSlots::MASK>());
		}

		void SimpleSlider::onClick() {
			const auto renderer = getRoot()->getComponent<Renderer>();

			const int mouseX = gfx::Input::getMouseX(), mouseY = gfx::Input::getMouseY();
			if (mouseX >= 0 && mouseY >= 0) {
				const Color pixel = renderer->getPixelAt(static_cast<unsigned int>(mouseX), static_cast<unsigned int>(mouseY), FrameBufferTarget::DATA);

				setProgress(minimumProgress + (pixel.r * (maximumProgress - minimumProgress)));
			}
		}

		void SimpleSlider::onHover() {
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

		bool Letter::operator==(const Letter& other) const {
			return GraphicsEntity::operator==(other) && texture == other.texture && glyph == other.glyph && glyphMetrics == other.glyphMetrics;
		}

		bool Letter::operator!=(const Letter& other) const {
			return !operator==(other);
		}

		void Letter::onInit() {}

		void Letter::onUpdate() {}

		void Letter::onRender(Painter& p) {
			p.disableRenderFeatures(Painter::RenderFeatures::DISCARD_INVISIBLE | Painter::RenderFeatures::FILTER | Painter::RenderFeatures::STORE_ID);
			p.setTexture(texture, PainterTextureSlots::FOREGROUND);
			p.setTexture(glyph, PainterTextureSlots::BACKGROUND);
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

		Text::Text() noexcept : TexturedEntity(), text(), font() {}

		Text::Text(const std::string& s, const Font& f) : Text(os::toWideString(s), f) {}

		Text::Text(const std::wstring& t, const Font& f) : TexturedEntity(), text(t), font(f) {}

		void mc::gfx::Text::setText(const std::string& newText) {
			setText(os::toWideString(newText));
		}

		void Text::setText(const std::wstring& newText) {
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

		void Text::setFont(const Font& f) {
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

		bool Text::operator==(const Text& other) const {
			return TexturedEntity::operator==(other) && letters == other.letters && text == other.text;
		}

		bool Text::operator!=(const Text& other) const {
			return !operator==(other);
		}

		void Text::onInit() {
			addComponent(ComponentPtr<UninheritScaleComponent>(new UninheritScaleComponent()));
		}

		void Text::onUpdate() {}

		void Text::onRender(Painter&) {}

		void Text::onDestroy() {
			if (font.isCreated()) {
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
				letter->addComponent(ComponentPtr<UninheritScaleComponent>(new UninheritScaleComponent()));
				letters.push_back(letter);
				addChild(letter);
			}

			const FontMetrics fontMetrics = font.getFontMetrics();

			const RelativeTranslation linegap = fontMetrics.ascent - fontMetrics.descent + fontMetrics.height;

			RelativeTranslation x = 0;

			std::vector<RelativeScale> lineWidths{};
			for (Index i = 0; i < text.length(); ++i) {
				const Glyph& glyph = font.getGlyph(text[i]);
				const GlyphMetrics& glyphMetrics = glyph.metrics;

				letters[i]->glyph = glyph.texture;

				if (getTexture().isCreated()) {
					letters[i]->texture = getTexture();
				} else {
					letters[i]->texture = getRoot()->getComponent<GraphicsContextComponent>()->getSolidColor(Colors::WHITE);
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
					letters[i]->setWidth(glyphMetrics.width);
					letters[i]->setHeight(glyphMetrics.height);

					Vector<RelativeTranslation, 2> position = {x, -static_cast<RelativeTranslation>(lineWidths.size()) * linegap};

					if (i > 0 && fontMetrics.kerning) {
						const Vector<RelativeTranslation, 2> delta = font.getKerning(text[i - 1], text[i]);

						position[0] += delta[0];
						position[1] += delta[1];
					}

					position[1] += glyphMetrics.height;
					//i cant bear this
					position[1] -= (glyphMetrics.height - glyphMetrics.bearingY) * 2.0f;
					position[1] -= linegap;
					position[1] -= fontMetrics.descent * 2.0f;

					letters[i]->setX(position[0] + glyphMetrics.width);
					letters[i]->setY(position[1]);

					letters[i]->getPainter().setOpacity(getPainter().getOpacity());

					x += glyphMetrics.advanceX;
					x += glyphMetrics.width;
				}
			}

			lineWidths.push_back(x);

			RelativeScale height = static_cast<RelativeScale>(lineWidths.size()) * linegap;
			RelativeScale width = 0;
			for (auto lineWidth : lineWidths) {
				if (lineWidth > width) {
					width = lineWidth;
				}
			}

			width /= 2.0f;
			height /= 2.0f;

			setWidth(width);
			setHeight(height);

			for (auto letter : letters) {
				letter->translate(-width, height);
			}
		}
	}//gfx
}//mc
