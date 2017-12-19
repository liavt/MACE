/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE__CORE_INTERFACES_H
#define MACE__CORE_INTERFACES_H
#include <MACE/Core/Error.h>

namespace mc {
	class Beginable {
	public:
		virtual ~Beginable() = default;

		virtual void begin() = 0;

		virtual void end() = 0;
	};

	class Bindable {
	public:
		virtual ~Bindable() = default;

		virtual void bind() const = 0;

		virtual void unbind() const = 0;
	};

	class Binder {
	public:
		Binder(Bindable& o);
		Binder(Bindable* o);
		~Binder();

		Bindable* get();
		const Bindable* get() const;

		Bindable* operator->();
		const Bindable* operator->() const;

		Bindable* operator*();
		const Bindable* operator*() const;

		bool operator==(const Binder& other) const;
		bool operator!=(const Binder& other) const;
	private:
		Bindable * obj;
	};

	class Initializer;

	class Initializable {
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

	class Progressable {
	public:
		virtual ~Progressable() = default;

		virtual void setProgress(const float prog) = 0;
	};
}//mc

#endif//MACE_CORE_INTERFACES_H
