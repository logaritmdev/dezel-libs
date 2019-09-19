#include "RulesetStatement.h"

namespace Press {

void RulesetStatement::setRuleset(Ruleset *r) {
	ruleset = r;
}

Ruleset *RulesetStatement::getRuleset() const {
	return ruleset;
}

}
