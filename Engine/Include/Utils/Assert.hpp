#pragma once

#include <iostream>

#ifdef DEBUG_BUILD
	#define ASSERT(cond, message)											\
		if (!(cond)) {														\
			std::cerr << "Assertion Error! " << message << ". In file: " <<	\
			__FILE__ << ", at line: " << __LINE__ << '\n';					\
			__debugbreak();													\
		}																	\
		else { }
	#define STATIC_ASSERT(cond, message) static_assert(cond, message)
#else
	#define ASSERT(cond, message)
	#define STATIC_ASSERT(cond, message)
#endif
