#ifndef __VariableMap_h__
#define __VariableMap_h__

#include <map>
#include "TokenList.h"

namespace Press {

using std::map;
using std::string;

class VariableMap : public map<string, TokenList> {

public:
	const TokenList* getVariable(const string &key) const;
	void merge(const VariableMap &map);
	void overwrite(const VariableMap &map);
	string toString() const;
};

}

#endif
