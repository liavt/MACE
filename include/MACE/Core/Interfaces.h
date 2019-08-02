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
		Beginable() noexcept = default;
		virtual MACE__DEFAULT_OPERATORS(Beginable);

		virtual void begin() = 0;

		virtual void end() = 0;
	};

	class Beginner {
	public:
		Beginner(Beginable& o);
		Beginner(Beginable* o);
		~Beginner();

		MACE_NODISCARD Beginable* get();
		MACE_NODISCARD const Beginable* get() const;

		MACE_NODISCARD Beginable* operator->();
		MACE_NODISCARD const Beginable* operator->() const;

		MACE_NODISCARD Beginable* operator*();
		MACE_NODISCARD const Beginable* operator*() const;

		bool operator==(const Beginner& other) const;
		bool operator!=(const Beginner& other) const;
	private:
		Beginable* obj;
	};

	class Initializer;

	class MACE_NOVTABLE Initializable {
		friend class Initializer;
	public:
		Initializable() noexcept = default;
		virtual MACE__DEFAULT_OPERATORS(Initializable);

		virtual bool isInit() const noexcept = 0;

		explicit operator bool() const noexcept;
	protected:
		virtual void init() = 0;

		virtual void destroy() = 0;
	};//Initializable

	class Initializer final {
	public:
		Initializer(Initializable* o);
		Initializer(Initializable& o);
		~Initializer();

		MACE_NODISCARD Initializable* get();
		MACE_NODISCARD const Initializable* get() const;

		MACE_NODISCARD Initializable* operator->();
		MACE_NODISCARD const Initializable* operator->() const;

		MACE_NODISCARD Initializable* operator*();
		MACE_NODISCARD const Initializable* operator*() const;

		bool operator==(const Initializer& other) const;
		bool operator!=(const Initializer& other) const;
	private:
		Initializable* obj;
	};//Initializer
}//mc

#endif//MACE__CORE_INTERFACES_H
