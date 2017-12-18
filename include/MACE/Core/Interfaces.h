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

#include <memory>

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

	template<typename T>
	class SmartPointer {
	public:
		typedef void(*Deleter)(T*);

		static void DoNothing(T*) {};
		static void DeletePointer(T* ptr) {
			delete ptr;
		};
		static void DeleteArray(T ptr[]) {
			delete[] ptr;
		};

		explicit SmartPointer(T& p) : SmartPointer(&p) {}
		
		explicit SmartPointer(T* p, const Deleter deleter = &DeletePointer) : ptr(p, deleter) {}

		SmartPointer(const std::unique_ptr<T>&& otherPtr) : ptr(otherPtr) {}

		SmartPointer(const std::unique_ptr<T, Deleter>&& otherPtr) : ptr(otherPtr) {}

		SmartPointer(const SmartPointer& other) : SmartPointer(other.ptr.get()) {}

		SmartPointer(const SmartPointer&& other) : ptr(other.ptr, other.getDeleter()) {}

		T* get() {
			return ptr.get();
		}
		const T* get() const {
			return ptr.get();
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
		Swaps ownership and the pointer to another `SmartPointer`
		@param other What to give the ownership of the pointer to.
		*/
		void swap(SmartPointer& other) {
			ptr.swap(other.ptr);
		}

		void reset(T* newPtr = nullptr) {
			ptr.reset(newPtr);
		}

		/**
		Releases ownership of the pointer. The held pointer will not be deleted at destruction.
		*/
		T* release() {
			return ptr.release();
		}

		Deleter& getDeleter() {
			return ptr.get_deleter();
		}

		SmartPointer operator=(SmartPointer& other) {
			return SmartPointer<T>(other.ptr.get(), DoNothing);
		};

		SmartPointer& operator=(SmartPointer&& other) {
			ptr.operator=(std::move(other.ptr));
			return *this;
		}

		operator bool() const noexcept {
			return ptr.get() == nullptr;
		}

		bool operator==(const SmartPointer& other) const {
			return ptr == other.ptr;
		}

		bool operator!=(const SmartPointer& other) const {
			return !operator==(other);
		}

		bool operator==(const T* other) const {
			return ptr.get() == other;
		}

		bool operator!=(const T* other) const {
			return !operator==(other);
		}
	private:
		std::unique_ptr<T, Deleter> ptr;
	};//SmartPointer
}//mc

#endif//MACE_CORE_INTERFACES_H