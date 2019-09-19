#include "PressMixinException.h"

namespace Press {

PressMixinException::PressMixinException(const PressMixin &mixin) :
	Exception(mixin.name.front()), mixin(&mixin) {
	err.append("No ruleset definition found for: ");
	err.append(mixin.name.toString());
}

const char* PressMixinException::what() const throw() {
	return err.c_str();
}

}
