/*
Copyright (c) 2016-2019 Liav Turkia

See LICENSE.md for full copyright information
*/
#pragma once
#ifndef MACE__CORE_INTERFACES_H
#define MACE__CORE_INTERFACES_H
#include <MACE/Core/Constants.h>
#include <MACE/Core/Error.h>

namespace mc {
	class MACE_NOVTABLE Beginable {
	public:
		virtual ~Beginable() = default;

		virtual void begin() = 0;

		virtual void end() = 0;
	};

	class Beginner {
	public:
		Beginner(Beginable& o);
		Beginner(Beginable* o);
		~Beginner();

		Beginable* get();
		const Beginable* get() const;

		Beginable* operator->();
		const Beginable* operator->() const;

		Beginable* operator*();
		const Beginable* operator*() const;

		bool operator==(const Beginner& other) const;
		bool operator!=(const Beginner& other) const;
	private:
		Beginable * obj;
	};

	class Initializer;

	class MACE_NOVTABLE Initializable {
		friend class Initializer;
	public:
		virtual ~Initializable() = default;
	protected:
		virtual void init() = 0;

		virtual void destroy() = 0;
	};//Initializable

	class Initializer final {
	public:
		Initializer(Initializable* o);
		Initializer(Initializable& o);
		~Initializer();

		Initializable* get();
		const Initializable* get() const;

		Initializable* operator->();
		const Initializable* operator->() const;

		Initializable* operator*();
		const Initializable* operator*() const;

		bool operator==(const Initializer& other) const;
		bool operator!=(const Initializer& other) const;
	private:
		Initializable * obj;
	};//Initializer
}//mc

#endif//MACE_CORE_INTERFACES_H
