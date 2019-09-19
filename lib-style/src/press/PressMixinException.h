#ifndef __PressMixinException_h__
#define __PressMixinException_h__

#include <exception>
#include <string>
#include "PressMixin.h"
#include "Exception.h"

namespace Press {

class PressMixinException : public Exception {
	std::string err;

public:
	const PressMixin *mixin;
	PressMixinException(const PressMixin &mixin);
	~PressMixinException() throw() {};
	virtual const char* what() const throw();
};

}

#endif
