#include "PressStylesheet.h"
#include "PressMediaQuery.h"

namespace Press {

PressStylesheet::PressStylesheet() {
}

PressStylesheet::~PressStylesheet() {
}

PressRuleset *PressStylesheet::createPressRuleset(PressSelector &selector) {
	std::list<TokenList>::iterator it;
	PressRuleset *r = new PressRuleset(selector, *this);

	addRuleset(*r);
	for (it = selector.begin(); it != selector.end(); it++) {
		pressrulesets.insert(std::pair<TokenList, PressRuleset *>((*it), r));
	}
	return r;
}

PressMixin *PressStylesheet::createMixin(const TokenList &selector) {
	PressMixin *m = new PressMixin(selector, *this);

	addStatement(*m);
	return m;
}

PressAtRule *PressStylesheet::createPressAtRule(const Token &keyword) {
	PressAtRule *atrule = new PressAtRule(keyword);
	addAtRule(*atrule);
	return atrule;
}

PressMediaQuery *PressStylesheet::createPressMediaQuery(const TokenList &selector) {
	PressMediaQuery *q = new PressMediaQuery(selector, *this);

	addStatement(*q);
	return q;
}

void PressStylesheet::deletePressRuleset(PressRuleset &ruleset) {
	std::list<TokenList>::const_iterator it;
	for (it = ruleset.getPressSelector().begin();
		 it != ruleset.getPressSelector().end();
		 it++) {
		pressrulesets.erase(*it);
	}

	deleteStatement(ruleset);
}

void PressStylesheet::deleteMixin(PressMixin &mixin) {
	deleteStatement(mixin);
}

void PressStylesheet::getFunctions(std::list<const PressFunction *> &functionList,
	const PressMixin &mixin,
	const ProcessingContext &context) const {
	std::multimap<TokenList, PressRuleset *>::const_iterator i, low, up;
	const std::list<PressClosure *> *closures;
	std::list<PressClosure *>::const_iterator c_it;
	TokenList::const_iterator t_it;
	TokenList search;
	for (t_it = mixin.name.begin();
		 t_it != mixin.name.end() &&
		 (*t_it).type != Token::WHITESPACE &&
		 (*t_it) != ">";
		 t_it++) {
		search.push_back(*t_it);
	}

	low = pressrulesets.lower_bound(search);
	up = pressrulesets.upper_bound(search);

	for (i = low; i != up; i++) {
		(*i).second->getFunctions(functionList, mixin, mixin.name.begin(), context);
	}

	closures = context.getBaseClosures();
	for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
		(*c_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
	}
}

void PressStylesheet::putVariable(const std::string &key, const TokenList &value) {
	variables[key] = value;
}

const TokenList *PressStylesheet::getVariable(const std::string &key) const {
	return variables.getVariable(key);
}

const TokenList *PressStylesheet::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t;

	if ((t = getVariable(key)) != NULL)
		return t;

	return context.getBaseVariable(key);
}

void PressStylesheet::process(Stylesheet &s, void *context) const {
	std::list<PressExtension> extensions;

	std::list<Ruleset *>::const_iterator r_it;
	std::list<PressExtension>::iterator e_it;
	std::list<PressClosure *> closureScope;

	((ProcessingContext *) context)->setPressStylesheet(*this);
	((ProcessingContext *) context)->pushExtensionScope(extensions);

	Stylesheet::process(s, context);

	// post processing
	for (e_it = extensions.begin(); e_it != extensions.end(); e_it++) {
		for (r_it = s.getRulesets().begin(); r_it != s.getRulesets().end(); r_it++) {
			(*e_it).updateSelector((*r_it)->getSelector());
		}
	}
	((ProcessingContext *) context)->popExtensionScope();

}

}
