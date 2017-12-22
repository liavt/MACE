/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#include <MACE/Core/Interfaces.h>
#include <MACE/Core/Error.h>

namespace mc {
	Beginner::Beginner(Beginable & o) : Beginner(&o) {}

	Beginner::Beginner(Beginable * o) : obj(o) {
#ifdef MACE_DEBUG
		if (o == nullptr) {
			MACE__THROW(NullPointer, "Input Beginable to Beginner is nullptr!");
		}
#endif
		obj->begin();
	}

	Beginner::~Beginner() {
		obj->end();
	}

	Beginable * Beginner::get() {
		return obj;
	}

	const Beginable * Beginner::get() const {
		return obj;
	}

	Beginable * Beginner::operator->() {
		return obj;
	}

	const Beginable * Beginner::operator->() const {
		return obj;
	}

	Beginable * Beginner::operator*() {
		return obj;
	}

	const Beginable * Beginner::operator*() const {
		return obj;
	}

	bool Beginner::operator==(const Beginner & other) const {
		return obj == other.obj;
	}

	bool Beginner::operator!=(const Beginner & other) const {
		return !operator==(other);
	}

	Binder::Binder(Bindable & o) : Binder(&o) {}

	Binder::Binder(Bindable * o) : obj(o) {
#ifdef MACE_DEBUG
		if (o == nullptr) {
			MACE__THROW(NullPointer, "Input Bindable to Binder is nullptr!");
		}
#endif
		obj->bind();
	}

	Binder::~Binder() {
		obj->unbind();
	}

	Bindable * Binder::get() {
		return obj;
	}

	const Bindable * Binder::get() const {
		return obj;
	}

	Bindable * Binder::operator->() {
		return obj;
	}

	const Bindable * Binder::operator->() const {
		return obj;
	}

	Bindable * Binder::operator*() {
		return obj;
	}

	const Bindable * Binder::operator*() const {
		return obj;
	}

	bool Binder::operator==(const Binder & other) const {
		return obj == other.obj;
	}

	bool Binder::operator!=(const Binder & other) const {
		return !operator==(other);
	}

	Initializer::Initializer(Initializable * o) : obj(o) {
		if (obj == nullptr) {
			MACE__THROW(NullPointer, "Input to Initializer was null!");
		}

		obj->init();
	}

	Initializer::Initializer(Initializable & o) : Initializer(&o) {}

	Initializer::~Initializer() {
		obj->destroy();
	}

	Initializable * Initializer::get() {
		return obj;
	}

	const Initializable * Initializer::get() const {
		return obj;
	}

	Initializable * Initializer::operator->() {
		return obj;
	}

	const Initializable * Initializer::operator->() const {
		return obj;
	}

	Initializable * Initializer::operator*() {
		return obj;
	}

	const Initializable * Initializer::operator*() const {
		return obj;
	}

	bool Initializer::operator==(const Initializer & other) const {
		return obj == other.obj;
	}

	bool Initializer::operator!=(const Initializer & other) const {
		return !operator==(other);
	}
}//mc