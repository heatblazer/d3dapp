#include "String.h"
#include "utils.h"
#include "iUtils.h"


void String::bzero()
{
	for (int i = 0; i < length; i++)
		text[i] = 0;
}

String::String() : text{ nullptr }, length{ 0 } {}

String::String(const char s[]) : text{ nullptr }, length{ 0 } //, ..... 
	// construction of the object 
{
	length = ilian::stringlen(s);
	text = new char[length + 1];
	bzero();
	// a lot of bugs
	for (int i = 0; s[i] != '\0'; i++)
	{
		text[i] = s[i];
	}
	text[length] = '\0';
}

String::String(const char s[], int sz) : text{ nullptr }, length{ sz } //, ..... 
	// construction of the object 
{
	text = new char[sz + 1];
	bzero();
	// a lot of bugs
	for (int i = 0; i < sz; i++)
	{
		text[i] = s[i];
	}
	text[length] = '\0';
}

String::~String()
{
	delete[] text;
	length = 0;
}

int String::Length() const { return length; }

int String::findchar(const char symbol)
{
	int i = 0;
	if (!text) return 0;
	while (text[i++] != '\0')
	{
		if (text[i] == symbol) return i;
	}
	return 0;
}

bool String::compare(const String& ref)
{
	if (ref.Length() != Length())
		return false;
	for (int i = 0; i < Length(); i++)
		if (text[i] != ref.text[i])
			return false;
	return true;
}

String& String::operator=(const String& lref)
{
	text = new char[lref.length + 1];

	int debuglen = lref.Length();
	for (int i = 0; i < debuglen; i++)
	{
		text[i] = lref.text[i];
	}
	length = lref.length;
	text[length] = '\0';

	return *this;
}

const char* String::data() { return text; }

bool String::operator==(const String& b)
{
	// todo - viki
	return compare(b);
}

String String::operator+(const String& b)
{
	char* result = ilian::strcat(this->text, b.text, this->length, b.length);
	String* resultL = new String(result);
	return *resultL;
}
