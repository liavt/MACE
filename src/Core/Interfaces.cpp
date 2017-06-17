/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Interfaces.h>
#include <MACE/Core/Constants.h>

namespace mc {
	Initializer::Initializer(Initializable * in) {
		obj = in;

		init();
	}

	Initializer::~Initializer() {
		if (isInitialized()) {
			destroy();
		}
	}

	void Initializer::init() {
		if (isInitialized()) {
			MACE__THROW(InitializationFailed, "This object is already initialized!");
		}

		obj->init();

		initialized = true;
	}

	void Initializer::destroy() {
		if (!isInitialized()) {
			MACE__THROW(InvalidState, "Can\'t destroy an uninitialized object!");
		}

		obj->destroy();

		initialized = false;
	}

	Initializable * Initializer::get() {
		return obj;
	}

	const Initializable * Initializer::get() const {
		return obj;
	}

	Initializable * Initializer::operator*() {
		return obj;
	}

	const Initializable * Initializer::operator*() const {
		return obj;
	}

	Initializable * Initializer::operator->() {
		return obj;
	}

	const Initializable * Initializer::operator->() const {
		return obj;
	}

	bool Initializer::isInitialized() const {
		return initialized;
	}

	bool Initializer::operator==(const Initializer & other) const {
		return obj == other.obj && initialized == other.initialized;
	}

	bool Initializer::operator!=(const Initializer & other) const {
		return !operator==(other);
	}
}//mc