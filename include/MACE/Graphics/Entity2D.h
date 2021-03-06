/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once

#ifndef MACE__GRAPHICS_ENTITY2D_H
#define MACE__GRAPHICS_ENTITY2D_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Graphics/Components.h>
#include <MACE/Utility/Vector.h>

namespace mc {
	namespace gfx {
		/**
		Thrown when something relate to Freetype or fonts fails
		*/
		MACE__DECLARE_ERROR(Font);

		class Entity2D: public GraphicsEntity {
		public:
			Entity2D();

			bool operator==(const Entity2D& other) const;
			bool operator!=(const Entity2D& other) const;
		};//Entity2D

		class Selectable {
		public:
			virtual ~Selectable() = default;

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

		class TexturedEntity2D: public Entity2D {
		public:
			virtual ~TexturedEntity2D() = default;

			/**
			@dirty
			*/
			virtual void setTexture(const Texture& tex) = 0;
			/**
			@copydoc Texturable::getTexture() const
			@dirty
			*/
			virtual Texture& getTexture() = 0;
			/**

			*/
			virtual const Texture& getTexture() const = 0;
		};

		class Image: public TexturedEntity2D {
		public:
			Image() noexcept;
			Image(const Texture& col);
			~Image() = default;

			void setTexture(const Texture& tex) override;
			Texture& getTexture() override;
			const Texture& getTexture() const override;

			bool operator==(const Image& other) const;
			bool operator!=(const Image& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender(Painter& p) override final;
			void onDestroy() override final;
			void onClean() override final;
		private:
			Texture texture;
		};//Image

		/**
		*/
		class ProgressBar: public Entity2D, public Progressable {
		public:
			ProgressBar() noexcept;
			ProgressBar(const float minimum, const float maximum, const float progress = 0) noexcept;
			virtual ~ProgressBar() = default;

			/**
			@dirty
			*/
			void setBackgroundTexture(const Texture& tex);
			/**
			@dirty
			*/
			Texture& getBackgroundTexture();
			const Texture& getBackgroundTexture() const;

			/**
			@dirty
			*/
			void setForegroundTexture(const Texture& tex);
			/**
			@dirty
			*/
			Texture& getForegroundTexture();
			const Texture& getForegroundTexture() const;

			/**
			@dirty
			*/
			void setSelectionTexture(const Texture& tex);
			/**
			@dirty
			*/
			Texture& getSelectionTexture();
			const Texture& getSelectionTexture() const;

			/**
			@dirty
			*/
			void setMinimum(const float tex);

			float getMinimum() override;
			const float getMinimum() const override;


			/**
			@dirty
			*/
			void setMaximum(const float tex);

			float getMaximum() override;
			const float getMaximum() const override;


			/**
			@dirty
			*/
			void setProgress(const float tex) override;

			/**
			@dirty
			*/
			float& getProgress() override;
			const float& getProgress() const override;

			void easeTo(const float progress, const EaseSettings settings);

			bool operator==(const ProgressBar& other) const;
			bool operator!=(const ProgressBar& other) const;
		protected:
			virtual void onRender(Painter& p) override;
			void onDestroy() override final;

			float minimumProgress = 0, maximumProgress = 0, progress = 0;

			Texture backgroundTexture;
			Texture foregroundTexture;
			Texture selectionTexture;
		};//ProgressBar

		class Slider: public ProgressBar, public Selectable {
		public:
			Slider() noexcept;
			Slider(const float minimum, const float maximum, const float progress = 0) noexcept;
		private:
			void onRender(Painter& p) override;
			void onClick() override;
			void onHover() override;
		};//Slider

		//TEXT IS UP AHEAD

		class Letter;
		class Text;

		enum class Fonts: Byte {
			CODE,
			SANS,
			SERIF,
		};

		/**
		@todo instead of using an id system add FT_Face
		@todo get rid of FT_Library global constant
		@bug HorizontalAlignment::RIGHT misses the last letter in the width calculation
		*/
		class Font {
			friend class Text;
		public:
			static Font loadFont(const std::string& name, unsigned int size = 12);
			static Font loadFont(const char* name, unsigned int size = 12);
			static Font loadFontFromMemory(const unsigned char* data, unsigned long int dataSize, unsigned int size = 12);
			template<std::size_t N>
			static inline Font loadFontFromMemory(const unsigned char data[N], unsigned int size = 12) {
				return loadFontFromMemory(data, static_cast<long int>(N));
			}

			Font(const Font& f);
			Font(const Index id = 0, const unsigned int h = 0);
			Font(const Fonts f, const unsigned int height = 12);

			void destroy();

			/**
			@todo cache characters
			*/
			void getCharacter(const wchar_t character, std::shared_ptr<Letter> let) const;

			bool hasKerning() const;

			signed long getDescent() const;

			signed long getAscent() const;

			/**
			Returns the scaled height in fractional pixels (26.6px)
			*/
			signed long getHeight() const;

			Vector<signed long, 2> getKerning(const wchar_t prev, const wchar_t current) const;

			Index getID() const;

			bool operator==(const Font& other) const;
			bool operator!=(const Font& other) const;
		private:
			Index id;
			unsigned int height;

			void calculateMetrics() const;
		};//Font

		struct GlyphMetrics {
			signed long width = 0;
			signed long height = 0;
			signed long bearingX = 0;
			signed long bearingY = 0;
			signed long advanceX = 0;
			signed long advanceY = 0;

			bool operator ==(const GlyphMetrics& other) const;
			bool operator !=(const GlyphMetrics& other) const;
		};

		class Letter: public Entity2D {
			friend class Font;
			friend class Text;
		public:
			Letter();
			~Letter() = default;

			/**
			@internal
			*/
			const Texture& getGlyph() const;

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
		};//Letter

		/**
		@bug newline with vertical align doesnt really work
		*/
		class Text: public TexturedEntity2D {
		public:
			Text(const std::string& t, const Font& f = Font());
			Text(const std::wstring& t = L"", const Font& f = Font());
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

			void setTexture(const Texture& tex) override;
			Texture& getTexture() override;
			const Texture& getTexture() const override;

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

			Texture texture;
		};//Text

		class Button: public TexturedEntity2D, public Selectable {
		public:
			void setTexture(const Texture& c) override;
			const Texture& getTexture() const override;
			Texture& getTexture() override;

			const Texture& getHoverTexture() const;
			/**
			@copydoc Button::getHoverTexture() const
			@dirty
			*/
			Texture& getHoverTexture();
			/**
			@dirty
			*/
			void setHoverTexture(const Texture& c);

			const Texture& getClickedTexture() const;
			/**
			@copydoc Button::getClickedTexture() const
			@dirty
			*/
			Texture& getClickedTexture();
			/**
			@dirty
			*/
			void setClickedTexture(const Texture& c);

			const Texture& getDisabledTexture() const;
			/**
			@copydoc Button::getDisabledTexture() const
			@dirty
			*/
			Texture& getDisabledTexture();
			/**
			@dirty
			*/
			void setDisabledTexture(const Texture& c);

		protected:
			void onRender(Painter& p) override;
			void onHover() override;
			void onDestroy() override;
		private:
			Texture texture;
			Texture hoverTexture;
			Texture clickedTexture;
			Texture disabledTexture;
		};
	}//gfx
}//mc

#endif
