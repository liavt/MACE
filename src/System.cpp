/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/System/System.h>
#include <MACE/System/Exceptions.h>
#include <MACE/System/Constants.h>

namespace mc {

std::vector<Module*> System::modules;
ByteField System::flags = 0;

Index System::addModule(Module& m)
{
	modules.push_back(&m);
	return (Index)(modules.size() - 1);
}
void System::removeModule(Module& m)
{
	for (Index i = 0; i < modules.size(); i++) {
		if (modules[i]==&m) {
			removeModule(i);
			return;
		}
	}
	throw ObjectNotFoundInArrayException("No module by the name of " + m.getName() + " found!");
}
void System::removeModule(std::string module)
{
	for (Index i = 0; i < modules.size(); i++) {
		if (modules[i]->getName()==module) {
			removeModule(i);
			return;
		}
	}
	throw ObjectNotFoundInArrayException("No module by the name of "+module+" found!");
}
void System::removeModule(Index i)
{
	if (i<0 || i>numberOfModules())throw IndexOutOfBoundsException("Input is not a valid index!");
	modules.erase(modules.begin()+i);
}
Module * System::getModule(std::string keyword)
{
	for (Index i = 0; i < modules.size();i++) {
		if (modules[i]->getName() == keyword) {
			return modules[i];
		}
	}
	throw ObjectNotFoundInArrayException("No module by the name of " + keyword + " found!");
}
Module * System::getModule(Index i)
{
	if (i<0 || i>numberOfModules())throw IndexOutOfBoundsException("Input is not a valid index!");
	return modules[i];
}
bool System::moduleExists(std::string module)
{
	for (Index i = 0; i < modules.size();i++) {
		if (modules[i]->getName() == module) {
			return true;
		}
	}
	return false;
}
bool System::moduleExists(Module * module)
{
	for (Index i = 0; i < modules.size(); i++) {
		if (modules[i]==module) {
			return true;
		}
	}
	return false;
}
Size System::numberOfModules()
{
	return modules.size();
}
void System::assertModule(std::string module, std::string errorMessage)
{
	if (!moduleExists(module)) {
		throw AssertionError(errorMessage);
	}
}
void System::assertModule(std::string module)
{
	assertModule(module, "\'"+module+"\' module has not been registered!");
}

unsigned int System::indexOf(Module& m) {
	for (Index i = 0; i < modules.size(); i++) {
		if (modules[i]==&m) {
			return i;
		}
	}
	return -1;
}

unsigned int System::indexOf(std::string name) {
	for (Index i = 0; i < modules.size(); i++) {
		if (modules[i]->getName() ==name) {
			return i;
		}
	}
	return -1;
}

void System::init() {
	if (modules.size()==0)throw InitializationError("Must add a Module via System::addModule!");
	flags.untoggleBit(SYSTEM_FLAG_DESTROYED);
	flags.toggleBit(SYSTEM_FLAG_INIT);

	for (Index i = 0; i < modules.size(); i++) {
		modules[i]->init();
	}
}

void System::terminate() {
	if (!flags.getBit(SYSTEM_FLAG_INIT)) {
		throw InitializationError("Can't terminate System without calling init() first!");
	}
	flags.toggleBit(SYSTEM_FLAG_DESTROYED);
	flags.untoggleBit(SYSTEM_FLAG_INIT);
	flags.untoggleBit(SYSTEM_FLAG_STOP_REQUESTED);
	for (Index i = 0; i < modules.size(); i++) {
		modules[i]->destroy();
	}
}

void System::update() {
	if (!flags.getBit(SYSTEM_FLAG_INIT))throw InitializationError("init() must be called!");
	for (Index i = 0; i < modules.size(); i++) {
		modules[i]->update();
	}
}
bool System::isRunning()
{
	return !flags.getBit(SYSTEM_FLAG_STOP_REQUESTED)&&flags.getBit(SYSTEM_FLAG_INIT)&&!flags.getBit(SYSTEM_FLAG_DESTROYED);
}
void System::requestStop()
{
	flags.toggleBit(SYSTEM_FLAG_STOP_REQUESTED);
}
bool System::getFlag(Index flag)
{
	return flags.getBit(flag);
}
void System::reset()
{
	modules.clear();
	flags = 0;
}

}//mc