#include "StylerExtension.h"

StylerExtension::StylerExtension() : all(false) {
}

StylerExtension::~StylerExtension() {
}

Selector &StylerExtension::getTarget() {
	return target;
}

Selector &StylerExtension::getExtension() {
	return extension;
}

const Selector &StylerExtension::getTarget() const {
	return target;
}

const Selector &StylerExtension::getExtension() const {
	return extension;
}

void StylerExtension::setExtension(const Selector &s) {
	extension = s;
}


void StylerExtension::setAll(bool b) {
	all = b;
}

bool StylerExtension::isAll() const {
	return all;
}


void StylerExtension::updateSelector(Selector &s) const {
	if (isAll()) {
		replaceInSelector(s);

	} else if (s.match(target)) {
		s.appendSelector(extension);
	}
}

void StylerExtension::replaceInSelector(Selector &s) const {
	std::list<TokenList>::const_iterator it1, it2;

	for (it1 = target.begin();
		 it1 != target.end(); it1++) {

		for (it2 = extension.begin();
			 it2 != extension.end(); it2++) {

			// If no matches are found, there is no need to try other extension
			// selectors.
			if (!s.replace(*it1, *it2))
				break;
		}
	}
}
