/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#pragma once
#include <vector>
#include <string>
#include <MACE/System/Constants.h>
#include <MACE/System/Utility/BitField.h>

/**
Namespace for everything in MACE. This includes constants, typedefs, tests, classes, and variables.
<p>
Preproccessor definitions will start with `MACE_` and `MACE_INCLUDED` will be defined to equal `true` if MACE is included.
*/
namespace mc {
	class System;

	/**
	Abstract class used for doing a task in MACE. Plugged into {@link System} via {@link System#addModule(Module&) addModule().}
	*/
	class Module {
		friend class System;
	protected: 
		/**
		Called when {@link System#init} is called and this `Module` is registered.
		<p>
		Use this to initialize variables, start streams, or launch a context.
		<p>
		Should only be called once.
		*/
		virtual void init()=0;
		/**
		Called when {@link System#update} is called and this `Module` is registered.
		<p>
		Use this update states, send messages, swap graphics buffers, render, process, or anything that needs to be periodically checked.
		*/
		virtual void update() = 0;
		/**
		Called when {@link System#terminate} is called and this `Module` is registered.
		<p>
		Use this to clear memory, close streams, or clean up anything that needs to be cleaned up.
		<p>
		Should only be called before the program is closed.
		*/
		virtual void destroy()=0;
	
		/**
		Override this function and return a UNIQUE name for this `Module`
		<p>
		The name for your `Module` is used in comparison, so make sure it is as unique as possible.
		<p>
		It is akin to a hashcode.
		*/
		virtual std::string getName() const =0;
	};

	/**
	Parameter for `System.getFlag(Index)`. Is `true` if `System.init()` has been called.
	*/
	const Index SYSTEM_FLAG_INIT = 0;
	/**
	Parameter for `System.getFlag(Index)`. Is `true` if `System.destroy()` has been called.
	*/
	const Index SYSTEM_FLAG_DESTROYED = 1;
	/**
	Parameter for `System.getFlag(Index)`. Is `true` if `System.requestStop()` has been called.
	*/
	const Index SYSTEM_FLAG_STOP_REQUESTED = 2;

	/**
	Core class of MACE, managing `Modules`. `init()` should be called after all `Modules` are added and before the main loop. `update()` should be called in the loop, and `terminate()` should be called at the end of your program.
	<p>
	If `isRunning()` returns `false`, you should end your program and call `terminate()`
	<p>
	Your main loop should look like this:{@code
	
	//add modules that you need

	mc::System::init();

	while(mc::System::isRunning()){
		mc::System::update();
	}
	
	mc::System::terminate();

	}
	*/
	class System final {
	public:
		/**
		Register a {@link Module}.
		<p>
		Registered `Modules` will have `update(), init(),` and `destroy()` are respectivaly called when `System.update(), System.init(),` and `System.terminate()` are called.
		@param m Reference to a `Module.` If the `Module` referenced leaves scope or gets deallocated, a `read access violation will occur.`
		@return Location of the `Module` in the buffer. This index can be used in various other methods.
		*/
		static Index addModule(Module& m);
		/**
		Remove a `Module` by reference. This is generally more safe than `removeModule(std::string)` as this checks by reference, not by name.
		@param m Reference to Module in the buffer.
		@throws ObjectNotFoundInArray if the referenced `Module` doesn't exist.
		*/
		static void removeModule(Module& m);
		/**
		Remove a `Module` by name. 
		<p>
		It will find the `Module` based on their {@link Module#getName() getName()} function, so if there are multiple `Modules` with the same name, this function may produce unexpected behavior.
		@param module Name of a `Module`
		@throws ObjectNotFoundInArray if a `Module` with the given name doesn't exist.
		*/
		static void removeModule(std::string module);
		/**
		Remove a `Module` by it's index. The index is provided from {@link #addModule(Module&)} or {@link #indexOf(Module&)}
		@throw IndexOutOfBounds if `i<0` or `i>numberOfModules()`
		@param i Index of a `Module` to delete
		*/
		static void removeModule(Index i );
		/**
		Retrieve a pointer to a `Module` with the specified name.
		<p>
		It will find the `Module` based on their {@link Module#getName() getName()} function, so if there are multiple `Modules` with the same name, this function may produce unexpected behavior.
		@throw ObjectNotFoundInArray if there is no `Module` with the specified name
		@return A `Module` whose `getName()` function returns the specified keyword
		@param keyword Name to look for
		@see #getModule(Index)
		*/
		static Module* getModule(std::string keyword);
		/**
		Retrieves the `Module` at a certain index.
		@return A pointer to a `Module` at the specified location
		@param i Valid index to a `Module`
		@throw IndexOutOfBounds if `i<0` or `i>numberOfModules()`
		*/
		static Module* getModule(Index i);
		/**
		Checks whether a `Module` exists via it's `getName()` function.
		@param module Name to search for
		@return `true` if there is a `Module` with the specified name, `false` otherwise.
		*/
		static bool moduleExists(std::string module);
		
		/**
		Checks whether a `Module` exists via a pointer.
		@param module `Module` to search for
		@return `true` if the `Module` exists, `false` otherwise.
		*/
		static bool moduleExists(Module* module);
		/**
		Retrieves the amount of `Module` currently being updated by `System`
		@return `Size` of the internal `Module` buffer
		*/
		static Size numberOfModules();
		/**
		Retrieves the location of a `Module` in the buffer.
		<p>
		Equivalent to calling {@code 
			indexOf(m.getName());
		}
		@param m Module to find the index of.
		@return Location of the `Module,` or `-1` if it doesnt exist.
		@see indexOf(Module&)
		*/
		//the parameter not being const is not a mistake; it gets derefenced to check for equality
		static int indexOf(Module& m);
		/**
		Find a `Module` with the specified name.
		@param name Name to search for
		@return Location of a `Module` whose `getName()` function returns `name,` or `-1` if wasn't found
		*/
		static int indexOf(std::string name);

		/**
		Require that a `Module` with the specified name exists, or throw an exception.
		<p>
		If your `Module` depends on another `Module,` use this function to require your dependencies.
		@param module Name to assert
		@param errorMessage What to print if the assertion fails
		@throw AssertionError if there isn't a `Module` with the requested name.
		*/
		static void assertModule(std::string module,std::string errorMessage);
		/**
		Overloads {@link #assertModule(std::string,std::string)} with `errorMessage` being `The specified Module does not exist!`
		@param module Name of a `Module` to assert
		@see #assertModule(std::string,std::string)
		*/
		static void assertModule(std::string module);

		/**
		Initializes MACE and calls {@link Module#init() init()} on all registered `Modules.`
		<p>
		Should be called at the start of the program.
		@see #addModule(Module&)
		@see System for an optimal main loop
		*/
		static void init();
		/**
		Destroys MACE and calls {@link Module#destroy() destroy()} on all registered `Modules.`
		<p>
		Should be called at the end of the program after `System.isRunning()` is `false`
		@throw InitializationError if `init()` has not been called yet
		@see #addModule(Module&)
		@see System for an optimal main loop
		*/
		static void terminate();
		/**
		Update MACE and all `Modules` registered, and checks if a close has been requested.
		<p>
		Should be called in your main loop.
		@return `true` if it updated succesfully. `false` if an error occurred, or a close has been requested from a `Module`. When this returns `false`, you should end the main loop and call `terminate()`
		@throw InitializationError if `init()` has not been called yet or `terminate()` has been called.
		@see #addModule(Module&)
		@see System for an optimal main loop
		*/
		static void update();

		/**
		Checks whether the `System` is ready to be updated. `init()` must have been called and `terminate()` must not have been called. Additionally, if `shouldStop()` is `true`, this function also returns `false`.
		@return If `update()` should be called. If this returns `false`, you should exit the main loop and call `terminate()`
		@see requestStop()
		@see System for an optimal main loop
		*/
		static bool isRunning();

		/**
		Tell the `System` to terminate. This is not a guarentee, as it is up to the client running the main loop to actually shut down the program. Use of this function makes `update()` and `isRunning()` return `false`,
		@see System for an optimal main loop
		*/
		static void requestStop();

		/**
		Retrieve an internal flag about the current state of the `System`.
		<p>
		Example usage:{@code
			mc::System::getFlag(SYSTEM_FLAG_INIT);//get whether init() has been called
		}
		@param flag Location of the flag to retrieve. Locations are stored as `const Index` and start with `SYSTEM_FLAG_`
		@return Whether the specified flag is `true`
		*/
		static bool getFlag(Index flag);

		/**
		"Resets" the `System` to its default state. `Modules` are cleared, and all flags are set to 0.
		*/
		static void reset();
	private: 
		/**
		Private constructor to prevent initialization.
		*/
		System();

		/**
		All of the `Modules` registered
		*/
		static std::vector<Module*> modules;

		/**
		Stores various flags for the System, like whether it is running, or a close is requested.
		*/
		static ByteField flags;
	};

}


