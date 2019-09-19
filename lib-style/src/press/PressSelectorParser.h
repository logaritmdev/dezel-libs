#ifndef __PressSelectorParser_h__
#define __PressSelectorParser_h__

#include "Token.h"
#include "TokenList.h"
#include "PressSelector.h"
#include "SelectorParser.h"

namespace Press {

class PressSelectorParser : public SelectorParser {

public:
	bool parse(TokenList &tokens,
		PressSelector &selector);

protected:
	SelectorParser selectorParser;
	bool parseExtension(TokenList &tokens, TokenList::iterator &offset, PressSelector &s);
	bool isArguments(TokenList &selector, TokenList::iterator it, std::string &delimiter);
	bool parseArguments(TokenList &selector, TokenList::iterator &offset, PressSelector &s);
	bool parseParameter(TokenList &selector, TokenList::iterator &it, Token &keyword, TokenList &value, const std::string &delimiter);
	bool parseDefaultValue(TokenList &arguments, TokenList::iterator &it, const std::string &delimiter, TokenList &value);
	bool parseConditions(TokenList &selector, TokenList::iterator &offset, PressSelector &s);
};

}

#endif
