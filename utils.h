#pragma once

#include "iUtils.h"
#include "Vector.h"
// this is COPY PASTE 1:1
namespace viki {

	int stringlen(const char text[]);
	// ArrayListChar/Byte/Float
	// compile time always!!!
	template <typename T>
	void swap(T& a, T& b)
	{
		T tmp = a;
		a = b;
		b = tmp;
	}

	template<typename T>
	void ssplit(const char* str, const char* delimiter, Vector<T>& vs)
	{
		for (const char* begin = str; begin != nullptr; begin = ilian::sstrstr(begin, delimiter))
		{
			const char* next = ilian::sstrstr(begin, delimiter);
			if (next)
			{
				int offset = next - begin;
				vs.PushBack(T{ begin, offset - ilian::stringlen(delimiter) });
			}
			else
			{
				int count = 0;
				const char* it = begin;
				while (*it++ != '\0') count++;
				if (count)
					vs.PushBack(T{ begin, count });
			}
		}
	}

	char* removeall(const char* text, const char character);

	char* replaceall(const char* text, const char old, const char newc);


}