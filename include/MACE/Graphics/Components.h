/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_GRAPHICS_COMPONENTS_H
#define MACE_GRAPHICS_COMPONENTS_H

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
			CallbackPtr destroyCallback = [] (Entity*) {},
				renderCallback = [] (Entity*) {},
				initCallback = [] (Entity*) {},
				hoverCallback = [] (Entity*) {},
				cleanCallback = [] (Entity*) {};
			UpdatePtr updateCallback = [] (Entity*) -> bool {
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

			TickCallbackPtr tickCallback = [] (FPSComponent*, Entity*) {};

			time_t lastTime = 0;
		};//FPSComponent
	}//gfx
}//mc

#endif