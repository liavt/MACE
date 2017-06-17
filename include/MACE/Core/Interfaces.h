/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_CORE_INTERFACES_H
#define MACE_CORE_INTERFACES_H
#include <MACE/Core/Constants.h>
#include <memory>

namespace mc {
	class Initializer;

	class Initializable {
		friend class Initializer;
	public:
		virtual ~Initializable() = default;
	protected:
		virtual void init() = 0;

		virtual void destroy() = 0;
	};//Initializable

	class Initializer final: public Initializable {
	public:
		Initializer(Initializable* in);
		~Initializer();

		void init() final;
		void destroy() final;

		Initializable* get();

		const Initializable* get() const;

		Initializable* operator*();

		const Initializable* operator*() const;

		Initializable* operator->();

		const Initializable* operator->() const;

		bool isInitialized() const;

		bool operator==(const Initializer& other) const;
		bool operator!=(const Initializer& other) const;
	private:
		bool initialized = false;

		Initializable* obj;
	};//Initializer

	template<typename T>
	class SmartPointer {
	public:
		constexpr SmartPointer(T& p) : SmartPointer(&p) {}

		constexpr SmartPointer(T* p) : SmartPointer(p, false) {}

		constexpr SmartPointer(T* p, const bool isDynamic) : ptr(p), dynamic(isDynamic) {
			if (ptr == nullptr) {
				MACE__THROW(NullPointer, "Inputted pointer can not be nullptr!");
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

		/**
		Releases ownership of the pointer. The held pointer will not be deleted at destruction.
		*/
		void release() {
			dynamic = false;
		}

		bool isDynamic() const {
			return dynamic
		}

		bool operator==(const SmartPointer& other) const {
			return ptr == other.ptr && dynamic == other.dynamic;
		}

		bool operator!=(const SmartPointer& other) const {
			return !operator==(other);
		}
	private:
		bool dynamic;

		T* ptr;
	};//SmartPointer
}//mc

#endif//MACE_CORE_INTERFACES_H