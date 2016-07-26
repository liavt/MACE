/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia and Shahar Sandhaus

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once

#include <MC-System/System.h>
#include <MC-Window/WindowModule.h>
#include <MC-Graphics/Entity.h>
#include <thread>
#include <mutex>

namespace mc {

	/**
	Namespace containing classes used for Graphics in MACE.
	*/
	namespace gfx{

		class GraphicsModule : public mc::gfx::EntityModule {
			win::Window* window;
			
			bool destroyed = false;

			//threading
			std::thread graphicsThread;
			std::mutex mutex;
			//this is called by the thread
			void graphicsThreadCallback();
		public:
			GraphicsModule(win::Window* window);

			using EntityModule::EntityModule;

			void init();
			void update();
			void destroy();
			std::string getName() const;
		};
	}
}

