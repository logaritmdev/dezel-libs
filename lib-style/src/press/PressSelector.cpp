#include "PressRuleset.h"

namespace Press {

PressSelector::PressSelector() : Selector() {
	_unlimitedArguments = false;
	_needsArguments = false;
}

PressSelector::~PressSelector() {
}


const TokenList *PressSelector::getDefault(const std::string &keyword) const {
	std::list<std::string>::const_iterator pit = parameters.begin();
	std::list<TokenList>::const_iterator dit = defaults.begin();

	for (; pit != parameters.end(); pit++, dit++) {
		if ((*pit) == keyword)
			return &(*dit);
	}
	return NULL;
}

const std::list<std::string> &PressSelector::getParameters() const {
	return parameters;
}

const std::list<TokenList> &PressSelector::getConditions() const {
	return conditions;
}

void PressSelector::addExtension(PressExtension &extension) {
	extensions.push_back(extension);
}

const std::list<PressExtension> &PressSelector::getExtensions() const {
	return extensions;
}

void PressSelector::addParameter(Token &keyword, TokenList &value) {
	parameters.push_back(keyword);
	defaults.push_back(value);
}

void PressSelector::setUnlimitedArguments(bool b) {
	_unlimitedArguments = b;
}

void PressSelector::setUnlimitedArguments(bool b, Token restKeyword) {
	_unlimitedArguments = b;
	restIdentifier = restKeyword;
}

void PressSelector::setNeedsArguments(bool b) {
	_needsArguments = b;
}

void PressSelector::eraseArguments() {
	parameters.clear();
	defaults.clear();
	_unlimitedArguments = false;
	restIdentifier = "";
	_needsArguments = false;
}

void PressSelector::addCondition(TokenList &condition) {
	conditions.push_back(condition);
}

bool PressSelector::matchArguments(const PressMixinArguments &args) const {
	std::list<std::string>::const_iterator p_it = parameters.begin();
	std::list<TokenList>::const_iterator d_it = defaults.begin();
	size_t pos = 0;
	const TokenList *a;

	for (; p_it != parameters.end(); p_it++, d_it++) {
		if ((a = args.get(*p_it)) == NULL &&
			(a = args.get(pos++)) == NULL &&
			(a = &(*d_it))->empty()) {
			return false;
		} else if ((*p_it)[0] != '@' &&
				   a->size() == 1 &&
				   (*p_it) != a->front()) {
			return false;
		}
	}
	return (pos >= args.count() || unlimitedArguments());
}

bool PressSelector::needsArguments() const {
	return _needsArguments;
}

bool PressSelector::unlimitedArguments() const {
	return _unlimitedArguments;
}

string PressSelector::getRestIdentifier() const {
	return restIdentifier;
}

}
