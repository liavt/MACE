/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#ifndef MACE_GRAPHICS_ENTITY2D_H
#define MACE_GRAPHICS_ENTITY2D_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Utility/Vector.h>

namespace mc {
	namespace gfx {

		enum class VerticalAlign: Byte {
			TOP,
			CENTER,
			BOTTOM
		};

		enum class HorizontalAlign: Byte {
			LEFT,
			CENTER,
			RIGHT
		};

		enum class EaseFunction: Byte {
			//Starts fast, decelerates at end
			SINUSOIDAL,
			//Starts slow, accelerates at end
			COSINE,
			CUBIC,
			QUADRATIC,
			QUARTIC,
			QUINTIC,
			//Circular acceleration from zero
			SQUARE_ROOT,
			//Even faster acceleration at start, and slow down at end
			CUBE_ROOT,
			LINEAR
		};

		class Entity2D: public GraphicsEntity {
		public:
			Entity2D();

			/**
			@copydoc Entity2D::getBuffer() const
			@dirty
			*/
			ogl::UniformBuffer& getBuffer();
			/**
			@internal
			*/
			const ogl::UniformBuffer& getBuffer() const;
			/**
			@internal
			@dirty
			*/
			void setBuffer(const ogl::UniformBuffer& newBuffer);

			bool operator==(const Entity2D& other) const;
			bool operator!=(const Entity2D& other) const;
		protected:
			ogl::UniformBuffer buffer = ogl::UniformBuffer();
		};//Entity2D

		class Image;

		/**
		@internal
		@opengl
		*/
		template<>
		class RenderProtocol<Image>: public RenderImpl {
		public:
			void init(const Size originalWidth, const Size originalHeight) override;
			void initEntity(GraphicsEntity* en) override;

			void renderEntity(os::WindowModule* win, GraphicsEntity* entity) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};//RenderProtocol<Image>

		class Image: public Entity2D {
			friend class RenderProtocol<Image>;
		public:
			static int getProtocol();

			Image() noexcept;
			Image(const ColorAttachment& col);
			~Image() = default;

			/**
			@dirty
			*/
			void setTexture(const ColorAttachment& tex);
			/**
			@copydoc Image::getTexture() const
			@dirty
			*/
			ColorAttachment& getTexture();
			const ColorAttachment& getTexture() const;

			bool operator==(const Image& other) const;
			bool operator!=(const Image& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender() override final;
			void onDestroy() override final;
			void onClean() override final;
		private:
			ColorAttachment texture;
		};//Image

		class ProgressBar;

		/**
		@internal
		@opengl
		*/
		template<>
		class RenderProtocol<ProgressBar>: public RenderImpl {
		public:
			void init(const Size originalWidth, const Size originalHeight) override;
			void initEntity(GraphicsEntity* en) override;

			void renderEntity(os::WindowModule* win, GraphicsEntity* entity) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};//RenderProtocol<ProgressBar>

		/**
		@bug some weird artifacting because of anti-aliasing
		*/
		class ProgressBar: public Entity2D {
			friend class RenderProtocol<ProgressBar>;
		public:
			typedef void(*EaseDoneCallback)(ProgressBar*);

			static int getProtocol();

			ProgressBar() noexcept;
			ProgressBar(const float minimum, const float maximum, const float progress = 0) noexcept;
			~ProgressBar() = default;

			/**
			@dirty
			*/
			void setBackgroundTexture(const ColorAttachment& tex);
			/**
			@dirty
			*/
			ColorAttachment& getBackgroundTexture();
			const ColorAttachment& getBackgroundTexture() const;

			/**
			@dirty
			*/
			void setForegroundTexture(const ColorAttachment& tex);
			/**
			@dirty
			*/
			ColorAttachment& getForegroundTexture();
			const ColorAttachment& getForegroundTexture() const;

			/**
			@dirty
			*/
			void setSelectionTexture(const ColorAttachment& tex);
			/**
			@dirty
			*/
			ColorAttachment& getSelectionTexture();
			const ColorAttachment& getSelectionTexture() const;

			/**
			@dirty
			*/
			void setMinimum(const float tex);
			/**
			@dirty
			*/
			float& getMinimum();
			const float& getMinimum() const;


			/**
			@dirty
			*/
			void setMaximum(const float tex);
			/**
			@dirty
			*/
			float& getMaximum();
			const float& getMaximum() const;


			/**
			@dirty
			*/
			void setProgress(const float tex);
			/**
			@dirty
			*/
			void addProgress(const float prog);

			/**
			@dirty
			*/
			float& getProgress();
			const float& getProgress() const;

			/**
			@dirty
			*/
			void easeTo(const float progress, const float time = 1000, const EaseFunction func = EaseFunction::SINUSOIDAL, const EaseDoneCallback callback = [] (ProgressBar*) {});

			bool operator==(const ProgressBar& other) const;
			bool operator!=(const ProgressBar& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender() override final;
			void onClean() override final;
			void onDestroy() override final;
		private:
			float max = 0, min = 0, progress = 0;

			ColorAttachment backgroundTexture;
			ColorAttachment foregroundTexture;
			ColorAttachment selectionTexture;
		};//ProgressBar

		//TEXT IS UP AHEAD

		class Letter;
		class Text;

		/**
		@bug non-monospaced fonts dont have correct spacing
		@todo Make a default font system so people dont have to create new font classes each time they use text
		@todo instead of using an id system add FT_Face
		*/
		class Font {
		public:
			static Font loadFont(const std::string& name);
			static Font loadFont(const char* name);

			Font(const Font& f);
			Font(const Index id = 0, const Size h = 0);

			void destroy();

			/**
			@todo find a way to dynamically allocate letters without passing in a pointer
			@todo cache characters
			*/
			void getCharacter(const wchar_t character, Letter* let) const;

			void setSize(const Size height);
			Size& getSize();
			const Size& getSize() const;

			bool hasKerning() const;

			Vector<unsigned int, 2> getKerning(const wchar_t prev, const wchar_t current) const;

			Index getID() const;

			bool operator==(const Font& other) const;
			bool operator!=(const Font& other) const;
		private:
			Index id;
			Size height;
		};//Font

		/**
		@internal
		@opengl
		*/
		template<>
		class RenderProtocol<Letter>: public RenderImpl {
		public:
			void init(const Size originalWidth, const Size originalHeight) override;
			void initEntity(GraphicsEntity* en) override;

			void renderEntity(os::WindowModule* win, GraphicsEntity* entity) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};//RenderProtocol<Letter>

		class Letter: public Entity2D {
			friend class RenderProtocol<Letter>;
			friend class Font;
			friend class Text;
		public:
			static int getProtocol();

			Letter(const ogl::Texture& mask = ogl::Texture());
			~Letter() = default;

			const ogl::Texture& getMask() const;

			const ColorAttachment& getTexture() const;

			const Size& getCharacterWidth() const;
			const Size& getCharacterHeight() const;

			const Index& getXBearing() const;
			const Index& getYBearing() const;

			const Index& getXAdvance() const;
			const Index& getYAdvance() const;

			bool operator==(const Letter& other) const;
			bool operator!=(const Letter& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender() override final;
			void onDestroy() override final;
			void onClean() override final;
		private:
			ogl::Texture mask;

			ColorAttachment texture = ColorAttachment();

			Size width;
			Size height;
			Index bearingX;
			Index bearingY;
			Index advanceX;
			Index advanceY;
		};//Letter

		/**
		@bug newline with vertical align doesnt really work
		*/
		class Text: public Entity2D {
		public:
			Text(const std::string& t, const Font& f = Font());
			Text(const std::wstring& t = L"", const Font& f = Font());
			~Text() = default;

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

			const Group& getLetters() const;

			/**
			@dirty
			*/
			void setVerticalAlign(const VerticalAlign align);
			const VerticalAlign getVerticalAlign() const;

			/**
			@dirty
			*/
			void setHorizontalAlign(HorizontalAlign align);
			const HorizontalAlign getHorizontalAlign() const;

			/**
			@dirty
			*/
			void setTexture(const ColorAttachment& tex);
			/**
			@copydoc Image::getTexture() const
			@dirty
			*/
			ColorAttachment& getTexture();
			const ColorAttachment& getTexture() const;

			bool operator==(const Text& other) const;
			bool operator!=(const Text& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender() override final;
			void onDestroy() override final;
			void onClean() override final;
		private:
			Group letters = Group();

			std::wstring text;

			VerticalAlign vertAlign = VerticalAlign::CENTER;
			HorizontalAlign horzAlign = HorizontalAlign::CENTER;

			Font font;

			ColorAttachment texture;
		};//Text
	}//gfx
}//mc

#endif
