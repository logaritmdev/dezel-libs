#include "StylerMixinCall.h"
#include "StylerFunction.h"
#include "StylerMixin.h"
#include "ProcessingContext.h"

StylerMixinCall::StylerMixinCall(StylerMixinCall *parent,
	const StylerFunction &function,
	bool savepoint,
	bool important) :
	parent(parent), function(&function), savepoint(savepoint) {

	this->important = important || (parent != NULL && parent->important);
}

const TokenList *StylerMixinCall::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t;

	if ((t = function->getVariable(key, context)) != NULL)
		return t;

	if (parent != NULL)
		return parent->getVariable(key, context);
	return NULL;
}

void StylerMixinCall::getFunctions(std::list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	const ProcessingContext &context) const {
	function->getLocalFunctions(functionList, mixin, context);
	if (!functionList.empty())
		return;

	if (parent != NULL)
		parent->getFunctions(functionList, mixin, context);
}

bool StylerMixinCall::isInStack(const StylerFunction &function) const {
	return (this->function == &function) ||
		   (parent != NULL && parent->isInStack(function));
}

const VariableMap *StylerMixinCall::getArguments(const StylerFunction &function) const {
	if (this->function == &function)
		return &arguments;

	if (parent != NULL)
		return parent->getArguments(function);

	return NULL;
}
