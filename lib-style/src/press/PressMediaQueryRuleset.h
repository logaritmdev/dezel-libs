#ifndef __PressMediaQueryRuleset_h__
#define __PressMediaQueryRuleset_h__

#include <list>
#include <map>

#include "Selector.h"
#include "Stylesheet.h"
#include "Token.h"
#include "PressRuleset.h"

namespace Press {

class PressMediaQueryRuleset : public PressRuleset {

private:
	TokenList selector;

public:
	PressMediaQueryRuleset(TokenList &selector, const PressRuleset &parent);
	virtual ~PressMediaQueryRuleset();
	virtual void process(Stylesheet &s, const Selector *prefix, ProcessingContext &context) const;
};

}

#endif
