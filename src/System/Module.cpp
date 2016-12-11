/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/System/System.h>
#include <MACE/Utility/BitField.h>
#include <vector>

namespace mc {

	namespace {
		/**
		All of the `Modules` registered
		*/
		std::vector<Module*> modules;

		/**
		Stores various flags for the System, like whether it is running, or a close is requested.
		*/
		BitField flags = 0;
	}

	Index System::addModule(Module& m) {
		modules.push_back(&m);
		return static_cast<Index>(modules.size() - 1);
	}
	void System::removeModule(const Module& m) {
		const int location = indexOf(m);
		if( location < 0 )throw ObjectNotFoundInArrayException("Module by name of " + m.getName() + " not found! Can\'t remove!");
		removeModule(location);
	}
	void System::removeModule(const std::string module) {
		const int location = indexOf(module);
		if( location < 0 )throw ObjectNotFoundInArrayException("Module by name of " + module + " not found! Can\'t remove!");
		removeModule(location);
	}
	void System::removeModule(const Index i) {
		if( i >= numberOfModules() )throw IndexOutOfBoundsException("Input is greater than the amount of modules!");
		modules.erase(modules.begin() + i);
	}
	Module * System::getModule(const std::string keyword) {
		const int location = indexOf(keyword);
		if( location < 0 )throw ObjectNotFoundInArrayException("No module by the name of " + keyword + " found!");
		return modules[location];
	}
	Module * System::getModule(const Index i) {
		if( i >= numberOfModules() )throw IndexOutOfBoundsException("Input is not a valid index!");
		return modules[i];
	}
	bool System::moduleExists(const std::string module) {
		return indexOf(module) >= 0;
	}
	bool System::moduleExists(const Module * module) {
		return indexOf(*module) >= 0;
	}
	Size System::numberOfModules() {
		return modules.size();
	}
	void System::assertModule(const std::string module, const std::string errorMessage) {
		if( !moduleExists(module) ) {
			throw AssertionError(errorMessage);
		}
	}
	void System::assertModule(const std::string module) {
		assertModule(module, "\'" + module + "\' module has not been registered!");
	}

	int System::indexOf(const Module& m) {
		for( Index i = 0; i < modules.size(); i++ ) {
			if( modules[i] == &m ) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	int System::indexOf(const std::string name) {
		for( Index i = 0; i < modules.size(); i++ ) {
			if( modules[i]->getName() == name ) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	void System::init() {
		if( modules.size() == 0 )throw InitializationError("Must add a Module via System::addModule!");
		flags.untoggleBit(System::DESTROYED);
		flags.toggleBit(System::INIT);

		for( Index i = 0; i < modules.size(); i++ ) {
			modules[i]->init();
		}
	}

	void System::destroy() {
		if( !flags.getBit(System::INIT) ) {
			throw InitializationError("Can't destroy System without calling init() first!");
		}
		flags.toggleBit(System::DESTROYED);
		flags.untoggleBit(System::INIT);
		flags.untoggleBit(System::STOP_REQUESTED);
		for( Index i = 0; i < modules.size(); i++ ) {
			modules[i]->destroy();
		}
	}

	void System::update() {
		if( !flags.getBit(System::INIT) )throw InitializationError("init() must be called!");
		for( Index i = 0; i < modules.size(); i++ ) {
			modules[i]->update();
		}
	}
	bool System::isRunning() {
		return !flags.getBit(System::STOP_REQUESTED) && flags.getBit(System::INIT) && !flags.getBit(System::DESTROYED);
	}
	void System::requestStop() {
		flags.toggleBit(System::STOP_REQUESTED);
	}
	bool System::getFlag(const Byte flag) {
		return flags.getBit(flag);
	}
	void System::reset() {
		modules.clear();
		flags = 0;
	}

}//mc