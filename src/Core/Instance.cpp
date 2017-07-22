/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Instance.h>
#include <MACE/Core/System.h>
#include <MACE/Core/Error.h>
#include <memory>

namespace mc {
	Index Instance::addModule(Module& m) {
		if (m.getInstance() != nullptr) {
			MACE__THROW(AlreadyExists, "Can\'t add a Module to 2 Instance\'s!");
		}

		m.instance = this;
		modules.push_back(&m);
		return static_cast<Index>(modules.size() - 1);
	}

	void Instance::removeModule(const Module& m) {
		const int location = indexOf(m);
		if (location < 0) {
			MACE__THROW(ObjectNotFound, "Module by name of " + m.getName() + " not found! Can\'t remove!");
		}
		removeModule(static_cast<Index>(location));
	}

	void Instance::removeModule(const std::string module) {
		const int location = indexOf(module);
		if (location < 0) {
			MACE__THROW(ObjectNotFound, "Module by name of " + module + " not found! Can\'t remove!");
		}
		removeModule(static_cast<Index>(location));
	}

	void Instance::removeModule(const Index i) {
		if (i >= numberOfModules()) {
			MACE__THROW(ObjectNotFound, "Input is greater than the amount of modules!");
		}
		modules[i]->instance = nullptr;
		modules.erase(modules.begin() + i);
	}

	Module * Instance::getModule(const std::string keyword) {
		//this line duplicates the getModule() (const version)
		return const_cast<Module*>(static_cast<const Instance*>(this)->getModule(keyword));
	}

	Module * Instance::getModule(const Index i) {
		//this line duplicates the getModule() (const version)
		return const_cast<Module*>(static_cast<const Instance*>(this)->getModule(i));
	}


	const Module * Instance::getModule(const std::string keyword) const {
		const int location = indexOf(keyword);
		if (location < 0) {
			return nullptr;
		}
		return modules[static_cast<Index>(location)];
	}

	const Module * Instance::getModule(const Index i) const {
		if (i >= numberOfModules()) {
			MACE__THROW(ObjectNotFound, "Input is not a valid index!");
		}
		return modules[i];
	}

	bool Instance::moduleExists(const std::string module) const {
		return indexOf(module) >= 0;
	}

	bool Instance::moduleExists(const Module * module) const {
		return module->getInstance() == this && indexOf(*module) >= 0;
	}

	Size Instance::numberOfModules() const {
		//size() returns size_t which could be larger than unsigned int on some systems, causing problems. static_cast will fix it
		return static_cast<Size>(modules.size());
	}

	void Instance::assertModule(const std::string module, const std::string errorMessage) const {
		if (!moduleExists(module)) {
			MACE__THROW(AssertionFailed, errorMessage);
		}
	}

	void Instance::assertModule(const std::string module) const {
		assertModule(module, "\'" + module + "\' module has not been registered!");
	}

	void Instance::start(const long long ups) {
		mc::Initializer i(this);

		while (mc::Instance::isRunning()) {
			mc::Instance::update();

			mc::os::wait(1000 / ups);
		}
	}

	int Instance::indexOf(const Module& m) const {
		for (Index i = 0; i < modules.size(); ++i) {
			if (modules[i] == &m) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	int Instance::indexOf(const std::string name) const {
		for (Index i = 0; i < modules.size(); ++i) {
			if (modules[i]->getName() == name) {
				return static_cast<int>(i);
			}
		}
		return -1;
	}

	void Instance::init() {
		if (modules.empty()) {
			MACE__THROW(InitializationFailed, "Must add a Module via Instance::addModule!");
		}

		flags.untoggleBit(Instance::DESTROYED);
		flags.toggleBit(Instance::INIT);

		for (Index i = 0; i < modules.size(); ++i) {
			modules[i]->init();
		}
	}

	void Instance::destroy() {
		if (!flags.getBit(Instance::INIT)) {
			MACE__THROW(InitializationFailed, "Can't destroy MACE without calling init() first!");
		}

		flags.toggleBit(Instance::DESTROYED);
		flags.untoggleBit(Instance::INIT);
		flags.untoggleBit(Instance::STOP_REQUESTED);

		for (Index i = 0; i < modules.size(); ++i) {
			modules[i]->destroy();
		}
	}

	void Instance::update() {
		os::clearError(__LINE__, __FILE__);

		if (!flags.getBit(Instance::INIT)) {
			MACE__THROW(InitializationFailed, "init() must be called!");
		}
		for (Index i = 0; i < modules.size(); ++i) {
			modules[i]->update();
		}
	}

	bool Instance::isRunning() const {
		return !flags.getBit(Instance::STOP_REQUESTED) && flags.getBit(Instance::INIT) && !flags.getBit(Instance::DESTROYED);
	}

	void Instance::requestStop() {
		flags.toggleBit(Instance::STOP_REQUESTED);
	}

	bool Instance::getFlag(const Instance::Flag flag) const {
		return flags.getBit(static_cast<Byte>(flag));
	}

	void Instance::reset() {
		modules.clear();
		flags = 0;
	}

	bool Instance::operator==(const Instance & other) const {
		return modules == other.modules && flags == other.flags;
	}

	bool Instance::operator!=(const Instance & other) const {
		return !operator==(other);
	}

	Instance * Module::getInstance() {
		return instance;
	}

	const Instance * Module::getInstance() const {
		return instance;
	}
}//mc