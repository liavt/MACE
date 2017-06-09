/*
The MIT License (MIT)

Copyright (c) 2016 Liav Turkia

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/
#pragma once
#ifndef MACE_CORE_INTERFACES_H
#define MACE_CORE_INTERFACES_H
#include <memory>

namespace mc {
	class Initializer;

	class Initializable {
		friend class Initializer;
	protected:
		virtual void init() = 0;

		virtual void destroy() = 0;
	};//Initializable

	class Initializer final: public Initializable {
	public:
		Initializer();
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
	private:
		bool initialized = false;

		Initializable* obj;
	};//Initializer
}//mc

#endif//MACE_CORE_INTERFACES_H