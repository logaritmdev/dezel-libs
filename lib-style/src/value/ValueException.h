#ifndef __value_ValueException_h__
#define __value_ValueException_h__

#include <string>
#include "TokenList.h"
#include "Exception.h"

namespace Press {

class ValueException : public Exception {

public:

	std::string err;

	ValueException(std::string message, const TokenList &source) :
		Exception(source.front()) {
		err = message;
	}

	virtual ~ValueException() throw() {};

	virtual const char* what() const throw() {
		return err.c_str();
	}
};

}

#endif
