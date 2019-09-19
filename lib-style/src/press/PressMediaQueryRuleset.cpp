#include "PressMediaQueryRuleset.h"
#include "PressStylesheet.h"
#include "MediaQuery.h"

namespace Press {

PressMediaQueryRuleset::PressMediaQueryRuleset(TokenList &selector,
	const PressRuleset &parent)
	: PressRuleset(*(new PressSelector()), parent) {
	this->selector = selector;
}

PressMediaQueryRuleset::~PressMediaQueryRuleset() {
}

void PressMediaQueryRuleset::process(Stylesheet &s,
	const Selector *prefix,
	ProcessingContext &context) const {
	Selector *rulesetselector;
	TokenList queryselector;
	MediaQuery *query;
	Ruleset *target;

	queryselector = selector;
	context.processValue(queryselector);

	query = s.createMediaQuery(queryselector);

	if (prefix != NULL) {
		rulesetselector = new Selector(*prefix);
		context.interpolate(*rulesetselector);
		target = query->createRuleset(*rulesetselector);

		processStatements(*target, &context);
	} else
		processStatements(*query, &context);
}

}
