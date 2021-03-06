#include "Stylesheet.h"
#include "AtRule.h"
#include "Comment.h"
#include "MediaQuery.h"
#include "Ruleset.h"
#include "StylesheetStatement.h"

namespace Press {

Stylesheet::~Stylesheet() {
	rulesets.clear();
	atrules.clear();
	while (!statements.empty()) {
		delete statements.back();
		statements.pop_back();
	}
}

void Stylesheet::addStatement(StylesheetStatement &statement) {
	statements.push_back(&statement);
	statement.setStylesheet(this);
}

void Stylesheet::addRuleset(Ruleset &ruleset) {
	addStatement(ruleset);
	rulesets.push_back(&ruleset);
}

void Stylesheet::addAtRule(AtRule &rule) {
	addStatement(rule);
	atrules.push_back(&rule);
}

Ruleset *Stylesheet::createRuleset(Selector &selector) {
	Ruleset *r = new Ruleset(selector);
	addRuleset(*r);
	return r;
}

AtRule *Stylesheet::createAtRule(const Token &keyword) {
	AtRule *r = new AtRule(keyword);

	addStatement(*r);
	atrules.push_back(r);
	return r;
}

Comment *Stylesheet::createComment() {
	Comment *c = new Comment();
	addStatement(*c);
	return c;
}

MediaQuery *Stylesheet::createMediaQuery(const TokenList &selector) {
	MediaQuery *q = new MediaQuery(selector);

	addStatement(*q);
	return q;
}

void Stylesheet::deleteStatement(StylesheetStatement &statement) {
	statements.remove(&statement);
	delete &statement;
}

void Stylesheet::deleteRuleset(Ruleset &ruleset) {
	rulesets.remove(&ruleset);
	deleteStatement(ruleset);
}

void Stylesheet::deleteAtRule(AtRule &atrule) {
	atrules.remove(&atrule);
	deleteStatement(atrule);
}

void Stylesheet::deleteMediaQuery(MediaQuery &query) {
	deleteStatement(query);
}

const std::list<AtRule *> &Stylesheet::getAtRules() const {
	return atrules;
}

const std::list<Ruleset *> &Stylesheet::getRulesets() const {
	return rulesets;
}

const std::list<StylesheetStatement *> &Stylesheet::getStatements() const {
	return statements;
}

Ruleset *Stylesheet::getRuleset(const Selector &selector) const {
	std::list<Ruleset *>::const_iterator it;

	for (it = rulesets.begin(); it != rulesets.end(); it++) {
		if ((*it)->getSelector().match(selector))
			return *it;
	}
	return NULL;
}

void Stylesheet::process(Stylesheet &s, void *context) const {
	std::list<StylesheetStatement *> statements = getStatements();
	std::list<StylesheetStatement *>::iterator i;

	for (i = statements.begin(); i != statements.end(); i++) {
		if ((*i)->isReference() == false)
			(*i)->process(s, context);
	}
}

void Stylesheet::write(Writer &writer) const {
	std::list<StylesheetStatement *> statements = getStatements();
	std::list<StylesheetStatement *>::iterator i;

	for (i = statements.begin(); i != statements.end(); i++) {
		(*i)->write(writer);
	}
}

}
