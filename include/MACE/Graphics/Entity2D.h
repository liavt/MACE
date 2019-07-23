/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once

#ifndef MACE__GRAPHICS_ENTITY2D_H
#define MACE__GRAPHICS_ENTITY2D_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Font.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Components.h>
#include <MACE/Utility/Vector.h>

namespace mc {
	namespace gfx {
		class MACE_NOVTABLE Selectable {
		public:
			virtual ~Selectable() noexcept = default;

			bool isClicked() const;
			bool isDisabled() const;
			bool isHovered() const;

			void click();

			void disable();
			void enable();

			void trigger();
		protected:
			enum SelectableProperty: Byte {
				CLICKED = 0x01,
				DISABLED = 0x02,
				HOVERED = 0x04
			};

			Byte selectableProperties = 0;

			virtual void onClick();

			virtual void onEnable();
			virtual void onDisable();

			virtual void onTrigger();

			void doHover();
		};

		enum class ImageTextureSlots: TextureSlot {
			IMAGE
		};

		class Image: public TexturedEntity<ImageTextureSlots, ImageTextureSlots::IMAGE> {
		public:
			Image() noexcept = default;
			Image(const Texture& col);

			bool operator==(const Image& other) const;
			bool operator!=(const Image& other) const;
		protected:
			void onRender(Painter& p) override final;
		};

		enum class SimpleProgressBarTextureSlots: TextureSlot {
			FOREGROUND,
			BACKGROUND,
			MASK
		};

		/**
		*/
		class SimpleProgressBar: public TexturedEntity< SimpleProgressBarTextureSlots, SimpleProgressBarTextureSlots::FOREGROUND>, public Progressable {
		public:
			SimpleProgressBar() noexcept;
			SimpleProgressBar(const Progress minimum, const Progress maximum, const Progress progress = 0) noexcept;
			virtual ~SimpleProgressBar() = default;

			/**
			@dirty
			*/
			void setMinimum(const Progress tex);

			Progress getMinimum() override;
			const Progress getMinimum() const override;


			/**
			@dirty
			*/
			void setMaximum(const Progress tex);

			Progress getMaximum() override;
			const Progress getMaximum() const override;


			/**
			@dirty
			*/
			void setProgress(const Progress tex) override;

			/**
			@dirty
			*/
			Progress& getProgress() override;
			const Progress& getProgress() const override;

			void easeTo(const Progress progress, const EaseSettings settings);

			bool operator==(const SimpleProgressBar& other) const;
			bool operator!=(const SimpleProgressBar& other) const;
		protected:
			virtual void onRender(Painter& p) override;
			void onDestroy() override final;

			Progress minimumProgress = 0, maximumProgress = 0, progress = 0;
		};//SimpleProgressBar

		using SimpleSliderTextureSlots = SimpleProgressBarTextureSlots;

		class SimpleSlider: public SimpleProgressBar, public Selectable {
		public:
			SimpleSlider() noexcept;
			SimpleSlider(const Progress minimum, const Progress maximum, const Progress progress = 0) noexcept;
		private:
			void onRender(Painter& p) override;
			void onClick() override;
			void onHover() override;
		};//SimpleSlider

		//TEXT IS UP AHEAD

		class Letter;
		class Text;

		class Letter: public GraphicsEntity {
			friend class Font;
			friend class Text;
		public:
			Letter();
			~Letter() = default;

			const Texture& getTexture() const;

			const GlyphMetrics& getGlpyhMetrics() const;

			bool operator==(const Letter& other) const;
			bool operator!=(const Letter& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender(Painter& p) override final;
			void onDestroy() override final;
			void onClean() override final;
		private:
			Texture glyph = Texture();

			Texture texture = Texture();

			GlyphMetrics glyphMetrics{};

			/**
			@internal
			*/
			const Texture& getGlyph() const;
		};//Letter

		enum class TextTextureSlots: TextureSlot {
			TEXTURE
		};

		/**
		@bug newline with vertical align doesnt really work
		*/
		class Text: public TexturedEntity<TextTextureSlots, TextTextureSlots::TEXTURE> {
		public:
			Text() noexcept;
			/**
			@rendercontext
			*/
			Text(const std::string& t, const Font& f = Fonts::CODE);
			/**
			@rendercontext
			*/
			Text(const std::wstring& t, const Font& f = Fonts::CODE);
			~Text() = default;

			/**
			@dirty
			*/
			void setText(const std::string& newText);
			/**
			@dirty
			*/
			void setText(const std::wstring& newText);
			/**
			@dirty
			*/
			std::wstring& getText();
			const std::wstring& getText() const;

			/**
			@dirty
			*/
			void setFont(const Font& f);
			/**
			@dirty
			*/
			Font& getFont();
			const Font& getFont() const;

			const std::vector<std::shared_ptr<Letter>>& getLetters() const;

			bool operator==(const Text& other) const;
			bool operator!=(const Text& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender(Painter& p) override final;
			void onDestroy() override final;
			void onClean() override final;
		private:
			std::vector<std::shared_ptr<Letter>> letters;

			std::wstring text;

			Font font;
		};//Text
	}//gfx
}//mc

#endif
