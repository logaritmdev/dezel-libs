#ifndef __PressDeclaration_h__
#define __PressDeclaration_h__

#include "Declaration.h"
#include "Ruleset.h"

namespace Press {

class PressRuleset;

class PressDeclaration : public Declaration {
	PressRuleset *pressRuleset;

public:
	void setPressRuleset(PressRuleset &r);
	PressRuleset *getPressRuleset();
	virtual void process(Ruleset &r, void *context) const;
};

}

#endif
