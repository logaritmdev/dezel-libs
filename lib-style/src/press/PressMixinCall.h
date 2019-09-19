#ifndef __PressMixinCall_h__
#define __PressMixinCall_h__

#include "TokenList.h"
#include "VariableMap.h"

namespace Press {

class PressFunction;
class PressMixin;
class ProcessingContext;

class PressMixinCall {

public:
	PressMixinCall *parent;
	const PressFunction *function;
	VariableMap arguments;
	bool savepoint, important;
	PressMixinCall(PressMixinCall *parent, const PressFunction &function, bool savepoint = false,	bool important = false);
	const TokenList *getVariable(const std::string &key, const ProcessingContext &context) const;
	void getFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, const ProcessingContext &context) const;
	bool isInStack(const PressFunction &function) const;
	const VariableMap *getArguments(const PressFunction &function) const;
};

}

#endif
