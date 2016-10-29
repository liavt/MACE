/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#include <MACE/System/System.h>
#include <MACE/Window/Window.h>
#include <thread>

namespace mc {
	namespace win
	{
		class GraphicsContext {
		public:
			virtual void setUpWindow(Window* win) = 0;
			virtual void init(Window* win) = 0;
			virtual void render(Window* win) = 0;
			virtual void destroy(Window* win) = 0;
			virtual void update() = 0;
		};

		class WindowModule : public Module {
			Window* window;

			bool destroyed = false;

			std::thread windowThread;

			void threadCallback();

			GraphicsContext* context =	0;//initailize an empty context
		public:
			WindowModule(Window* window);

			void init();
			void update();
			void destroy();

			void setContext(GraphicsContext* con);
			GraphicsContext* getContext();
			const GraphicsContext* getContext() const;

			std::string getName() const;
		};
	}
}