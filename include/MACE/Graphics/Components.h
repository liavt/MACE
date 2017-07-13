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
			float LINEAR(float, const float, const float, const float);
			float BACK_IN(float, const float, const float, const float);
			float BACK_OUT(float, const float, const float, const float);
			float BACK_IN_OUT(float, const float, const float, const float);
			float BOUNCE_OUT(float, const float, const float, const float);
			float BOUNCE_IN(float, const float, const float, const float);
			float BOUNCE_IN_OUT(float, const float, const float, const float);
			float CIRCLE_IN(float, const float, const float, const float);
			float CIRCLE_OUT(float, const float, const float, const float);
			float CIRCLE_IN_OUT(float, const float, const float, const float);
			float CUBIC_IN(float, const float, const float, const float);
			float CUBIC_OUT(float, const float, const float, const float);
			float CUBIC_IN_OUT(float, const float, const float, const float);
			float ELASTIC_IN(float, const float, const float, const float);
			float ELASTIC_OUT(float, const float, const float, const float);
			float ELASTIC_IN_OUT(float, const float, const float, const float);
			float EXPONENTIAL_IN(float, const float, const float, const float);
			float EXPONENTIAL_OUT(float, const float, const float, const float);
			float EXPONENTIAL_IN_OUT(float, const float, const float, const float);
			float QUADRATIC_IN(float, const float, const float, const float);
			float QUADRATIC_OUT(float, const float, const float, const float);
			float QUADRATIC_IN_OUT(float, const float, const float, const float);
			float QUARTIC_IN(float, const float, const float, const float);
			float QUARTIC_OUT(float, const float, const float, const float);
			float QUARTIC_IN_OUT(float, const float, const float, const float);
			float QUINTIC_IN(float, const float, const float, const float);
			float QUINTIC_OUT(float, const float, const float, const float);
			float QUINTIC_IN_OUT(float, const float, const float, const float);
			float SINUSOIDAL_IN(float, const float, const float, const float);
			float SINUSOIDAL_OUT(float, const float, const float, const float);
			float SINUSOIDAL_IN_OUT(float, const float, const float, const float);
		}

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

		class EaseComponent: public Component {
		public:
			typedef void(*EaseDoneCallback)(Entity*);
			typedef void(*EaseUpdateCallback)(Entity*, float);

			EaseComponent(const float duration, const float startingProgress, const float destination, const EaseUpdateCallback callback, const EaseFunction easeFunction = EaseFunctions::SINUSOIDAL_OUT, const EaseDoneCallback done = [](Entity*) {});

			bool operator==(const EaseComponent& other) const;
			bool operator!=(const EaseComponent& other) const;
		protected:
			void init() override;
			bool update() override;
			void render() override;
			void destroy() override;
		private:
			float t;
			const float b;
			const float c;
			const float d;
			const EaseUpdateCallback updateCallback;
			const EaseFunction ease;
			const EaseDoneCallback done;
		};

		class CallbackComponent: public Component {
		public:
			typedef void(*CallbackPtr)(Entity*);
			typedef bool(*UpdatePtr)(Entity*);

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
			typedef void(*TickCallbackPtr)(FPSComponent*, Entity*);

			Index getUpdatesPerSecond() const;
			Index getFramesPerSecond() const;
			Index getCleansPerSecond() const;
			Index getHoversPerSecond() const;

			void setTickCallback(const TickCallbackPtr callback);
			TickCallbackPtr getTickCallback();
			const TickCallbackPtr getTickCallback() const;

			bool operator==(const FPSComponent& other) const;
			bool operator!=(const FPSComponent& other) const;
		protected:
			void init() final;
			bool update() final;
			void render() final;
			void clean() final;
			void hover() final;
			void destroy() final;
		private:
			Index updatesPerSecond = 0, nbUpdates = 0;
			Index framesPerSecond = 0, nbFrames = 0;
			Index cleansPerSecond = 0, nbCleans = 0;
			Index hoversPerSecond = 0, nbHovers = 0;

			TickCallbackPtr tickCallback = [](FPSComponent*, Entity*) {};

			time_t lastTime = 0;
		};//FPSComponent
	}//gfx
}//mc

#endif