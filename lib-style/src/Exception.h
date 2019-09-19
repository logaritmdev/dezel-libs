#ifndef __Exception_h__
#define __Exception_h__

#include <exception>
#include <string>

#include "Token.h"

using std::string;

namespace Press {

class Exception : public std::exception {

public:

	string source;
	unsigned int line;
	unsigned int column;

	Exception(unsigned int line, unsigned int column, string source);
	Exception(const Token &token);
	~Exception() throw() {};

	void setLocation(unsigned int line, unsigned int column);
	unsigned int getLine();
	unsigned int getColumn();
	void setSource(string source);
	string getSource();

	virtual const char* what() const throw() = 0;

};

}

#endif
