#ifndef __PressAtRule_h__
#define __PressAtRule_h__

#include "AtRule.h"
#include "Stylesheet.h"
#include "RulesetStatement.h"
#include "Token.h"

namespace Press {

class PressStylesheet;

class PressAtRule : public AtRule, public RulesetStatement {

public:
	PressAtRule(const Token &keyword);
	virtual ~PressAtRule();
	virtual void process(Stylesheet &s, void *context) const;
	virtual void process(Ruleset &r, void *context) const;
	virtual void write(Writer &writer) const;
};

}

#endif
