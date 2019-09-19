#include "ProcessingContext.h"
#include "PressRuleset.h"
#include "PressStylesheet.h"
#include "PressMixinCall.h"

namespace Press {

ProcessingContext::ProcessingContext() {
	stack = NULL;
	contextStylesheet = NULL;
}

ProcessingContext::~ProcessingContext() {
}

void ProcessingContext::setPressStylesheet(const PressStylesheet &stylesheet) {
	contextStylesheet = &stylesheet;
}

const PressStylesheet *ProcessingContext::getPressStylesheet() const {
	return contextStylesheet;
}

const TokenList *ProcessingContext::getVariable(const std::string &key) const {
	const TokenList *t;

	if (stack != NULL) {
		if ((t = stack->getVariable(key, *this)) != NULL)
			return t;
		return NULL;
	} else
		return getPressStylesheet()->getVariable(key, *this);
}

const TokenList *ProcessingContext::getFunctionVariable
	(const std::string &key,
		const PressFunction *function) const {

	std::map<const PressFunction *, VariableMap>::const_iterator it;

	if ((it = variables.find(function)) != variables.end())
		return (*it).second.getVariable(key);
	else
		return NULL;
}

const TokenList *ProcessingContext::getBaseVariable
	(const std::string &key) const {
	return base_variables.getVariable(key);
}

void ProcessingContext::pushMixinCall(const PressFunction &function,
	bool savepoint,
	bool important) {
	stack = new PressMixinCall(stack, function, savepoint, important);
}

void ProcessingContext::popMixinCall() {
	if (stack != NULL) {
		stack = stack->parent;
	}
}

VariableMap *ProcessingContext::getStackArguments() const {
	if (stack != NULL)
		return &stack->arguments;
	else
		return NULL;
}

VariableMap *ProcessingContext::getStackArguments(const PressFunction *function) const {
	PressMixinCall *tmp = stack;
	while (tmp != NULL) {
		if (tmp->function == function)
			return &tmp->arguments;
		tmp = tmp->parent;
	}
	return NULL;
}

bool ProcessingContext::isStackEmpty() const {
	return stack == NULL;
}

bool ProcessingContext::isSavePoint() const {
	return (stack != NULL && stack->savepoint);
}

const PressFunction *ProcessingContext::getSavePoint() const {
	PressMixinCall *tmp = stack;
	while (tmp != NULL && !tmp->savepoint)
		tmp = tmp->parent;
	return (tmp == NULL) ? NULL : tmp->function;
}

bool ProcessingContext::isImportant() const {
	return (stack != NULL && stack->important);
}

void ProcessingContext::getFunctions(std::list<const PressFunction *> &functionList,
	const PressMixin &mixin) const {
	if (stack != NULL)
		stack->getFunctions(functionList, mixin, *this);
	else if (contextStylesheet != NULL)
		contextStylesheet->getFunctions(functionList, mixin, *this);
}

bool ProcessingContext::isInStack(const PressFunction &function) const {
	if (stack != NULL)
		return stack->isInStack(function);
	else
		return false;
}

void ProcessingContext::pushExtensionScope(std::list<PressExtension> &scope) {
	extensions.push_back(&scope);
}

void ProcessingContext::popExtensionScope() {
	extensions.pop_back();
}

void ProcessingContext::addExtension(PressExtension &extension) {
	if (!extensions.empty())
		extensions.back()->push_back(extension);
}

std::list<PressExtension> *ProcessingContext::getExtensions() {
	return (!extensions.empty()) ? extensions.back() : NULL;
}

void ProcessingContext::addClosure(const PressRuleset &ruleset) {
	if (stack == NULL)
		return;

	const PressFunction *fnc = getSavePoint();
	PressClosure *c = new PressClosure(ruleset, *stack);

	if (fnc != NULL)
		closures[fnc].push_back(c);
	else
		base_closures.push_back(c);
}

void ProcessingContext::addVariables(const VariableMap &variables) {
	const PressFunction *fnc = getSavePoint();
	if (fnc != NULL)
		this->variables[fnc].overwrite(variables);
	else
		base_variables.overwrite(variables);
}

const std::list<PressClosure *> *ProcessingContext::getClosures(const PressFunction *function) const {
	std::map<const PressFunction *, std::list<PressClosure *>>::const_iterator it;

	if ((it = closures.find(function)) != closures.end())
		return &(*it).second;
	else
		return NULL;
}

const std::list<PressClosure *> *ProcessingContext::getBaseClosures() const {
	return &base_closures;
}

ValueProcessor *ProcessingContext::getValueProcessor() {
	return &processor;
}

void ProcessingContext::interpolate(Selector &selector) const {
	std::list<TokenList>::iterator it;

	for (it = selector.begin(); it != selector.end(); it++) {
		processor.interpolate(*it, *this);
	}
}

void ProcessingContext::interpolate(TokenList &tokens) const {
	processor.interpolate(tokens, *this);
}

void ProcessingContext::interpolate(std::string &str) const {
	processor.interpolate(str, *this);
}

void ProcessingContext::processValue(TokenList &value) const {
	processor.processValue(value, *this);
}

bool ProcessingContext::validateCondition(const TokenList &value, bool defaultVal) const {
	return processor.validateCondition(value, *this, defaultVal);
}

}
