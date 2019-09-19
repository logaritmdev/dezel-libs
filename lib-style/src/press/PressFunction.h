#ifndef __Function_h__
#define __Function_h__

#include <list>

#include "TokenList.h"
#include "Ruleset.h"

namespace Press {

class PressSelector;
class PressMixin;
class PressMixinArguments;
class ProcessingContext;
class PressSelector;
class PressMixinCall;

class PressFunction {

public:
	virtual bool call(PressMixinArguments &args, Ruleset &target, ProcessingContext &context, bool defaultVal = false) const = 0;
	virtual bool call(PressMixinArguments &args, Stylesheet &s, ProcessingContext &context, bool defaultVal = false) const = 0;
	virtual void getFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, TokenList::const_iterator selector_offset, const ProcessingContext &context) const = 0;
	virtual void getLocalFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, const ProcessingContext &context) const = 0;
	virtual const TokenList *getVariable(const std::string &key, const ProcessingContext &context) const = 0;
	virtual const PressSelector &getPressSelector() const = 0;
};

}

#endif
