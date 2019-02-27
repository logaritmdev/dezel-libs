#include "StylerRuleset.h"
#include "StylerStylesheet.h"
#include "StylerMediaQueryRuleset.h"

StylerRuleset::StylerRuleset(StylerSelector &selector,
	const StylerRuleset &parent) :
	Ruleset(selector),
	parent(&parent), lessStylesheet(NULL), selector(&selector) {

}

StylerRuleset::StylerRuleset(StylerSelector &selector,
	const StylerStylesheet &parent) :
	Ruleset(selector),
	parent(NULL), lessStylesheet(&parent), selector(&selector) {
}

StylerRuleset::~StylerRuleset() {
	while (!nestedRules.empty()) {
		delete nestedRules.back();
		nestedRules.pop_back();
	}
}

const StylerSelector &StylerRuleset::getLessSelector() const {
	return *selector;
}

void StylerRuleset::addExtension(StylerExtension &extension) {
	extensions.push_back(extension);
}

StylerDeclaration *StylerRuleset::createLessDeclaration() {
	StylerDeclaration *d = new StylerDeclaration();

	Ruleset::addStatement(*d);
	d->setLessRuleset(*this);
	lessDeclarations.push_back(d);
	return d;
}

const std::list<StylerDeclaration *> &StylerRuleset::getLessDeclarations()
const {
	return lessDeclarations;
}

StylerMixin *StylerRuleset::createMixin(const TokenList &selector) {
	StylerMixin *m = new StylerMixin(selector, *this);

	Ruleset::addStatement(*m);
	mixins.push_back(m);
	stylesheetStatements.push_back(m);
	return m;
}

const std::list<StylerMixin *> &StylerRuleset::getMixins()
const {
	return mixins;
}

StylerAtRule *StylerRuleset::createLessAtRule(const Token &keyword) {
	StylerAtRule *r = new StylerAtRule(keyword);

	Ruleset::addStatement(*r);
	lessAtRules.push_back(r);
	stylesheetStatements.push_back(r);
	return r;
}

const std::list<StylerAtRule *> &StylerRuleset::getLessAtRules()
const {
	return lessAtRules;
}

const std::list<StylesheetStatement *> &StylerRuleset::getStylesheetStatements()
const {
	return stylesheetStatements;
}

StylerRuleset *StylerRuleset::createNestedRule(StylerSelector &selector) {
	StylerRuleset *r = new StylerRuleset(selector, *this);

	nestedRules.push_back(r);
	return r;
}

StylerMediaQueryRuleset *StylerRuleset::createMediaQuery(TokenList &selector) {
	StylerMediaQueryRuleset *r = new StylerMediaQueryRuleset(selector, *this);

	nestedRules.push_back(r);
	return r;
}

void StylerRuleset::deleteNestedRule(StylerRuleset &ruleset) {
	nestedRules.remove(&ruleset);
	delete &ruleset;
}

const std::list<StylerRuleset *> &StylerRuleset::getNestedRules() const {
	return nestedRules;
}

void StylerRuleset::putVariable(const std::string &key, const TokenList &value) {
	variables[key] = value;
}

VariableMap &StylerRuleset::getVariables() {
	return variables;
}

const TokenList *StylerRuleset::getVariable(const std::string &key) const {
	return variables.getVariable(key);
}

void StylerRuleset::setParent(const StylerRuleset *r) {
	parent = r;
}

const StylerRuleset *StylerRuleset::getParent() const {
	return parent;
}

void StylerRuleset::setLessStylesheet(const StylerStylesheet &s) {
	lessStylesheet = &s;
}

const StylerStylesheet *StylerRuleset::getLessStylesheet() const {
	if (lessStylesheet != NULL)
		return lessStylesheet;
	else if (parent != NULL)
		return parent->getLessStylesheet();
	else
		return NULL;
}

void StylerRuleset::processExtensions(ProcessingContext &context,
	const Selector *prefix) const {
	const std::list<StylerExtension> &e = getLessSelector().getExtensions();
	std::list<StylerExtension>::const_iterator e_it;
	StylerExtension extension;

	for (e_it = e.begin(); e_it != e.end(); e_it++) {
		extension = *e_it;
		if (prefix != NULL)
			extension.getExtension().addPrefix(*prefix);

		context.interpolate(extension.getExtension());

		context.addExtension(extension);
	}
}

void StylerRuleset::processInlineExtensions(ProcessingContext &context,
	const Selector &selector) const {
	std::list<StylerExtension>::const_iterator e_it;
	StylerExtension extension;

	for (e_it = extensions.begin(); e_it != extensions.end(); e_it++) {
		extension = *e_it;

		extension.setExtension(selector);

		context.addExtension(extension);
	}
}

bool StylerRuleset::call(StylerMixinArguments &args,
	Ruleset &target,
	ProcessingContext &context,
	bool defaultVal) const {
	if (call(args, context, &target, NULL, defaultVal)) {
		processInlineExtensions(context, target.getSelector());
		return true;
	} else
		return false;
}

bool StylerRuleset::call(StylerMixinArguments &args,
	Stylesheet &target,
	ProcessingContext &context,
	bool defaultVal) const {
	return call(args, context, NULL, &target, defaultVal);
}

bool StylerRuleset::call(StylerMixinArguments &args,
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

void StylerRuleset::process(Stylesheet &s, void *context) const {
	process(s, NULL, *((ProcessingContext *) context));
}

void StylerRuleset::process(Stylesheet &s,
	const Selector *prefix,
	ProcessingContext &context) const {
	Ruleset *target;
	Selector *selector;

	if (getLessSelector().needsArguments())
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

void StylerRuleset::processStatements(Ruleset &target,
	void *context) const {
	// process statements
	Ruleset::processStatements(target, context);

	// insert nested rules
	insertNestedRules(*target.getStylesheet(), &target.getSelector(),
					  *(ProcessingContext *) context);
}

void StylerRuleset::processStatements(Stylesheet &target,
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

const TokenList *StylerRuleset::getVariable(const std::string &key,
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
		return getLessStylesheet()->getVariable(key, context);
}

void StylerRuleset::getFunctions(list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	TokenList::const_iterator offset,
	const ProcessingContext &context) const {
	std::list<StylerRuleset *>::const_iterator r_it;
	const std::list<StylerClosure *> *closures;
	std::list<StylerClosure *>::const_iterator c_it;
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

void StylerRuleset::getLocalFunctions(std::list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	const ProcessingContext &context) const {
	getLocalFunctions(functionList, mixin, NULL, context);
}

void StylerRuleset::getLocalFunctions(std::list<const StylerFunction *> &functionList,
	const StylerMixin &mixin,
	const StylerRuleset *exclude,
	const ProcessingContext &context) const {
	std::list<StylerRuleset *>::const_iterator r_it;
	const std::list<StylerClosure *> *closures;
	std::list<StylerClosure *>::const_iterator c_it;

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
		getLessStylesheet()->getFunctions(functionList, mixin, context);
}

void StylerRuleset::insertNestedRules(Stylesheet &s,
	const Selector *prefix,
	ProcessingContext &context) const {
	std::list<StylerRuleset *>::const_iterator r_it;

	for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
		(*r_it)->process(s, prefix, context);
	}
}

void StylerRuleset::addClosures(ProcessingContext &context) const {
	std::list<StylerRuleset *>::const_iterator r_it;

	for (r_it = nestedRules.begin(); r_it != nestedRules.end(); r_it++) {
		if ((*r_it)->selector->needsArguments()) {
			context.addClosure(*(*r_it));
		}
	}
}

bool StylerRuleset::matchConditions(const ProcessingContext &context,
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

bool StylerRuleset::putArguments(StylerMixinArguments &args, VariableMap &scope) const {
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

void StylerRuleset::mergeDeclarations(Ruleset &ruleset, Declaration *merge) const {
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
