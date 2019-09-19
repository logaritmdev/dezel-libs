#ifndef __PressMixin_h__
#define __PressMixin_h__

#include "Ruleset.h"
#include "Stylesheet.h"
#include "StylesheetStatement.h"
#include "RulesetStatement.h"
#include "PressMixinArguments.h"

namespace Press {

class PressStylesheet;
class PressRuleset;
class ProcessingContext;

class PressMixin : public StylesheetStatement, public RulesetStatement {

private:
	const PressStylesheet *pressStylesheet;
	const PressRuleset *pressRuleset;
	bool important;

public:
	TokenList name;
	PressMixinArguments arguments;
	PressMixin(const TokenList &name, const PressStylesheet &parent);
	PressMixin(const TokenList &name, const PressRuleset &parent);
	virtual ~PressMixin();
	bool call(ProcessingContext &context, Ruleset *ruleset, Stylesheet *stylesheet) const;
	void setImportant(bool b);
	bool isImportant() const;
	const PressStylesheet *getPressStylesheet() const;
	const PressRuleset *getPressRuleset() const;
	virtual void process(Ruleset &r, void *context) const;
	virtual void process(Stylesheet &s, void *context) const;
	virtual void write(Writer &writer) const;
};

}

#endif
