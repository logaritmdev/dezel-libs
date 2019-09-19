#ifndef __ProcessingContext_h__
#define __ProcessingContext_h__

#include <list>
#include <map>
#include <string>

#include "TokenList.h"
#include "VariableMap.h"
#include "PressClosure.h"
#include "PressExtension.h"
#include "PressFunction.h"
#include "PressMixinCall.h"
#include "ValueProcessor.h"
#include "ValueScope.h"

namespace Press {

class PressRuleset;
class PressFunction;
class PressClosure;
class PressMixin;
class PressStylesheet;

class ProcessingContext : public ValueScope {
private:
	PressMixinCall *stack;
	ValueProcessor processor;
	std::list<std::list<PressExtension> *> extensions;
	const PressStylesheet *contextStylesheet;
	// return values
	std::map<const PressFunction *, std::list<PressClosure *> > closures;
	std::map<const PressFunction *, VariableMap> variables;
	std::list<PressClosure *> base_closures;
	VariableMap base_variables;
public:
	ProcessingContext();
	virtual ~ProcessingContext();
	void setPressStylesheet(const PressStylesheet &stylesheet);
	const PressStylesheet *getPressStylesheet() const;
	virtual const TokenList *getVariable(const std::string &key) const;
	const TokenList *getFunctionVariable(const std::string &key, const PressFunction *function) const;
	const TokenList *getBaseVariable(const std::string &key) const;
	void pushMixinCall(const PressFunction &function, bool savepoint = false, bool important = false);
	void popMixinCall();
	bool isInStack(const PressFunction &function) const;
	VariableMap *getStackArguments() const;
	VariableMap *getStackArguments(const PressFunction *function) const;
	bool isStackEmpty() const;
	bool isSavePoint() const;
	const PressFunction *getSavePoint() const;
	bool isImportant() const;
	void getFunctions(std::list<const PressFunction *> &functionList, const PressMixin &mixin) const;
	const std::list<PressClosure *> *getClosures(const PressFunction *function) const;
	const std::list<PressClosure *> *getBaseClosures() const;
	void addClosure(const PressRuleset &ruleset);
	void addVariables(const VariableMap &variables);
	void pushExtensionScope(std::list<PressExtension> &scope);
	void popExtensionScope();
	void addExtension(PressExtension &extension);
	std::list<PressExtension> *getExtensions();
	ValueProcessor *getValueProcessor();
	void interpolate(Selector &selector) const;
	void interpolate(TokenList &tokens) const;
	void interpolate(std::string &str) const;
	void processValue(TokenList &value) const;
	bool validateCondition(const TokenList &value, bool defaultVal = false) const;
};

}

#endif
