/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Module.h>
#include <MACE/Utility/BitField.h>
#include <vector>

namespace mc {
	namespace {
		/**
		All of the `Modules` registered
		*/
		std::vector<Module*> modules;

		/**
		Stores various flags for the MACE, like whether it is running, or a close is requested.
		*/
		BitField flags = 0;
	}

	Index MACE::addModule(Module& m) {
		modules.push_back(&m);
		return static_cast<Index>(modules.size() - 1);
	}
	void MACE::removeModule(const Module& m) {
		const int location = indexOf(m);
		if( location < 0 )throw ObjectNotFoundInArrayException("Module by name of " + m.getName() + " not found! Can\'t remove!");
		removeModule(location);
	}
	void MACE::removeModule(const std::string module) {
		const int location = indexOf(module);
		if( location < 0 )throw ObjectNotFoundInArrayException("Module by name of " + module + " not found! Can\'t remove!");
		removeModule(location);
	}
	void MACE::removeModule(const Index i) {
		if( i >= numberOfModules() )throw IndexOutOfBoundsException("Input is greater than the amount of modules!");
		modules.erase(modules.begin() + i);
	}
	Module * MACE::getModule(const std::string keyword) {
		const int location = indexOf(keyword);
		if( location < 0 )throw ObjectNotFoundInArrayException("No module by the name of " + keyword + " found!");
		return modules[location];
	}
	Module * MACE::getModule(const Index i) {
		if( i >= numberOfModules() )throw IndexOutOfBoundsException("Input is not a valid index!");
		return modules[i];
	}
	bool MACE::moduleExists(const std::string module) {
		return indexOf(module) >= 0;
	}
	bool MACE::moduleExists(const Module * module) {
		return indexOf(*module) >= 0;
	}
	Size MACE::numberOfModules() {
		//size() returns size_t which could be larger than unsigned int on some systems, causing problems. static_cast will fix it
		return static_cast<Size>(modules.size());
	}
	void MACE::assertModule(const std::string module, const std::string errorMessage) {
		if( !moduleExists(module) ) {
			throw AssertionError(errorMessage);
		}
	}
	void MACE::assertModule(const std::string module) {
		assertModule(module, "\'" + module + "\' module has not been registered!");
	}

	int MACE::indexOf(const Module& m) {
		for( Index i = 0; i < modules.size(); i++ ) {
			if( modules[i] == &m ) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	int MACE::indexOf(const std::string name) {
		for( Index i = 0; i < modules.size(); i++ ) {
			if( modules[i]->getName() == name ) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	void MACE::init() {
		if( modules.size() == 0 )throw InitializationError("Must add a Module via MACE::addModule!");
		flags.untoggleBit(MACE::DESTROYED);
		flags.toggleBit(MACE::INIT);

		for( Index i = 0; i < modules.size(); i++ ) {
			modules[i]->init();
		}
	}

	void MACE::destroy() {
		if( !flags.getBit(MACE::INIT) ) {
			throw InitializationError("Can't destroy MACE without calling init() first!");
		}
		flags.toggleBit(MACE::DESTROYED);
		flags.untoggleBit(MACE::INIT);
		flags.untoggleBit(MACE::STOP_REQUESTED);
		for( Index i = 0; i < modules.size(); i++ ) {
			modules[i]->destroy();
		}
	}

	void MACE::update() {
		if( !flags.getBit(MACE::INIT) )throw InitializationError("init() must be called!");
		for( Index i = 0; i < modules.size(); i++ ) {
			modules[i]->update();
		}
	}
	bool MACE::isRunning() {
		return !flags.getBit(MACE::STOP_REQUESTED) && flags.getBit(MACE::INIT) && !flags.getBit(MACE::DESTROYED);
	}
	void MACE::requestStop() {
		flags.toggleBit(MACE::STOP_REQUESTED);
	}
	bool MACE::getFlag(const Byte flag) {
		return flags.getBit(flag);
	}
	void MACE::reset() {
		modules.clear();
		flags = 0;
	}

}//mc