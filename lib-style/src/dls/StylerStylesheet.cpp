#include "StylerStylesheet.h"
#include "StylerMediaQuery.h"

StylerStylesheet::StylerStylesheet() {
}

StylerStylesheet::~StylerStylesheet() {
}

StylerRuleset *StylerStylesheet::createStylerRuleset(StylerSelector &selector) {
	std::list<TokenList>::iterator it;
	StylerRuleset *r = new StylerRuleset(selector, *this);

	addRuleset(*r);
	for (it = selector.begin(); it != selector.end(); it++) {
		lessrulesets.insert(std::pair<TokenList, StylerRuleset *>((*it), r));
	}
	return r;
}

StylerMixin *StylerStylesheet::createMixin(const TokenList &selector) {
	StylerMixin *m = new StylerMixin(selector, *this);

	addStatement(*m);
	return m;
}

StylerAtRule *StylerStylesheet::createLessAtRule(const Token &keyword) {
	StylerAtRule *atrule = new StylerAtRule(keyword);
	addAtRule(*atrule);
	return atrule;
}

StylerMediaQuery *StylerStylesheet::createLessMediaQuery(const TokenList &selector) {
	StylerMediaQuery *q = new StylerMediaQuery(selector, *this);

	addStatement(*q);
	return q;
}

void StylerStylesheet::deleteLessRuleset(StylerRuleset &ruleset) {
	std::list<TokenList>::const_iterator it;
	for (it = ruleset.getLessSelector().begin();
		 it != ruleset.getLessSelector().end();
		 it++) {
		lessrulesets.erase(*it);
	}

	deleteStatement(ruleset);
}

void StylerStylesheet::deleteMixin(StylerMixin &mixin) {
	deleteStatement(mixin);
}

void StylerStylesheet::getFunctions(std::list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	const ProcessingContext &context) const {
	std::multimap<TokenList, StylerRuleset *>::const_iterator i, low, up;
	const std::list<StylerClosure *> *closures;
	std::list<StylerClosure *>::const_iterator c_it;
	TokenList::const_iterator t_it;
	TokenList search;
	for (t_it = mixin.name.begin();
		 t_it != mixin.name.end() &&
		 (*t_it).type != Token::WHITESPACE &&
		 (*t_it) != ">";
		 t_it++) {
		search.push_back(*t_it);
	}

	low = lessrulesets.lower_bound(search);
	up = lessrulesets.upper_bound(search);

	for (i = low; i != up; i++) {
		(*i).second->getFunctions(functionList, mixin, mixin.name.begin(), context);
	}

	closures = context.getBaseClosures();
	for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
		(*c_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
	}
}

void StylerStylesheet::putVariable(const std::string &key, const TokenList &value) {
	variables[key] = value;
}

const TokenList *StylerStylesheet::getVariable(const std::string &key) const {
	return variables.getVariable(key);
}

const TokenList *StylerStylesheet::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t;

	if ((t = getVariable(key)) != NULL)
		return t;

	return context.getBaseVariable(key);
}

void StylerStylesheet::process(Stylesheet &s, void *context) const {
	std::list<StylerExtension> extensions;

	std::list<Ruleset *>::const_iterator r_it;
	std::list<StylerExtension>::iterator e_it;
	std::list<StylerClosure *> closureScope;

	((ProcessingContext *) context)->setLessStylesheet(*this);
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

