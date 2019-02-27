#include "StylerAtRule.h"
#include "StylerStylesheet.h"

StylerAtRule::StylerAtRule(const Token &keyword) : AtRule(keyword) {
}

StylerAtRule::~StylerAtRule() {
}

void StylerAtRule::process(Stylesheet &s, void *context) const {
	AtRule *target = s.createAtRule(getKeyword());

	target->setRule(getRule());

	((ProcessingContext *) context)->processValue(target->getRule());
}

void StylerAtRule::process(Ruleset &r, void *context) const {
	// Can't add @-rules to rulesets so ignore the statement.
	(void) r;
	(void) context;
}

void StylerAtRule::write(Writer &writer) const {
	(void) writer;
}
