/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__CORE_INSTANCE_H
#define MACE__CORE_INSTANCE_H

#include <MACE/Core/Constants.h>
#include <MACE/Core/Interfaces.h>
#include <string>
#include <unordered_map>
#include <memory>

namespace mc {
	//forward declaration for Module class
	class Instance;

	/**
	Abstract class used for doing a task in MACE. Plugged into {@link Instance} via {@link Instance#addModule(Module&) addModule().}
	*/
	class MACE_NOVTABLE Module: public Initializable {
		friend class Instance;
	public:
		virtual MACE__DEFAULT_OPERATORS(Module);

		/**
		Called when {@link MACE#init} is called and this `Module` is registered.
		<p>
		Use this to initialize variables, start streams, or launch a context.
		<p>
		Should only be called once.
		*/
		virtual void init() override = 0;
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
		virtual void destroy() override = 0;

		/**
		Override this function and return a UNIQUE name for this `Module`
		<p>
		The name for your `Module` is used in comparison, so make sure it is as unique as possible.
		<p>
		It is akin to a hashcode.
		*/
		virtual std::string getName() const MACE_ENSURES(ret, !ret.empty()) = 0;

		Instance* getInstance();
		const Instance* getInstance() const;
	protected:
		Instance* instance = nullptr;

		Module() noexcept = default;
	};

	/**
	A pointer to a `Module` that automatically manages pointer lifecycle
	*/
	using ModulePtr = std::shared_ptr<Module>;

	/**
	Core class of MACE, managing `Modules`. To start running MACE, simply call `mc::MACE::start(const long long ups)`
	<p>
	If `isRunning()` returns `false`, you should end your program and call `destroy()`
	<p>
	`mc::MACE::start(const long long ups)` starts up the MACE main loop. You can make your own main loop by
	calling the appropriate function calls.
	<p>
	`init()` should be called after all `Modules` are added and before the main loop. `update()` should be called in the loop, and `destroy()` should be called at the end of your program.
	<p>
	An example of a custom main loop looks like this: {@code

	//add modules that you need via mc::MACE::addModule(Module&)

	mc::MACE::init();

	while(mc::MACE::isRunning()){
		mc::MACE::update();

		//execute non-MACE code

		//wait 33 ms for the next iteration
		mc::os::wait(33);
	}

	mc::MACE::destroy();

	}
	*/
	class Instance: public Initializable {
	public:
		enum Flag: Byte {
			/**
			Parameter for `MACE.getFlag(const Byte)`. Is `true` if `MACE.init()` has been called.
			*/
			INIT = 0x01,
			/**
			Parameter for `MACE.getFlag(const Byte)`. Is `true` if `MACE.destroy()` has been called.
			*/
			DESTROYED = 0x02,
			/**
			Parameter for `MACE.getFlag(const Byte)`. Is `true` if `MACE.requestStop()` has been called.
			*/
			STOP_REQUESTED = 0x04,
			VERBOSE_ERRORS = 0x08,
			WRITE_ERRORS_TO_LOG = 0x10,
		};

		/**
		@copydoc addModule(std::shared_ptr<Module>)
		*/
		void addModule(Module& m);

		/**
		@copydoc addModule(std::shared_ptr<Module>)
		*/
		void addModule(Module* m) MACE_EXPECTS(m != nullptr);

		/**
		Register a {@link Module}.
		<p>
		Registered `Modules` will have `update(), init(),` and `destroy()` are respectively called when `MACE.update(), MACE.init(),` and `MACE.destroy()` are called.
		@param m Selected `Module` to add.
		@throws AlreadyExistsError If a `Module` with the same name already exists
		@throws InvalidStateError If the input to this function is already associated with an `Instance`
		@throws NullPointerError If the input is `nullptr`
		*/
		void addModule(ModulePtr m) MACE_EXPECTS(m != nullptr);
		/**
		Remove a `Module`
		@param module Name of a `Module`
		*/
		void removeModule(const std::string& module) MACE_EXPECTS(!module.empty());
		/**
		Retrieve a pointer to a `Module` with the specified name.
		<p>
		It will find the `Module` based on their {@link Module#getName() getName()} function in O(1) time
		@return The found `Module`. If no `Module` with the specified name exists, returns a `std::shared_ptr` holding `nullptr`
		@param keyword Name to look for
		*/
		ModulePtr getModule(const std::string& keyword) MACE_EXPECTS(!keyword.empty());

		/**
		@copydoc Instance::getModule(const std::string)
		*/
		const ModulePtr getModule(const std::string& keyword) const MACE_EXPECTS(!keyword.empty());

		/**
		Checks whether a `Module` exists via it's `getName()` function.
		@param module Name to search for
		@return `true` if there is a `Module` with the specified name, `false` otherwise.
		*/
		bool hasModule(const std::string& module) const MACE_EXPECTS(!module.empty());
		/**
		Retrieves the amount of `Module` currently being updated by `MACE`
		@return `Size` of the internal `Module` buffer
		*/
		Size size() const;
		/**
		Calculates whether this `Instance` is empty of any modules
		@return `size()` == 0
		*/
		bool empty() const;

		void start(const long long ups = 30L);

		/**
		Initializes MACE and calls {@link Module#init() init()} on all registered `Modules.`
		<p>
		Should be called at the start of the program.
		@see #addModule(Module&)
		@see MACE for an optimal main loop
		*/
		void init() override MACE_EXPECTS(!empty());
		/**
		Update MACE and all `Modules` registered, and checks if a close has been requested.
		<p>
		Should be called in your main loop.
		@return `true` if it updated succesfully. `false` if an error occurred, or a close has been requested from a `Module`. When this returns `false`, you should end the main loop and call `destroy()`
		@throw InitializationError if `init()` has not been called yet or `destroy()` has been called.
		@see #addModule(Module&)
		@see MACE for an optimal main loop
		*/
		void update() MACE_EXPECTS(!empty());
		/**
		Destroys MACE and calls {@link Module#destroy() destroy()} on all registered `Modules.`
		<p>
		Should be called at the end of the program after `MACE.isRunning()` is `false`
		@throw InitializationError if `init()` has not been called yet
		@see MACE::addModule(Module&)
		@see MACE for an optimal main loop
		*/
		void destroy() override MACE_EXPECTS(!empty());

		/**
		Checks whether the `MACE` is ready to be updated. Will return true if `MACE::init()` has been called, and `MACE::destroy()` and `MACE::requestStop()` have not been called.
		@return If `update()` should be called. If this returns `false`, you should exit the main loop and call `destroy()`
		@see MACE::requestStop()
		@see MACE::getFlag(const Byte)
		@see MACE for an optimal main loop
		*/
		bool isRunning() const;

		/**
		Tell the `MACE` to destroy. This is not a guarentee, as it is up to the client running the main loop to actually shut down the program. Use of this function makes `update()` and `isRunning()` return `false`,
		@see MACE for an optimal main loop
		@see MACE::isRunning()
		*/
		void requestStop();

		/**
		Retrieve an internal flag about the current state of the `MACE`.
		<p>
		Example usage:{@code
			bool isInit = mc::MACE::getFlag(mc::MACE::INIT);//get whether init() has been called
		}
		@param flag Location of the flag to retrieve. Locations are stored as `const Index` and start with `SYSTEM_FLAG_`
		@return Whether the specified flag is `true`
		@see MACE::isRunning()
		*/
		bool getFlag(const Flag flag) const;

		/**
		"Resets" the `MACE` to its default state. `Modules` are cleared, and all flags are set to 0.
		*/
		void reset() MACE_ENSURES(ret, empty());

		const std::unordered_map<std::string, ModulePtr>& getModules() const;

		bool operator==(const Instance& other) const;
		bool operator!=(const Instance& other) const;
	private:
		/**
		All of the `Modules` registered
		*/
		std::unordered_map<std::string, ModulePtr> modules;

		/**
		Stores various flags for the MACE, like whether it is running, or a close is requested.
		*/
		Byte flags = 0;
	};
}


#endif