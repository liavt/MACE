/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Graphics/Entity2D.h>
#include <MACE/Core/System.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cmath>
#include <vector>
#include <clocale>
#include <iostream>

namespace mc {
	namespace gfx {
		namespace {
			int IMAGE_PROTOCOL = -1;
			int PROGRESS_BAR_PROTOCOL = -1;
			int LETTER_PROTOCOL = -1;

			ogl::Texture2D whiteTexture;

			FT_Library freetype;
			//error if freetype init failed or -1 if it hasnt been created
			int freetypeStatus = -1;

			std::vector<FT_Face> fonts = std::vector<FT_Face>();
		}//anon namespace

		Entity2D::Entity2D() : GraphicsEntity() {}

		ogl::UniformBuffer & Entity2D::getBuffer() {
			makeDirty();
			return buffer;
		}
		const ogl::UniformBuffer & Entity2D::getBuffer() const {
			return buffer;
		}
		void Entity2D::setBuffer(const ogl::UniformBuffer & newBuffer) {
			if( newBuffer != buffer ) {
				makeDirty();
				buffer = newBuffer;
			}
		}

		bool Entity2D::operator==(const Entity2D & other) const {
			return GraphicsEntity::operator==(other) && buffer == other.buffer;
		}

		bool Entity2D::operator!=(const Entity2D & other) const {
			return !operator==(other);
		}

		//IMAGE

		int Image::getProtocol() {
			return IMAGE_PROTOCOL;
		}

		Image::Image() noexcept : texture() {}

		Image::Image(const ColorAttachment& tex) : texture(tex) {}

		void Image::onInit() {
			if( !texture.isCreated() ) {
				texture.init();
			}

			if( IMAGE_PROTOCOL < 0 ) {
				IMAGE_PROTOCOL = Renderer::registerProtocol<Image>();
			}

			if( !buffer.isCreated() ) {
				buffer.init();
			}

			buffer.setLocation(0);

			Renderer::initEntity(this, IMAGE_PROTOCOL);
		}

		void Image::onUpdate() {}

		void Image::onRender() {
			Renderer::queue(this, IMAGE_PROTOCOL);
		}

		void Image::onDestroy() {
			if( texture.isCreated() ) {
				texture.destroy();
			}

			if( buffer.isCreated() ) {
				buffer.destroy();
			}
		}

		void Image::onClean() {
			buffer.setData(4 * sizeof(float), &texture.getPaint());
		}

		void Image::setTexture(const ColorAttachment & tex) {
			if( tex != texture ) {
				makeDirty();

				texture = tex;
			}
		}

		ColorAttachment & Image::getTexture() {
			makeDirty();

			return texture;
		}

		const ColorAttachment & Image::getTexture() const {
			return texture;
		}

		bool Image::operator==(const Image & other) const {
			return Entity2D::operator==(other) && texture == other.texture;
		}

		bool Image::operator!=(const Image & other) const {
			return !operator==(other);
		}

		void RenderProtocol<Image>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			const char* vertexShader2D = {
		#	include <MACE/Graphics/Shaders/image.v.glsl>
			};
			const char* fragmentShader2D = {
		#	include <MACE/Graphics/Shaders/image.f.glsl>
			};

			renderer.init(vertexShader2D, fragmentShader2D);
		}//init

		void RenderProtocol<Image>::initEntity(GraphicsEntity* e) {
			Image* img = dynamic_cast<Image*>(e);
			if( img == nullptr ) {
				throw InvalidTypeError("Input to RenderProtocol<Image>::initEntity must be of type Image");
			}

			img->getBuffer().bindToUniformBlock(renderer.getShader(), "textureData");
		}

		void RenderProtocol<Image>::renderEntity(os::WindowModule*, GraphicsEntity* e) {
			Image* entity = dynamic_cast<Image*>(e);
			if( entity == nullptr ) {
				throw InvalidTypeError("You must queue an Image for RenderProtocol<Image>");
			}

			entity->texture.bind();

			entity->buffer.bindForRender();

			renderer.bind();

			renderer.draw(entity);
		}//render

		void RenderProtocol<Image>::destroy() {
			renderer.destroy();
		}//destroy

		//PROGRESS BAR

		int ProgressBar::getProtocol() {
			return PROGRESS_BAR_PROTOCOL;
		}

		ProgressBar::ProgressBar() noexcept: ProgressBar(0, 0, 0) {}

		ProgressBar::ProgressBar(const float minimum, const float maximum, const float prog) noexcept : min(minimum), max(maximum), progress(prog) {}

		void ProgressBar::setBackgroundTexture(const ColorAttachment & tex) {
			if( backgroundTexture != tex ) {
				makeDirty();

				backgroundTexture = tex;
			}
		}

		ColorAttachment & ProgressBar::getBackgroundTexture() {
			makeDirty();

			return backgroundTexture;
		}

		const ColorAttachment & ProgressBar::getBackgroundTexture() const {
			return backgroundTexture;
		}

		void ProgressBar::setForegroundTexture(const ColorAttachment & tex) {
			if( foregroundTexture != tex ) {
				makeDirty();

				foregroundTexture = tex;
			}
		}

		ColorAttachment & ProgressBar::getForegroundTexture() {
			makeDirty();

			return foregroundTexture;
		}

		const ColorAttachment & ProgressBar::getForegroundTexture() const {
			return foregroundTexture;
		}

		void ProgressBar::setSelectionTexture(const ColorAttachment & tex) {
			if( selectionTexture != tex ) {
				makeDirty();

				selectionTexture = tex;

				selectionTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			}
		}

		ColorAttachment & ProgressBar::getSelectionTexture() {
			makeDirty();

			return selectionTexture;
		}

		const ColorAttachment & ProgressBar::getSelectionTexture() const {
			return selectionTexture;
		}


		void ProgressBar::setMinimum(const float minimum) {
			if( min != minimum ) {
				makeDirty();

				min = minimum;
			}
		}

		float & ProgressBar::getMinimum() {
			makeDirty();

			return min;
		}

		const float & ProgressBar::getMinimum() const {
			return min;
		}

		void ProgressBar::setMaximum(const float maximum) {
			if( max != maximum ) {
				makeDirty();

				max = maximum;
			}
		}

		float & ProgressBar::getMaximum() {
			makeDirty();

			return max;
		}

		const float & ProgressBar::getMaximum() const {
			return max;
		}

		void ProgressBar::setProgress(const float prog) {
			if( progress != prog ) {
				makeDirty();

				progress = prog;
			}
		}

		void ProgressBar::addProgress(const float prog) {
			if( prog != 0 ) {
				makeDirty();

				progress += prog;
			}
		}

		float & ProgressBar::getProgress() {
			makeDirty();

			return progress;
		}

		const float & ProgressBar::getProgress() const {
			return progress;
		}


		void ProgressBar::easeTo(const float destination, const float time, const EaseFunction function, const EaseDoneCallback callback) {
			class EaseComponent: public Component {
			public:
				EaseComponent(const float p, const float t, const float sp, const EaseFunction f, const EaseDoneCallback cb) : Component(), startProg(sp), prog(p), time(t), func(f), start(0), done(cb) {};

				const float startProg;
				const float prog;
				const float time;
				const EaseFunction func;
				float start;
				const EaseDoneCallback done;
			protected:
				void init(Entity*) override {}
				bool update(Entity* e) override {
					ProgressBar* bar = dynamic_cast<ProgressBar*>(e);
					if( bar == nullptr ) {
						//should never happen, as this class is only ever defined and used here, but just in caes
						throw InvalidTypeError("Internal error: EaseComponent did not receive a progress bar in update()");
					}

					//combine 2 operations into 1
					float difference = start++ / time;
					float percentDone;

					if( func == EaseFunction::SINUSOIDAL ) {
						percentDone = static_cast<float>(std::sin(difference * (math::pi() / 2)));
					} else if( func == EaseFunction::COSINE ) {
						percentDone = 1.0f - static_cast<float>(std::cos(difference * (math::pi() / 2)));
					} else if( func == EaseFunction::QUADRATIC ) {
						percentDone = difference*difference;
					} else if( func == EaseFunction::CUBIC ) {
						percentDone = difference*difference*difference;
					} else if( func == EaseFunction::QUARTIC ) {
						percentDone = difference*difference*difference*difference;
					} else if( func == EaseFunction::QUINTIC ) {
						percentDone = difference*difference*difference*difference*difference;
					} else if( func == EaseFunction::SQUARE_ROOT ) {
						percentDone = std::sqrt(difference);
					} else if( func == EaseFunction::SQUARE_ROOT ) {
						percentDone = std::cbrt(difference);
					} else {
						//linear as a fallback
						percentDone = difference;
					}

					//meaning that we are easing backwards
					if( startProg > prog ) {
						bar->setProgress(startProg - (startProg - prog)*(percentDone));
					} else {
						bar->setProgress(startProg + (prog - startProg)*(percentDone));
					}

					//if we got there or time has run out
					if( bar->getProgress() == prog || difference >= 1.0f ) {
						return true;
					}
					return false;
				}
				void render(Entity*) override {}
				void destroy(Entity* e) override {
					ProgressBar* bar = dynamic_cast<ProgressBar*>(e);
					if( bar == nullptr ) {
						throw InvalidTypeError("Internal error: EaseComponent did not receive a progress bar in destroy()");
					}
					done(bar);

					//honorable suicide
					delete this;
				}
			};

			//it deletes itself in destroy();
			//TODO find a way not to use new
			EaseComponent* com = new EaseComponent(destination, time, this->progress, function, callback);
			addComponent(com);

			makeDirty();
		}

		bool ProgressBar::operator==(const ProgressBar & other) const {
			return Entity2D::operator==(other) && max == other.max&&min == other.min&&progress == other.progress&&backgroundTexture == other.backgroundTexture&&foregroundTexture == other.foregroundTexture&&selectionTexture == other.selectionTexture;
		}

		bool ProgressBar::operator!=(const ProgressBar & other) const {
			return !operator==(other);
		}

		void ProgressBar::onInit() {
			if( !backgroundTexture.isCreated() ) {
				backgroundTexture.init();
			}

			if( !foregroundTexture.isCreated() ) {
				foregroundTexture.init();
			}

			if( !selectionTexture.isCreated() ) {
				selectionTexture.init();
			}

			if( PROGRESS_BAR_PROTOCOL < 0 ) {
				PROGRESS_BAR_PROTOCOL = Renderer::registerProtocol<ProgressBar>();
			}

			if( !buffer.isCreated() ) {
				buffer.init();
			}

			buffer.setLocation(0);

			buffer.setData((3 * sizeof(selectionTexture.getPaint())) + sizeof(progress), nullptr);

			Renderer::initEntity(this, PROGRESS_BAR_PROTOCOL);
		}

		void ProgressBar::onUpdate() {}

		void ProgressBar::onRender() {
			Renderer::queue(this, PROGRESS_BAR_PROTOCOL);
		}

		void ProgressBar::onClean() {
			Index offset = 0;
			buffer.setDataRange(sizeof(selectionTexture.getPaint()), &selectionTexture.getPaint(), offset);
			offset += sizeof(selectionTexture.getPaint());
			buffer.setDataRange(sizeof(foregroundTexture.getPaint()), &foregroundTexture.getPaint(), offset);
			offset += sizeof(foregroundTexture.getPaint());
			buffer.setDataRange(sizeof(backgroundTexture.getPaint()), &backgroundTexture.getPaint(), offset);
			offset += sizeof(backgroundTexture.getPaint());
			const float outProgress = (progress - min) / (max - min);
			buffer.setDataRange(sizeof(progress), &outProgress, offset);
		}

		void ProgressBar::onDestroy() {
			if( backgroundTexture.isCreated() ) {
				backgroundTexture.destroy();
			}

			if( foregroundTexture.isCreated() ) {
				foregroundTexture.destroy();
			}

			if( selectionTexture.isCreated() ) {
				selectionTexture.destroy();
			}

			if( buffer.isCreated() ) {
				buffer.destroy();
			}
		}

		void RenderProtocol<ProgressBar>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			const char* vertexShader2D = {
#	include <MACE/Graphics/Shaders/progressbar.v.glsl>
			};
			const char* fragmentShader2D = {
#	include <MACE/Graphics/Shaders/progressbar.f.glsl>
			};

			renderer.init(vertexShader2D, fragmentShader2D);

			ogl::ShaderProgram& prog = renderer.getShader();
			prog.bind();
			prog.createUniform("backgroundTexture");
			prog.createUniform("foregroundTexture");
			prog.createUniform("selectionTexture");

			prog.setUniform("backgroundTexture", 0);
			prog.setUniform("foregroundTexture", 1);
			prog.setUniform("selectionTexture", 2);
		}//init

		void RenderProtocol<ProgressBar>::initEntity(GraphicsEntity* e) {
			ProgressBar* bar = dynamic_cast<ProgressBar*>(e);
			if( bar == nullptr ) {
				throw InvalidTypeError("Input to RenderProtocol<ProgressBar>::initEntity must be of type ProgressBar");
			}

			bar->getBuffer().bindToUniformBlock(renderer.getShader(), "textureData");
		}

		void RenderProtocol<ProgressBar>::renderEntity(os::WindowModule*, GraphicsEntity* e) {
			ProgressBar* entity = dynamic_cast<ProgressBar*>(e);
			if( entity == nullptr ) {
				throw InvalidTypeError("You must queue an ProgressBar for RenderProtocol<ProgressBar>");
			}

			entity->backgroundTexture.bind(0);
			entity->foregroundTexture.bind(1);
			entity->selectionTexture.bind(2);

			entity->buffer.bindForRender();

			renderer.bind();

			renderer.draw(entity);

			ogl::checkGLError(__LINE__, __FILE__);
		}//render

		void RenderProtocol<ProgressBar>::destroy() {
			renderer.destroy();
		}//destroy

		Font Font::loadFont(const std::string& name) {
			return loadFont(name.c_str());
		}

		Font Font::loadFont(const char* name) {
			if( freetypeStatus < 0 ) {
				if( (freetypeStatus = FT_Init_FreeType(&freetype)) != FT_Err_Ok ) {
					throw FontError("Freetype failed to initailize with error code " + std::to_string(freetypeStatus));
				}
			}

			Index id = fonts.size();

			fonts.push_back(FT_Face());
			if( int result = FT_New_Face(freetype, name, 0, &fonts[id]) ) {
				throw FontError("Freetype failed to create font with result " + std::to_string(result));
			}

			if( int result = FT_Select_Charmap(fonts[id], FT_ENCODING_UNICODE) ) {
				throw FontError("Freetype failed to change charmap with result " + std::to_string(result));
			}

			return Font(id);
		}

		void Font::destroy() {
			if( int result = FT_Done_Face(fonts[id]) ) {
				throw FontError("Freetype failed to delete font with result " + std::to_string(result));
			}
		}

		void Font::setSize(const Size h) {
			this->height = h;
		}

		Size& Font::getSize() {
			return height;
		}

		const Size& Font::getSize() const {
			return height;
		}

		bool Font::hasKerning() const {
			return FT_HAS_KERNING(fonts[id]) == 1;
		}

		Index Font::getID() const {
			return id;
		}

		void Font::getCharacter(const wchar_t c, Letter* character) const {
			if( height <= 0 ) {
				throw IndexOutOfBoundsError("The height of the font cannot be 0 - you must set it!");
			}

			FT_Set_Pixel_Sizes(fonts[id], 0, height);

			if( int result = FT_Load_Char(fonts[id], c, FT_LOAD_RENDER | FT_LOAD_PEDANTIC | FT_LOAD_TARGET_LIGHT) ) {
				throw FontError("Failed to load glyph with error code " + std::to_string(result));
			}

			character->width = fonts[id]->glyph->metrics.width >> 6;
			character->height = fonts[id]->glyph->metrics.height >> 6;
			character->bearingX = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character->bearingY = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character->advanceX = fonts[id]->glyph->advance.x >> 6;
			character->advanceY = fonts[id]->glyph->advance.y >> 6;

			character->mask.init();
			character->mask.bind();

			character->mask.resetPixelStorage();

			character->mask.setPixelStorage(GL_UNPACK_ALIGNMENT, 1);

			character->mask.setData(fonts[id]->glyph->bitmap.buffer, character->width, character->height, GL_UNSIGNED_BYTE, GL_RED, GL_RED);
			character->mask.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			character->mask.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			character->mask.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			character->mask.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		}

		Vector<unsigned int, 2> Font::getKerning(const wchar_t prev, const wchar_t current) const {
			FT_Vector vec;

			FT_Get_Kerning(fonts[id], prev, current, FT_KERNING_DEFAULT, &vec);

			return{ static_cast<unsigned int>(vec.x >> 6), static_cast<unsigned int>(vec.y >> 6) };
		}

		bool Font::operator==(const Font& other) const {
			return id == other.id&&height == other.height;
		}

		bool Font::operator!=(const Font& other) const {
			return !operator==(other);
		}

		Font::Font(const Index fontID, const Size h) : id(fontID), height(h) {}

		Font::Font(const Font & f) : Font(f.id, f.height) {}

		int Letter::getProtocol() {
			return LETTER_PROTOCOL;
		}

		void RenderProtocol<Letter>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			const char* vertexShader2D = {
		#	include <MACE/Graphics/Shaders/letter.v.glsl>
			};
			const char* fragmentShader2D = {
		#	include <MACE/Graphics/Shaders/letter.f.glsl>
			};

			renderer.init(vertexShader2D, fragmentShader2D);

			ogl::ShaderProgram& prog = renderer.getShader();
			prog.bind();
			prog.createUniform("mask");
			prog.createUniform("tex");

			prog.setUniform("mask", 0);
			prog.setUniform("tex", 1);

			ogl::checkGLError(__LINE__, __FILE__);
		}//init

		void RenderProtocol<Letter>::initEntity(GraphicsEntity* en) {
			Letter* let = dynamic_cast<Letter*>(en);
			if( let == nullptr ) {
				throw InvalidTypeError("Internal error: Input to RenderProtocol<Letter>::initEntity must be of type Letter");
			}

			let->getBuffer().bindToUniformBlock(renderer.getShader(), "textureData");
		}

		void RenderProtocol<Letter>::renderEntity(os::WindowModule*, GraphicsEntity* e) {
			Letter* entity = dynamic_cast<Letter*>(e);
			if( entity == nullptr ) {
				throw InvalidTypeError("You must queue an Letter for RenderProtocol<Letter>");
			}

			entity->mask.bind(0);
			entity->texture.bind(1);

			entity->buffer.bindForRender();

			renderer.bind();

			renderer.draw(entity);

			ogl::checkGLError(__LINE__, __FILE__);
		}//render

		void RenderProtocol<Letter>::destroy() {
			renderer.destroy();

		}//destroy

		Letter::Letter(const ogl::Texture2D& tex) : mask(tex) {}

		const ogl::Texture2D& Letter::getMask() const {
			return mask;
		}
		const ColorAttachment & Letter::getTexture() const {
			return texture;
		}
		const Size& Letter::getCharacterWidth() const {
			return width;
		}

		const Size& Letter::getCharacterHeight() const {
			return height;
		}

		const Index& Letter::getXBearing() const {
			return bearingX;
		}

		const Index& Letter::getYBearing() const {
			return bearingY;
		}

		const Index& Letter::getXAdvance() const {
			return advanceX;
		}

		const Index& Letter::getYAdvance() const {
			return advanceY;
		}

		bool Letter::operator==(const Letter& other) const {
			return Entity2D::operator==(other) && mask == other.mask&&width == other.width&&height == other.height&&bearingX == other.bearingX&&bearingY == other.bearingY&&advanceX == other.advanceX&&advanceY == other.advanceY;
		}

		bool Letter::operator!=(const Letter& other) const {
			return !operator==(other);
		}

		void Letter::onInit() {
			if( !mask.isCreated() ) {
				mask.init();
			}

			if( !texture.isCreated() ) {
				texture.init();
			}

			if( LETTER_PROTOCOL < 0 ) {
				LETTER_PROTOCOL = Renderer::registerProtocol<Letter>();
			}

			if( !buffer.isCreated() ) {
				buffer.init();
			}

			buffer.setLocation(0);

			Renderer::initEntity(this, LETTER_PROTOCOL);
		}

		void Letter::onUpdate() {

		}

		void Letter::onRender() {
			Renderer::queue(this, LETTER_PROTOCOL);
		}

		void Letter::onDestroy() {
			if( mask.isCreated() ) {
				mask.destroy();
			}

			if( texture.isCreated() ) {
				texture.destroy();
			}

			if( buffer.isCreated() ) {
				buffer.destroy();
			}
		}

		void Letter::onClean() {
			buffer.setData(sizeof(texture.getPaint()), &texture.getPaint());
		}

		Text::Text(const std::string & s, const Font & f) : Text(os::toWideString(s), f) {}

		Text::Text(const std::wstring & t, const Font& f) : Entity2D(), text(t), font(f) {}

		void Text::setText(const std::wstring & newText) {
			if( text != newText ) {
				makeDirty();

				text = newText;
			}
		}

		std::wstring & Text::getText() {
			makeDirty();

			return text;
		}

		const std::wstring & Text::getText() const {
			return text;
		}

		void Text::setFont(const Font& f) {
			if( font != f ) {
				makeDirty();

				font = f;
			}
		}

		Font & Text::getFont() {
			makeDirty();

			return font;
		}

		const Font & Text::getFont() const {
			return font;
		}

		const Group& Text::getLetters() const {
			return letters;
		}

		void Text::setVerticalAlign(const VerticalAlign align) {
			if( vertAlign != align ) {
				makeDirty();

				vertAlign = align;
			}
		}

		const VerticalAlign Text::getVerticalAlign() const {
			return vertAlign;
		}

		void Text::setHorizontalAlign(const HorizontalAlign align) {
			if( horzAlign != align ) {
				makeDirty();

				horzAlign = align;
			}
		}

		const HorizontalAlign Text::getHorizontalAlign() const {
			return horzAlign;
		}

		void Text::setTexture(const ColorAttachment & tex) {
			if( tex != texture ) {
				makeDirty();

				texture = tex;
			}
		}

		ColorAttachment & Text::getTexture() {
			makeDirty();

			return texture;
		}

		const ColorAttachment & Text::getTexture() const {
			return texture;
		}

		bool Text::operator==(const Text & other) const {
			return Entity2D::operator==(other) && letters == other.letters && text == other.text && texture == other.texture && vertAlign == other.vertAlign && horzAlign == other.horzAlign;
		}

		bool Text::operator!=(const Text & other) const {
			return !operator==(other);
		}

		void Text::onInit() {}

		void Text::onUpdate() {}

		void Text::onRender() {}

		void Text::onDestroy() {}

		void Text::onClean() {
			for( Index i = 0; i < letters.size(); ++i ) {
				delete &letters[i];
			}

			letters.clearChildren();

			if( !hasChild(letters) ) {
				letters.setProperty(Entity::MAINTAIN_WIDTH, true);
				letters.setProperty(Entity::MAINTAIN_HEIGHT, true);
				letters.setProperty(Entity::MAINTAIN_X, getProperty(Entity::MAINTAIN_X));
				letters.setProperty(Entity::MAINTAIN_Y, getProperty(Entity::MAINTAIN_Y));

				addChild(letters);
			}

			Entity::Metrics metrics = getMetrics();

			const float widthScale = 1.0f / metrics.scale[0], heightScale = 1.0f / metrics.scale[1];

			const float origWidth = static_cast<const float>(Renderer::getOriginalWidth()),
				origHeight = static_cast<const float>(Renderer::getOriginalHeight());

			const bool useKerning = font.hasKerning();

			float x = 0, y = 0;

			float width = 0, height = 0;

			for( Index i = 0; i < text.length(); ++i ) {
				if( text[i] == '\n' ) {
					if( x > width ) {
						width = x;
					}

					y += static_cast<float>(font.getSize()) / origHeight;

					height = y;

					x = 0;
				} else {
					Letter* let = new Letter();
					font.getCharacter(text[i], let);

					//freetype uses absolute values (pixels) and we use relative. so by dividing the pixel by the size, we get relative values
					let->setWidth((static_cast<float>(let->width) / origWidth) * widthScale);
					let->setHeight((static_cast<float>(let->height) / origHeight) * heightScale);

					Vector<float, 2> position = { x, y };

					if( i > 0 && useKerning ) {
						Vector<unsigned int, 2> delta = font.getKerning(text[i - 1], text[i]);

						position[0] += static_cast<float>(delta[0]) / origWidth;
						position[1] += static_cast<float>(delta[1]) / origHeight;	
					}

					//position[0] -= static_cast<float>(let->bearingX - let->width) / origWidth;

					//font.getSize() - 
					//let->getHeight() - 
					//position[1] -= ((font.getSize() + (let->bearingY >> 1)) / origHeight);
					position[1] += let->getHeight();
					position[1] -= static_cast<float>(let->height - let->bearingY) / origHeight;

					//position[1] += let->getHeight();

					//i cant bear this|
					let->setX((position[0] + (static_cast<float>(let->width) / origWidth)));
					let->setY(position[1]);

					let->setOpacity(getOpacity());

					//it needs to be bit shifted by 6 to get raw pixel values because it is 1/64 of a pixel
					x += static_cast<float>(let->advanceX) / origWidth;
					x += let->getWidth();
					//y += static_cast<float>(let->advanceY) / origHeight;

					let->texture = this->texture;

					letters.addChild(let);
				}
			}

			y += static_cast<float>(font.getSize() << 1) / origHeight;

			if( x > width ) {
				width = x;
			}

			height += y;

			switch( horzAlign ) {
			default:
			case HorizontalAlign::CENTER:
				letters.setX((-width / 2) + static_cast<const float>(font.getSize() >> 1) / origWidth);
				break;
			case HorizontalAlign::RIGHT:
				letters.setX((1.0f - width) + static_cast<const float>(font.getSize() >> 1) / origWidth);
				break;
			case HorizontalAlign::LEFT:
				letters.setX(-1.0f + static_cast<const float>(font.getSize() >> 1) / origWidth);
				break;
			}

			switch( vertAlign ) {
			default:
			case VerticalAlign::CENTER:
				//letters.setY(1.0f + height * 2);
				//letters.setY(height + (static_cast<const float>(font.getSize() >> 1) / origHeight));
				break;
			case VerticalAlign::BOTTOM:
				letters.setY((-1.0f + height / 2) - static_cast<const float>(font.getSize() >> 1) / origHeight);
				break;
			case VerticalAlign::TOP:
				letters.setY((1.0f - height) + static_cast<const float>(font.getSize() >> 1) / origHeight);
				break;
			}
		}
	}//gfx
}//mc
