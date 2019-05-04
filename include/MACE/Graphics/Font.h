/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_FONT_H
#define MACE__GRAPHICS_FONT_H

#include <MACE/Core/Interfaces.h>
#include <MACE/Utility/Vector.h>

#include <memory>

namespace mc {
	namespace gfx {
		class Letter;

		enum class Fonts : Byte {
			CODE,
			SANS,
			SERIF,
		};

		class MACE_NOVTABLE FontImpl : public Initializable {
			friend class Font;
		public:
			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual bool hasKerning() const = 0;

			virtual Vector<unsigned int, 2> getKerning(const wchar_t prev, const wchar_t current) const = 0;

			virtual bool operator==(const FontImpl& other) const;
			virtual bool operator!=(const FontImpl& other) const;
		protected:
			unsigned int size;
		};

		/**
		@bug HorizontalAlignment::RIGHT misses the last letter in the width calculation
		*/
		class Font : public Initializable{
		public:
			Font(const Font& f, const unsigned int height = 12);
			Font(const Fonts f = Fonts::SANS, const unsigned int height = 12);
			Font(const std::shared_ptr<FontImpl> impl, const unsigned int height = 12);

			void init() override;
			void destroy() override;

			/**
			@todo cache characters
			*/
			void getCharacter(const wchar_t character, std::shared_ptr<Letter> let) const;

			void setSize(const unsigned int height);
			unsigned int& getSize();
			const unsigned int& getSize() const;

			bool hasKerning() const;

			Vector<unsigned int, 2> getKerning(const wchar_t prev, const wchar_t current) const;

			bool operator==(const Font& other) const;
			bool operator!=(const Font& other) const;
		private:
			Font(std::shared_ptr<FontImpl> impl, const unsigned int height = 12);

			std::shared_ptr<FontImpl> impl;
		};//Font
	}//gfx
}//mc

#endif
