#include "StylerMediaQueryRuleset.h"
#include "StylerStylesheet.h"
#include "MediaQuery.h"

StylerMediaQueryRuleset::StylerMediaQueryRuleset(TokenList &selector,
	const StylerRuleset &parent)
	: StylerRuleset(*(new StylerSelector()), parent) {
	this->selector = selector;
}

StylerMediaQueryRuleset::~StylerMediaQueryRuleset() {
}

void StylerMediaQueryRuleset::process(Stylesheet &s,
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
