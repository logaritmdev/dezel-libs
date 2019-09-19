#include "PressAtRule.h"
#include "PressStylesheet.h"

namespace Press {

PressAtRule::PressAtRule(const Token &keyword) : AtRule(keyword) {
}

PressAtRule::~PressAtRule() {
}

void PressAtRule::process(Stylesheet &s, void *context) const {
	AtRule *target = s.createAtRule(getKeyword());

	target->setRule(getRule());

	((ProcessingContext *) context)->processValue(target->getRule());
}

void PressAtRule::process(Ruleset &r, void *context) const {
	// Can't add @-rules to rulesets so ignore the statement.
	(void) r;
	(void) context;
}

void PressAtRule::write(Writer &writer) const {
	(void) writer;
}

}
