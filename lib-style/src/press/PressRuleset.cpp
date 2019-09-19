#include "PressRuleset.h"
#include "PressStylesheet.h"
#include "PressMediaQueryRuleset.h"

namespace Press {

PressRuleset::PressRuleset(PressSelector &selector,
	const PressRuleset &parent) :
	Ruleset(selector),
	parent(&parent), pressStylesheet(NULL), selector(&selector) {

}

PressRuleset::PressRuleset(PressSelector &selector,
	const PressStylesheet &parent) :
	Ruleset(selector),
	parent(NULL), pressStylesheet(&parent), selector(&selector) {
}

PressRuleset::~PressRuleset() {
	while (!nestedRules.empty()) {
		delete nestedRules.back();
		nestedRules.pop_back();
	}
}

const PressSelector &PressRuleset::getPressSelector() const {
	return *selector;
}

void PressRuleset::addExtension(PressExtension &extension) {
	pressExtensions.push_back(extension);
}

PressDeclaration *PressRuleset::createPressDeclaration() {
	PressDeclaration *d = new PressDeclaration();

	Ruleset::addStatement(*d);
	d->setPressRuleset(*this);
	pressDeclarations.push_back(d);
	return d;
}

const std::list<PressDeclaration *> &PressRuleset::getPressDeclarations()
const {
	return pressDeclarations;
}

PressMixin *PressRuleset::createMixin(const TokenList &selector) {
	PressMixin *m = new PressMixin(selector, *this);

	Ruleset::addStatement(*m);
	pressMixins.push_back(m);
	stylesheetStatements.push_back(m);
	return m;
}

const std::list<PressMixin *> &PressRuleset::getMixins()
const {
	return pressMixins;
}

PressAtRule *PressRuleset::createPressAtRule(const Token &keyword) {
	PressAtRule *r = new PressAtRule(keyword);

	Ruleset::addStatement(*r);
	pressAtRules.push_back(r);
	stylesheetStatements.push_back(r);
	return r;
}

const std::list<PressAtRule *> &PressRuleset::getPressAtRules()
const {
	return pressAtRules;
}

const std::list<StylesheetStatement *> &PressRuleset::getStylesheetStatements()
const {
	return stylesheetStatements;
}

PressRuleset *PressRuleset::createNestedRule(PressSelector &selector) {
	PressRuleset *r = new PressRuleset(selector, *this);

	nestedRules.push_back(r);
	return r;
}

PressMediaQueryRuleset *PressRuleset::createMediaQuery(TokenList &selector) {
	PressMediaQueryRuleset *r = new PressMediaQueryRuleset(selector, *this);

	nestedRules.push_back(r);
	return r;
}

void PressRuleset::deleteNestedRule(PressRuleset &ruleset) {
	nestedRules.remove(&ruleset);
	delete &ruleset;
}

const std::list<PressRuleset *> &PressRuleset::getNestedRules() const {
	return nestedRules;
}

void PressRuleset::putVariable(const std::string &key, const TokenList &value) {
	variables[key] = value;
}

VariableMap &PressRuleset::getVariables() {
	return variables;
}

const TokenList *PressRuleset::getVariable(const std::string &key) const {
	return variables.getVariable(key);
}

void PressRuleset::setParent(const PressRuleset *r) {
	parent = r;
}

const PressRuleset *PressRuleset::getParent() const {
	return parent;
}

void PressRuleset::setPressStylesheet(const PressStylesheet &s) {
	pressStylesheet = &s;
}

const PressStylesheet *PressRuleset::getPressStylesheet() const {
	if (pressStylesheet != NULL)
		return pressStylesheet;
	else if (parent != NULL)
		return parent->getPressStylesheet();
	else
		return NULL;
}

void PressRuleset::processExtensions(ProcessingContext &context,
	const Selector *prefix) const {
	const std::list<PressExtension> &e = getPressSelector().getExtensions();
	std::list<PressExtension>::const_iterator e_it;
	PressExtension extension;

	for (e_it = e.begin(); e_it != e.end(); e_it++) {
		extension = *e_it;
		if (prefix != NULL)
			extension.getExtension().addPrefix(*prefix);

		context.interpolate(extension.getExtension());

		context.addExtension(extension);
	}
}

void PressRuleset::processInlineExtensions(ProcessingContext &context,
	const Selector &selector) const {
	std::list<PressExtension>::const_iterator e_it;
	PressExtension extension;

	for (e_it = pressExtensions.begin(); e_it != pressExtensions.end(); e_it++) {
		extension = *e_it;

		extension.setExtension(selector);

		context.addExtension(extension);
	}
}

bool PressRuleset::call(PressMixinArguments &args,
	Ruleset &target,
	ProcessingContext &context,
	bool defaultVal) const {
	if (call(args, context, &target, NULL, defaultVal)) {
		processInlineExtensions(context, target.getSelector());
		return true;
	} else
		return false;
}

bool PressRuleset::call(PressMixinArguments &args,
	Stylesheet &target,
	ProcessingContext &context,
	bool defaultVal) const {
	return call(args, context, NULL, &target, defaultVal);
}

bool PressRuleset::call(PressMixinArguments &args,
	ProcessingContext &context,
	Ruleset *ruleset,
	Stylesheet *stylesheet,
	bool defaultVal) const {

	if (putArguments(args, *context.getStackArguments()) &&
		matchConditions(context, defaultVal)) {

		if (ruleset != NULL)
			processStatements(*ruleset, &context);
		else
			processStatements(*stylesheet, &context);

		addClosures(context);
		// process variables and add to context.variables
		if (context.getStackArguments() != NULL)
			context.addVariables(*context.getStackArguments());
		context.addVariables(variables);

		return true;
	} else
		return false;
}

void PressRuleset::process(Stylesheet &s, void *context) const {
	process(s, NULL, *((ProcessingContext *) context));
}

void PressRuleset::process(Stylesheet &s,
	const Selector *prefix,
	ProcessingContext &context) const {
	Ruleset *target;
	Selector *selector;

	if (getPressSelector().needsArguments())
		return;

	if (!matchConditions(context))
		return;

	selector = new Selector(getSelector());
	if (prefix != NULL)
		selector->addPrefix(*prefix);

	context.interpolate(*selector);

	target = s.createRuleset(*selector);

	processExtensions(context, prefix);
	processInlineExtensions(context, target->getSelector());
	context.pushMixinCall(*this, true);
	processStatements(*target, &context);
	context.popMixinCall();

	mergeDeclarations(*target);
}

void PressRuleset::processStatements(Ruleset &target,
	void *context) const {
	// process statements
	Ruleset::processStatements(target, context);

	// insert nested rules
	insertNestedRules(*target.getStylesheet(), &target.getSelector(),
					  *(ProcessingContext *) context);
}

void PressRuleset::processStatements(Stylesheet &target,
	void *context) const {
	const std::list<StylesheetStatement *> &stylesheetStatements =
		getStylesheetStatements();
	std::list<StylesheetStatement *>::const_iterator it;

	// insert mixins
	for (it = stylesheetStatements.begin();
		 it != stylesheetStatements.end();
		 it++) {
		(*it)->process(target, context);
	}

	// insert nested rules
	insertNestedRules(target, NULL, *(ProcessingContext *) context);
}

const TokenList *PressRuleset::getVariable(const std::string &key,
	const ProcessingContext &context) const {
	const TokenList *t;
	const VariableMap *m;

	if ((t = getVariable(key)) != NULL)
		return t;

	if ((m = context.getStackArguments(this)) != NULL) {
		if ((t = m->getVariable(key)) != NULL)
			return t;
	}

	if ((t = context.getFunctionVariable(key, this)) != NULL)
		return t;

	if (parent != NULL)
		return parent->getVariable(key, context);
	else
		return getPressStylesheet()->getVariable(key, context);
}

void PressRuleset::getFunctions(list<const PressFunction *> &functionList,
	const PressMixin &mixin,
	TokenList::const_iterator offset,
	const ProcessingContext &context) const {
	std::list<PressRuleset *>::const_iterator r_it;
	const std::list<PressClosure *> *closures;
	std::list<PressClosure *>::const_iterator c_it;
	TokenList::const_iterator offset2;

	offset2 = getSelector().walk(offset, mixin.name.end());

	if (offset2 == offset)
		return;

	while (offset2 != mixin.name.end() &&
		   ((*offset2).type == Token::WHITESPACE || *offset2 == ">")) {
		offset2++;
	}

	if (offset2 == mixin.name.end()) {
		if (selector->matchArguments(mixin.arguments)) {
			functionList.push_back(this);
		}
	} else {
		if (!selector->needsArguments() && matchConditions(context)) {

			for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
				(*r_it)->getFunctions(functionList, mixin, offset2, context);
			}
			closures = context.getClosures(this);
			if (closures != NULL) {
				for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
					(*c_it)->getFunctions(functionList, mixin, offset2, context);
				}
			}
		}
	}
}

void PressRuleset::getLocalFunctions(std::list<const PressFunction *> &functionList,
	const PressMixin &mixin,
	const ProcessingContext &context) const {
	getLocalFunctions(functionList, mixin, NULL, context);
}

void PressRuleset::getLocalFunctions(std::list<const PressFunction *> &functionList,
	const PressMixin &mixin,
	const PressRuleset *exclude,
	const ProcessingContext &context) const {
	std::list<PressRuleset *>::const_iterator r_it;
	const std::list<PressClosure *> *closures;
	std::list<PressClosure *>::const_iterator c_it;

	for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
		if ((*r_it) != exclude) {
			(*r_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
		}
	}

	closures = context.getClosures(this);
	if (closures != NULL) {
		for (c_it = closures->begin(); c_it != closures->end(); c_it++) {
			(*c_it)->getFunctions(functionList, mixin, mixin.name.begin(), context);
		}
	}

	if (!functionList.empty())
		return;

	if (getParent() != NULL) {
		getParent()->getLocalFunctions(functionList,
									   mixin,
									   selector->needsArguments() ? NULL : this,
									   context);
	} else
		getPressStylesheet()->getFunctions(functionList, mixin, context);
}

void PressRuleset::insertNestedRules(Stylesheet &s,
	const Selector *prefix,
	ProcessingContext &context) const {
	std::list<PressRuleset *>::const_iterator r_it;

	for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
		(*r_it)->process(s, prefix, context);
	}
}

void PressRuleset::addClosures(ProcessingContext &context) const {
	std::list<PressRuleset *>::const_iterator r_it;

	for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
		if ((*r_it)->selector->needsArguments()) {
			context.addClosure(*(*r_it));
		}
	}
}

bool PressRuleset::matchConditions(const ProcessingContext &context,
	bool defaultVal) const {
	const std::list<TokenList> &conditions = selector->getConditions();
	std::list<TokenList>::const_iterator it;

	if (conditions.empty())
		return true;

	for (it = conditions.begin(); it != conditions.end(); it++) {

		if (context.validateCondition(*it, defaultVal)) {
			return true;
		}
	}
	return false;
}

bool PressRuleset::putArguments(PressMixinArguments &args, VariableMap &scope) const {
	const std::list<std::string> &parameters = selector->getParameters();
	std::list<std::string>::const_iterator pit;
	TokenList argsCombined;
	TokenList restVar;
	const TokenList *variable;
	size_t pos = 0;

	// combine with parameter names and add to local scope
	for (pit = parameters.begin(); pit != parameters.end(); pit++) {
		variable = args.get(*pit);

		if (variable == NULL)
			variable = args.get(pos++);

		if (variable == NULL)
			variable = selector->getDefault(*pit);

		if (variable == NULL || variable->empty())
			return false;

		scope.insert(pair<std::string, TokenList>(*pit, *variable));

		argsCombined.insert(argsCombined.end(), variable->begin(), variable->end());
		argsCombined.push_back(Token::BUILTIN_SPACE);
	}

	argsCombined.trim();

	if (selector->unlimitedArguments() && selector->getRestIdentifier() != "") {
		while (pos < args.count()) {
			variable = args.get(pos++);
			restVar.insert(restVar.end(), variable->begin(), variable->end());
			restVar.push_back(Token::BUILTIN_SPACE);
		}

		restVar.trim();
		scope.insert(
			pair<std::string, TokenList>(selector->getRestIdentifier(), restVar));
	}

	scope.insert(pair<std::string, TokenList>("@arguments", argsCombined));
	return true;
}

void PressRuleset::mergeDeclarations(Ruleset &ruleset, Declaration *merge) const {
	const std::list<Declaration *> *declarations = &ruleset.getDeclarations();
	std::list<Declaration *>::const_iterator it, del;
	Token *t;
	bool space;

	for (it = declarations->begin(); it != declarations->end();) {
		t = &(*it)->getProperty();

		if ((t->size() > 0 && t->at(t->size() - 1) == '+') ||
			(t->size() > 1 && t->compare(t->size() - 2, 2, "+_") == 0)) {

			space = t->at(t->size() - 1) == '_';

			if (merge == NULL) {

				t->resize(t->size() - (space ? 2 : 1));
				mergeDeclarations(ruleset, *it);
				it++;

			} else if (t->compare(0,
								  merge->getProperty().size(),
								  merge->getProperty()) == 0) {
				if (space) {
					merge->getValue().push_back(Token::BUILTIN_SPACE);
				} else {
					merge->getValue().push_back(Token::BUILTIN_COMMA);
					merge->getValue().push_back(Token::BUILTIN_SPACE);
				}
				merge->getValue().insert(merge->getValue().end(),
										 (*it)->getValue().begin(),
										 (*it)->getValue().end());
				del = it;
				it++;
				ruleset.deleteDeclaration(**del);
			} else
				it++;
		} else
			it++;
	}
}

}
