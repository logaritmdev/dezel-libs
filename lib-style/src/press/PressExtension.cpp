#include "PressExtension.h"

namespace Press {

PressExtension::PressExtension() : all(false) {
}

PressExtension::~PressExtension() {
}

Selector &PressExtension::getTarget() {
	return target;
}

Selector &PressExtension::getExtension() {
	return extension;
}

const Selector &PressExtension::getTarget() const {
	return target;
}

const Selector &PressExtension::getExtension() const {
	return extension;
}

void PressExtension::setExtension(const Selector &s) {
	extension = s;
}


void PressExtension::setAll(bool b) {
	all = b;
}

bool PressExtension::isAll() const {
	return all;
}


void PressExtension::updateSelector(Selector &s) const {
	if (isAll()) {
		replaceInSelector(s);

	} else if (s.match(target)) {
		s.appendSelector(extension);
	}
}

void PressExtension::replaceInSelector(Selector &s) const {
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

}
