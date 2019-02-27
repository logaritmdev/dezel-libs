#include "StylerMediaQuery.h"
#include "MediaQuery.h"

StylerMediaQuery::StylerMediaQuery(const TokenList &selector,
	const StylerStylesheet &parent) :
	selector(selector), parent(&parent) {

}

StylerMediaQuery::~StylerMediaQuery() {
}

TokenList &StylerMediaQuery::getSelector() {
	return selector;
}

const TokenList &StylerMediaQuery::getSelector() const {
	return selector;
}

const StylerStylesheet &StylerMediaQuery::getLessStylesheet() const {
	return *parent;
}

void StylerMediaQuery::getFunctions(std::list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	const ProcessingContext &context) const {
	StylerStylesheet::getFunctions(functionList, mixin, context);
	getLessStylesheet().getFunctions(functionList, mixin, context);
}

const TokenList *StylerMediaQuery::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t = StylerStylesheet::getVariable(key, context);
	if (t == NULL)
		t = getLessStylesheet().getVariable(key, context);
	return t;
}

void StylerMediaQuery::process(Stylesheet &s, void *context) const {
	MediaQuery *query;

	query = s.createMediaQuery(getSelector());
	((ProcessingContext *) context)->processValue(query->getSelector());

	StylerStylesheet::process(*query, ((ProcessingContext *) context));
	((ProcessingContext *) context)->setLessStylesheet(*parent);
}

void StylerMediaQuery::write(Writer &writer) const {
	StylerStylesheet::write(writer);
}
