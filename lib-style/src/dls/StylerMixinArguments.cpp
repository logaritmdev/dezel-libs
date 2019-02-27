#include "StylerMixinArguments.h"
#include "ProcessingContext.h"

const TokenList *StylerMixinArguments::get(const size_t i) const {
	if (i < arguments.size())
		return &arguments[i];
	else
		return NULL;
}

size_t StylerMixinArguments::count() const {
	return arguments.size();
}

const TokenList *StylerMixinArguments::get(const std::string &name) const {
	std::map<std::string, TokenList>::const_iterator i;

	i = namedArguments.find(name);

	if (i != namedArguments.end())
		return &i->second;
	else
		return NULL;
}

void StylerMixinArguments::add(TokenList &argument) {
	arguments.push_back(argument);
}

void StylerMixinArguments::add(std::string name, TokenList &argument) {
	namedArguments.insert(std::pair<std::string, TokenList>(name, argument));
}

void StylerMixinArguments::process(ProcessingContext &context) {
	std::vector<TokenList>::iterator arg_i;
	std::map<std::string, TokenList>::iterator argn_i;

	for (arg_i = arguments.begin(); arg_i != arguments.end(); arg_i++) {
		context.processValue(*arg_i);
	}

	for (argn_i = namedArguments.begin(); argn_i != namedArguments.end();
		 argn_i++) {
		context.processValue(argn_i->second);
	}

}
