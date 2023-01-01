#include "utils.h"


namespace viki {
	int stringlen(const char text[])
	{
		int count = 0, i = 0;
		if (text == 0)
			return 0;
		while (text[i++] != '\0')
			count++;
		return count;
	}

	// ArrayListChar/Byte/Float
	// compile time always!!!
	char* removeall(const char* text, const char character)
	{
		int totalsize = ilian::stringlen(text);
		//1 count all delimiters
		// loop all text and count
		const char* it = text, * it2 = text;
		int delimiters = 0;
		while (*it++ != '\0')
		{
			if (*it == character)
				delimiters++;
		}

		char* newmem = new char[totalsize - delimiters + 1]{ 0 }; //for null term
		char* n = newmem;
		for (const char* c = text; *c != '\0'; c++)
		{
			if (*c != character)
				*n++ = *c;
		}

		return newmem;
	}


	char* replaceall(const char* text, const char old, const char newc)
	{
		int totalsize = ilian::stringlen(text);
		//1 count all delimiters
		// loop all text and count
		const char* it = text, * it2 = text;


		char* newmem = new char[totalsize + 1]{ 0 }; //for null term
		char* n = newmem;
		for (const char* c = text; *c != '\0'; c++)
		{
			if (*c == old)
				*n++ = newc;
			else
				*n++ = *c;
		}
		*n++ = '\0';
		return newmem;
	}


}