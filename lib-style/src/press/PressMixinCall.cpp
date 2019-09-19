#include "PressMixinCall.h"
#include "PressFunction.h"
#include "PressMixin.h"
#include "ProcessingContext.h"

namespace Press {

PressMixinCall::PressMixinCall(PressMixinCall *parent,
	const PressFunction &function,
	bool savepoint,
	bool important) :
	parent(parent), function(&function), savepoint(savepoint) {

	this->important = important || (parent != NULL && parent->important);
}

const TokenList *PressMixinCall::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t;

	if ((t = function->getVariable(key, context)) != NULL)
		return t;

	if (parent != NULL)
		return parent->getVariable(key, context);
	return NULL;
}

void PressMixinCall::getFunctions(std::list<const PressFunction *> &functionList,
	const PressMixin &mixin,
	const ProcessingContext &context) const {
	function->getLocalFunctions(functionList, mixin, context);
	if (!functionList.empty())
		return;

	if (parent != NULL)
		parent->getFunctions(functionList, mixin, context);
}

bool PressMixinCall::isInStack(const PressFunction &function) const {
	return (this->function == &function) ||
		   (parent != NULL && parent->isInStack(function));
}

const VariableMap *PressMixinCall::getArguments(const PressFunction &function) const {
	if (this->function == &function)
		return &arguments;

	if (parent != NULL)
		return parent->getArguments(function);

	return NULL;
}

}
