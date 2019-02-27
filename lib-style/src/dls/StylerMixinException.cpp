#include "StylerMixinException.h"

StylerMixinException::StylerMixinException(const StylerMixin &mixin) :
	StylerException(mixin.name.front()), mixin(&mixin) {
	err.append("No ruleset definition found for: ");
	err.append(mixin.name.toString());
}

const char *StylerMixinException::what() const throw() {
	return err.c_str();
}
