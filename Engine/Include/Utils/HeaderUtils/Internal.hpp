#pragma once

/*
Implements an alternative to the "internal" access modifier available in C#.
Class members marked with "INTERNAL" will have different access levels based on
whether the preprocessor symbol "RIGEL_INTERNAL" is defined.

If "RIGEL_INTERNAL" is defined, "INTERNAL" is replaced with "public". Otherwise,
it is replaced with "private". This ensures that certain class members are only
publicly available inside the project that defines "RIGEL_INTERNAL".
*/

#ifdef RIGEL_INTERNAL
	#define INTERNAL public
#else
	#define INTERNAL private
#endif 
