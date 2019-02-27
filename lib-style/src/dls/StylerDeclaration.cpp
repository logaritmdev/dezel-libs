#include "StylerDeclaration.h"
#include "StylerRuleset.h"

void StylerDeclaration::setLessRuleset(StylerRuleset &r) {
	lessRuleset = &r;
}

StylerRuleset *StylerDeclaration::getLessRuleset() {
	return lessRuleset;
}


void StylerDeclaration::process(Ruleset &r, void *context) const {
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
