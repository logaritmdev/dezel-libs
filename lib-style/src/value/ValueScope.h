#ifndef __tree_ValueScope_h__
#define __tree_ValueScope_h__

#include <list>
#include <map>
#include <string>
#include "TokenList.h"

namespace Press {

class ValueScope {

public:
	virtual const TokenList *getVariable(const std::string &key) const = 0;
};

}

#endif
