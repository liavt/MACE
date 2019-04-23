/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__GRAPHICS_COMPONENTS_H
#define MACE__GRAPHICS_COMPONENTS_H

#include <MACE/Graphics/Entity.h>
#include <MACE/Graphics/Renderer.h>
#include <MACE/Graphics/Context.h>

#include <chrono>
#include <queue>
#include <functional>

namespace mc {
	namespace gfx {
		class TexturedEntity2D;

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

		/**
		Function defining a function used in an easing of a value, such as a translation or progress bar.
		@return The value of the ease at a specified timestamp, `t`
		@param t The current tick of the ease
		@param b The beginning progress value of the ease
		@param c The change between the beginning and destination value
		@param d The total time of the ease in frames
		@see EaseFunctions
		*/
		using EaseFunction = std::function<float(float, const float, const float, const float)>;

		/**
		Different easing functions commonly found in applications
		@see EaseFunction
		*/
		namespace EaseFunctions {
			/*MACE__MAKE_EASE_FUNCTION creates function declaration for an ease function of name.
			The reason this is a macro so in the future in case ease functions are ever replaced
			with const lambdas, enums, etc it can be easy to change it without editing too many
			things.

			This macro is used again in Components.cpp for the actual ease function definitions
			*/
#define MACE__MAKE_EASE_FUNCTION(name) float name (float t, const float b, const float c, const float d)
			MACE__MAKE_EASE_FUNCTION(LINEAR);
			MACE__MAKE_EASE_FUNCTION(BACK_IN);
			MACE__MAKE_EASE_FUNCTION(BACK_OUT);
			MACE__MAKE_EASE_FUNCTION(BACK_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(BOUNCE_OUT);
			MACE__MAKE_EASE_FUNCTION(BOUNCE_IN);
			MACE__MAKE_EASE_FUNCTION(BOUNCE_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(CIRCLE_IN);
			MACE__MAKE_EASE_FUNCTION(CIRCLE_OUT);
			MACE__MAKE_EASE_FUNCTION(CIRCLE_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(CUBIC_IN);
			MACE__MAKE_EASE_FUNCTION(CUBIC_OUT);
			MACE__MAKE_EASE_FUNCTION(CUBIC_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(ELASTIC_IN);
			MACE__MAKE_EASE_FUNCTION(ELASTIC_OUT);
			MACE__MAKE_EASE_FUNCTION(ELASTIC_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_IN);
			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_OUT);
			MACE__MAKE_EASE_FUNCTION(EXPONENTIAL_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(QUADRATIC_IN);
			MACE__MAKE_EASE_FUNCTION(QUADRATIC_OUT);
			MACE__MAKE_EASE_FUNCTION(QUADRATIC_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(QUARTIC_IN);
			MACE__MAKE_EASE_FUNCTION(QUARTIC_OUT);
			MACE__MAKE_EASE_FUNCTION(QUARTIC_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(QUINTIC_IN);
			MACE__MAKE_EASE_FUNCTION(QUINTIC_OUT);
			MACE__MAKE_EASE_FUNCTION(QUINTIC_IN_OUT);
			MACE__MAKE_EASE_FUNCTION(SINUSOIDAL_IN);
			MACE__MAKE_EASE_FUNCTION(SINUSOIDAL_OUT);
			MACE__MAKE_EASE_FUNCTION(SINUSOIDAL_IN_OUT);
			//Components.cpp needs MACE__MAKE_EASE_FUNCTION, so it defines this macro to expose it
#ifndef MACE__COMPONENTS_EXPOSE_MAKE_EASE_FUNCTION
#	undef MACE__MAKE_EASE_FUNCTION
#endif
		}

		class MACE_NOVTABLE Progressable {
		public:
			virtual ~Progressable() = default;

			virtual void setProgress(const float prog) = 0;

			/**
			@copydoc Progressable::getProgress() const
			@dirty
			*/
			virtual float& getProgress() = 0;
			virtual const float& getProgress() const = 0;

			void addProgress(const float prog);
			void removeProgress(const float prog);

			virtual float getMinimum() = 0;
			virtual const float getMinimum() const = 0;

			virtual float getMaximum() = 0;
			virtual const float getMaximum() const = 0;
		};

		class ComponentQueue: public Component {
		public:
			ComponentQueue(std::queue<std::shared_ptr<Component>> com);
			ComponentQueue();

			void addComponent(Component& com);
			void addComponent(Component* com);
			void addComponent(std::shared_ptr<Component> com);

			std::queue<std::shared_ptr<Component>>& getComponents();
			const std::queue<std::shared_ptr<Component>>& getComponents() const;
			void setComponents(const std::queue<std::shared_ptr<Component>>& other);

			bool operator==(const ComponentQueue& other) const;
			bool operator!=(const ComponentQueue& other) const;
		protected:
			void init() final;
			bool update() final;
			void render() final;
			void destroy() final;
			void hover() final;
			void clean() final;
		private:
			std::queue<std::shared_ptr<Component>> components;
		};

		class AlignmentComponent: public Component {
		public:
			AlignmentComponent(const VerticalAlign vert = VerticalAlign::CENTER, const HorizontalAlign horz = HorizontalAlign::CENTER);

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

			bool operator==(const AlignmentComponent& other) const;
			bool operator!=(const AlignmentComponent& other) const;
		protected:
			void clean() final;
		private:
			VerticalAlign vertAlign;
			HorizontalAlign horzAlign;
		};

		struct EaseSettings {
		public:
			using EaseDoneCallback = std::function<void(Entity*)>;

			EaseDoneCallback done = [](Entity*) {};
			EaseFunction ease = EaseFunctions::SINUSOIDAL_OUT;
			long long ms = 1000;
			/**
			How many times the Ease will repeat.
			Set to -1 to repeat infinitely.
			*/
			signed long repeats = 1;
			bool reverseOnRepeat = false;

			bool operator==(const EaseSettings& other) const;
			bool operator!=(const EaseSettings& other) const;
		};

		class EaseComponent: public Component, public Progressable {
		public:
			using EaseUpdateCallback = std::function<void(Entity*, float)>;

			EaseComponent(const EaseUpdateCallback callback, const EaseSettings settings = EaseSettings(), const float start = 0.0f, const float dest = 1.0f);

			void setProgress(const float prog) override;

			float& getProgress() override;
			const float& getProgress() const override;

			float getMinimum() override;
			const float getMinimum() const override;

			float getMaximum() override;
			const float getMaximum() const override;

			bool operator==(const EaseComponent& other) const;
			bool operator!=(const EaseComponent& other) const;
		protected:
			void init() override;
			bool update() override;
			void render() override;
			void destroy() override;
		private:
			const EaseSettings settings;

			std::chrono::time_point<std::chrono::steady_clock> startTime;
			float progress;
			signed long currentRepetition;
			std::chrono::duration<float> duration;
			const EaseUpdateCallback updateCallback;
			const float startingProgress;
			const float destination;
		};

		class TweenComponent: public EaseComponent {
		public:
			TweenComponent(Entity* const en, const TransformMatrix dest, const EaseSettings settings);
			TweenComponent(Entity* const en, const TransformMatrix start, const TransformMatrix dest, const EaseSettings settings);

			Entity* const getEntity();
			const Entity* const getEntity() const;

			bool operator==(const TweenComponent& other) const;
			bool operator!=(const TweenComponent& other) const;
		private:
			Entity * const entity;
		};

		class CallbackComponent: public Component {
		public:
			using CallbackPtr = std::function<void(Entity*)>;
			using UpdatePtr = std::function<bool(Entity*)>;

			void setInitCallback(const CallbackPtr func);
			CallbackPtr getInitCallback();
			const CallbackPtr getInitCallback() const;

			void setUpdateCallback(const UpdatePtr func);
			UpdatePtr getUpdateCallback();
			const UpdatePtr getUpdateCallback() const;

			void setRenderCallback(const CallbackPtr func);
			CallbackPtr getRenderCallback();
			const CallbackPtr getRenderCallback() const;

			void setDestroyCallback(const CallbackPtr func);
			CallbackPtr getDestroyCallback();
			const CallbackPtr getDestroyCallback() const;

			void setHoverCallback(const CallbackPtr func);
			CallbackPtr getHoverCallback();
			const CallbackPtr getHoverCallback() const;

			void setCleanCallback(const CallbackPtr func);
			CallbackPtr getCleanCallback();
			const CallbackPtr getCleanCallback() const;

			bool operator==(const CallbackComponent& other) const;
			bool operator!=(const CallbackComponent& other) const;
		protected:
			void init() final;
			bool update() final;
			void render() final;
			void destroy() final;
			void hover() final;
			void clean() final;
		private:
			CallbackPtr destroyCallback = [](Entity*) {},
				renderCallback = [](Entity*) {},
				initCallback = [](Entity*) {},
				hoverCallback = [](Entity*) {},
				cleanCallback = [](Entity*) {};
			UpdatePtr updateCallback = [](Entity*) -> bool {
				return false;
			};
		};//CallbackEntity

		class FPSComponent: public Component {
		public:
			using TickCallback = std::function<void(FPSComponent*, Entity*)>;

			unsigned int getUpdatesPerSecond() const;
			unsigned int getFramesPerSecond() const;
			unsigned int getCleansPerSecond() const;
			unsigned int getHoversPerSecond() const;

			void setTickCallback(const TickCallback callback);
			TickCallback getTickCallback();
			const TickCallback getTickCallback() const;

			bool operator==(const FPSComponent& other) const;
			bool operator!=(const FPSComponent& other) const;
		private:
			unsigned int updatesPerSecond = 0, nbUpdates = 0;
			unsigned int framesPerSecond = 0, nbFrames = 0;
			unsigned int cleansPerSecond = 0, nbCleans = 0;
			unsigned int hoversPerSecond = 0, nbHovers = 0;

			TickCallback tickCallback = [](FPSComponent*, Entity*) {};

			std::chrono::time_point<std::chrono::steady_clock> lastTime = std::chrono::steady_clock::now();

			void init() final;
			bool update() final;
			void render() final;
			void clean() final;
			void hover() final;
			void destroy() final;
		};//FPSComponent

		//the Return argument is to allow functions with return types to be used without a wrapper
		template<typename T, typename Return = void>
		class ConstraintComponent: public Component {
		public:
			using ConstraintCallback = std::function<Return(T)>;

			ConstraintComponent(const T constraint = T(), const ConstraintCallback constrainFunc = [](T) -> Return {})
				: constraint(constraint), constraintCallback(constrainFunc) {}

			~ConstraintComponent() = default;

			T getConstraint() {
				return constraint;
			}

			const T getConstraint() const {
				return constraint;
			}

			void setConstraint(const T val) {
				constraint = val;
			}

			ConstraintCallback getConstraintCallback() {
				return constraintCallback;
			}

			const ConstraintCallback getConstraintCallback() const {
				return constraintCallback;
			}

			void setConstraintCallback(const ConstraintCallback callback) {
				constraintCallback = callback;
			}

			bool operator==(const ConstraintComponent& other) {
				return constraint == other.constraint;
			}
			bool operator!=(const ConstraintComponent& other) {
				return !operator==(other);
			}
			bool operator>(const ConstraintComponent& other) {
				return constraint > other.constraint;
			}
			bool operator>=(const ConstraintComponent& other) {
				return operator>(other) || operator==(other);
			}
			bool operator<(const ConstraintComponent& other) {
				return !operator>=(other);
			}
			bool operator<=(const ConstraintComponent& other) {
				return !operator>(other);
			}
		private:
			void clean() final {
				constraintCallback(constraint);
			}

			ConstraintCallback constraintCallback;

			T constraint;
		};

		class XAxisConstraintComponent final: public ConstraintComponent<float> {
		public:
			XAxisConstraintComponent(const float constraint = 0.0f) : ConstraintComponent<float>(constraint, [&](float val) {
				parent->setX(val);
			}) {}
		};

		class YAxisConstraintComponent final: public ConstraintComponent<float> {
		public:
			YAxisConstraintComponent(const float constraint = 0.0f) : ConstraintComponent<float>(constraint, [&](float val) {
				parent->setY(val);
			}) {}
		};

		class ZAxisConstraintComponent final: public ConstraintComponent<float> {
		public:
			ZAxisConstraintComponent(const float constraint = 0.0f) : ConstraintComponent<float>(constraint, [&](float val) {
				parent->setZ(val);
			}) {}
		};

		class BoundsComponent: public Component {
		public:
			using BoundsReachedCallback = std::function<void(Entity*, BoundsComponent*)>;

			BoundsComponent(const Vector<float, 2> boundsX, const Vector<float, 2> boundsY,
							const Vector<float, 2> boundsZ,
							const BoundsReachedCallback boundsReached = [](Entity*, BoundsComponent*) {});
			BoundsComponent(const float minX = -1.0f, const float maxX = 1.0f, const float minY = -1.0f,
							const float maxY = 1.0f, const float minZ = -1.0f, const float maxZ = 1.0f,
							const BoundsReachedCallback boundsReached = [](Entity*, BoundsComponent*) {});

			BoundsReachedCallback getCallback();
			const BoundsReachedCallback getCallback() const;
			void setCallback(const BoundsReachedCallback boundsReached);

			Vector<float, 2>& getBoundsX();
			const Vector<float, 2>& getBoundsX() const;
			void setBoundsX(const Vector<float, 2>& vec);

			Vector<float, 2>& getBoundsY();
			const Vector<float, 2>& getBoundsY() const;
			void setBoundsY(const Vector<float, 2>& vec);

			Vector<float, 2>& getBoundsZ();
			const Vector<float, 2>& getBoundsZ() const;
			void setBoundsZ(const Vector<float, 2>& vec);

			bool operator==(const BoundsComponent& other);
			bool operator!=(const BoundsComponent& other);
			bool operator>(const BoundsComponent& other);
			bool operator>=(const BoundsComponent& other);
			bool operator<(const BoundsComponent& other);
			bool operator<=(const BoundsComponent& other);
		private:
			void clean() final;

			Vector<float, 2> boundsX, boundsY, boundsZ;

			BoundsReachedCallback callback;
		};

		class TextureFramesComponent: public Component,public Progressable {
		public:
			using FrameCallback = std::function<void(const Texture&, Entity*, TextureFramesComponent*)>;

			TextureFramesComponent(const std::vector<Texture>& tex, const FrameCallback callback);

			void setProgress(const float prog) override;

			float& getProgress() override;
			const float& getProgress() const override;

			float getMinimum() override;
			const float getMinimum() const override;

			float getMaximum() override;
			const float getMaximum() const override;

			bool operator==(const TextureFramesComponent& other) const;
			bool operator!=(const TextureFramesComponent& other) const;
		private:
			void init() final;
			bool update() final;

			const FrameCallback callback;
			
			float progress;

			const std::vector<Texture> frames;
		};
	}//gfx
}//mc

#endif