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

namespace mc {
	namespace gfx {
		namespace {
			int IMAGE_PROTOCOL = -1;
			int PROGRESS_BAR_PROTOCOL = -1;
			int LETTER_PROTOCOL = -1;
			int BUTTON_PROTOCOL = -1;

			ogl::Texture2D whiteTexture;

			FT_Library freetype;
			//error if freetype init failed or -1 if it hasnt been created
			int freetypeStatus = -1;

			std::vector<FT_Face> fonts = std::vector<FT_Face>();
		}//anon namespace

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
				t /= d;
				return c*((t - 1)*t*((s + 1)*t + s) + 1) + b;
			};

			const EaseFunction BACK_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				const float s = 2.5949095f;
				if ((t /= d / 2) < 1) return c / 2 * (t*t*((s + 1)*t - s)) + b;
				const float postFix = t -= 2;
				return c / 2 * ((postFix)*t*((s + 1)*t + s) + 2) + b;
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
				t /= d;
				return -c * (std::sqrt(1 - t*t) - 1) + b;
			};

			const EaseFunction CIRCLE_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c * std::sqrt(1 - (t - 1)*t) + b;
			};

			const EaseFunction CIRCLE_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) {
					return -c / 2 * (std::sqrt(1 - t*t) - 1) + b;
				}

				t -= 2;
				return c / 2 * (std::sqrt(1 - t*t) + 1) + b;
			};

			const EaseFunction CUBIC_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*t*t*t + b;
			};

			const EaseFunction CUBIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*((t - 1)*t*t + 1) + b;
			};

			const EaseFunction CUBIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) {
					return c / 2 * t*t*t + b;
				}

				t -= 2;
				return c / 2 * (t*t*t + 2) + b;
			};

			const EaseFunction ELASTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) {
					return b;
				} else if ((t /= d) == 1) {
					return b + c;
				}

				const float p = d*.3f;
				const float s = p / 4;
				--t;
				const float postFix = c*static_cast<float>(std::pow(2, 10 * t));
				return -static_cast<float>(postFix * static_cast<float>(std::sin((t*d - s))*(2 * static_cast<float>(math::pi())) / p)) + b;
			};

			const EaseFunction ELASTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) {
					return b;
				} else if ((t /= d) == 1) {
					return b + c;
				}

				const float p = d*.3f;
				const float s = p / 4;
				return static_cast<float>(c*std::pow(2, -10 * t) * std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p) + c + b);
			};

			const EaseFunction ELASTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if (t == 0) {
					return b;
				} else if ((t /= d / 2) == 2) {
					return b + c;
				}

				const float p = d*(.3f*1.5f);
				const float s = p / 4;

				if (t < 1) {
					--t;
					const float postFix = c*static_cast<float>(std::pow(2, 10 * t));
					return -.5f*(postFix* static_cast<float>(std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p))) + b;
				}
				--t;
				const float postFix = c*static_cast<float>(std::pow(2, -10 * (t)));
				return postFix * std::sin((t*d - s)*(2 * static_cast<float>(math::pi())) / p)*0.5f + c + b;
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
				t /= d;
				return c*t*t + b;
			};

			const EaseFunction QUADRATIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return -c *t*(t - 2) + b;
			};

			const EaseFunction QUADRATIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				if ((t /= d / 2) < 1) return ((c / 2)*(t*t)) + b;
				--t;
				return -c / 2 * (((t - 1)*(t)) - 1) + b;
			};

			const EaseFunction QUARTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*(t)*t*t*t + b;
			};

			const EaseFunction QUARTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return -c * ((t - 1)*t*t*t - 1) + b;
			};

			const EaseFunction QUARTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				if ((t / 2) < 1) {
					return c / 2 * t*t*t*t + b;
				}
				return -c / 2 * (t*t*t*t - 2) + b;
			};

			const EaseFunction QUINTIC_IN = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*t*t*t*t*t + b;
			};

			const EaseFunction QUINTIC_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				return c*((t - 1)*t*t*t*t + 1) + b;
			};

			const EaseFunction QUINTIC_IN_OUT = [](float t, const float b, const float c, const float d) -> float {
				t /= d;
				if ((t / 2) < 1) {
					return c / 2 * t*t*t*t*t + b;
				}
				t -= 2;
				return c / 2 * (t*t*t*t*t + 2) + b;
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

		Entity2D::Entity2D() : GraphicsEntity() {}

		ogl::UniformBuffer & Entity2D::getBuffer() {
			makeDirty();
			return buffer;
		}
		const ogl::UniformBuffer & Entity2D::getBuffer() const {
			return buffer;
		}
		void Entity2D::setBuffer(const ogl::UniformBuffer & newBuffer) {
			if (newBuffer != buffer) {
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
			if (!texture.isCreated()) {
				texture.init();
			}

			if (IMAGE_PROTOCOL < 0) {
				IMAGE_PROTOCOL = getRenderer()->registerProtocol<Image>();
			}

			if (!buffer.isCreated()) {
				buffer.init();
			}

			buffer.setLocation(0);

			getRenderer()->initEntity(this, IMAGE_PROTOCOL);
		}

		void Image::onUpdate() {}

		void Image::onRender() {
			getRenderer()->queue(this, IMAGE_PROTOCOL);
		}

		void Image::onDestroy() {
			if (texture.isCreated()) {
				texture.destroy();
			}

			if (buffer.isCreated()) {
				buffer.destroy();
			}
		}

		void Image::onClean() {
			buffer.setData(4 * sizeof(float), &texture.getPaint());
		}

		void Image::setTexture(const ColorAttachment & tex) {
			if (tex != texture) {
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
			renderer.init(
#	include <MACE/Graphics/Shaders/image.v.glsl>
				,
#	include <MACE/Graphics/Shaders/image.f.glsl>
			);
		}//init

		void RenderProtocol<Image>::initEntity(GraphicsEntity* e) {
			Image* img = dynamic_cast<Image*>(e);
			if (img == nullptr) {
				throw InvalidTypeError("Input to RenderProtocol<Image>::initEntity must be of type Image");
			}

			img->getBuffer().bindToUniformBlock(renderer.getShader(), "textureData");
		}

		void RenderProtocol<Image>::renderEntity(os::WindowModule*, GraphicsEntity* e) {
			Image* entity = dynamic_cast<Image*>(e);
			if (entity == nullptr) {
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

		ProgressBar::ProgressBar(const float minimum, const float maximum, const float prog) noexcept : minimumProgress(minimum), maximumProgress(maximum), progress(prog) {}

		void ProgressBar::setBackgroundTexture(const ColorAttachment & tex) {
			if (backgroundTexture != tex) {
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
			if (foregroundTexture != tex) {
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
			if (selectionTexture != tex) {
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
			if (minimumProgress != minimum) {
				makeDirty();

				minimumProgress = minimum;
			}
		}

		float & ProgressBar::getMinimum() {
			makeDirty();

			return minimumProgress;
		}

		const float & ProgressBar::getMinimum() const {
			return minimumProgress;
		}

		void ProgressBar::setMaximum(const float maximum) {
			if (maximumProgress != maximum) {
				makeDirty();

				maximumProgress = maximum;
			}
		}

		float & ProgressBar::getMaximum() {
			makeDirty();

			return maximumProgress;
		}

		const float & ProgressBar::getMaximum() const {
			return maximumProgress;
		}

		void ProgressBar::setProgress(const float prog) {
			if (progress != prog) {
				makeDirty();

				progress = prog;
			}
		}

		void ProgressBar::addProgress(const float prog) {
			if (prog != 0) {
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


		void ProgressBar::easeTo(const float destination, const float time, const EaseFunction function, const ProgressBarEaseDoneCallback callback) {
			class EaseComponent: public Component {
			public:
				EaseComponent(const float p, const float duration, const float sp, const EaseFunction f, const ProgressBarEaseDoneCallback cb) : Component(), b(sp), c(p), d(duration), ease(f), t(0), done(cb) {};

				//http://upshots.org/actionscript/jsas-understanding-easing for explanation of these variables
				const float b;
				const float c;
				const float d;
				const EaseFunction ease;
				float t;
				const ProgressBarEaseDoneCallback done;
			protected:
				void init() override {}
				bool update() override {
					ProgressBar* bar = dynamic_cast<ProgressBar*>(parent);
					if (bar == nullptr) {
						//should never happen, as this class is only ever defined and used here, but just in caes
						throw InvalidTypeError("Internal error: EaseComponent did not receive a progress bar in update()");
					}


					bar->setProgress(ease(t++, b, c - b, d));

					//if we got there or time has run out
					if (bar->getProgress() == c || t >= d) {
						return true;
					}

					return false;
				}
				void render() override {}
				void destroy() override {
					ProgressBar* bar = dynamic_cast<ProgressBar*>(parent);
					if (bar == nullptr) {
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
			return Entity2D::operator==(other) && maximumProgress == other.maximumProgress&&minimumProgress == other.minimumProgress&&progress == other.progress&&backgroundTexture == other.backgroundTexture&&foregroundTexture == other.foregroundTexture&&selectionTexture == other.selectionTexture;
		}

		bool ProgressBar::operator!=(const ProgressBar & other) const {
			return !operator==(other);
		}

		void ProgressBar::onInit() {
			if (!backgroundTexture.isCreated()) {
				backgroundTexture.init();
			}

			if (!foregroundTexture.isCreated()) {
				foregroundTexture.init();
			}

			if (!selectionTexture.isCreated()) {
				selectionTexture.init();
			}

			if (PROGRESS_BAR_PROTOCOL < 0) {
				PROGRESS_BAR_PROTOCOL = getRenderer()->registerProtocol<ProgressBar>();
			}

			if (!buffer.isCreated()) {
				buffer.init();
			}

			buffer.setLocation(0);

			buffer.setData((3 * sizeof(selectionTexture.getPaint())) + sizeof(progress), nullptr);

			getRenderer()->initEntity(this, PROGRESS_BAR_PROTOCOL);
		}

		void ProgressBar::onUpdate() {}

		void ProgressBar::onRender() {
			getRenderer()->queue(this, PROGRESS_BAR_PROTOCOL);
		}

		void ProgressBar::onClean() {
			Index offset = 0;
			buffer.setDataRange(sizeof(selectionTexture.getPaint()), &selectionTexture.getPaint(), offset);
			offset += sizeof(selectionTexture.getPaint());
			buffer.setDataRange(sizeof(foregroundTexture.getPaint()), &foregroundTexture.getPaint(), offset);
			offset += sizeof(foregroundTexture.getPaint());
			buffer.setDataRange(sizeof(backgroundTexture.getPaint()), &backgroundTexture.getPaint(), offset);
			offset += sizeof(backgroundTexture.getPaint());
			const float outProgress = (progress - minimumProgress) / (maximumProgress - minimumProgress);
			buffer.setDataRange(sizeof(progress), &outProgress, offset);
		}

		void ProgressBar::onDestroy() {
			if (backgroundTexture.isCreated()) {
				backgroundTexture.destroy();
			}

			if (foregroundTexture.isCreated()) {
				foregroundTexture.destroy();
			}

			if (selectionTexture.isCreated()) {
				selectionTexture.destroy();
			}

			if (buffer.isCreated()) {
				buffer.destroy();
			}
		}

		void RenderProtocol<ProgressBar>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			renderer.init(
#	include <MACE/Graphics/Shaders/progressbar.v.glsl>
				,
#	include <MACE/Graphics/Shaders/progressbar.f.glsl>
			);

			ogl::ShaderProgram& prog = renderer.getShader();
			prog.bind();
			prog.createUniform("backgroundTexture");
			prog.createUniform("foregroundTexture");
			prog.createUniform("selectionTexture");

			prog.setUniform("backgroundTexture", 0);
			prog.setUniform("foregroundTexture", 1);
			prog.setUniform("selectionTexture", 2);

			ogl::checkGLError(__LINE__, __FILE__, "Error initializing RenderProtocol<ProgressBar>");
		}//init

		void RenderProtocol<ProgressBar>::initEntity(GraphicsEntity* e) {
			ProgressBar* bar = dynamic_cast<ProgressBar*>(e);
			if (bar == nullptr) {
				throw InvalidTypeError("Input to RenderProtocol<ProgressBar>::initEntity must be of type ProgressBar");
			}

			bar->getBuffer().bindToUniformBlock(renderer.getShader(), "textureData");
		}

		void RenderProtocol<ProgressBar>::renderEntity(os::WindowModule*, GraphicsEntity* e) {
			ProgressBar* entity = dynamic_cast<ProgressBar*>(e);
			if (entity == nullptr) {
				throw InvalidTypeError("You must queue an ProgressBar for RenderProtocol<ProgressBar>");
			}

			entity->backgroundTexture.bind(0);
			entity->foregroundTexture.bind(1);
			entity->selectionTexture.bind(2);

			entity->buffer.bindForRender();

			renderer.bind();

			renderer.draw(entity);

			ogl::checkGLError(__LINE__, __FILE__, "Error rendering ProgressBar");
		}//render

		void RenderProtocol<ProgressBar>::destroy() {
			renderer.destroy();
		}//destroy

		Font Font::loadFont(const std::string& name) {
			return loadFont(name.c_str());
		}

		Font Font::loadFont(const char* name) {
			if (freetypeStatus < 0) {
				if ((freetypeStatus = FT_Init_FreeType(&freetype)) != FT_Err_Ok) {
					throw FontError("Freetype failed to initialize with error code " + std::to_string(freetypeStatus));
				}

				//font 0 should be null
				fonts.resize(2);
			}

			//on 64 bit systems this cast is required
			Index id = static_cast<Index>(fonts.size());

			fonts.push_back(FT_Face());
			if (int result = FT_New_Face(freetype, name, 0, &fonts[id])) {
				throw FontError("Freetype failed to create font at " + std::string(name) + " with result " + std::to_string(result));
			}

			if (int result = FT_Select_Charmap(fonts[id], FT_ENCODING_UNICODE)) {
				throw FontError("Freetype failed to change charmap with result " + std::to_string(result));
			}

			return Font(id);
		}

		Font Font::loadFontFromMemory(const unsigned char * data, long int size) {
			if (size <= 0) {
				throw IndexOutOfBoundsError("Input size for loadFontFromMemory is less or equal to than 0!");
			}

			if (freetypeStatus < 0) {
				if ((freetypeStatus = FT_Init_FreeType(&freetype)) != FT_Err_Ok) {
					throw FontError("Freetype failed to initailize with error code " + std::to_string(freetypeStatus));
				}

				fonts.resize(2);
			}

			//on 64 bit systems this cast is required
			Index id = static_cast<Index>(fonts.size());

			fonts.push_back(FT_Face());
			if (int result = FT_New_Memory_Face(freetype, data, size, 0, &fonts[id])) {
				throw FontError("Freetype failed to create font from memory with result " + std::to_string(result));
			}

			if (int result = FT_Select_Charmap(fonts[id], FT_ENCODING_UNICODE)) {
				throw FontError("Freetype failed to change charmap with result " + std::to_string(result));
			}

			return Font(id);
		}

		void Font::destroy() {
			if (int result = FT_Done_Face(fonts[id])) {
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

		void Font::getCharacter(const wchar_t c, Letter& character) const {
			if (height <= 0) {
				throw IndexOutOfBoundsError("The height of the font cannot be 0 - you must set it!");
			}

			FT_Set_Pixel_Sizes(fonts[id], 0, height);

			if (int result = FT_Load_Char(fonts[id], c, FT_LOAD_RENDER | FT_LOAD_PEDANTIC | FT_LOAD_TARGET_LIGHT)) {
				throw FontError("Failed to load glyph with error code " + std::to_string(result));
			}

			character.width = fonts[id]->glyph->metrics.width >> 6;
			character.height = fonts[id]->glyph->metrics.height >> 6;
			character.bearingX = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character.bearingY = fonts[id]->glyph->metrics.horiBearingY >> 6;
			character.advanceX = fonts[id]->glyph->advance.x >> 6;
			character.advanceY = fonts[id]->glyph->advance.y >> 6;

			character.mask.init();
			character.mask.bind();

			character.mask.resetPixelStorage();

			character.mask.setPixelStorage(GL_UNPACK_ALIGNMENT, 1);

			character.mask.setData(fonts[id]->glyph->bitmap.buffer, character.width, character.height, GL_UNSIGNED_BYTE, GL_RED, GL_RED);
			character.mask.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			character.mask.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			character.mask.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			character.mask.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

		//font data, compiled in another file to increase compilation time
		extern unsigned char sourceCodeProData[];
		extern unsigned int sourceCodeProLength;

		extern unsigned char sourceSansProData[];
		extern unsigned int sourceSansProLength;

		extern unsigned char sourceSerifProData[];
		extern unsigned int sourceSerifProLength;

		Font::Font(const Fonts f, const Size h) : Font(0, h) {
			if (f == Fonts::CODE) {
				static Font sourceCodePro;

				if (sourceCodePro.getID() == 0) {
					sourceCodePro = Font::loadFontFromMemory(sourceCodeProData, sourceCodeProLength);
				}

				id = sourceCodePro.getID();
			} else if (f == Fonts::SANS) {
				static Font sourceSansPro;

				if (sourceSansPro.getID() == 0) {
					sourceSansPro = Font::loadFontFromMemory(sourceSansProData, sourceSansProLength);
				}

				id = sourceSansPro.getID();
			} else if (f == Fonts::SERIF) {
				static Font sourceSerifPro;

				if (sourceSerifPro.getID() == 0) {
					sourceSerifPro = Font::loadFontFromMemory(sourceSerifProData, sourceSerifProLength);
				}

				id = sourceSerifPro.getID();
			} else {
				//should never be reached, but just to be safe
				throw FontError("Unknown Fonts enum constant");
			}
		}

		Font::Font(const Font & f) : Font(f.id, f.height) {}

		int Letter::getProtocol() {
			return LETTER_PROTOCOL;
		}

		void RenderProtocol<Letter>::init(const Size, const Size) {

			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			renderer.init(
#	include <MACE/Graphics/Shaders/letter.v.glsl>
				,
#	include <MACE/Graphics/Shaders/letter.f.glsl>
			);

			ogl::ShaderProgram& prog = renderer.getShader();
			prog.bind();
			prog.createUniform("mask");
			prog.createUniform("tex");

			prog.setUniform("mask", 0);
			prog.setUniform("tex", 1);

			ogl::checkGLError(__LINE__, __FILE__, "Error creating RenderProtocol<Letter>");
		}//init

		void RenderProtocol<Letter>::initEntity(GraphicsEntity* en) {
			Letter* let = dynamic_cast<Letter*>(en);
			if (let == nullptr) {
				throw InvalidTypeError("Internal error: Input to RenderProtocol<Letter>::initEntity must be of type Letter");
			}

			let->getBuffer().bindToUniformBlock(renderer.getShader(), "textureData");
		}

		void RenderProtocol<Letter>::renderEntity(os::WindowModule*, GraphicsEntity* e) {
			Letter* entity = dynamic_cast<Letter*>(e);
			if (entity == nullptr) {
				throw InvalidTypeError("You must queue an Letter for RenderProtocol<Letter>");
			}

			entity->mask.bind(0);
			entity->texture.bind(1);

			entity->buffer.bindForRender();

			renderer.bind();

			renderer.draw(entity);

			ogl::checkGLError(__LINE__, __FILE__, "Error rendering Letter");
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
			if (!mask.isCreated()) {
				mask.init();
			}

			if (!texture.isCreated()) {
				texture.init();
			}

			if (LETTER_PROTOCOL < 0) {
				LETTER_PROTOCOL = getRenderer()->registerProtocol<Letter>();
			}

			if (!buffer.isCreated()) {
				buffer.init();
			}

			buffer.setLocation(0);

			getRenderer()->initEntity(this, LETTER_PROTOCOL);
		}

		void Letter::onUpdate() {

		}

		void Letter::onRender() {
			getRenderer()->queue(this, LETTER_PROTOCOL);
		}

		void Letter::onDestroy() {
			if (mask.isCreated()) {
				mask.destroy();
			}

			if (texture.isCreated()) {
				texture.destroy();
			}

			if (buffer.isCreated()) {
				buffer.destroy();
			}
		}

		void Letter::onClean() {
			buffer.setData(sizeof(texture.getPaint()), &texture.getPaint());
		}

		Text::Text(const std::string & s, const Font & f) : Text(os::toWideString(s), f) {}

		Text::Text(const std::wstring & t, const Font& f) : Entity2D(), text(t), font(f) {}

		void Text::setText(const std::wstring & newText) {
			if (text != newText) {
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
			if (font != f) {
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

		const std::vector<Letter>& Text::getLetters() const {
			return letters;
		}

		void Text::setVerticalAlign(const VerticalAlign align) {
			if (vertAlign != align) {
				makeDirty();

				vertAlign = align;
			}
		}

		const VerticalAlign Text::getVerticalAlign() const {
			return vertAlign;
		}

		void Text::setHorizontalAlign(const HorizontalAlign align) {
			if (horzAlign != align) {
				makeDirty();

				horzAlign = align;
			}
		}

		const HorizontalAlign Text::getHorizontalAlign() const {
			return horzAlign;
		}

		void Text::setTexture(const ColorAttachment & tex) {
			if (tex != texture) {
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
			if (font.getID() == 0) {
				throw InitializationFailedError("Can\'t render Text with unitialized font!");
			}

			for (Index i = 0; i < letters.size(); ++i) {
				if (hasChild(letters[i])) {
					removeChild(letters[i]);
				}
			}

			letters.clear();

			letters.resize(text.length());

			Entity::Metrics metrics = getMetrics();

			const float widthScale = 1.0f / metrics.scale[0], heightScale = 1.0f / metrics.scale[1];

			const float origWidth = static_cast<const float>(getRenderer()->getOriginalWidth()),
				origHeight = static_cast<const float>(getRenderer()->getOriginalHeight());

			const bool hasKerning = font.hasKerning();

			float x = 0, y = 0;

			float width = 0, height = 0;

			for (Index i = 0; i < text.length(); ++i) {
				if (text[i] == '\n') {
					if (x > width) {
						width = x;
					}

					y -= static_cast<float>(font.getSize()) / origHeight;

					height = y;

					x = 0;
				} else {
					letters[i] = Letter();
					font.getCharacter(text[i], letters[i]);

					//freetype uses absolute values (pixels) and we use relative. so by dividing the pixel by the size, we get relative values
					letters[i].setWidth((static_cast<float>(letters[i].width) / origWidth) * widthScale);
					letters[i].setHeight((static_cast<float>(letters[i].height) / origHeight) * heightScale);

					Vector<float, 2> position = { x, y };

					if (i > 0 && hasKerning) {
						Vector<unsigned int, 2> delta = font.getKerning(text[i - 1], text[i]);

						position[0] += static_cast<float>(delta[0]) / origWidth;
						position[1] += static_cast<float>(delta[1]) / origHeight;
					}

					position[1] += letters[i].getHeight();
					//i cant bear this
					position[1] -= static_cast<float>(letters[i].height - letters[i].bearingY) / origHeight;

					letters[i].setX((position[0] + (static_cast<float>(letters[i].width) / origWidth)));
					letters[i].setY(position[1]);

					letters[i].setOpacity(getOpacity());

					x += static_cast<float>(letters[i].advanceX) / origWidth;
					x += letters[i].getWidth();

					letters[i].texture = this->texture;

					addChild(letters[i]);
				}
			}

			y += static_cast<float>(font.getSize() << 1) / origHeight;

			if (x > width) {
				width = x;
			}

			height += y;

			//alignment has to be calculated after the initial pass so it knows how much screen space the text takes up
			float xAlignment, yAlignment;

			switch (horzAlign) {
				default:
				case HorizontalAlign::CENTER:
					xAlignment = ((-width / 2) + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
				case HorizontalAlign::RIGHT:
					xAlignment = ((1.0f - width) + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
				case HorizontalAlign::LEFT:
					xAlignment = (-1.0f + static_cast<const float>(font.getSize() >> 1) / origWidth);
					break;
			}

			switch (vertAlign) {
				default:
				case VerticalAlign::CENTER:
					yAlignment = 0.0f;
					break;
				case VerticalAlign::BOTTOM:
					yAlignment = ((-1.0f + height / 2) - static_cast<const float>(font.getSize() >> 1) / origHeight);
					break;
				case VerticalAlign::TOP:
					yAlignment = ((1.0f - height) + static_cast<const float>(font.getSize() >> 1) / origHeight);
					break;
			}

			for (Index i = 0; i < letters.size(); ++i) {
				letters[i].translate(xAlignment, yAlignment);
			}
		}

		void RenderProtocol<Button>::init(const Size, const Size) {
			//including shader code inline is hard to edit, and shipping shader code with an executable reduces portability (mace should be able to run without any runtime dependencies)
			//the preprocessor will just copy and paste an actual shader file at compile time, which means that you can use any text editor and syntax highlighting you want
			renderer.init(
#	include <MACE/Graphics/Shaders/button.v.glsl>
				,
#	include <MACE/Graphics/Shaders/button.f.glsl>
			);

			ogl::ShaderProgram& prog = renderer.getShader();
			prog.bind();
			prog.createUniform("backgroundTexture");
			prog.createUniform("hoverTexture");
			prog.createUniform("clickedTexture");
			prog.createUniform("disabledTexture");

			prog.setUniform("backgroundTexture", 0);
			prog.setUniform("hoverTexture", 1);
			prog.setUniform("clickedTexture", 2);
			prog.setUniform("disabledTexture", 3);

			ogl::checkGLError(__LINE__, __FILE__, "Error initializing RenderProtocol<Button>");
		}

		void RenderProtocol<Button>::initEntity(GraphicsEntity* e) {
			Button* but = dynamic_cast<Button*>(e);
			if (but == nullptr) {
				throw InvalidTypeError("Input to RenderProtocol<Button>::initEntity must be of type Button");
			}

			but->getBuffer().bindToUniformBlock(renderer.getShader(), "buttonData");
		}

		void RenderProtocol<Button>::renderEntity(os::WindowModule*, GraphicsEntity* e) {
			Button* entity = dynamic_cast<Button*>(e);
			if (entity == nullptr) {
				throw InvalidTypeError("You must queue a Button for RenderProtocol<Button>");
			}

			entity->texture.bind(0);
			entity->hoverTexture.bind(1);
			entity->clickedTexture.bind(2);
			entity->disabledTexture.bind(2);

			entity->buffer.bindForRender();

			renderer.bind();

			renderer.draw(entity);

			ogl::checkGLError(__LINE__, __FILE__, "Error rendering Button");
		}

		void RenderProtocol<Button>::destroy() {
			renderer.destroy();
		}

		int Button::getProtocol() {
			return BUTTON_PROTOCOL;
		}

		const ColorAttachment & Button::getTexture() const {
			return texture;
		}

		ColorAttachment & Button::getTexture() {
			makeDirty();

			return texture;
		}

		void Button::setTexture(const ColorAttachment & c) {
			if (texture != c) {
				makeDirty();

				texture = c;
			}
		}

		const ColorAttachment & Button::getHoverTexture() const {
			return hoverTexture;
		}

		ColorAttachment & Button::getHoverTexture() {
			makeDirty();

			return hoverTexture;
		}

		void Button::setHoverTexture(const ColorAttachment & c) {
			if (hoverTexture != c) {
				makeDirty();

				hoverTexture = c;
			}
		}

		const ColorAttachment & Button::getClickedTexture() const {
			return clickedTexture;
		}

		ColorAttachment & Button::getClickedTexture() {
			makeDirty();

			return clickedTexture;
		}

		void Button::setClickedTexture(const ColorAttachment & c) {
			if (clickedTexture != c) {
				makeDirty();

				clickedTexture = c;
			}
		}

		const ColorAttachment & Button::getDisabledTexture() const {
			return disabledTexture;
		}

		ColorAttachment & Button::getDisabledTexture() {
			makeDirty();

			return disabledTexture;
		}

		void Button::setDisabledTexture(const ColorAttachment & c) {
			if (disabledTexture != c) {
				makeDirty();

				disabledTexture = c;
			}
		}

		void Button::onInit() {
			if (!texture.isCreated()) {
				texture.init();
			}

			if (!hoverTexture.isCreated()) {
				hoverTexture.init();
			}

			if (!clickedTexture.isCreated()) {
				clickedTexture.init();
			}

			if (!disabledTexture.isCreated()) {
				disabledTexture.init();
			}

			if (BUTTON_PROTOCOL < 0) {
				BUTTON_PROTOCOL = getRenderer()->registerProtocol<Button>();
			}

			if (!buffer.isCreated()) {
				buffer.init();
			}

			buffer.setLocation(0);

			buffer.setData((4 * sizeof(texture.getPaint())) + (3 * sizeof(float)), nullptr);

			getRenderer()->initEntity(this, BUTTON_PROTOCOL);
		}

		void Button::onUpdate() {}

		void Button::onRender() {
			getRenderer()->queue(this, BUTTON_PROTOCOL);
		}

		void Button::onDestroy() {
			if (texture.isCreated()) {
				texture.destroy();
			}

			if (hoverTexture.isCreated()) {
				hoverTexture.destroy();
			}

			if (clickedTexture.isCreated()) {
				clickedTexture.destroy();
			}

			if (disabledTexture.isCreated()) {
				disabledTexture.destroy();
			}

			if (buffer.isCreated()) {
				buffer.destroy();
			}
		}

		void Button::onHover() {
			hover();

			if (os::Input::isKeyDown(os::Input::MOUSE_LEFT)) {
				click();
			}

			if (os::Input::isKeyReleased(os::Input::MOUSE_LEFT) && isClicked()) {
				selectableProperties.setBit(Selectable::CLICKED, false);

				trigger();
			}
		}

		void Button::onClean() {
			const float hovered = selectableProperties.getBit(Selectable::HOVERED) ? 1.0f : 0.0f;
			const float clicked = selectableProperties.getBit(Selectable::CLICKED) ? 1.0f : 0.0f;
			const float disabled = selectableProperties.getBit(Selectable::DISABLED) ? 1.0f : 0.0f;


			Index offset = 0;
			buffer.setDataRange(sizeof(texture.getPaint()), &texture.getPaint(), offset);
			offset += sizeof(texture.getPaint());
			buffer.setDataRange(sizeof(hoverTexture.getPaint()), &hoverTexture.getPaint(), offset);
			offset += sizeof(hoverTexture.getPaint());
			buffer.setDataRange(sizeof(clickedTexture.getPaint()), &clickedTexture.getPaint(), offset);
			offset += sizeof(clickedTexture.getPaint());
			buffer.setDataRange(sizeof(disabledTexture.getPaint()), &disabledTexture.getPaint(), offset);
			offset += sizeof(disabledTexture.getPaint());
			buffer.setDataRange(sizeof(hovered), &hovered, offset);
			offset += sizeof(hovered);
			buffer.setDataRange(sizeof(clicked), &clicked, offset);
			offset += sizeof(clicked);
			buffer.setDataRange(sizeof(disabled), &disabled, offset);
		}
	}//gfx
}//mc
