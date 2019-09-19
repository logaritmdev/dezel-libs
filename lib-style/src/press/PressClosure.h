#ifndef __PressClosure_h__
#define __PressClosure_h__

#include "PressFunction.h"
#include "PressMixin.h"
#include "Ruleset.h"

namespace Press {

class PressRuleset;
class ProcessingContext;
class PressMixinCall;

class PressClosure : public PressFunction {
public:
	const PressRuleset *ruleset;
	const PressMixinCall *stack;
	PressClosure(const PressRuleset &ruleset, const PressMixinCall &stack);
	virtual bool call(PressMixinArguments &args, Ruleset &target, ProcessingContext &context, bool defaultVal = false) const;
	virtual bool call(PressMixinArguments &args, Stylesheet &s, ProcessingContext &context, bool defaultVal = false) const;
	virtual void getFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, TokenList::const_iterator selector_offset, const ProcessingContext &context) const;
	virtual void getLocalFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, const ProcessingContext &context) const;
	virtual const PressSelector &getPressSelector() const;
	virtual const TokenList *getVariable(const std::string &key, const ProcessingContext &context) const;
	bool isInStack(const PressRuleset &ruleset);
};

}

#endif
