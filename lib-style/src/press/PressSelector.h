#ifndef __PressSelector_h__
#define __PressSelector_h__

#include <iterator>
#include <list>
#include <map>
#include <string>

#include "PressMixinArguments.h"
#include "PressExtension.h"
#include "Selector.h"

namespace Press {

class PressSelector : public Selector {

private:
	std::list<PressExtension> extensions;
	std::list<std::string> parameters;
	std::list<TokenList> defaults;
	std::list<TokenList> conditions;
	bool _unlimitedArguments;
	bool _needsArguments;
	std::string restIdentifier;

public:
	PressSelector();
	virtual ~PressSelector();
	void addExtension(PressExtension &extension);
	void addParameter(Token &keyword, TokenList &value);
	void setUnlimitedArguments(bool b);
	void setUnlimitedArguments(bool b, Token restKeyword);
	void eraseArguments();
	void addCondition(TokenList &condition);
	void setNeedsArguments(bool b);
	const std::list<PressExtension> &getExtensions() const;
	const std::list<std::string> &getParameters() const;
	const TokenList *getDefault(const std::string &parameter) const;
	const std::list<TokenList> &getConditions() const;
	bool matchArguments(const PressMixinArguments &arguments) const;
	bool needsArguments() const;
	bool unlimitedArguments() const;
	std::string getRestIdentifier() const;
};

}

#endif
