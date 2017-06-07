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
#include <MACE/Graphics/Components.h>
#include <MACE/Graphics/OGL.h>
#include <MACE/Utility/Vector.h>

#include <cmath>

namespace mc {
	namespace gfx {
		/**
		Thrown when something relate to Freetype or fonts fails
		*/
		MACE_DECLARE_ERROR(Font);

		/**
		Function defining a function used in an easing of a value, such as a translation or progress bar.
		@return The value of the ease at a specified timestamp, `t`
		@param t The current tick of the ease
		@param b The beginning progress value of the ease
		@param c The change between the beginning and destination value
		@param d The total time of the ease in frames
		@see EaseFunctions
		*/
		typedef float(*EaseFunction)(float t, const float b, const float c, const float d);

		/**
		Different easing functions commonly found in applications
		@see EaseFunction
		*/
		namespace EaseFunctions {
			//these fucntions are derived from https://github.com/jesusgollonet/ofpennereasing . Thank you!

			const EaseFunction LINEAR = [](float t, const float b, const float c, const float d) -> float {
				return b + c*(t / d);
			};

			const EaseFunction BACK_IN = [](float t, const float b, const float c, const float d) -> float {
				const float s = 1.70158f;
				const float postFix = t /= d;
				return c*(postFix)*t*((s + 1)*t - s) + b;
			};

			//shameless resturant promotion
			const EaseFunction BACK_OUT = [](float t, const float b, const float c, const float d) -> float {
				const float s = 1.70158f;
				return c*((t /= d - 1)*t*((s + 1)*t + s) + 1) + b;
			};

			const EaseFunction BACK_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				float s = 1.70158f;
				if ((t /= d / 2) < 1) return c / 2 * (t*t*(((s *= (1.525f)) + 1)*t - s)) + b;
				const float postFix = t -= 2;
				return c / 2 * ((postFix)*t*(((s *= (1.525f)) + 1)*t + s) + 2) + b;
			};

			const EaseFunction BOUNCE_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d) < (1 / 2.75f)) {
					return c*(7.5625f*t*t) + b;
				} else if (t < (2 / 2.75f)) {
					const float postFix = t -= (1.5f / 2.75f);
					return c*(7.5625f*(postFix)*t + .75f) + b;
				} else if (t < (2.5 / 2.75)) {
					const float postFix = t -= (2.25f / 2.75f);
					return c*(7.5625f*(postFix)*t + .9375f) + b;
				} else {
					const float postFix = t -= (2.625f / 2.75f);
					return c*(7.5625f*(postFix)*t + .984375f) + b;
				}
			};

			//BOUNCE_IN is defined after BOUNCE_OUT just because it requires BOUNCE_OUT to be eclared
			const EaseFunction BOUNCE_IN = [](float t, const float b, const float c, const float d) -> float {
				return c - EaseFunctions::BOUNCE_OUT(d - t, 0, c, d) + b;
			};

			const EaseFunction BOUNCE_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t < d / 2) return EaseFunctions::BOUNCE_IN(t * 2, 0, c, d) * .5f + b;
				else return EaseFunctions::BOUNCE_OUT(t * 2 - d, 0, c, d) * .5f + c*.5f + b;
			};

			const EaseFunction CIRCLE_IN = [](float t, const float b, const float c, const float d) -> float {
				return -c * (std::sqrt(1 - (t /= d)*t) - 1) + b;
			};

			const EaseFunction CIRCLE_OUT = [](float t, const float b, const float c, const float d) -> float {
				return c * std::sqrt(1 - (t = t / d - 1)*t) + b;
			};

			const EaseFunction CIRCLE_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) return -c / 2 * (std::sqrt(1 - t*t) - 1) + b;
				return c / 2 * (std::sqrt(1 - t*(t -= 2)) + 1) + b;
			};

			const EaseFunction CUBIC_IN = [](float t, const float b, const float c, const float d) -> float {
				return c*(t /= d)*t*t + b;
			};

			const EaseFunction CUBIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				return c*((t = t / d - 1)*t*t + 1) + b;
			};

			const EaseFunction CUBIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) return c / 2 * t*t*t + b;
				return c / 2 * ((t -= 2)*t*t + 2) + b;
			};

			const EaseFunction ELASTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) return b;  if ((t /= d) == 1) return b + c;
				const float p = d*.3f;
				const float a = c;
				const float s = p / 4;
				const float postFix = a*static_cast<float>(std::pow(2, 10 * (t -= 1)));
				return -static_cast<float>(postFix * static_cast<float>(std::sin((t*d - s))*(2 * static_cast<float>(math::pi())) / p)) + b;
			};

			const EaseFunction ELASTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) return b;  if ((t /= d) == 1) return b + c;
				const float p = d*.3f;
				const float a = c;
				const float s = p / 4;
				return static_cast<float>(a*std::pow(2, -10 * t) * std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p) + c + b);
			};

			const EaseFunction ELASTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) return b;  if ((t /= d / 2) == 2) return b + c;
				const float p = d*(.3f*1.5f);
				const float a = c;
				const float s = p / 4;

				if (t < 1) {
					const float postFix = a*static_cast<float>(std::pow(2, 10 * (t -= 1)));
					return -.5f*(postFix* static_cast<float>(std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p))) + b;
				}
				const float postFix = a*static_cast<float>(std::pow(2, -10 * (t -= 1)));
				return postFix * std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p)*.5f + c + b;
			};

			const EaseFunction EXPONENTIAL_IN = [](float t, const float b, const float c, const float d) -> float {
				return (t == 0) ? b : c * static_cast<float>(std::pow(2, 10 * (t / d - 1))) + b;
			};

			const EaseFunction EXPONENTIAL_OUT = [](float t, const float b, const float c, const float d) -> float {
				return (t == d) ? b + c : c * static_cast<float>(-std::pow(2, -10 * t / d) + 1) + b;
			};

			const EaseFunction EXPONENTIAL_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) return b;
				if (t == d) return b + c;
				if ((t /= d / 2) < 1) return c / 2 * static_cast<float>(std::pow(2, 10 * (t - 1))) + b;
				return c / 2 * static_cast<float>(-std::pow(2, -10 * --t) + 2) + b;
			};

			const EaseFunction QUADRATIC_IN = [](float t, const float b, const float c, const float d) -> float {
				return c*(t /= d)*t + b;
			};

			const EaseFunction QUADRATIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				return -c *(t /= d)*(t - 2) + b;
			};

			const EaseFunction QUADRATIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) return ((c / 2)*(t*t)) + b;
				return -c / 2 * (((t - 2)*(--t)) - 1) + b;
			};

			const EaseFunction QUARTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				return c*(t /= d)*t*t*t + b;
			};

			const EaseFunction QUARTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				return -c * ((t = t / d - 1)*t*t*t - 1) + b;
			};

			const EaseFunction QUARTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) return c / 2 * t*t*t*t + b;
				return -c / 2 * ((t -= 2)*t*t*t - 2) + b;
			};

			const EaseFunction QUINTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				return c*(t /= d)*t*t*t*t + b;
			};

			const EaseFunction QUINTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				return c*((t = t / d - 1)*t*t*t*t + 1) + b;
			};

			const EaseFunction QUINTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) return c / 2 * t*t*t*t*t + b;
				return c / 2 * ((t -= 2)*t*t*t*t + 2) + b;
			};

			const EaseFunction SINUSODIAL_IN = [](float t, const float b, const float c, const float d) -> float {
				return -c * std::cos(t / d * (static_cast<float>(math::pi()) / 2)) + c + b;
			};

			const EaseFunction SINUSODIAL_OUT = [](float t, const float b, const float c, const float d) -> float {
				return c * std::sin(t / d * (static_cast<float>(math::pi()) / 2)) + b;
			};

			const EaseFunction SINUSODIAL_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				return -c / 2 * (std::cos(static_cast<float>(math::pi())*t / d) - 1) + b;
			};
		}

		class Entity2D : public GraphicsEntity {
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
		class RenderProtocol<Image> : public RenderImpl {
		public:
			void init(const Size originalWidth, const Size originalHeight) override;
			void initEntity(GraphicsEntity* en) override;

			void renderEntity(os::WindowModule* win, GraphicsEntity* entity) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};//RenderProtocol<Image>

		class Image : public Entity2D {
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
		class RenderProtocol<ProgressBar> : public RenderImpl {
		public:
			void init(const Size originalWidth, const Size originalHeight) override;
			void initEntity(GraphicsEntity* en) override;

			void renderEntity(os::WindowModule* win, GraphicsEntity* entity) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};//RenderProtocol<ProgressBar>

		/**
		@todo Replace the EaseComponent with a non dynamic pointer
		*/
		class ProgressBar : public Entity2D {
			friend class RenderProtocol<ProgressBar>;
		public:
			typedef void(*ProgressBarEaseDoneCallback)(ProgressBar*);

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
			void easeTo(const float progress, const float time = 1000, const EaseFunction func = EaseFunctions::LINEAR, const ProgressBarEaseDoneCallback callback = [](ProgressBar*) {});

			bool operator==(const ProgressBar& other) const;
			bool operator!=(const ProgressBar& other) const;
		protected:
			void onInit() override final;
			void onUpdate() override final;
			void onRender() override final;
			void onClean() override final;
			void onDestroy() override final;
		private:
			float minimumProgress = 0, maximumProgress = 0, progress = 0;

			ColorAttachment backgroundTexture;
			ColorAttachment foregroundTexture;
			ColorAttachment selectionTexture;
		};//ProgressBar

		//TEXT IS UP AHEAD

		class Letter;
		class Text;

		enum class Fonts : Byte {
			CODE,
			SANS,
			SERIF,
		};

		/**
		@todo instead of using an id system add FT_Face
		@bug HorizontalAlignment::RIGHT misses the last letter in the width calculation
		*/
		class Font {
		public:
			static Font loadFont(const std::string& name);
			static Font loadFont(const char* name);
			static Font loadFontFromMemory(const unsigned char* data, long int size);
			template<std::size_t N>
			static inline Font loadFontFromMemory(const unsigned char data[N]) {
				return loadFontFromMemory(data, static_cast<long int>(N));
			}

			Font(const Font& f);
			Font(const Index id = 0, const Size h = 0);
			Font(const Fonts f, const Size height = 32);

			void destroy();

			/**
			@todo cache characters
			*/
			void getCharacter(const wchar_t character, Letter& let) const;

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
		class RenderProtocol<Letter> : public RenderImpl {
		public:
			void init(const Size originalWidth, const Size originalHeight) override;
			void initEntity(GraphicsEntity* en) override;

			void renderEntity(os::WindowModule* win, GraphicsEntity* entity) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};//RenderProtocol<Letter>

		class Letter : public Entity2D {
			friend class RenderProtocol<Letter>;
			friend class Font;
			friend class Text;
		public:
			static int getProtocol();

			Letter(const ogl::Texture2D& mask = ogl::Texture2D());
			~Letter() = default;

			/**
			@internal
			*/
			const ogl::Texture2D& getMask() const;

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
			//it is a Texture2D and not a ColorAttachment because it is supposed to be internal
			ogl::Texture2D mask;

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
		class Text : public Entity2D {
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

			const std::vector<Letter>& getLetters() const;

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
			std::vector<Letter> letters;

			std::wstring text;

			VerticalAlign vertAlign = VerticalAlign::CENTER;
			HorizontalAlign horzAlign = HorizontalAlign::CENTER;

			Font font;

			ColorAttachment texture;
		};//Text

		class Button;

		/**
		@internal
		@opengl
		*/
		template<>
		class RenderProtocol<Button> : public RenderImpl {
		public:
			void init(const Size originalWidth, const Size originalHeight) override;
			void initEntity(GraphicsEntity* en) override;

			void renderEntity(os::WindowModule* win, GraphicsEntity* entity) override;

			void destroy() override;
		private:
			SimpleQuadRenderer renderer = SimpleQuadRenderer(true);
		};//RenderProtocol<Button>

		class Button : public Selectable, public Entity2D {
			friend class RenderProtocol<Button>;
		public:
			static int getProtocol();

			const ColorAttachment& getTexture() const;
			/**
			@copydoc Button::getTexture() const
			@dirty
			*/
			ColorAttachment& getTexture();
			/**
			@dirty
			*/
			void setTexture(const ColorAttachment& c);

			const ColorAttachment& getHoverTexture() const;
			/**
			@copydoc Button::getHoverTexture() const
			@dirty
			*/
			ColorAttachment& getHoverTexture();
			/**
			@dirty
			*/
			void setHoverTexture(const ColorAttachment& c);

			const ColorAttachment& getClickedTexture() const;
			/**
			@copydoc Button::getClickedTexture() const
			@dirty
			*/
			ColorAttachment& getClickedTexture();
			/**
			@dirty
			*/
			void setClickedTexture(const ColorAttachment& c);

			const ColorAttachment& getDisabledTexture() const;
			/**
			@copydoc Button::getDisabledTexture() const
			@dirty
			*/
			ColorAttachment& getDisabledTexture();
			/**
			@dirty
			*/
			void setDisabledTexture(const ColorAttachment& c);

		protected:
			void onInit() override;
			void onUpdate() override;
			void onRender() override;
			void onDestroy() override;
			void onHover() override;
			void onClean() override;
		private:
			ColorAttachment texture;
			ColorAttachment hoverTexture;
			ColorAttachment clickedTexture;
			ColorAttachment disabledTexture;
		};
	}//gfx
}//mc

#endif
