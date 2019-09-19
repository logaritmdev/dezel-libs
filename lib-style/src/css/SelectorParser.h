#ifndef __css_CssSelectorParser_h__
#define __css_CssSelectorParser_h__

#include "Token.h"
#include "TokenList.h"
#include "Selector.h"

namespace Press {

class SelectorParser {

public:
	bool parse(TokenList &tokens, Selector &selector);

private:
	TokenList::const_iterator findComma(TokenList &tokens, TokenList::const_iterator offset) const;
};

}

#endif
