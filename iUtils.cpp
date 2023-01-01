#include "iUtils.h"
#include <fstream>

namespace ilian
{


	//Rh  -
//   abAbAbabababababab -> &[0]++++++++++++
	int stringlen(const char* text)
	{
		int count = 0, i = 0;
		while (*text++ != '\0')
			count++;
		return count;
	}

	const char* sstrchr(const char* str, char c)
	{
		while (*str++ != '\0') {
			if (*str == c)
				return ++str;
		}
		return nullptr;
	}
	// viki@world -> (viki@world@home@bar, home) -> *> world if or null <summary>
	const char* sstrstr(const char* str, const char* pat)
	{
		do {
			// will enter at least 1 time 
			const char* temp = pat; // home 
			const char* it = str; // original string without modify
			do {
				if (*temp == '\0') return str + ilian::stringlen(pat);
			} while (*temp++ == *it++);
			//????
		} while (*str++ != '\0');      ////// -- till the end 
		return nullptr;
	}

	// viki: do not read 
	std::string readfile(const char* fname)
	{
		std::ifstream ifs{ fname };
		std::string content((std::istreambuf_iterator<char>(ifs)),
			(std::istreambuf_iterator<char>()));
		return content;
	}

	bool isdigit(char d)
	{
		return d >= '0' && d <= '9';
	}

	int alphatoint(const char* str)
	{
		if (!str) return 0;
		int sign = 1, index = 0, result = 0;
		if (str[0] == '-')
		{
			sign = -1;
			index++;
		}
		for (; str[index] != '\0'; index++)
		{
			result = 10 * result + (str[index] - '0');
		}
		return result * sign;
	}

	double alphatof(const char* s)
	{
		double val, power;
		int i = 0, sign;

		for (i = 0; s[i] == ' ' || s[i] == '\t' || s[i] == '\n'; i++);


		sign = (s[i] == '-') ? -1 : 1;
		if (s[i] == '-') i++;
		for (val = 0.0; s[i] >= '0' && s[i] <= '9'; i++)
		{
			val = 10.0 * val + (s[i] - '0');
		}
		if (s[i] == '.')
			i++;
		for (power = 1.0; s[i] >= '0' && s[i] <= '9'; i++)
		{
			val = 10 * val + (s[i] - '0');
			power *= 10;
		}

		return sign * val / power;
	}



	char* strcat(const char* a, const char* b, int sizea, int sizeb)
	{
		char* result = new char[sizea + sizeb + 1];
		for (int i = 0; i < sizea; i++)
			result[i] = *a++;
		for (int i = sizea; i < sizeb + sizea; i++)
			result[i] = *b++;
		result[sizea + sizeb] = '\0';
		return result;
	}





}