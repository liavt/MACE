/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Core/Instance.h>
#include <MACE/Core/System.h>
#include <MACE/Core/Error.h>
#include <memory>

namespace mc {
	void Instance::addModule(Module& m) {
		addModule(&m);
	}

	void Instance::addModule(Module* m) {
		addModule(std::shared_ptr<Module>(m, [](Module*) {}));
	}

	void Instance::addModule(ModulePtr m) {
		MACE_ASSERT(m != nullptr, "Input to addModule() was nullptr");
		MACE_ASSERT(!m->isInit(), "Module is already init!");
		MACE_ASSERT(!hasModule(m->getName()), "Module with name " + m->getName() + " already exists in this Instance");

		m->instance = this;
		modules.emplace(m->getName(), m);
	}

	void Instance::removeModule(const std::string& key) {
		modules.erase(key);
	}

	ModulePtr Instance::getModule(const std::string& keyword) {
		auto result = modules.find(keyword);
		if (result == modules.end()) {
			return std::shared_ptr<Module>(nullptr);
		}

		return result->second;
	}

	const ModulePtr Instance::getModule(const std::string& keyword) const {
		auto result = modules.find(keyword);
		if (result == modules.end()) {
			return std::shared_ptr<Module>(nullptr);
		}

		return result->second;
	}

	bool Instance::hasModule(const std::string& mod) const {
		return getModule(mod) != nullptr;
	}

	Size Instance::size() const {
		//size() returns size_t which could be larger than unsigned int on some systems, causing problems. static_cast will fix it
		return static_cast<Size>(modules.size());
	}

	bool mc::Instance::empty() const {
		return size() == 0;
	}

	void Instance::start(const long long ups) {
		init();

		while (mc::Instance::isRunning()) {
			mc::Instance::update();

			mc::os::wait(1000 / ups);
		}
	}

	void Instance::init() {
		MACE_ASSERT(!modules.empty(), "Must add a Module via Instance::addModule!");

		flags &= ~Instance::DESTROYED;
		flags |= Instance::INIT;

		for (auto val : modules) {
			val.second->init();
		}
	}

	void Instance::update() {
		MACE_ASSERT(isInit(), "init() must be called!");

		for (auto val : modules) {
			val.second->update();
		}
	}

	bool Instance::isInit() const noexcept {
		return flags & Instance::INIT;
	}

	bool Instance::isRunning() const {
		return isInit() && !(flags & (Instance::DESTROYED | Instance::STOP_REQUESTED));
	}

	void Instance::requestStop() {
		flags |= Instance::STOP_REQUESTED;
	}

	bool Instance::getFlag(const Instance::Flag flag) const {
		return (flags & flag) != 0;
	}

	void Instance::reset() {
		modules.clear();
		flags = 0;
	}

	const std::unordered_map<std::string, ModulePtr>& Instance::getModules() const {
		return modules;
	}

	bool Instance::operator==(const Instance& other) const {
		return modules == other.modules && flags == other.flags;
	}

	bool Instance::operator!=(const Instance& other) const {
		return !operator==(other);
	}

	bool Module::isInit() const noexcept {
		return instance != nullptr;
	}

	Instance* Module::getInstance() {
		return instance;
	}

	const Instance* Module::getInstance() const {
		return instance;
	}
}//mc