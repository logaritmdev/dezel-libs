#include "StylerClosure.h"
#include "ProcessingContext.h"
#include "StylerRuleset.h"

StylerClosure::StylerClosure(const StylerRuleset &ruleset, const StylerMixinCall &stack) {
	this->ruleset = &ruleset;
	this->stack = &stack;
}

bool StylerClosure::call(StylerMixinArguments &args,
	Ruleset &target,
	ProcessingContext &context,
	bool defaultVal) const {
	return ruleset->call(args, target, context, defaultVal);
}

bool StylerClosure::call(StylerMixinArguments &args,
	Stylesheet &s,
	ProcessingContext &context,
	bool defaultVal) const {
	return ruleset->call(args, s, context, defaultVal);
}

void StylerClosure::getFunctions(std::list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	TokenList::const_iterator offset,
	const ProcessingContext &context) const {
	const std::list<StylerRuleset *> &nestedRules = ruleset->getNestedRules();
	const std::list<StylerClosure *> *closures = context.getClosures(ruleset);
	std::list<StylerRuleset *>::const_iterator r_it;
	std::list<StylerClosure *>::const_iterator c_it;
	TokenList::const_iterator offset2;

	offset2 = getLessSelector().walk(offset, mixin.name.end());

	if (offset2 == offset)
		return;

	while (offset2 != mixin.name.end() &&
		   ((*offset2).type == Token::WHITESPACE || *offset2 == ">")) {
		offset2++;
	}

	if (getLessSelector().matchArguments(mixin.arguments)) {
		if (offset2 == mixin.name.end()) {
			functionList.push_back(this);
		} else {
			for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
				(*r_it)->getFunctions(functionList, mixin, offset2, context);
			}
			if (closures != NULL) {
				for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
					(*c_it)->getFunctions(functionList, mixin, offset2, context);
				}
			}
		}
	}
}

const StylerSelector &StylerClosure::getLessSelector() const {
	return ruleset->getLessSelector();
}

const TokenList *StylerClosure::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t;

	if ((t = ruleset->getVariable(key, context)) != NULL)
		return t;
	return this->stack->getVariable(key, context);
}

void StylerClosure::getLocalFunctions(std::list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	const ProcessingContext &context) const {
	ruleset->getFunctions(functionList, mixin, mixin.name.begin(), context);
	if (functionList.empty())
		stack->getFunctions(functionList, mixin, context);
}
