/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__GRAPHICS_RENDERER_H
#define MACE__GRAPHICS_RENDERER_H

#include <MACE/Core/Interfaces.h>
#include <MACE/Core/Error.h>
#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Window.h>
#include <MACE/Graphics/Context.h>
#include <MACE/Utility/Vector.h>
#include <MACE/Utility/Transform.h>
#include <MACE/Utility/Color.h>

#include <unordered_map>
#include <stack>

namespace mc {
	namespace gfx {
		//forward declare dependencies
		class Renderer;
		class GraphicsEntity;
		class PainterImpl;
		struct EaseSettings;

		enum class FrameBufferTarget: int {
			COLOR = 0,
			DATA = 1
		};

		class Painter: public Beginable, private Initializable {
			friend class GraphicsEntity;
			friend class PainterImpl;
			friend class Renderer;
			friend class PainterComponent;
		public:
			enum class Brush: Byte {
				/**
				Renders the foreground texture as-is
				*/
				TEXTURE = 0,
				/**
				Renders the foreground color as-is
				*/
				COLOR = 1,
				/**
				Renders the foreground texture masked by the mask texture
				*/
				MASK = 2,
				/**
				Renders either the foreground or background texture based
				on the value of the mask texture.
				<br>
				`data[0]` and `data[1]` define a range,
				where `data[0]` is the lower bound (must be >= 0.0f)
				and `data[1]` is the upper bound (must be <= 1.0f).
				<br>
				If the value of the mask texture is within the range,
				the foreground texture is rendered. Otherwise the
				backgrond texture is rendered.
				<br>
				This is used in the rendering of the `ProgressBar`.
				*/
				CONDITIONAL_MASK = 3,
				/**
				Blends between the foreground and background textures.
				<br>
				The amount of blending performed depends on `data[0]`
				(a value of 0 means 100% foreground and 0% background,
				while a value of 1 means 0% foreground and 100% background,
				with values in between creating a linear interpreation)
				*/
				BLEND = 4,
				/**
				Blends the foregound texture onto the render target using
				the background texture as a multicomponent alpha target.
				<br>
				This is used in the subpixel rendering of text.
				<br>
				Due to the nature of this `Brush`, the value of
				`RenderFeature::STORE_ID` is ignored and the `Painter` acts
				as if that `RenderFeature` is `false`
				*/
				MULTICOMPONENT_BLEND = 5,
			};

			enum class RenderFeatures: Byte {
				DISCARD_INVISIBLE = 0x01,
				FILTER = 0x02,
				TEXTURE = 0x04,
				TEXTURE_TRANSFORM = 0x08,
				STORE_ID = 0x10,

				NONE = 0x00,
				DEFAULT = FILTER | TEXTURE | TEXTURE_TRANSFORM | STORE_ID,
			};

			struct State {
				Color foregroundColor, backgroundColor, maskColor;

				Vector<RelativeUnit, 4> foregroundTransform, backgroundTransform, maskTransform;

				Vector<float, 4> data;

				Transformation transformation;

				Matrix<float, 4, 4> filter = math::identity<float, 4>();

				RenderFeatures renderFeatures = RenderFeatures::DEFAULT;

				bool operator==(const State& other) const;
				bool operator!=(const State& other) const;
			};

			void drawModel(const Model& m, const Texture& img);

			void fillModel(const Model& m);

			void fillRect(const RelativeTranslation x = 0.0f, const RelativeTranslation y = 0.0f, const RelativeScale w = 1.0f, const RelativeScale h = 1.0f);
			void fillRect(const Vector<RelativeTranslation, 2> & pos, const Vector<RelativeScale, 2> & size);
			void fillRect(const Vector<RelativeUnit, 4> & dim);

			void drawImage(const Texture& img);

			void maskImage(const Texture& img, const Texture& mask);

			void conditionalMaskImages(const Texture& foreground, const Texture& background, const Texture& mask, const float minimumThreshold = 0.0f, const float maximumThreshold = 1.0f);

			void blendImages(const Texture& foreground, const Texture& background, const Progress amount = 0.5f);

			void drawQuad(const Brush brush);
			void draw(const Model& m, const Brush brush);

			const Entity* const getEntity() const;

			void setTexture(const Texture& t, const TextureSlot slot = TextureSlot::FOREGROUND);

			void setForegroundColor(const Color& col);
			Color& getForegroundColor();
			const Color& getForegroundColor() const;

			void setForegroundTransform(const Vector<RelativeUnit, 4> & trans);
			Vector<RelativeUnit, 4> & getForegroundTransform();
			const Vector<RelativeUnit, 4> & getForegroundTransform() const;

			void setBackgroundColor(const Color& col);
			Color& getBackgroundColor();
			const Color& getBackgroundColor() const;

			void setBackgroundTransform(const Vector<RelativeUnit, 4> & trans);
			Vector<RelativeUnit, 4> & getBackgroundTransform();
			const Vector<RelativeUnit, 4> & getBackgroundTransform() const;

			void setMaskColor(const Color& col);
			Color& getMaskColor();
			const Color& getMaskColor() const;

			void setMaskTransform(const Vector<RelativeUnit, 4> & trans);
			Vector<RelativeUnit, 4> & getMaskTransform();
			const Vector<RelativeUnit, 4> & getMaskTransform() const;

			void enableRenderFeatures(const RenderFeatures feature);
			void disableRenderFeatures(const RenderFeatures feature);
			void setRenderFeatures(const RenderFeatures feature);
			RenderFeatures& getRenderFeatures();
			const RenderFeatures& getRenderFeatures() const;

			void setFilter(const float r, const float g, const float b, const float a = 1.0f);
			void setFilter(const Vector<float, 4> & col);
			void setFilter(const Matrix<float, 4, 4> & col);
			Matrix<float, 4, 4> & getFilter();
			const Matrix<float, 4, 4> & getFilter() const;

			void setData(const float a, const float b, const float c, const float d);
			void setData(const Vector<float, 4> & col);
			Vector<float, 4> & getData();
			const Vector<float, 4> & getData() const;

			void setTransformation(const Transformation& trans);
			Transformation& getTransformation();
			const Transformation& getTransformation() const;

			void setOpacity(const float opacity);
			float getOpacity();
			const float getOpacity() const;

			void setTarget(const FrameBufferTarget& target);

			void translate(const Vector<RelativeTranslation, 3> & vec);
			void translate(const RelativeTranslation x, const RelativeTranslation y, const RelativeTranslation z = 0.0f);

			void rotate(const Vector<RelativeRadian, 3> & vec);
			void rotate(const RelativeRadian x, const RelativeRadian y, const RelativeRadian z);

			void scale(const Vector<RelativeScale, 3> & vec);
			void scale(const RelativeScale x, const RelativeScale y, const RelativeScale z = 1.0f);

			void resetTransform();

			void push();
			void pop();

			void reset();

			void setState(const State& s);
			State& getState();
			const State& getState() const;

			bool operator==(const Painter& other) const;
			bool operator!=(const Painter& other) const;
		private:
			std::shared_ptr<PainterImpl> impl = nullptr;

			Painter::State state = Painter::State();

			//for pushing/popping the state
			std::stack<Painter::State> stateStack{};

			Entity* entity;

			Painter();
			Painter(Entity* const en, const std::shared_ptr<PainterImpl> im);
			Painter(Entity* const en, const Painter& other);

			void begin() override;
			void end() override;

			void init() override;
			void destroy() override;

			void clean();
		};

		MACE_CONSTEXPR inline Painter::RenderFeatures operator|(const Painter::RenderFeatures& left, const Painter::RenderFeatures& right) {
			return static_cast<Painter::RenderFeatures>(static_cast<Byte>(left) | static_cast<Byte>(right));
		}

		MACE_CONSTEXPR inline Painter::RenderFeatures operator&(const Painter::RenderFeatures& left, const Painter::RenderFeatures& right) {
			return static_cast<Painter::RenderFeatures>(static_cast<Byte>(left) & static_cast<Byte>(right));
		}

		MACE_CONSTEXPR inline Painter::RenderFeatures operator~(const Painter::RenderFeatures& r) {
			return static_cast<Painter::RenderFeatures>(~static_cast<Byte>(r));
		}

		class MACE_NOVTABLE PainterImpl: public Initializable, public Beginable {
			friend class Renderer;
			friend class Painter;
		public:
			virtual MACE__DEFAULT_OPERATORS(PainterImpl);

			virtual void init() override = 0;
			virtual void destroy() override = 0;

			virtual void begin() override = 0;
			virtual void end() override = 0;

			virtual void clean() = 0;

			virtual void setTarget(const FrameBufferTarget& target) = 0;

			virtual void loadSettings(const Painter::State& state) = 0;
			virtual void draw(const Model& m, const Painter::Brush brush) = 0;

			bool operator==(const PainterImpl& other) const;
			bool operator!=(const PainterImpl& other) const;
		protected:
			Painter* painter = nullptr;

			PainterImpl() noexcept = default;
		};

		/**
		@todo add function to change how many samples msaa uses
		@todo add renderers for directx, cpu, vulkan, opengl es, opengl 1.1/2.1
		*/
		class MACE_NOVTABLE Renderer {
			friend class Painter;
			friend class GraphicsContextComponent;
			friend class WindowModule;
			friend class GraphicsEntity;
			friend class PainterComponent;
		public:
			virtual MACE__DEFAULT_OPERATORS(Renderer);

			EntityID getEntityAt(const RelativeTranslation x, const RelativeTranslation y) const;
			EntityID getEntityAt(const Pixels x, const Pixels y) const;


			virtual void getEntitiesAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, EntityID* arr) const = 0;
			template<Size W, Size H>
			inline void getEntitiesAt(const Pixels x, const Pixels y, EntityID arr[W][H]) const {
				getEntitiesAt(x, y, W, H, arr);
			}

			virtual void setRefreshColor(const float r, const float g, const float b, const float a = 1.0f) = 0;

			Color getPixelAt(const RelativeTranslation x, const RelativeTranslation y, const FrameBufferTarget target = FrameBufferTarget::COLOR) const;
			Color getPixelAt(const Pixels x, const Pixels y, const FrameBufferTarget target = FrameBufferTarget::COLOR) const;

			virtual void getPixelsAt(const Pixels x, const Pixels y, const Pixels w, const Pixels h, Color* arr, const FrameBufferTarget target = FrameBufferTarget::COLOR) const = 0;
			template<Pixels W, Pixels H>
			inline void getPixelsAt(const Pixels x, const Pixels y, Color arr[W][H], const FrameBufferTarget target = FrameBufferTarget::COLOR) const {
				getPixelsAt(x, y, W, H, arr, target);
			}

			void setRefreshColor(const Color& c);

			Pixels getWidth() const;
			Pixels getHeight() const;

			unsigned int getSamples() const;

			Vector<float, 2> getWindowRatios() const;

			bool isResized() const;


			/**
			@internal
			@todo remove this from the public interface (see glfwWindowResized in Window.cpp for why this is a problem)
			*/
			void flagResize();
		protected:
			unsigned int samples = 1;

			bool resized = false;

			Vector<float, 2> windowRatios;

			Renderer() noexcept = default;

			virtual void onResize(gfx::WindowModule* win, const Pixels width, const Pixels height) = 0;
			virtual void onInit(gfx::WindowModule* win) = 0;
			virtual void onSetUp(gfx::WindowModule* win) = 0;
			virtual void onTearDown(gfx::WindowModule* win) = 0;
			virtual void onDestroy() = 0;
			virtual void onQueue(Entity* en) = 0;

			//not declared const because some of the functions require modification to an internal buffer of impls
			virtual std::shared_ptr<PainterImpl> createPainterImpl() = 0;
		private:
			Pixels currentWidth = 0, currentHeight = 0;

			/**
			@internal
			@rendercontext
			*/
			void resize(gfx::WindowModule* win, const Pixels width, const Pixels height);

			/**
			@internal
			@rendercontext
			*/
			void init(gfx::WindowModule* win);

			/**
			@internal
			@rendercontext
			*/
			void setUp(gfx::WindowModule* win);

			/**
			@internal
			@rendercontext
			*/
			void tearDown(gfx::WindowModule* win);

			/**
			@internal
			@rendercontext
			*/
			void checkInput(gfx::WindowModule* win);


			/**
			@internal
			@rendercontext
			*/
			void destroy();

			void queue(Entity* const e, Painter& p);
		};//Renderer

		class MACE_NOVTABLE GraphicsEntity: public virtual Entity {
		public:
			GraphicsEntity() noexcept;

			virtual ~GraphicsEntity() noexcept override = default;

			/**
			@dirty
			*/
			Painter& getPainter();
			const Painter& getPainter() const;

			ComponentPtr<GraphicsContextComponent> getContext();
			const ComponentPtr<GraphicsContextComponent> getContext() const;

			bool operator==(const GraphicsEntity& other) const noexcept;
			bool operator!=(const GraphicsEntity& other) const noexcept;
		protected:
			virtual void onRender(Painter& painter) = 0;

			void clean() override final;

			void init() override final;

			void destroy() override final;
		private:
			Painter painter;

			void onRender() override final;
		};//GraphicsEntity

		class PainterComponent: public Component {
		public:
			Painter& getPainter();
			const Painter& getPainter() const;
		private:
			Painter painter;

			void clean(Metrics& metrics) override final;

			void init() override final;

			void destroy() override final;
		};
	}//gfx
}//mc

#endif
