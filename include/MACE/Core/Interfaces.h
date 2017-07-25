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

	class Binder{
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
		Bindable* obj;
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

	class Initializer final{
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
		Initializable* obj;
	};//Initializer

	template<typename T>
	class SmartPointer {
	public:
		explicit SmartPointer(T& p) : SmartPointer(&p) {}

		explicit SmartPointer(T* p) : SmartPointer(p, false) {}

		explicit SmartPointer(T* p, const bool isDynamic) : dynamic(isDynamic), ptr(p)  {
			if (ptr == nullptr) {
				dynamic = false;
			}
		}

		SmartPointer(const SmartPointer& other) : SmartPointer(other.ptr) {}

		~SmartPointer() {
			if (dynamic&&ptr != nullptr) {
				delete ptr;
			}
		}

		T* get() {
			return ptr;
		}
		const T* get() const {
			return ptr;
		}

		inline T operator*() {
			return *ptr;
		}
		inline const T operator*() const {
			return *ptr;
		}

		inline T* operator->() {
			return get();
		}
		inline const T* operator->() const {
			return get();
		}

		/**
		Gives ownership to another `SmartPointer`
		<p>
		If `this` does not hold ownership, nothing will occur when `swap()` is called.
		@param other What to give the ownership of the pointer to. It must hold the same pointer.
		@throws AssertionFailedError if `other` does not hold the same pointer as `this`
		*/
		void swap(SmartPointer& other) {
			if (other.ptr == ptr) {
				MACE__THROW(AssertionFailed, "You can only swap PtrContainers that hold the same pointer!");
			}

			other.dynamic = dynamic;
			release();
		}

		T* reset(T* newPtr = nullptr, bool newDynamic = false) {
			T* oldPtr = ptr;
			ptr = newPtr;
			if (dynamic&&oldPtr != nullptr) {
				delete oldPtr;
			}
			dynamic = newDynamic;
			return oldPtr;
		}

		/**
		Releases ownership of the pointer. The held pointer will not be deleted at destruction.
		*/
		T* release() {
			T* oldPtr = ptr;
			ptr = nullptr;
			dynamic = false;
			return oldPtr;
		}

		bool isDynamic() const {
			return dynamic;
		}

		bool operator==(const SmartPointer& other) const {
			return ptr == other.ptr && dynamic == other.dynamic;
		}

		bool operator!=(const SmartPointer& other) const {
			return !operator==(other);
		}

		bool operator==(const T* other) const {
			return ptr == other;
		}

		bool operator!=(const T* other) const {
			return !operator==(other);
		}
	private:
		bool dynamic;

		T* ptr;
	};//SmartPointer
}//mc

#endif//MACE_CORE_INTERFACES_H