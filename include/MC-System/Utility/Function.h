#pragma once

namespace mc {
	/**
	Holds a Function from a lambda expression or a function pointer. Allows for the versatility of using anonymous functions or predefined functions.
	<p>
	Example:
	{@code
		#include <iostream>

		void foo(Function<void> bar){
			bar();
		}

		void exampleFunction(){
			std::cout<<"Functions!"
		}

		int main(int argc, char* argv){
			foo([]->(std::cout<<"Lambdas!";));
			foo(exampleFunction);
		}
	}
	<p>
	Examples:
	{@code
		Function<double, double> tan; //creates a function that returns a double and takes in a double
		//called like this:
		double result = tan(5.0);

		Fuction<void, char*> log;//creates a function that returns void and takes in a char*
		//called like this:
		log("Hello world!");
	}
	@tparam R The return type of the `Function`. Can also be `void`
	@tparam P The parameters of the `Function`. Can have an infinite amount of parameters, or none.
	*/
	template<typename R,typename... P>
	class Function {
	public:
		R operator()(P... param) {
		
		};
	};
}