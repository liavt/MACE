/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#pragma once
#ifndef MACE_CORE_MODULE_H
#define MACE_CORE_MODULE_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>
#include <string>

namespace mc {
	/**
	Abstract class used for doing a task in MACE. Plugged into {@link MACE} via {@link MACE#addModule(Module&) addModule().}
	*/
	class Module: public Initializable {
	public:
		virtual ~Module() = default;

		/**
		Called when {@link MACE#init} is called and this `Module` is registered.
		<p>
		Use this to initialize variables, start streams, or launch a context.
		<p>
		Should only be called once.
		*/
		virtual void init() = 0;
		/**
		Called when {@link MACE#update} is called and this `Module` is registered.
		<p>
		Use this update states, send messages, swap graphics buffers, render, process, or anything that needs to be periodically checked.
		*/
		virtual void update() = 0;
		/**
		Called when {@link MACE#destroy} is called and this `Module` is registered.
		<p>
		Use this to clear memory, close streams, or clean up anything that needs to be cleaned up.
		<p>
		Should only be called before the program is closed.
		*/
		virtual void destroy() = 0;

		/**
		Override this function and return a UNIQUE name for this `Module`
		<p>
		The name for your `Module` is used in comparison, so make sure it is as unique as possible.
		<p>
		It is akin to a hashcode.
		*/
		virtual std::string getName() const = 0;
	};



	/**
	Core class of MACE, managing `Modules`. `init()` should be called after all `Modules` are added and before the main loop. `update()` should be called in the loop, and `destroy()` should be called at the end of your program.
	<p>
	If `isRunning()` returns `false`, you should end your program and call `destroy()`
	<p>
	Your main loop should look like this:{@code

	//add modules that you need

	mc::MACE::init();

	while(mc::MACE::isRunning()){
		mc::MACE::update();
	}

	mc::MACE::destroy();

	}
	*/
	namespace MACE {
		enum Flags: Byte {
			/**
			Parameter for `MACE.getFlag(const Byte)`. Is `true` if `MACE.init()` has been called.
			*/
			INIT = 0,
			/**
			Parameter for `MACE.getFlag(const Byte)`. Is `true` if `MACE.destroy()` has been called.
			*/
			DESTROYED = 1,
			/**
			Parameter for `MACE.getFlag(const Byte)`. Is `true` if `MACE.requestStop()` has been called.
			*/
			STOP_REQUESTED = 2
		};

		/**
		Register a {@link Module}.
		<p>
		Registered `Modules` will have `update(), init(),` and `destroy()` are respectivaly called when `MACE.update(), MACE.init(),` and `MACE.destroy()` are called.
		@param m Reference to a `Module.` If the `Module` referenced leaves scope or gets deallocated, a `read access violation will occur.`
		@return Location of the `Module` in the sslBuffer. This index can be used in various other methods.
		*/
		Index addModule(Module& m);
		/**
		Remove a `Module` by reference. This is generally more safe than `removeModule(std::string)` as this checks by reference, not by name.
		@param m Reference to Module in the sslBuffer.
		@throws ObjectNotFoundInArray if the referenced `Module` doesn't exist.
		*/
		void removeModule(const Module& m);
		/**
		Remove a `Module` by name.
		<p>
		It will find the `Module` based on their {@link Module#getName() getName()} function, so if there are multiple `Modules` with the same name, this function may produce unexpected behavior.
		@param module Name of a `Module`
		@throws ObjectNotFoundInArray if a `Module` with the given name doesn't exist.
		*/
		void removeModule(const std::string module);
		/**
		Remove a `Module` by it's index. The index is provided from MACE::addModule(Module&) or MACE::indexOf(Module&)
		@throw IndexOutOfBounds if `i<0` or `i>numberOfModules()`
		@param i Index of a `Module` to delete
		*/
		void removeModule(const Index i);
		/**
		Retrieve a pointer to a `Module` with the specified name.
		<p>
		It will find the `Module` based on their {@link Module#getName() getName()} function, so if there are multiple `Modules` with the same name, this function may produce unexpected behavior.
		@throw ObjectNotFoundInArray if there is no `Module` with the specified name
		@return A `Module` whose `getName()` function returns the specified keyword
		@param keyword Name to look for
		@see #getModule(Index)
		*/
		Module* getModule(const std::string keyword);
		/**
		Retrieves the `Module` at a certain index.
		@return A pointer to a `Module` at the specified location
		@param i Valid index to a `Module`
		@throw IndexOutOfBounds if `i<0` or `i>numberOfModules()`
		*/
		Module* getModule(const Index i);
		/**
		Checks whether a `Module` exists via it's `getName()` function.
		@param module Name to search for
		@return `true` if there is a `Module` with the specified name, `false` otherwise.
		*/
		bool moduleExists(const std::string module);

		/**
		Checks whether a `Module` exists via a pointer.
		@param module `Module` to search for
		@return `true` if the `Module` exists, `false` otherwise.
		*/
		bool moduleExists(const Module* module);
		/**
		Retrieves the amount of `Module` currently being updated by `MACE`
		@return `Size` of the internal `Module` sslBuffer
		*/
		Size numberOfModules();
		/**
		Retrieves the location of a `Module` in the sslBuffer.
		<p>
		Equivalent to calling {@code
			indexOf(m.getName());
		}
		@param m Module to find the index of.
		@return Location of the `Module,` or `-1` if it doesnt exist.
		@see indexOf(Module&)
		*/
		//the parameter not being const is not a mistake; it gets derefenced to check for equality
		int indexOf(const Module& m);
		/**
		Find a `Module` with the specified name.
		@param name Name to search for
		@return Location of a `Module` whose `getName()` function returns `name,` or `-1` if wasn't found
		*/
		int indexOf(const std::string name);

		/**
		Require that a `Module` with the specified name exists, or throw an exception.
		<p>
		If your `Module` depends on another `Module,` use this function to require your dependencies.
		@param module Name to assertion
		@param errorMessage What to print if the assertion fails
		@throw AssertionError if there isn't a `Module` with the requested name.
		*/
		void assertModule(const std::string module, std::string errorMessage);
		/**
		Overloads {@link #assertModule(std::string,std::string)} with `errorMessage` being `The specified Module does not exist!`
		@param module Name of a `Module` to assertion
		@see #assertModule(std::string,std::string)
		*/
		void assertModule(const std::string module);

		void start(const long long ups = 30L);

		/**
		Initializes MACE and calls {@link Module#init() init()} on all registered `Modules.`
		<p>
		Should be called at the start of the program.
		@see #addModule(Module&)
		@see MACE for an optimal main loop
		*/
		void init();
		/**
		Update MACE and all `Modules` registered, and checks if a close has been requested.
		<p>
		Should be called in your main loop.
		@return `true` if it updated succesfully. `false` if an error occurred, or a close has been requested from a `Module`. When this returns `false`, you should end the main loop and call `destroy()`
		@throw InitializationError if `init()` has not been called yet or `destroy()` has been called.
		@see #addModule(Module&)
		@see MACE for an optimal main loop
		*/
		void update();
		/**
		Destroys MACE and calls {@link Module#destroy() destroy()} on all registered `Modules.`
		<p>
		Should be called at the end of the program after `MACE.isRunning()` is `false`
		@throw InitializationError if `init()` has not been called yet
		@see #addModule(Module&)
		@see MACE for an optimal main loop
		*/
		void destroy();

		/**
		Checks whether the `MACE` is ready to be updated. Will return true if `MACE::init()` has been called, and `MACE::destroy()` and `MACE::requestStop()` have not been called.
		@return If `update()` should be called. If this returns `false`, you should exit the main loop and call `destroy()`
		@see MACE::requestStop()
		@see MACE for an optimal main loop
		*/
		bool isRunning();

		/**
		Tell the `MACE` to destroy. This is not a guarentee, as it is up to the client running the main loop to actually shut down the program. Use of this function makes `update()` and `isRunning()` return `false`,
		@see MACE for an optimal main loop
		*/
		void requestStop();

		/**
		Retrieve an internal flag about the current state of the `MACE`.
		<p>
		Example usage:{@code
			mc::MACE::getFlag(SYSTEM_FLAG_INIT);//get whether init() has been called
		}
		@param flag Location of the flag to retrieve. Locations are stored as `const Index` and start with `SYSTEM_FLAG_`
		@return Whether the specified flag is `true`
		*/
		bool getFlag(const Byte flag);

		/**
		"Resets" the `MACE` to its default state. `Modules` are cleared, and all flags are set to 0.
		*/
		void reset();
	}

}


#endif