#pragma once
#include <vector>
#include <MC-System/Constants.h>

/**
Namespace for everything in MACE. This includes constants, typedefs, tests, classes, and variables.
<p>
Preproccessor definitions will start with `_MACE.` The defintion `__MACE` will be defined to equal `true` if MACE is included.
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
	Core class of MACE, managing `Modules.`
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
		Remove a `Module` by reference. 
		<p>
		It will find the `Module` based on their {@link Module#getName() getName()} function, so if there are multiple `Modules` with the same name, this function may produce unexpected behavior.
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
		Remove a `Module` by it's index. The index is provided from {@link #addModule(Module&)} or {@link #indexOf(const Module&)}
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
		<p>
		Equivalent to {@code 
			moduleExists(module->getName());
		}
		@param module `Module` to search for
		@return `true` if there is a `Module` with the same name as the parameter, `false` otherwise.
		*/
		static bool moduleExists(const Module* module);
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
		static Index indexOf(const Module& m);
		/**
		Find a `Module` with the specified name.
		@param name Name to search for
		@return Location of a `Module` whose `getName()` function returns `name,` or `-1` if wasn't found
		*/
		static Index indexOf(std::string name);

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
		*/
		static void init();
		/**
		Destroys MACE and calls {@link Module#destroy() destroy()} on all registered `Modules.`
		<p>
		Should be called at the end of the program.
		@see #addModule(Module&)
		*/
		static void terminate();
		/**
		Update MACE and all `Modules` registered
		<p>
		Should be called in your main loop.
		@see #addModule(Module&)
		*/
		static void update();
	private: 
		/**
		Private constructor to prevent initialization.
		*/
		System();

		/**
		All of the `Modules` registered
		*/
		static std::vector<Module*> modules;
	};

}


