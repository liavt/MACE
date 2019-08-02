/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#include <MACE/Core/Interfaces.h>
#include <MACE/Core/Error.h>

namespace mc {
	Beginner::Beginner(Beginable& o) : Beginner(&o) {}

	Beginner::Beginner(Beginable* o) : obj(o) {
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

	Beginable* Beginner::get() {
		return obj;
	}

	const Beginable* Beginner::get() const {
		return obj;
	}

	Beginable* Beginner::operator->() {
		return obj;
	}

	const Beginable* Beginner::operator->() const {
		return obj;
	}

	Beginable* Beginner::operator*() {
		return obj;
	}

	const Beginable* Beginner::operator*() const {
		return obj;
	}

	bool Beginner::operator==(const Beginner& other) const {
		return obj == other.obj;
	}

	bool Beginner::operator!=(const Beginner& other) const {
		return !operator==(other);
	}

	Initializable::operator bool() const noexcept{
		return isInit();
	}

	Initializer::Initializer(Initializable* o) : obj(o) {
		if (obj == nullptr) {
			MACE__THROW(NullPointer, "Input to Initializer was null!");
		}

		obj->init();
	}

	Initializer::Initializer(Initializable& o) : Initializer(&o) {}

	Initializer::~Initializer() {
		obj->destroy();
	}

	Initializable* Initializer::get() {
		return obj;
	}

	const Initializable* Initializer::get() const {
		return obj;
	}

	Initializable* Initializer::operator->() {
		return obj;
	}

	const Initializable* Initializer::operator->() const {
		return obj;
	}

	Initializable* Initializer::operator*() {
		return obj;
	}

	const Initializable* Initializer::operator*() const {
		return obj;
	}

	bool Initializer::operator==(const Initializer& other) const {
		return obj == other.obj;
	}

	bool Initializer::operator!=(const Initializer& other) const {
		return !operator==(other);
	}
}//mc