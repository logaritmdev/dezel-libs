#ifndef __PressRuleset_h__
#define __PressRuleset_h__

#include <list>
#include <map>
#include <string>

#include "Ruleset.h"
#include "Selector.h"
#include "Stylesheet.h"
#include "ParseException.h"
#include "ValueProcessor.h"
#include "Token.h"
#include "TokenList.h"
#include "VariableMap.h"
#include "PressFunction.h"
#include "PressSelector.h"
#include "PressMixin.h"
#include "PressMixinArguments.h"
#include "ProcessingContext.h"
#include "PressDeclaration.h"
#include "PressAtRule.h"

namespace Press {

class PressStylesheet;
class PressMediaQueryRuleset;
class PressClosure;

class PressRuleset : public Ruleset, public PressFunction {

protected:
	VariableMap variables;
	std::list<StylesheetStatement*> stylesheetStatements;
	std::list<PressRuleset*> nestedRules;
	std::list<PressClosure*> pressClosures;
	std::list<PressExtension> pressExtensions;
	std::list<PressDeclaration*> pressDeclarations;
	std::list<PressMixin*> pressMixins;
	std::list<PressAtRule*> pressAtRules;
	const PressRuleset *parent;
	const PressStylesheet *pressStylesheet;
	PressSelector *selector;
	ProcessingContext *context;
	void processVariables();
	void insertNestedRules(Stylesheet &s, const Selector *prefix, ProcessingContext &context) const;
	void addClosures(ProcessingContext &context) const;
	bool call(PressMixinArguments &args, ProcessingContext &context, Ruleset *ruleset, Stylesheet *stylesheet, bool defaultVal = false) const;
	void mergeDeclarations(Ruleset &ruleset, Declaration *merge = NULL) const;

public:
	PressRuleset(PressSelector &selector, const PressRuleset &parent);
	PressRuleset(PressSelector &selector, const PressStylesheet &parent);
	virtual ~PressRuleset();
	virtual const PressSelector &getPressSelector() const;
	void addExtension(PressExtension &extension);
	PressDeclaration *createPressDeclaration();
	PressMixin *createMixin(const TokenList &selector);
	PressAtRule *createPressAtRule(const Token &keyword);
	PressRuleset *createNestedRule(PressSelector &selector);
	PressMediaQueryRuleset *createMediaQuery(TokenList &selector);
	void deleteNestedRule(PressRuleset &ruleset);
	const list<PressDeclaration *> &getPressDeclarations() const;
	const list<PressMixin *> &getMixins() const;
	const list<PressAtRule *> &getPressAtRules() const;
	const list<StylesheetStatement *> &getStylesheetStatements() const;
	const list<PressRuleset *> &getNestedRules() const;
	void putVariable(const std::string &key, const TokenList &value);
	VariableMap &getVariables();
	const TokenList *getVariable(const std::string &key) const;
	const TokenList *getInheritedVariable(const std::string &key, const PressMixinCall &stack) const;
	const list<PressClosure *> &getClosures() const;
	void setParent(const PressRuleset *r);
	const PressRuleset *getParent() const;
	void setPressStylesheet(const PressStylesheet &stylesheet);
	const PressStylesheet *getPressStylesheet() const;
	void processExtensions(ProcessingContext &context, const Selector *prefix) const;
	void processInlineExtensions(ProcessingContext &context, const Selector &selector) const;
	virtual bool call(PressMixinArguments &args, Ruleset &target, ProcessingContext &context, bool defaultVal = false) const;
	virtual bool call(PressMixinArguments &args, Stylesheet &s, ProcessingContext &context, bool defaultVal = false) const;
	virtual void processStatements(Ruleset &target, void *context) const;
	void processStatements(Stylesheet &target, void *context) const;
	virtual void process(Stylesheet &s, void *context) const;
	virtual void process(Stylesheet &s, const Selector *prefix, ProcessingContext &context) const;
	const TokenList *getVariable(const std::string &key, const ProcessingContext &context) const;
	virtual void getFunctions(list<const PressFunction *> &functionList, const PressMixin &mixin, TokenList::const_iterator selector_offset, const ProcessingContext &context) const;

	/**
	 * Look for a ruleset inside this ruleset and scope up to
	 * getParent(), or getPressStylesheet() if getParent() is NULL.
	 */
	void getLocalFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, const ProcessingContext &context) const;
	void getLocalFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin, const PressRuleset *exclude, const ProcessingContext &context) const;
	bool matchConditions(const ProcessingContext &context, bool defaultVal = false) const;
	bool putArguments(PressMixinArguments &args, VariableMap &scope) const;
};

}

#endif
