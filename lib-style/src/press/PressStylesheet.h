#ifndef __PressStylesheet_h__
#define __PressStylesheet_h__

#include <list>
#include <map>
#include <string>

#include "Stylesheet.h"
#include "Token.h"
#include "TokenList.h"
#include "PressAtRule.h"
#include "PressRuleset.h"
#include "PressMixin.h"
#include "ProcessingContext.h"

namespace Press {

class PressMediaQuery;

class PressStylesheet : public Stylesheet {

private:
	std::multimap<TokenList, PressRuleset *> pressrulesets;
	VariableMap variables;

public:
	PressStylesheet();
	virtual ~PressStylesheet();
	PressRuleset *createPressRuleset(PressSelector &selector);
	PressMixin *createMixin(const TokenList &selector);
	PressAtRule *createPressAtRule(const Token &keyword);
	PressMediaQuery *createPressMediaQuery(const TokenList &selector);
	void deletePressRuleset(PressRuleset &ruleset);
	void deleteMixin(PressMixin &mixin);
	void putVariable(const std::string &key, const TokenList &value);
	virtual void getFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, const ProcessingContext &context) const;
	const TokenList *getVariable(const std::string &key) const;
	virtual const TokenList *getVariable(const std::string &key, const ProcessingContext &context) const;
	virtual void process(Stylesheet &s, void *context) const;
};

}

#endif
