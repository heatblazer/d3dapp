#pragma once


class String
{
private: // only for the scope of the class 
	// resources
	char* text;
	int length; // member for length 
	//


	//private members
	void bzero();

public:

	String();

	String(const char s[]);

	String(const char s[], int sz);
		// construction of the object 
	~String();

	int Length() const;

	int findchar(const char symbol);
	
	bool compare(const String& ref);

	String& operator=(const String& lref);
	
	const char* data();

	bool operator==(const String& b);

	String operator+(const String& b);



};

