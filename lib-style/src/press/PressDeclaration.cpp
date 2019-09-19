#include "PressDeclaration.h"
#include "PressRuleset.h"

namespace Press {

void PressDeclaration::setPressRuleset(PressRuleset &r) {
	pressRuleset = &r;
}

PressRuleset *PressDeclaration::getPressRuleset() {
	return pressRuleset;
}


void PressDeclaration::process(Ruleset &r, void *context) const {
	Declaration *d = r.createDeclaration();
	d->setProperty(property);
	d->setValue(value);

	((ProcessingContext *) context)->interpolate(d->getProperty());
	((ProcessingContext *) context)->processValue(d->getValue());

	// If the `important` flag is set, append '!important'
	if (((ProcessingContext *) context)->isImportant()) {

		if (d->getValue().size() < 3 ||
			d->getValue().back() == "important") {
			d->getValue().push_back(Token::BUILTIN_SPACE);
			d->getValue().push_back(Token::BUILTIN_IMPORTANT);
		}

	}
}

}
