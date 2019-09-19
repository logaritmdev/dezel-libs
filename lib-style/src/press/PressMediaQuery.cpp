#include "PressMediaQuery.h"
#include "MediaQuery.h"

namespace Press {

PressMediaQuery::PressMediaQuery(const TokenList &selector,
	const PressStylesheet &parent) :
	selector(selector), parent(&parent) {

}

PressMediaQuery::~PressMediaQuery() {
}

TokenList &PressMediaQuery::getSelector() {
	return selector;
}

const TokenList &PressMediaQuery::getSelector() const {
	return selector;
}

const PressStylesheet &PressMediaQuery::getPressStylesheet() const {
	return *parent;
}

void PressMediaQuery::getFunctions(std::list<const PressFunction *> &functionList,
	const PressMixin &mixin,
	const ProcessingContext &context) const {
	PressStylesheet::getFunctions(functionList, mixin, context);
	getPressStylesheet().getFunctions(functionList, mixin, context);
}

const TokenList *PressMediaQuery::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t = PressStylesheet::getVariable(key, context);
	if (t == NULL)
		t = getPressStylesheet().getVariable(key, context);
	return t;
}

void PressMediaQuery::process(Stylesheet &s, void *context) const {
	MediaQuery *query;

	query = s.createMediaQuery(getSelector());
	((ProcessingContext *) context)->processValue(query->getSelector());

	PressStylesheet::process(*query, ((ProcessingContext *) context));
	((ProcessingContext *) context)->setPressStylesheet(*parent);
}

void PressMediaQuery::write(Writer &writer) const {
	PressStylesheet::write(writer);
}

}
