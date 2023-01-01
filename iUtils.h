#pragma once

#include <fstream>

namespace ilian
{


	//Rh  -
	//   abAbAbabababababab -> &[0]++++++++++++
	int stringlen(const char* text);

	const char* sstrchr(const char* str, char c);
	
	// viki@world -> (viki@world@home@bar, home) -> *> world if or null <summary>
	const char* sstrstr(const char* str, const char* pat);
	
	// viki: do not read 
	std::string readfile(const char* fname);
	
	bool isdigit(char d);

	int alphatoint(const char* str);

	double alphatof(const char* s);
	
	char* strcat(const char* a, const char* b, int sizea, int sizeb);


} // ilian 


//ax7777@ilian@7778ad@stringlen