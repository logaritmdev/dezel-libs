#ifndef __PressMixinArguments_h__
#define __PressMixinArguments_h__

#include <map>
#include <string>
#include <vector>

#include "TokenList.h"

namespace Press {

class ProcessingContext;

class PressMixinArguments {

private:
	std::vector<TokenList> arguments;
	std::map<std::string, TokenList> namedArguments;

public:
	size_t count() const;
	const TokenList *get(const size_t i) const;
	const TokenList *get(const std::string &name) const;
	void add(TokenList &argument);
	void add(std::string name, TokenList &argument);
	void process(ProcessingContext &context);
};

}

#endif
