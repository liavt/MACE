/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once

#ifndef MACE__GRAPHICS_FONT_H
#define MACE__GRAPHICS_FONT_H

#include <MACE/Core/Interfaces.h>
#include <MACE/Graphics/Context.h>
#include <memory>
#include <unordered_map>

namespace mc {
	namespace gfx {
		/**
		Thrown when something relating to fonts fails
		*/
		MACE__DECLARE_ERROR(Font);

		using FontSize = unsigned int;

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
			bool kerning = false;

			bool operator ==(const FontMetrics& other) const;
			bool operator !=(const FontMetrics& other) const;
		};

		class MACE_NOVTABLE FontImpl {
			friend class Font;
		protected:
			virtual ~FontImpl() = default;

			/**
			@rendercontext
			*/
			virtual void fillGlyph(Glyph& glyph, const wchar_t character) const = 0;

			/**
			@rendercontext
			*/
			virtual void calculateMetricsForSize(const FontSize size) = 0;

			/**
			@rendercontext
			*/
			virtual FontMetrics getFontMetrics() = 0;

			/**
			@rendercontext
			*/
			virtual Vector<signed long, 2> getKerning(const wchar_t prev, const wchar_t current) const = 0;
		};

		enum class Fonts: Byte {
			CODE,
			SANS,
			SERIF,
		};

		enum class FontLoadType: Byte {
			FILE, MEMORY
		};

		struct FontDesc {
			FontLoadType loadType = FontLoadType::FILE;
			union {
				CString path;
				struct {
					const unsigned char* data;
					Size size;
				} memory;
			} input;
		};

		class Font {
		public:
			static Font loadFont(const std::string& name, const FontSize size = 12);
			static Font loadFont(CString name, const FontSize size = 12);
			static Font loadFontFromMemory(const unsigned char* data, Size dataSize, const FontSize size = 12);
			template<Size N>
			static inline Font loadFontFromMemory(const unsigned char data[N], const FontSize size = 12) {
				return loadFontFromMemory(data, N);
			}

			Font() noexcept;
			/**
			@rendercontext
			*/
			Font(const FontDesc& desc, const FontSize size = 12);
			Font(const Font& other);
			Font(const Font& other, const FontSize size);
			Font(const Fonts font, const FontSize soze = 12);
			~Font() = default;

			/**
			@rendercontext
			*/
			void init(const FontDesc& desc);
			/**
			@rendercontext
			*/
			void destroy();

			/**
			@rendercontext
			*/
			Glyph& getGlyph(const wchar_t character);

			/**
			@rendercontext
			*/
			FontMetrics getFontMetrics();

			/**
			@rendercontext
			*/
			Vector<signed long, 2> getKerning(const wchar_t prev, const wchar_t current);

			MACE_GETTER_SETTER_DEC(Size, FontSize);

			bool isCreated() const;

			bool operator==(const Font& other) const;
			bool operator!=(const Font& other) const;
		private:
			std::shared_ptr<FontImpl> impl;

			std::unordered_map<FontSize, std::unordered_map<wchar_t, Glyph>> glyphs{};

			FontSize size = 12, lastCalculatedSize = 0;

			/**
			@rendercontext
			*/
			void checkHeight();
		};
	}
}

#endif//MACE__GRAPHICS_FONT_H