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

		class MACE_NOVTABLE Entity2D: public GraphicsEntity {
		public:
			Entity2D();
			virtual ~Entity2D() noexcept override = default;

			bool operator==(const Entity2D& other) const;
			bool operator!=(const Entity2D& other) const;
		};//Entity2D

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

		class MACE_NOVTABLE TexturedEntity2D: public Entity2D {
		public:
			virtual ~TexturedEntity2D() noexcept = default;

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
		class SimpleProgressBar: public Entity2D, public Progressable {
		public:
			SimpleProgressBar() noexcept;
			SimpleProgressBar(const Progress minimum, const Progress maximum, const Progress progress = 0) noexcept;
			virtual ~SimpleProgressBar() = default;

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

			Texture backgroundTexture;
			Texture foregroundTexture;
			Texture selectionTexture;
		};//SimpleProgressBar

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

		class SimpleButton: public TexturedEntity2D, public Selectable {
		public:
			void setTexture(const Texture& c) override;
			const Texture& getTexture() const override;
			Texture& getTexture() override;

			const Texture& getHoverTexture() const;
			/**
			@copydoc SimpleButton::getHoverTexture() const
			@dirty
			*/
			Texture& getHoverTexture();
			/**
			@dirty
			*/
			void setHoverTexture(const Texture& c);

			const Texture& getClickedTexture() const;
			/**
			@copydoc SimpleButton::getClickedTexture() const
			@dirty
			*/
			Texture& getClickedTexture();
			/**
			@dirty
			*/
			void setClickedTexture(const Texture& c);

			const Texture& getDisabledTexture() const;
			/**
			@copydoc SimpleButton::getDisabledTexture() const
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
