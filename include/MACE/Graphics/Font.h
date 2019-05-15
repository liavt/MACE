/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once

#ifndef MACE__GRAPHICS_FONT_H
#define MACE__GRAPHICS_FONT_H

#include <MACE/Core/Interfaces.h>
#include <memory>

namespace mc {
	namespace gfx {
		/**
		Thrown when something relate to Freetype or fonts fails
		*/
		MACE__DECLARE_ERROR(Font);

		class Texture;

		/**
		All units are in fractional pixels (26.6)
		*/
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

		struct Glyph {
			GlyphMetrics metrics;
			Texture texture;

			bool operator == (const Glyph& other) const;
			bool operator != (const Glyph& other) const;
		};

		/**
		All units are in fractional pixels (26.6)
		*/
		struct FontMetrics {
			signed long descent = 0;
			signed long ascent = 0;
			signed long height = 0;

			bool operator ==(const FontMetrics& other) const;
			bool operator !=(const FontMetrics& other) const;
		};

		class FontImpl: public Initializable {
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual signed long getDesc

			virtual bool hasKerning() const = 0;

			virtual bool isCreated() const = 0;
		};

		enum class Fonts: Byte {
			CODE,
			SANS,
			SERIF,
		};

		class Font2: public Initializable {
		public:
			Font2();
			Font2(const std::shared_ptr<FontImpl> mod);
			Font2(const Font2& other);
			~Font2() = default;

			/**
			@rendercontext
			*/
			void init() override;
			/**
			@rendercontext
			*/
			void destroy() override;

			/**
			@rendercontext
			*/
			Glyph getGlyph(const wchar_t character) const;

			/**
			@rendercontext
			*/
			void calculateMetricsForHeight(const unsigned int height);

			bool hasKerning() const;

			/**
			@rendercontext
			*/
			FontMetrics getFontMetrics();

			/**
			@rendercontext
			*/
			Vector<signed long, 2> getKerning(const wchar_t prev, const wchar_t current) const;

			/**
			@rendercontext
			*/
			bool isCreated() const;

			bool operator==(const Font2& other) const;
			bool operator!=(const Font2& other) const;
		private:
			std::shared_ptr<FontImpl> impl;
		};
	}
}

#endif//MACE__GRAPHICS_FONT_H