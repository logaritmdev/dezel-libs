#ifndef __value_StringFunctions_h__
#define __value_StringFunctions_h__

#include <vector>

namespace Press {

class Value;
class FunctionLibrary;

class StringFunctions {

public:
	static void loadFunctions(FunctionLibrary &lib);
	static Value* e(const std::vector<const Value*> &arguments);
	static Value* escape(const std::vector<const Value*> &arguments);
	static Value* format(const std::vector<const Value*> &arguments);
	static Value* replace(const std::vector<const Value*> &arguments);
	static Value* color(const std::vector<const Value*> &arguments);
	static Value* data_uri(const std::vector<const Value*> &arguments);
};

}

#endif
